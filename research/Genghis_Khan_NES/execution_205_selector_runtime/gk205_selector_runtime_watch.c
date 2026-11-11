#include "gk205_selector_runtime_watch.h"
#include <limits.h>
Gk205Result gk205_note_completed_selector_store(Gk205SelectorWatch *w,
 uint16_t pc,uint16_t addr,uint8_t value,uint8_t known,uint8_t bank,
 uint64_t cycle){
 if(!w||pc<0x8000u||known>1u||bank>15u)return GK205_BAD_INPUT;
 if(addr>=0x2000u||(addr&0x07ffu)!=0x0050u)return GK205_OUTSIDE;
 if(w->completed_selector_writes==UINT32_MAX)return GK205_BAD_INPUT;
 if(w->completed_selector_writes>=GK205_STORED_EVENTS &&
    w->dropped_event_details==UINT32_MAX)return GK205_BAD_INPUT;
 Gk205Store event={pc,addr,value,bank,known,cycle};
 if(w->completed_selector_writes<GK205_STORED_EVENTS)
  w->first[w->completed_selector_writes]=event;
 else w->dropped_event_details++;
 w->completed_selector_writes++;
 w->last=event;
 return GK205_RECORDED;
}
Gk205Result gk205_note_brk_gate(Gk205SelectorWatch *w,uint16_t pc,
 uint8_t selector_known,uint8_t selector,uint8_t bank_known,uint8_t bank){
 if(!w||pc<0x8000u||selector_known>1u||bank_known>1u||bank>15u||
    w->brk_gates==UINT32_MAX)return GK205_BAD_INPUT;
 ++w->brk_gates;
 w->brk_pc=pc;
 w->brk_selector_known=selector_known;
 w->brk_selector_value=selector_known?selector:0u;
 w->brk_bank_known=bank_known;
 w->brk_bank=bank_known?bank:0u;
 return GK205_BRK_ATTEMPT;
}
