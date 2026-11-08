#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv){
 if(argc!=2)return 2;
 unsigned tests=0;
 #define C(x) do{tests++;if(!(x)){fprintf(stderr,"HASH TEST FAIL %s:%d: %s\n",__FILE__,__LINE__,#x);return 1;}}while(0)
 uint8_t d[32];gk1_sha256_bytes((const uint8_t *)"",0,d);
 const uint8_t empty[32]={0xe3,0xb0,0xc4,0x42,0x98,0xfc,0x1c,0x14,0x9a,0xfb,0xf4,0xc8,0x99,0x6f,0xb9,0x24,
  0x27,0xae,0x41,0xe4,0x64,0x9b,0x93,0x4c,0xa4,0x95,0x99,0x1b,0x78,0x52,0xb8,0x55};
 C(memcmp(d,empty,32)==0);
 gk1_sha256_bytes((const uint8_t *)"abc",3,d);
 const uint8_t abc[32]={0xba,0x78,0x16,0xbf,0x8f,0x01,0xcf,0xea,0x41,0x41,0x40,0xde,0x5d,0xae,0x22,0x23,
  0xb0,0x03,0x61,0xa3,0x96,0x17,0x7a,0x9c,0xb4,0x10,0xff,0x61,0xf2,0x00,0x15,0xad};
 C(memcmp(d,abc,32)==0);
 C(gk1_verified_original_csv(argv[1]));
 C(!gk1_verified_original_csv("/nonexistent/gk1.csv"));
 C(!gk1_verified_original_csv(NULL));
 FILE *in=fopen(argv[1],"rb");C(in!=NULL);
 unsigned char buf[16000];size_t n=fread(buf,1,sizeof buf,in);C(!ferror(in));C(!fclose(in));
 C(n==10345);C(n<sizeof buf);buf[n]=0;
 /* Change an ordinary numeric value without changing column widths or row count.
  * Loader arithmetic could still accept it, but original source identity must not. */
 unsigned char *hit=(unsigned char*)strstr((char*)buf,",425,");C(hit!=NULL);
 hit[1]='4';hit[2]='2';hit[3]='6';
 FILE *out=tmpfile();C(out!=NULL);
 C(fwrite(buf,1,n,out)==n);C(fflush(out)==0);
 /* The app uses filename identity, so write a uniquely named path for rejection. */
 C(fclose(out)==0);
 char path[256];snprintf(path,sizeof path,"gk1_exec0050_mutated_negative.csv");
 out=fopen(path,"wb");C(out!=NULL);C(fwrite(buf,1,n,out)==n);C(fclose(out)==0);
 C(!gk1_verified_original_csv(path));C(remove(path)==0);
 printf("EXEC0050 portable SHA256/source identity checks %u/%u PASS\n",tests,tests);
 return 0;
}
