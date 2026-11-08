#include "gk98_controller_port.h"
#include <stddef.h>
void gk98_pad_reset(Gk98Pad *pad,uint8_t buttons){
 if(!pad)return;
 pad->buttons=buttons;pad->latched=buttons;pad->strobe=0u;
 pad->index=0u;pad->reads=0u;pad->writes=0u;
}
void gk98_pad_write(Gk98Pad *pad,uint8_t value){
 if(!pad)return;
 uint8_t next=(uint8_t)(value&1u);
 if(next||pad->strobe){pad->latched=pad->buttons;pad->index=0u;}
 pad->strobe=next;
 pad->writes++;
}
uint8_t gk98_pad_read(Gk98Pad *pad,uint8_t supplied_high_bits){
 if(!pad)return (uint8_t)(supplied_high_bits&0xfeu);
 uint8_t bit;
 if(pad->strobe)bit=pad->buttons&1u;
 else if(pad->index<8u){bit=(pad->latched>>pad->index)&1u;pad->index++;}
 else bit=1u; /* NES standard controller's eighth+ shift-out pull-up. */
 pad->reads++;
 return (uint8_t)((supplied_high_bits&0xfeu)|bit);
}
