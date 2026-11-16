#include "gk326_sha256.h"
#include <string.h>
static const uint32_t k[64]={
0x428a2f98u,0x71374491u,0xb5c0fbcfu,0xe9b5dba5u,0x3956c25bu,0x59f111f1u,0x923f82a4u,0xab1c5ed5u,
0xd807aa98u,0x12835b01u,0x243185beu,0x550c7dc3u,0x72be5d74u,0x80deb1feu,0x9bdc06a7u,0xc19bf174u,
0xe49b69c1u,0xefbe4786u,0x0fc19dc6u,0x240ca1ccu,0x2de92c6fu,0x4a7484aau,0x5cb0a9dcu,0x76f988dau,
0x983e5152u,0xa831c66du,0xb00327c8u,0xbf597fc7u,0xc6e00bf3u,0xd5a79147u,0x06ca6351u,0x14292967u,
0x27b70a85u,0x2e1b2138u,0x4d2c6dfcu,0x53380d13u,0x650a7354u,0x766a0abbu,0x81c2c92eu,0x92722c85u,
0xa2bfe8a1u,0xa81a664bu,0xc24b8b70u,0xc76c51a3u,0xd192e819u,0xd6990624u,0xf40e3585u,0x106aa070u,
0x19a4c116u,0x1e376c08u,0x2748774cu,0x34b0bcb5u,0x391c0cb3u,0x4ed8aa4au,0x5b9cca4fu,0x682e6ff3u,
0x748f82eeu,0x78a5636fu,0x84c87814u,0x8cc70208u,0x90befffau,0xa4506cebu,0xbef9a3f7u,0xc67178f2u};
static uint32_t r(uint32_t a,unsigned n){return (a>>n)|(a<<(32-n));}
static uint32_t load32(const uint8_t *p){return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|p[3];}
static void block(uint32_t s[8],const uint8_t data[64]){
 uint32_t w[64];for(unsigned i=0;i<16;i++)w[i]=load32(data+i*4);
 for(unsigned i=16;i<64;i++){
  uint32_t a=w[i-15],b=w[i-2];
  w[i]=(r(b,17)^r(b,19)^(b>>10))+w[i-7]+(r(a,7)^r(a,18)^(a>>3))+w[i-16];
 }
 uint32_t a=s[0],b=s[1],c=s[2],d=s[3],e=s[4],f=s[5],g=s[6],h=s[7];
 for(unsigned i=0;i<64;i++){
  uint32_t ch=(e&f)^(~e&g),maj=(a&b)^(a&c)^(b&c);
  uint32_t t1=h+(r(e,6)^r(e,11)^r(e,25))+ch+k[i]+w[i];
  uint32_t t2=(r(a,2)^r(a,13)^r(a,22))+maj;
  h=g;g=f;f=e;e=d+t1;d=c;c=b;b=a;a=t1+t2;
 }
 s[0]+=a;s[1]+=b;s[2]+=c;s[3]+=d;s[4]+=e;s[5]+=f;s[6]+=g;s[7]+=h;
}
void gk326_sha256(const uint8_t *p,size_t n,uint8_t out[32]){
 uint32_t s[8]={0x6a09e667u,0xbb67ae85u,0x3c6ef372u,0xa54ff53au,
                0x510e527fu,0x9b05688cu,0x1f83d9abu,0x5be0cd19u};
 size_t full=n/64u;
 for(size_t i=0;i<full;i++)block(s,p+i*64u);
 uint8_t tail[128]={0};size_t rem=n%64u;
 if(rem)memcpy(tail,p+full*64u,rem);
 tail[rem]=0x80u;
 uint64_t bits=(uint64_t)n*8u;
 size_t total=rem>=56u?128u:64u;
 for(unsigned i=0;i<8;i++)tail[total-1u-i]=(uint8_t)(bits>>(8u*i));
 block(s,tail);if(total==128u)block(s,tail+64u);
 for(unsigned i=0;i<8;i++)for(unsigned j=0;j<4;j++)out[i*4u+j]=(uint8_t)(s[i]>>(24u-8u*j));
}
