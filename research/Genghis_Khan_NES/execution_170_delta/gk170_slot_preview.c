#include "gk170_slot_preview.h"
#include <stdlib.h>
#include <string.h>
static void scrub(void *p,size_t n){volatile unsigned char *q=p;while(n--)*q++=0;}
Gk170SlotStatus gk170_preview_private_slot(const char *path,const char *csv,
 const Gk1App *live,Gk170SlotPreview *out){
 if(!path||!*path||!csv||!*csv||!live||!out)return GK170_SLOT_INVALID;
 Gk170SlotPreview proposed={0};
 Gk1App *copy=malloc(sizeof *copy);
 if(!copy){proposed.status=GK170_SLOT_REFUSED;*out=proposed;return proposed.status;}
 /* Retain the installed and unmodified canonical PRE_SETUP worlds only.
  * The same real source verifier performs private file validation. */
 *copy=*live;
 Gk167Result loaded=gk167_cold_load_private(path,csv,copy);
 if(loaded==GK167_OK){
  const Gk79Checkpoint *cp=&copy->stock_checkpoint;
  proposed.status=GK170_SLOT_VERIFIED;
  proposed.profile=cp->profile;
  proposed.country_count=cp->world.country_count;
  proposed.player_index=cp->player_index;
  proposed.orders_remaining=cp->orders.remaining;
  proposed.orders_ended=cp->orders.ended;
  proposed.year=cp->year;
  proposed.rate=cp->rate;
 }else{
  proposed.status=loaded==GK167_IO?GK170_SLOT_UNAVAILABLE:GK170_SLOT_REFUSED;
 }
 scrub(copy,sizeof *copy);free(copy);
 *out=proposed;return proposed.status;
}
