#include "gk108_cpu_rmw_bus.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdint.h>
#include <string.h>
typedef struct {
 uint16_t addr[2];uint8_t value[2];uint64_t cycle[2];unsigned count;
} Spy;
static void spy(void *ctx,uint16_t a,uint8_t v,uint64_t c){
 Spy *s=(Spy *)ctx;
 assert(s->count<2u);
 unsigned i=s->count++;
 s->addr[i]=a;s->value[i]=v;s->cycle[i]=c;
}
int main(void){
 Spy s;
 for(unsigned old=0;old<256u;old++)for(unsigned next=0;next<256u;next++){
  memset(&s,0,sizeof s);
  uint64_t last=1u+(uint64_t)old*257u+next;
  assert(gk108_cpu_rmw_pair(last,0x8000u,(uint8_t)old,(uint8_t)next,spy,&s));
  assert(s.count==2u&&s.addr[0]==0x8000u&&s.addr[1]==0x8000u);
  assert(s.cycle[0]==last-1u&&s.cycle[1]==last);
  assert(s.value[0]==old&&s.value[1]==next);
 }
 memset(&s,0x5a,sizeof s);Spy untouched=s;
 assert(!gk108_cpu_rmw_pair(0u,0x2000u,0x55u,0xaau,spy,&s));
 assert(memcmp(&s,&untouched,sizeof s)==0);
 assert(!gk108_cpu_rmw_pair(1u,0x2000u,0x55u,0xaau,0,&s));
 assert(memcmp(&s,&untouched,sizeof s)==0);
 assert(!gk108_cpu_rmw_pair(1u,0x2000u,0x55u,0xaau,spy,0));
 assert(memcmp(&s,&untouched,sizeof s)==0);
 memset(&s,0,sizeof s);
 assert(gk108_cpu_rmw_pair(UINT64_MAX,0x2001u,0x82u,0x02u,spy,&s));
 assert(s.count==2u&&s.cycle[0]==UINT64_MAX-1u&&s.cycle[1]==UINT64_MAX);
 assert(s.addr[0]==0x2001u&&s.addr[1]==0x2001u);
 return 0;
}
