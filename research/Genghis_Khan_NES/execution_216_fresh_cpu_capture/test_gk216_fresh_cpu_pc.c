#include "gk216_fresh_cpu_pc.h"
#include "gk1_app_state.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
int main(int argc,char **argv){
 /* Private ROM and JSON are NEVER bundled with public binary/source/CTest.
  * Baseline CTest exercises atomic invalid-input and forged-source refusal.
  * Run with private ROM and fresh directory for real positive
  * and negative provenance gates. */
 Gk216CpuReport value;
 memset(&value,0xa5,sizeof value);
 Gk216CpuReport before=value;
 assert(gk216_verify_fresh_cpu_pc(NULL,"x",&value)==GK216_BAD_ARGUMENT);
 assert(memcmp(&before,&value,sizeof value)==0);
 assert(gk216_verify_fresh_cpu_pc("","x",&value)==GK216_BAD_ARGUMENT);
 assert(gk216_verify_fresh_cpu_pc("x",NULL,&value)==GK216_BAD_ARGUMENT);
 assert(gk216_verify_fresh_cpu_pc("x","",&value)==GK216_BAD_ARGUMENT);
 assert(gk216_verify_fresh_cpu_pc("x","x",NULL)==GK216_BAD_ARGUMENT);
 assert(gk216_verify_fresh_cpu_pc("/absent/stock.nes","/absent",&value)==GK216_SOURCE_REFUSED);
 assert(memcmp(&before,&value,sizeof value)==0);
 Gk1App app;gk1_app_init(&app);
 Gk1App same=app;
 assert(gk1_app_original_fresh_cpu_pc(&app,"/missing/canonical.csv",
  "/missing/stock.nes","/missing/fresh",&value)==GK216_SOURCE_REFUSED);
 assert(memcmp(&same,&app,sizeof app)==0);
 assert(memcmp(&before,&value,sizeof value)==0);
 if(argc==1){puts("GK216 negative-source CTest PASS; original evidence remains private");return 0;}
 if(argc!=3)return 2;
 Gk216Status status=gk216_verify_fresh_cpu_pc(argv[1],argv[2],&value);
 if(status!=GK216_OK){printf("GK216 private positive validation FAILED status=%d\n",(int)status);return 3;}
 assert(value.count==14&&value.fresh_workram_only==1);
 assert(value.sampled_at_capture_end==1&&value.mapper_bank_not_derived==1);
 assert(value.actual_memory_writes_not_traced==1&&value.first_spring_not_generated==1);
 assert(value.stages[0].frame<value.stages[13].frame);
 assert(value.stages[13].change_mask==32u);
 for(unsigned i=0;i<14;i++){
  assert(value.stages[i].pc>=0x8000);
  assert(value.stages[i].upper_program_window==(value.stages[i].pc>=0xc000));
  if(i)assert(value.stages[i].cycles>value.stages[i-1].cycles);
 }
 /* Regression: absent/corrupted source must not clobber output. */
 Gk216CpuReport accepted=value;
 assert(gk216_verify_fresh_cpu_pc("/absent/stock.nes",argv[2],&value)==GK216_SOURCE_REFUSED);
 assert(memcmp(&accepted,&value,sizeof value)==0);
 assert(gk216_verify_fresh_cpu_pc(argv[1],"/absent/fresh",&value)==GK216_EVIDENCE_REFUSED);
 assert(memcmp(&accepted,&value,sizeof value)==0);
 puts("GK216 authenticated private original Mesen 14 CPU samples and rollback PASS");
 return 0;
}
