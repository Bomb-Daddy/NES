#include "gk325_opening_sequence.h"
#include <limits.h>
#include <string.h>
static int valid_sample(const Gk325Sample *p){
 return p->gold<=999u && p->food<=999u && p->troops<=999u &&
        p->rate<=999u && p->source_orders<=3 &&
        p->observed_event_dialog<=1 && p->observed_command_menu<=1;
}
Gk325Status gk325_audit_trace(const Gk325Trace *in,Gk325Audit *out){
 if(!in||!out||!in->samples||!in->sample_count||in->sample_count>1024)
  return GK325_BAD_INPUT;
 if((in->world!=GK325_MONGOL&&in->world!=GK325_WORLD)||
    (in->country_count!=(in->world==GK325_MONGOL?14:27)))
  return GK325_WRONG_SOURCE;
 if((in->condition==GK325_FRESH_RESET&&in->bank!=GK325_WORK_RAM)||
    (in->condition==GK325_ARCHIVED_RESTORED&&in->bank!=GK325_SAVE_RAM)||
    (in->condition!=GK325_FRESH_RESET&&in->condition!=GK325_ARCHIVED_RESTORED))
  return GK325_WRONG_SOURCE;
 const Gk325Sample *s=in->samples;
 for(size_t i=0;i<in->sample_count;i++){
  if(!valid_sample(s+i))return GK325_BAD_INPUT;
  if(i && s[i].frame<=s[i-1].frame)return GK325_INCONSISTENT;
  if(s[i].source_orders && !s[i].observed_command_menu)return GK325_INCONSISTENT;
 }
 Gk325Audit v={0};
 const Gk325Sample *last=s+in->sample_count-1;
 if(in->condition==GK325_ARCHIVED_RESTORED){
  if(last->year!=(in->world==GK325_MONGOL?1175:1206)||
     !last->observed_command_menu||last->source_orders!=3)
   return GK325_MISSING_PHASE;
  for(size_t i=0;i+1<in->sample_count;i++)
   if(s[i].source_orders!=3||!s[i].observed_command_menu)
    return GK325_INCONSISTENT;
  v.command_frame=last->frame;
 }else{
  if(in->world!=GK325_MONGOL) /* no authenticated fresh World trace */
   return GK325_WRONG_SOURCE;
  if(in->sample_count<5||s[0].year!=1174||s[0].source_orders||
     s[0].observed_command_menu||last->year!=1175||
     last->source_orders!=3||!last->observed_command_menu)
   return GK325_MISSING_PHASE;
  /* First setup-stage food/stat change while still winter. A distinct
   * later rate/year transition and finance/personnel phase are required.
   * We do not assert food doubles or any absolute financial constant. */
  size_t setup=SIZE_MAX,calendar=SIZE_MAX,finance=SIZE_MAX,event=SIZE_MAX;
  for(size_t i=1;i<in->sample_count;i++){
   if(s[i].year!=1174&&s[i].year!=1175)return GK325_INCONSISTENT;
   if(s[i].source_orders && i+1!=in->sample_count)return GK325_INCONSISTENT;
   if(setup==SIZE_MAX&&s[i].year==1174&&s[i].food!=s[0].food)
    setup=i;
   if(setup!=SIZE_MAX&&i>setup&&calendar==SIZE_MAX&&
      s[i].year==1175&&s[i].rate>0)
    calendar=i;
   if(calendar!=SIZE_MAX&&i>calendar&&finance==SIZE_MAX&&
      (s[i].gold!=s[calendar].gold||s[i].food!=s[calendar].food||
       s[i].troops!=s[calendar].troops))
    finance=i;
   if(finance!=SIZE_MAX&&i>=finance&&event==SIZE_MAX&&
      s[i].observed_event_dialog)event=i;
  }
  if(setup==SIZE_MAX||calendar==SIZE_MAX||finance==SIZE_MAX||
     event==SIZE_MAX||event>=in->sample_count-1||
     last->frame<=s[event].frame)return GK325_MISSING_PHASE;
  /* The event must have been acknowledged before controller order entry. */
  for(size_t i=0;i+1<in->sample_count;i++)
   if(s[i].observed_command_menu||s[i].source_orders)return GK325_INCONSISTENT;
  v.setup_frame=s[setup].frame;
  v.calendar_frame=s[calendar].frame;
  v.finance_frame=s[finance].frame;
  v.event_frame=s[event].frame;
  v.command_frame=last->frame;
 }
 v.source_year=last->year;v.source_gold=last->gold;v.source_food=last->food;
 v.source_troops=last->troops;v.source_rate=last->rate;
 v.source_orders=last->source_orders;
 v.observed_stock=1; /* only the supplied source trace, not port run */
 *out=v;return GK325_OK;
}
