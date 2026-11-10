#include "gk145_fresh_progress.h"
Gk145Status gk145_trace_fresh_progress(const Gk145FreshObservation *rows,
 size_t count,uint16_t scenario_year,uint16_t expected_countries,
 Gk145FreshProgress *out){
 if(!rows||!out||count<2u||count>255u||
    (scenario_year!=1175u&&scenario_year!=1206u)||
    (expected_countries!=14u&&expected_countries!=27u)||
    (scenario_year==1175u)!=(expected_countries==14u))return GK145_ARGUMENT;
 Gk145FreshProgress r={0};r.samples=(uint8_t)count;
 uint16_t first_gold=0u;int seen_countries=0,seen_year=0,seen_rate=0,seen_gold=0,seen_orders=0;
 for(size_t i=0;i<count;i++){
  const Gk145FreshObservation *p=&rows[i];
  if((i>0&&(p->frame<=rows[i-1u].frame||p->cpu_cycles<=rows[i-1u].cpu_cycles))||
     p->country_count>expected_countries||
     (p->country_count!=0u&&p->country_count!=expected_countries)||
     p->year>scenario_year||
     (p->year!=0u&&p->year!=scenario_year&&p->year!=(uint16_t)(scenario_year-1u))||
     p->gold>999u||p->rate>999u||p->orders>3u)
    return GK145_INVALID_CHRONOLOGY;
  if(i&&p->country_count<rows[i-1u].country_count)
    return GK145_AMBIGUOUS_PROGRESS;
  if(i&&p->year&&rows[i-1u].year&&p->year<rows[i-1u].year)
    return GK145_AMBIGUOUS_PROGRESS;
  if(p->country_count==expected_countries&&!seen_countries){seen_countries=1;r.first_world_setup=(uint8_t)(i+1u);first_gold=p->gold;}
  if(p->year==scenario_year&&!seen_year){seen_year=1;r.first_spring_year=(uint8_t)(i+1u);}
  if(p->rate>0u&&!seen_rate){seen_rate=1;r.first_nonzero_rate=(uint8_t)(i+1u);}
  if(seen_countries&&p->gold!=first_gold&&!seen_gold){seen_gold=1;r.first_gold_change=(uint8_t)(i+1u);}
  if(p->orders>0u&&(p->orders!=3u||p->year!=scenario_year||
     p->country_count!=expected_countries||p->rate==0u))
    return GK145_AMBIGUOUS_PROGRESS;
  if(seen_rate&&p->rate==0u)return GK145_AMBIGUOUS_PROGRESS;
  if(p->orders==3u&&!seen_orders){seen_orders=1;r.first_three_orders=(uint8_t)(i+1u);}
  if(seen_orders&&p->orders!=3u)return GK145_AMBIGUOUS_PROGRESS;
  if(!r.first_full_spring&&p->year==scenario_year&&p->country_count==expected_countries&&p->rate>0u&&p->orders==3u){
   r.first_full_spring=(uint8_t)(i+1u);
  }
 }
 if(!seen_countries||!seen_year||!seen_rate||!seen_orders||!r.first_full_spring)
  return GK145_AMBIGUOUS_PROGRESS;
 /* Discrete progress does not establish causation between inputs/events. */
 r.temporal_separation_observed=(uint8_t)(r.first_spring_year!=r.first_three_orders ||
      r.first_nonzero_rate!=r.first_three_orders ||
      (r.first_gold_change&&r.first_gold_change!=r.first_three_orders));
 *out=r;return GK145_OK;
}
