/* Small C11 SHA-256; compatible with Linux and Win32 without external libs.
 * Public-domain algorithm written here for source-byte identity checks. */
#include "gk1_source_identity.h"
#include <stdio.h>
#include <string.h>
static const uint32_t K[64]={
 0x428a2f98u,0x71374491u,0xb5c0fbcfu,0xe9b5dba5u,0x3956c25bu,0x59f111f1u,0x923f82a4u,0xab1c5ed5u,
 0xd807aa98u,0x12835b01u,0x243185beu,0x550c7dc3u,0x72be5d74u,0x80deb1feu,0x9bdc06a7u,0xc19bf174u,
 0xe49b69c1u,0xefbe4786u,0x0fc19dc6u,0x240ca1ccu,0x2de92c6fu,0x4a7484aau,0x5cb0a9dcu,0x76f988dau,
 0x983e5152u,0xa831c66du,0xb00327c8u,0xbf597fc7u,0xc6e00bf3u,0xd5a79147u,0x06ca6351u,0x14292967u,
 0x27b70a85u,0x2e1b2138u,0x4d2c6dfcu,0x53380d13u,0x650a7354u,0x766a0abbu,0x81c2c92eu,0x92722c85u,
 0xa2bfe8a1u,0xa81a664bu,0xc24b8b70u,0xc76c51a3u,0xd192e819u,0xd6990624u,0xf40e3585u,0x106aa070u,
 0x19a4c116u,0x1e376c08u,0x2748774cu,0x34b0bcb5u,0x391c0cb3u,0x4ed8aa4au,0x5b9cca4fu,0x682e6ff3u,
 0x748f82eeu,0x78a5636fu,0x84c87814u,0x8cc70208u,0x90befffau,0xa4506cebu,0xbef9a3f7u,0xc67178f2u
};
static uint32_t rr(uint32_t x,unsigned n){return (x>>n)|(x<<(32u-n));}
static void transform(uint32_t h[8],const uint8_t p[64]){
 uint32_t w[64];
 for(unsigned i=0;i<16;i++)w[i]=((uint32_t)p[i*4]<<24)|((uint32_t)p[i*4+1]<<16)|((uint32_t)p[i*4+2]<<8)|p[i*4+3];
 for(unsigned i=16;i<64;i++){
  uint32_t a=rr(w[i-15],7)^rr(w[i-15],18)^(w[i-15]>>3);
  uint32_t b=rr(w[i-2],17)^rr(w[i-2],19)^(w[i-2]>>10);
  w[i]=w[i-16]+a+w[i-7]+b;
 }
 uint32_t a=h[0],b=h[1],c=h[2],d=h[3],e=h[4],f=h[5],g=h[6],v=h[7];
 for(unsigned i=0;i<64;i++){
  uint32_t S1=rr(e,6)^rr(e,11)^rr(e,25),ch=(e&f)^((~e)&g);
  uint32_t t1=v+S1+ch+K[i]+w[i];
  uint32_t S0=rr(a,2)^rr(a,13)^rr(a,22),maj=(a&b)^(a&c)^(b&c);
  uint32_t t2=S0+maj;
  v=g;g=f;f=e;e=d+t1;d=c;c=b;b=a;a=t1+t2;
 }
 h[0]+=a;h[1]+=b;h[2]+=c;h[3]+=d;h[4]+=e;h[5]+=f;h[6]+=g;h[7]+=v;
}
static void hash_init(uint32_t h[8]){
 const uint32_t initial[8]={0x6a09e667u,0xbb67ae85u,0x3c6ef372u,0xa54ff53au,
  0x510e527fu,0x9b05688cu,0x1f83d9abu,0x5be0cd19u};
 memcpy(h,initial,sizeof initial);
}
/* length validation belongs to callers; expected public original is ~10KiB. */
void gk1_sha256_bytes(const uint8_t *data,size_t size,uint8_t out[32]){
 if(!out || (size&&!data))return;
 uint32_t h[8];hash_init(h);
 size_t i=0;
 while(size-i>=64){transform(h,data+i);i+=64;}
 uint8_t tail[128]={0};size_t n=size-i;
 if(n)memcpy(tail,data+i,n);
 tail[n]=0x80;
 size_t end=n>=56?120:56;
 uint64_t bits=(uint64_t)size*8u;
 for(unsigned j=0;j<8;j++)tail[end+j]=(uint8_t)(bits>>(56u-j*8u));
 transform(h,tail);if(end==120)transform(h,tail+64);
 for(unsigned j=0;j<8;j++)for(unsigned k=0;k<4;k++)out[j*4+k]=(uint8_t)(h[j]>>(24u-k*8u));
}
int gk1_verified_original_csv(const char *path){
 static const uint8_t known[32]={
 0xfa,0x39,0x58,0xf1,0x56,0xf0,0x9d,0x4b,0x4c,0x15,0xbb,0x55,0x9c,0x12,0xaa,0x35,
 0x7f,0xc6,0x17,0xa9,0xb1,0xf9,0x21,0x75,0x05,0xfe,0x07,0xe8,0x80,0x0d,0xe5,0x81};
 if(!path)return 0;
 FILE *f=fopen(path,"rb");if(!f)return 0;
 /* Bound file size to 1 MiB and use only a small fixed chunk allocation.
  * Matches exact original 10,345-byte source. */
 uint8_t bytes[16384];size_t n=fread(bytes,1,sizeof bytes,f);
 int too_big=(n==sizeof bytes),fail=ferror(f);
 int extra=fgetc(f);
 int close_error=fclose(f);
 if(too_big||fail||extra!=EOF||close_error)return 0;
 uint8_t digest[32];gk1_sha256_bytes(bytes,n,digest);
 return memcmp(digest,known,sizeof known)==0;
}
