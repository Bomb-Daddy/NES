#include "gk82_stock_cartridge.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void expect_unchanged(Gk82RomBootstrap a,const Gk82RomBootstrap *b){
 assert(memcmp(&a,b,sizeof a)==0);
}
int main(int argc,char **argv){
 char path[2300]="UNMODIFIED";
 assert(gk82_rom_path("/tmp/user",path,sizeof path));
 assert(strcmp(path,"/tmp/user/GK1NativeResearch/stock_genghis_khan_usa.nes")==0);
 assert(gk82_rom_path("C:\\Users\\Test",path,sizeof path));
 assert(strcmp(path,"C:\\Users\\Test\\GK1NativeResearch\\stock_genghis_khan_usa.nes")==0);
 strcpy(path,"UNMODIFIED");
 assert(!gk82_rom_path("../relative",path,sizeof path));
 assert(!gk82_rom_path("/tmp/../escape",path,sizeof path));
 assert(!gk82_rom_path("/tmp/user",path,10));
 assert(strcmp(path,"UNMODIFIED")==0);
 Gk82RomBootstrap info;
 memset(&info,0xa5,sizeof info);Gk82RomBootstrap before=info;
 assert(gk82_probe_stock_rom("/missing/owner-stock.nes",&info)==GK82_ROM_IO);
 expect_unchanged(before,&info);
 assert(gk82_probe_stock_rom(NULL,&info)==GK82_ROM_INVALID);
 expect_unchanged(before,&info);
 const char *bad="test_gk82_bad_rom.bin";
 FILE *f=fopen(bad,"wb");assert(f);
 assert(fwrite("NES",1,3,f)==3);assert(!fclose(f));
 assert(gk82_probe_stock_rom(bad,&info)==GK82_ROM_LENGTH);
 expect_unchanged(before,&info);assert(remove(bad)==0);
 if(argc==2){
  /* Owner supplies original 262160B NES source at test time only.
   * It is NEVER included in public git source or Debian package. */
  assert(gk82_probe_stock_rom(argv[1],&info)==GK82_ROM_OK);
  assert(info.mapper==1&&info.prg_banks==16&&info.chr_banks==0);
  assert(info.reset_vector==0xc000u&&info.nmi_vector==0xc0d9u);
  assert(info.irq_vector==0xc14bu&&info.boot_instruction_count==5);
  f=fopen(argv[1],"rb");assert(f);
  unsigned char *bytes=malloc(262160);assert(bytes);
  assert(fread(bytes,1,262160,f)==262160);assert(!fclose(f));
  for(int edit=0;edit<4;edit++){
    size_t at=edit==0?0:edit==1?16:edit==2?262159:262154;
    unsigned char old=bytes[at];bytes[at]^=0x01;
    f=fopen(bad,"wb");assert(f);
    assert(fwrite(bytes,1,262160,f)==262160);assert(!fclose(f));
    Gk82RomBootstrap p=info;
    Gk82RomStatus code=gk82_probe_stock_rom(bad,&info);
    assert(code==(edit==0?GK82_ROM_HEADER:GK82_ROM_SHA));
    expect_unchanged(p,&info);
    assert(remove(bad)==0);bytes[at]=old;
  }
  free(bytes);
 }
 puts("EXEC0082 PASS: exact owner iNES MMC1 reset static bootstrap, private SHA, deliberate corruption/size/path rollback");
 return 0;
}
