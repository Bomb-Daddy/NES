#include "gk130_dma_bus.h"
#include "gk83_reset_trace.h"
#include "gk82_stock_cartridge.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"GK130 FAIL %s:%d %s\n",__FILE__,__LINE__,#x);return 1;}}while(0)
static void fresh(Gk128Oam *o,uint8_t a){memset(o,0,sizeof *o);(void)gk128_set_oam_address(o,a);}
static int source_matches(const Gk128Oam *o,const uint8_t *src,uint8_t start){
 for(unsigned i=0;i<256;i++)if(o->known[(start+i)&255u]!=1u||
    o->data[(start+i)&255u]!=src[i])return 0;
 return 1;
}
int main(void){
 uint8_t work[2048],known[2048];Gk94PrgRam save={0};
 uint8_t *prg=malloc(GK130_PRG_BYTES);CHECK(prg);
 for(unsigned i=0;i<2048;i++){work[i]=(uint8_t)(i*13u+7u);known[i]=1u;}
 for(unsigned i=0;i<GK130_PRG_BYTES;i++)prg[i]=(uint8_t)((i/16384u)*17u+(i&255u));
 Gk128Oam o,before;fresh(&o,251u);uint16_t stall=999u;
 before=o;
 #define DMA(pg,au,ctrl,bnk,ck,bk,phase) gk130_dma_cpu_bus(&o,work,known,&save,prg,GK130_PRG_BYTES,au,ctrl,bnk,ck,bk,pg,1u,phase,&stall)
 CHECK(DMA(0x20u,1,0x0c,0,1,1,0)==GK130_DMA_IO_UNMODELLED);
 CHECK(DMA(0x3fu,1,0x0c,0,1,1,0)==GK130_DMA_IO_UNMODELLED);
 CHECK(DMA(0x40u,1,0x0c,0,1,1,0)==GK130_DMA_IO_UNMODELLED);
 CHECK(DMA(0x5fu,1,0x0c,0,1,1,0)==GK130_DMA_IO_UNMODELLED);
 CHECK(memcmp(&o,&before,sizeof o)==0&&stall==999u);
 for(unsigned page=0x60u;page<0x80u;page++){
  before=o;stall=999u;
  CHECK(DMA((uint8_t)page,1,0x0c,0,1,1,0)==GK130_DMA_UNKNOWN_SOURCE);
  CHECK(memcmp(&o,&before,sizeof o)==0&&stall==999u);
  uint8_t blob[256];for(unsigned i=0;i<256;i++)blob[i]=(uint8_t)(page+i*7u);
  CHECK(gk94_prgram_seed_physical(&save,(page-0x60u)*256u,blob,256u)==GK94_RAM_OK);
  CHECK(DMA((uint8_t)page,1,0x0c,0,1,1,1)==GK130_DMA_OK);
  CHECK(source_matches(&o,blob,251u)&&stall==514u);
 }
 /* The SRAM bank is distinct from work RAM even at a matching physical
  * 0x0DCD. An unknown PRG-RAM byte may not borrow a known work RAM byte. */
 fresh(&o,251u);stall=999u;before=o;
 save.known[0x0dcdu]=0u;
 CHECK(DMA(0x6du,1,0x0c,0,1,1,0)==GK130_DMA_UNKNOWN_SOURCE);
 CHECK(memcmp(&o,&before,sizeof o)==0&&stall==999u);
 save.known[0x0dcdu]=1u;
 for(unsigned i=0;i<8;i++){
  unsigned page=i;
  CHECK(DMA((uint8_t)page,1,0x0c,0,1,1,0)==GK130_DMA_OK);
  CHECK(source_matches(&o,work+page*256u,251u)&&stall==513u);
 }
 /* A source read in $8000-$FFFF requires a separately authenticated
  * cartridge AND sufficiently known MMC1 mapping; no ROM bytes shipped. */
 fresh(&o,251u);before=o;stall=999u;
 CHECK(DMA(0x80u,0,0x0c,2,1,1,0)==GK130_DMA_ROM_UNVERIFIED);
 CHECK(DMA(0x80u,1,0x0c,2,0,1,0)==GK130_DMA_BANK_UNKNOWN);
 CHECK(DMA(0x80u,1,0x0c,2,1,0,0)==GK130_DMA_BANK_UNKNOWN);
 CHECK(DMA(0xc0u,1,0x08,2,1,0,0)==GK130_DMA_BANK_UNKNOWN);
 CHECK(DMA(0xc0u,1,0x0c,2,0,1,0)==GK130_DMA_BANK_UNKNOWN); /* control unknown: bank 15 is NOT established */
 CHECK(memcmp(&o,&before,sizeof o)==0&&stall==999u);
 for(unsigned bank=0;bank<16;bank++){
  CHECK(DMA(0x80u,1,0x0c,bank,1,1,0)==GK130_DMA_OK);
  CHECK(source_matches(&o,prg+bank*16384u,251u));
 }
 /* MMC1 modes 0/1 select 32KiB pairs, mode2 fixes low to bank0,
  * mode3 fixes high to bank15. Compare both source windows. */
 CHECK(DMA(0x80u,1,0x00,7,1,1,0)==GK130_DMA_OK);
 CHECK(source_matches(&o,prg+6u*16384u,251u));
 CHECK(DMA(0xc0u,1,0x00,7,1,1,0)==GK130_DMA_OK);
 CHECK(source_matches(&o,prg+7u*16384u,251u));
 CHECK(DMA(0x80u,1,0x08,7,1,1,0)==GK130_DMA_OK);
 CHECK(source_matches(&o,prg,251u));
 CHECK(DMA(0xc0u,1,0x08,7,1,1,0)==GK130_DMA_OK);
 CHECK(source_matches(&o,prg+7u*16384u,251u));
 CHECK(DMA(0xc0u,1,0x0c,7,1,1,0)==GK130_DMA_OK);
 CHECK(source_matches(&o,prg+15u*16384u,251u));
 CHECK(DMA(0xffu,1,0x0c,7,1,1,0)==GK130_DMA_OK);
 CHECK(source_matches(&o,prg+15u*16384u+0x3f00u,251u));
 /* Invalid source metadata must never mutate the OAM or caller's stall. */
 before=o;stall=999u;
 CHECK(gk130_dma_cpu_bus(&o,work,known,&save,prg,GK130_PRG_BYTES-1u,1u,0x0c,7,1,1,0x80,1,0,&stall)==GK130_DMA_ROM_UNVERIFIED);
 CHECK(gk130_dma_cpu_bus(&o,work,known,&save,prg,GK130_PRG_BYTES,1u,0x0c,7,1,1,0x80,0,0,&stall)==GK130_DMA_UNKNOWN_PHASE);
 CHECK(gk130_dma_cpu_bus(&o,work,known,&save,prg,GK130_PRG_BYTES,1u,0x0c,7,1,1,0x80,1,2,&stall)==GK130_DMA_BAD);
 CHECK(memcmp(&o,&before,sizeof o)==0&&stall==999u);
 const char *stock=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(stock&&*stock){
  Gk82RomBootstrap b;CHECK(gk82_probe_stock_rom(stock,&b)==GK82_ROM_OK);
  FILE *f=fopen(stock,"rb");CHECK(f);
  uint8_t *orig=malloc(GK130_PRG_BYTES+16u);CHECK(orig);
  CHECK(fread(orig,1,GK130_PRG_BYTES+16u,f)==GK130_PRG_BYTES+16u);
  CHECK(fclose(f)==0);
  fresh(&o,251u);
  /* Real exact private ROM (hash checked) used ONLY in process; this
   * test does not publish the original cartridge's proprietary bytes. */
  CHECK(gk130_dma_cpu_bus(&o,work,known,&save,orig+16,GK130_PRG_BYTES,1,0x0c,0,1,1,0xff,1,0,&stall)==GK130_DMA_OK);
  CHECK(source_matches(&o,orig+16+15u*16384u+0x3f00u,251u));
  free(orig);
  Gk83Trace trace={0};CHECK(gk83_trace_original_reset(stock,20000u,&trace)==GK83_UNKNOWN_RAM_GATE);
  CHECK(trace.stop_pc==0x8060u&&trace.first_unknown_ram_read==0x00f5u&&trace.instructions==70u);
 }
 free(prg);
 puts("GK130 PASS: authenticated cartridge 16-bank MMC1 DMA, 32 PRG-RAM pages, CPU RAM mirrors, rollback and original stock reset gate; no first-Spring claim");
 return 0;
}
