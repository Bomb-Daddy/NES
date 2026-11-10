#include "gk148_fresh_bank_trace.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <string.h>
/* Private original-stock fresh Mesen 2026-11-07 event receipt normalized
 * from pre-existing CAPTURE rows 00_boot..14_after_wait, all bank-validated.
 * No copyrighted NES bytes or private checkpoint observations shipped. */
static const uint8_t authentic_sha[32]={
 0x5c,0x05,0xbe,0x5b,0xe2,0xad,0xee,0x96,0xd3,0x1f,0x3d,0x18,0xc5,0xce,0x7c,0x9b,
 0x51,0x0c,0x1e,0xf5,0xc8,0x66,0x70,0xf6,0x25,0x0a,0xc1,0x59,0x4d,0x54,0x00,0xdb};
static uint16_t u16(const uint8_t *b){return (uint16_t)((unsigned)b[0]|((unsigned)b[1]<<8u));}
static uint32_t u32(const uint8_t *b){
 return (uint32_t)b[0]|((uint32_t)b[1]<<8u)|((uint32_t)b[2]<<16u)|((uint32_t)b[3]<<24u);
}
static uint64_t u64(const uint8_t *b){
 uint64_t v=0u;
 for(unsigned i=0;i<8u;i++)v|=(uint64_t)b[i]<<(8u*i);
 return v;
}
Gk148Result gk148_analyze_ticks(const Gk148Tick *rows,size_t n,Gk148FreshTimeline *out){
 if(!rows||!out||n!=GK148_TICKS)return GK148_ARGUMENT;
 Gk148FreshTimeline next={0};
 uint16_t baseline_food=0u,first_gold=0u,first_troops=0u;
 for(size_t i=0;i<n;i++){
  const Gk148Tick *p=&rows[i];
  if(p->stage!=i||p->gold>999u||p->food>999u||p->troops>999u||p->rate>999u||
     p->orders>3u||(!i&&p->frame==0u)||
     (i&&(p->frame<=rows[i-1].frame||p->cycles<=rows[i-1].cycles)))
    return GK148_CHRONOLOGY;
  /* Source was captured on WorkRAM for a FRESH original RESET. The separate
   * archived-restored original Spring is in SaveRAM and never substitutes. */
  if(i<2u){
   if(p->bank!=0u||p->countries!=0u||p->year!=0u||p->gold!=0u||
      p->food!=0u||p->troops!=0u||p->rate!=0u||p->orders!=0u)
    return GK148_CHRONOLOGY;
  }else{
   if(p->bank!=1u||p->countries!=14u||
      (p->year!=1174u&&p->year!=1175u)||
      (i==2u&&(p->year!=1174u||p->rate!=0u||p->orders!=0u))||
      (i&&p->year<rows[i-1u].year))return GK148_CHRONOLOGY;
   if(!next.first_world_setup){
    next.first_world_setup=(uint8_t)i;
    baseline_food=p->food;first_gold=p->gold;first_troops=p->troops;
   }
   if(!next.first_setup_food_change && p->year==1174u&&p->food!=baseline_food)
    next.first_setup_food_change=(uint8_t)i;
   if(!next.first_year_1175 && p->year==1175u)
    next.first_year_1175=(uint8_t)i;
   if(!next.first_rate && p->rate!=0u)
    next.first_rate=(uint8_t)i;
   if(!next.first_spring_finance && p->year==1175u&&
       (p->gold!=first_gold||p->troops!=first_troops))
    next.first_spring_finance=(uint8_t)i;
   if(p->orders!=0u&&p->orders!=3u)return GK148_CHRONOLOGY;
   if(p->orders==3u&&!next.first_three_orders)
    next.first_three_orders=(uint8_t)i;
  }
  if(next.first_three_orders&&p->orders!=3u)return GK148_CHRONOLOGY;
  if(next.first_rate&&p->rate==0u)return GK148_CHRONOLOGY;
 }
 if(!next.first_world_setup||!next.first_setup_food_change||
    !next.first_year_1175||!next.first_rate||!next.first_spring_finance||
    !next.first_three_orders||
    next.first_setup_food_change>=next.first_year_1175||
    next.first_rate!=next.first_year_1175||
    next.first_spring_finance<=next.first_year_1175||
    next.first_three_orders<=next.first_spring_finance||
    rows[n-1].orders!=3u||rows[n-1].year!=1175u||
    rows[n-1].rate==0u)return GK148_CHRONOLOGY;
 next.samples=(uint8_t)n;
 next.final_gold=rows[n-1].gold;
 next.final_food=rows[n-1].food;
 next.final_troops=rows[n-1].troops;
 next.final_rate=rows[n-1].rate;
 next.first_frame=rows[0].frame;
 next.ready_frame=rows[n-1].frame;
 next.first_cycles=rows[0].cycles;
 next.ready_cycles=rows[n-1].cycles;
 /* These are *observed original* events only; the port's independently
  * executed startup and first-Spring legal orders are still missing. */
 next.original_fresh_attested=0u;
 next.first_spring_generated_by_port=0u;
 next.original_gameplay_commands_implemented=0u;
 *out=next;
 return GK148_OK;
}
Gk148Result gk148_verify_private_fresh(const char *path,Gk148FreshTimeline *out){
 if(!path||!*path||!out)return GK148_ARGUMENT;
 FILE *f=fopen(path,"rb");if(!f)return GK148_SOURCE_UNVERIFIED;
 uint8_t buffer[GK148_TICK_BYTES*GK148_TICKS+1u];
 size_t got=fread(buffer,1,sizeof buffer,f);
 int bad=ferror(f),close_bad=fclose(f);
 if(bad||close_bad||got!=sizeof buffer-1u)return GK148_SOURCE_UNVERIFIED;
 uint8_t digest[32];gk1_sha256_bytes(buffer,got,digest);
 if(memcmp(digest,authentic_sha,sizeof digest)!=0)return GK148_SOURCE_UNVERIFIED;
 Gk148Tick rows[GK148_TICKS]={0};
 for(unsigned i=0;i<GK148_TICKS;i++){
  const uint8_t *r=buffer+GK148_TICK_BYTES*i;
  for(unsigned p=26u;p<32u;p++)if(r[p]!=0u)return GK148_SOURCE_UNVERIFIED;
  rows[i].stage=r[0];rows[i].bank=r[1];rows[i].countries=r[2];rows[i].orders=r[3];
  rows[i].year=u16(r+4);rows[i].gold=u16(r+6);rows[i].food=u16(r+8);
  rows[i].troops=u16(r+10);rows[i].rate=u16(r+12);
  rows[i].frame=u32(r+14);rows[i].cycles=u64(r+18);
 }
 Gk148FreshTimeline next={0};
 if(gk148_analyze_ticks(rows,GK148_TICKS,&next)!=GK148_OK)return GK148_CHRONOLOGY;
 next.original_fresh_attested=1u;
 *out=next;
 return GK148_OK;
}
