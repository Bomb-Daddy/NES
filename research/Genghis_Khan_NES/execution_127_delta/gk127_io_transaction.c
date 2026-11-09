#include "gk127_io_transaction.h"
int gk127_preflight_rmw(uint16_t address,uint8_t old_value,
                        uint8_t new_value,int ppu_timing_enabled,
                        Gk127RmwCheck *out){
 if(!out)return 0;
 Gk127RmwCheck next={GK126_IO_KNOWN_OR_DEFERRED,0u,0u};
 next.barrier=gk126_io_write_barrier(address,old_value,ppu_timing_enabled);
 if(next.barrier!=GK126_IO_KNOWN_OR_DEFERRED){next.rejected_value=old_value;*out=next;return 0;}
 next.barrier=gk126_io_write_barrier(address,new_value,ppu_timing_enabled);
 if(next.barrier!=GK126_IO_KNOWN_OR_DEFERRED){next.rejected_value=new_value;next.rejected_write_index=1u;*out=next;return 0;}
 *out=next;
 return 1;
}
