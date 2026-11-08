#include "gk58_slots.h"
#include <stdio.h>
#include <string.h>
uint8_t gk58_slot_cycle(uint8_t slot,int direction){
 if(slot<1||slot>GK58_SLOT_MAX)return 0;
 if(direction>0)return slot==GK58_SLOT_MAX?1u:(uint8_t)(slot+1u);
 if(direction<0)return slot==1u?(uint8_t)GK58_SLOT_MAX:(uint8_t)(slot-1u);
 return slot;
}
int gk58_slot_path(const char *base,uint8_t slot,char *output,size_t length){
 if(!base||!output||!length||slot<1||slot>GK58_SLOT_MAX)return 0;
 size_t n=0;while(n<1024&&base[n])n++;
 if(n==0||n==1024||n+8>=length)return 0;
 /* Refuse overlapping input/output buffers. Always pass separate storage. */
 if(base==output)return 0;
 int written=slot==1?snprintf(output,length,"%s",base):
  snprintf(output,length,"%s.slot%u",base,(unsigned)slot);
 return written>0&&(size_t)written<length;
}
