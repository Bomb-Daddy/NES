#include "gk208_archived_rng.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define CHECK(cond) do {if(!(cond)){fprintf(stderr,"GK208 FAIL line %d: %s\n", __LINE__,#cond);return 2;}checks++;}while(0)
static unsigned checks;
int main(int argc,char **argv){
 Gk208ArchiveProof value,original;
 memset(&value,0xa5,sizeof value);memcpy(&original,&value,sizeof original);
 CHECK(gk208_archived_source_rng(NULL,"mss",GK77_ARCHIVED_MONGOL_SAVERAM,&value)==GK208_ARGUMENT);
 CHECK(gk208_archived_source_rng("rom",NULL,GK77_ARCHIVED_MONGOL_SAVERAM,&value)==GK208_ARGUMENT);
 CHECK(gk208_archived_source_rng("rom","mss",GK77_FRESH_MONGOL_WORKRAM,&value)==GK208_ARGUMENT);
 CHECK(gk208_archived_source_rng("rom","mss",GK77_ARCHIVED_MONGOL_SAVERAM,NULL)==GK208_ARGUMENT);
 CHECK(!memcmp(&value,&original,sizeof value));
 CHECK(gk208_archived_source_rng("missing.rom","missing.mss",GK77_ARCHIVED_MONGOL_SAVERAM,&value)==GK208_ARCHIVE_IDENTITY);
 CHECK(!memcmp(&value,&original,sizeof value));
 if(argc==4){
  for(unsigned i=0;i<2;i++){
   Gk77Profile profile=i?GK77_ARCHIVED_WORLD_SAVERAM:GK77_ARCHIVED_MONGOL_SAVERAM;
   Gk208Status status=gk208_archived_source_rng(argv[1],argv[2+i],profile,&value);
   CHECK(status==GK208_OK);
   CHECK(value.profile==profile);
   CHECK(value.country_count==(i?27u:14u));
   CHECK(value.archived_seed48>0u);
   CHECK(value.archived_pc>=0x8000u);
   CHECK(value.archived_cpu_cycles>0u);
   CHECK(value.hypothetical_next_seed48!=value.archived_seed48);
   /* Checks source-authentication of the exact original NES and the
    * exact profile, not 'source-generated initial Spring'. */
   memcpy(&original,&value,sizeof original);
   CHECK(gk208_archived_source_rng("/not-the-original-stock.nes",argv[2+i],profile,&value)==GK208_SOURCE);
   CHECK(!memcmp(&value,&original,sizeof value));
   CHECK(gk208_archived_source_rng(argv[1],argv[2+i],i?GK77_ARCHIVED_MONGOL_SAVERAM:GK77_ARCHIVED_WORLD_SAVERAM,&value)==GK208_ARCHIVE_IDENTITY);
   CHECK(!memcmp(&value,&original,sizeof value));
   /* Hostile QA: exact country digest is unchanged by modifying only the
    * compressed screenshot. A clone is NOT an authorized archive. */
   const char *forged="GK208_mutated_archive.mss";
   FILE *in=fopen(argv[2+i],"rb"),*fp=fopen(forged,"wb");
   CHECK(in!=NULL&&fp!=NULL);
   unsigned char ch;size_t copied=0;int clone_ok=1;
   while(fread(&ch,1,1,in)==1){if(copied==46u)ch^=1u;
     if(fwrite(&ch,1,1,fp)!=1){clone_ok=0;}
     copied++;}
   CHECK(clone_ok && copied>5000u);
   CHECK(!fclose(in)&&!fclose(fp));
   CHECK(gk208_archived_source_rng(argv[1],forged,profile,&value)==GK208_ARCHIVE_IDENTITY);
   CHECK(!memcmp(&value,&original,sizeof value));
   CHECK(remove(forged)==0);
   printf("GK208 private verified archived %s country_count=%u seed-present=YES next-source-parity=PASS\n",i?"world1206":"mongol1175",value.country_count);
  }
 }else CHECK(argc==1);
 printf("GK208 %u assertions PASS; archived original snapshots are not independent Spring generation.\n",checks);
 return 0;
}
