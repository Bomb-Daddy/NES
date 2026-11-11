#include "gk180_seed_writer_scan.h"
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned tests=0;
#define CHECK(x) do{tests++;if(!(x)){fprintf(stderr,"FAIL %u line %d: %s\n",tests,__LINE__,#x);return 1;}}while(0)
static void emit(uint8_t *bytes,uint16_t pc,const uint8_t *code,size_t n){
 memcpy(bytes+16u+15u*16384u+(size_t)(pc-0xc000u),code,n);
}
static const Gk180Site *site(const Gk180WriterMap *r,uint16_t pc){
 for(uint32_t i=0;i<r->source_sites;i++)if(r->sites[i].pc==pc)return &r->sites[i];
 return NULL;
}
int main(int argc,char **argv){
 uint8_t *image=(uint8_t*)calloc(1u,GK180_ROM_BYTES);CHECK(image!=NULL);
 image[0]='N';image[1]='E';image[2]='S';image[3]=0x1au;
 image[4]=16u;image[5]=0u;image[6]=0x12u;
 memset(image+16u+15u*16384u,0x60,16384u); /* legal RTS */
 uint8_t *bank=image+16u+15u*16384u;
 bank[0x3ffc]=0x00;bank[0x3ffd]=0xc1; /* RESET */
 bank[0x3ffa]=0x00;bank[0x3ffb]=0xc2; /* NMI */
 bank[0x3ffe]=0x00;bank[0x3fff]=0xc3; /* IRQ */
 static const uint8_t reset[]={0xa9,0x12,0x85,0x84,0x20,0x80,0xc1,0x4c,0xa0,0xc1};
 static const uint8_t call[]={0x86,0x85,0x60};
 static const uint8_t rmw[]={0xe6,0x86,0x6c,0x00,0x20};
 static const uint8_t nmi[]={0x66,0x87,0x20,0x89,0xc2,0x60};
 static const uint8_t irq[]={0x8d,0x89,0x00,0x60};
 static const uint8_t unreachable[]={0x85,0x88,0x60};
 emit(image,0xc100,reset,sizeof reset);emit(image,0xc180,call,sizeof call);
 emit(image,0xc1a0,rmw,sizeof rmw);emit(image,0xc200,nmi,sizeof nmi);
 emit(image,0xc300,irq,sizeof irq);emit(image,0xc050,unreachable,sizeof unreachable);
 Gk180WriterMap map={0};CHECK(gk180_scan_image(image,GK180_ROM_BYTES,&map)==GK180_OK);
 CHECK(map.vectors[0]==0xc100 && map.vectors[1]==0xc200&&map.vectors[2]==0xc300);
 CHECK(map.source_sites==5u);CHECK(map.internal_rng_sites==0u&&map.external_seed_site_candidates==5u);
 const Gk180Site *s=site(&map,0xc102);CHECK(s&&s->address==0x84u&&s->kind==GK180_STORE&&s->roots==1u);
 s=site(&map,0xc180);CHECK(s&&s->address==0x85u&&s->roots==1u);
 s=site(&map,0xc1a0);CHECK(s&&s->address==0x86u&&s->kind==GK180_RMW&&s->roots==1u);
 s=site(&map,0xc200);CHECK(s&&s->address==0x87u&&s->roots==2u);
 s=site(&map,0xc300);CHECK(s&&s->address==0x89u&&s->roots==4u);
 CHECK(!site(&map,0xc050));CHECK(map.rng_entry_edges==1u);
 CHECK(map.unknown_indirect_jumps==1u);
 /* Instruction root overlap is counted once; the bitset records both. */
 bank[0x3ffa]=0x00;bank[0x3ffb]=0xc1;
 CHECK(gk180_scan_image(image,GK180_ROM_BYTES,&map)==GK180_OK);
 s=site(&map,0xc102);CHECK(s&&(s->roots&3u)==3u);
 bank[0x3ffa]=0x00;bank[0x3ffb]=0xc2;
 /* Overlong, malformed and missing source preserve caller out unchanged. */
 Gk180WriterMap before=map;
 CHECK(gk180_scan_image(image,GK180_ROM_BYTES-1u,&map)==GK180_BAD_INPUT);
 CHECK(memcmp(&before,&map,sizeof map)==0);
 image[0]=0;CHECK(gk180_scan_image(image,GK180_ROM_BYTES,&map)==GK180_BAD_INPUT);
 CHECK(memcmp(&before,&map,sizeof map)==0);image[0]='N';
 CHECK(gk180_scan_image(NULL,GK180_ROM_BYTES,&map)==GK180_BAD_INPUT);
 CHECK(gk180_scan_image(image,GK180_ROM_BYTES,NULL)==GK180_BAD_INPUT);
 CHECK(gk180_scan_authenticated("/this/rom/does/not/exist.nes",&map)==GK180_SOURCE);
 CHECK(memcmp(&before,&map,sizeof map)==0);
 CHECK(gk180_scan_authenticated(NULL,&map)==GK180_BAD_INPUT);
 /* Synthetic image is structurally valid, not a copyright-authenticated ROM. */
 char temp[L_tmpnam];(void)temp;
 free(image);
 if(argc==2){
  Gk180WriterMap original={0};
  CHECK(gk180_scan_authenticated(argv[1],&original)==GK180_OK);
  CHECK(original.decoded[0]>0u);
  CHECK(original.source_sites<=GK180_MAX_SITES);
  Gk180WriterMap snapshot=original;
  FILE *f=fopen(argv[1],"rb");CHECK(f!=NULL);fclose(f);
  Gk1App app;gk1_app_init(&app);
  CHECK(gk1_app_source_seed_writers(&app,argv[1],&original)==GK180_BAD_INPUT);
  CHECK(memcmp(&original,&snapshot,sizeof original)==0);
  /* A source-intent completed setup is still NOT source-generated Spring. */
  app.active_world=0;app.world_ready[0]=1;
  app.worlds[0].scenario_id=GK1_MONGOL_CONQUEST;
  app.worlds[0].country_count=14;
  app.worlds[0].phase=GK1_PRE_SETUP_TEMPLATE;
  app.setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
  app.setup_difficulty[0]=3u;
  CHECK(gk1_app_source_seed_writers(&app,argv[1],&original)==GK180_OK);
  CHECK(memcmp(&snapshot,&original,sizeof original)==0);
  app.stock_checkpoint.valid=1;
  CHECK(gk1_app_source_seed_writers(&app,argv[1],&original)==GK180_BAD_INPUT);
  CHECK(memcmp(&snapshot,&original,sizeof original)==0);
  printf("VERIFIED_STATIC authenticated source reset/NMI/IRQ writer candidates=%u, RNG_INTERNAL=%u, EXTERNAL_STATIC=%u, unmapped=%u; NO FIRST SPRING, NO NES Mesen RUN\n",
   (unsigned)snapshot.source_sites,(unsigned)snapshot.internal_rng_sites,(unsigned)snapshot.external_seed_site_candidates,
   (unsigned)snapshot.unmapped_edges);
 }
 printf("GK180 synthetic/authenticated count %u PASS\n",tests);
 return 0;
}
