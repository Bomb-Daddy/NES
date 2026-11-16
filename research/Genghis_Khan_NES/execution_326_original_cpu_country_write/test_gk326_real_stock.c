#include "gk326_cpu_country_store.h"
#include "gk326_sha256.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint8_t stock[GK326_ORIGINAL_ROM_SIZE],ram[GK326_RAM_BYTES];
static int checks;
#define CHECK(x) do { checks++;if(!(x)){fprintf(stderr,"FAIL %u %s\n",__LINE__,#x);exit(1);}}while(0)
int main(int argc,char **argv){
 CHECK(argc==2);
 FILE *f=fopen(argv[1],"rb");CHECK(f!=NULL);
 CHECK(fread(stock,1,sizeof stock,f)==sizeof stock);
 CHECK(fgetc(f)==EOF);CHECK(!fclose(f));
 static const uint8_t abc[3]={'a','b','c'};
 static const uint8_t abc_sha[32]={0xba,0x78,0x16,0xbf,0x8f,0x01,0xcf,0xea,0x41,0x41,0x40,0xde,0x5d,0xae,0x22,0x23,0xb0,0x03,0x61,0xa3,0x96,0x17,0x7a,0x9c,0xb4,0x10,0xff,0x61,0xf2,0x00,0x15,0xad};
 uint8_t digest[32];gk326_sha256(abc,3,digest);CHECK(memcmp(digest,abc_sha,32)==0);
 Gk326CpuSource cpu={0};Gk326StoreReceipt receipt={0};
 cpu.pc=0xc44c;cpu.bank=15;cpu.condition=GK326_FRESH_WORKRAM;cpu.country_count=14;
 cpu.zp[0]=0xcd;cpu.zp[1]=0x6d;cpu.y=0x22;cpu.a=0xaa;
 CHECK(stock[16+15*16384+(0xc44c-0xc000)]==0x91);
 CHECK(gk326_apply_stock_country_store(stock,sizeof stock,&cpu,ram,&receipt)==GK326_OK);
 CHECK(receipt.cpu_address==0x6def && receipt.offset_in_country==0x22);
 CHECK(receipt.original_rom_hash_verified==1 && receipt.first_spring_generated==0);
 CHECK(receipt.player_orders_granted==0);
 /* Source register values above are SYNTHETIC and prove decoding, not an
  * OBSERVED stock write, exact event formula or first-Spring provenance. */
 cpu.pc=0xc2bf;cpu.x=5;cpu.a=129;cpu.y=4;
 Gk326StoreReceipt old=receipt;uint8_t before[GK326_RAM_BYTES];
 memcpy(before,ram,sizeof ram);
 CHECK(stock[16+15*16384+(0xc2bf-0xc000)]==0x9d);
 CHECK(gk326_apply_stock_country_store(stock,sizeof stock,&cpu,ram,&receipt)==GK326_NOT_COUNTRY);
 CHECK(memcmp(before,ram,sizeof ram)==0);
 CHECK(memcmp(&old,&receipt,sizeof receipt)==0);
 /* Changed ROM is NOT the verified original and cannot write. */
 stock[200]^=1;
 CHECK(gk326_apply_stock_country_store(stock,sizeof stock,&cpu,ram,&receipt)==GK326_BAD_ROM);
 CHECK(memcmp(before,ram,sizeof ram)==0);
 printf("PASS %d real-stock identity, decoded bytecode, negative controls\n",checks);
 return 0;
}
