#include "gk101_mmc1_cycles.h"
Gk101Decision gk101_mmc1_write(Gk101Mmc1Filter *f,
                                uint64_t cycle,uint8_t value){
 if(!f)return GK101_NONMONOTONIC;
 if(f->has_previous && cycle<=f->previous_cycle)return GK101_NONMONOTONIC;
 int consecutive=f->has_previous && cycle-f->previous_cycle==1u;
 f->previous_cycle=cycle;f->has_previous=1u;
 /* An MMC1 reset write bypasses the consecutive-cycle bit0 inhibit. */
 if(value&0x80u){f->accepted++;f->resets++;return GK101_RESET;}
 if(consecutive){f->ignored++;return GK101_IGNORED;}
 f->accepted++;return GK101_ACCEPT;
}
