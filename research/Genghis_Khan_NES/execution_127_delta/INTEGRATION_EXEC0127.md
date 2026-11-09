# GK1 EXEC0127 — reproducible source integration

**Status:** Linux-tested research build 1.27.0-dev1. Complete historical C11 source ZIP is a separate verified artifact; this public GitHub directory is a portable delta, not the whole game. No licensed original ROM/SRAM/live RAM/image/audio is contained here.

## Source ancestry

Start with EXEC0126 complete historical native C11 app, not EXEC0125 or a standalone interpreter. Preserve all country templates, controller flows, scenario/profile code, previous test files and three-slot save compatibility. The new implementation was applied to this inherited game at `execution_127`, which keeps every prior archive entry.

## Integration changes

1. Copy `gk127_io_transaction.[ch]` and `test_gk127_io_transaction.c` beside the inherited `gk126_io_barrier.[ch]`. Add `#include "gk127_io_transaction.h"` to `gk83_reset_trace.c`.

2. In `rmw_write_pair`, **before** `gk108_cpu_rmw_pair` can issue the OLD or NEW writes, preflight both:

```c
Gk127RmwCheck check;
if(!gk127_preflight_rmw(address,old_value,new_value,
                         m->ppu_schedule_enabled,&check)){
 m->ppu_schedule_gate=1;
 if(!m->result.first_unmodelled_io_write){
  m->result.first_unmodelled_io_write=address;
  m->result.first_unmodelled_io_value=check.rejected_value;
  m->result.first_unmodelled_io_reason=(uint8_t)check.barrier;
 }
 m->result.unmodelled_io_write_attempts++;
 m->result.unmodelled_rmw_preflight_rejections++;
 return;
}
```

3. Add two fields to the `Gk83Trace` result structure in `gk83_reset_trace.h`:

```c
uint32_t unmodelled_rmw_preflight_rejections;
uint8_t unmodelled_instruction_cpu_rolled_back;
```

4. At the beginning of `one(Machine *m)`, save the current `pc,a,x,y,sp,p`, before decoding the opcode. At the `finish_one` label, replace the prior early return on `ppu_schedule_gate` with:

```c
if(s==GK83_INVALID && m->ppu_schedule_gate){
 m->pc=start_pc;
 m->a=before_a; m->x=before_x; m->y=before_y;
 m->sp=before_sp; m->p=before_p;
 m->result.unmodelled_instruction_cpu_rolled_back=1u;
 return GK83_HARDWARE_GATE;
}
```

Here, `start_pc` is the existing instruction-start PC and `before_a`, etc. are copied register values captured before the instruction. Diagnostics of the refused device write must remain observable. This does **not** claim peripheral side effects are modeled or that genuine Mesen startup finished.

5. Add `gk127_io_transaction.c` to the existing native `gk1_app_state` CMake library, then:

```cmake
add_executable(test_gk127_io_transaction test_gk127_io_transaction.c)
target_link_libraries(test_gk127_io_transaction PRIVATE gk1_app_state)
add_test(NAME GK1_127_BusAtomic_RMW_Preflight_And_StockRollback COMMAND test_gk127_io_transaction)
```

6. Update native shell and .desktop/CPack version labels to `1.27.0-dev1`, retaining clear 'research / not full game' wording.

## Results and limits

Release, Debug and independent clean historical-source rebuild each passed **85/85 CTest**, using authenticated original cartridge solely as an optional private test input. Focused ASan/UBSan RMW preflight passed **2,162,688/2,162,688** checks; **full sanitizer suite not certified**. Linux 1280×720 graphics passed 120 frames each for compiled and extracted Debian executable. A private original cartridge source-gated, *hypothetical* PPU origin path now refuses `STA $2001` at PC `$C5E9` with CPU rollback; this is NOT a new original-stock Mesen runtime observation. Default unknown-RAM reset still gates on `$00F5` at PC `$8060` after 70 instructions. Authentic first Spring, real three orders, full campaigns and physical Windows/Xbox tests remain unfinished.

Numbered Collaboration Drive handoff: https://docs.google.com/document/d/1HuibU9iWQwRk76wzNJAWhImGvPnjHCKP86NVI-Fw3hQ/edit

Do not treat this delta alone as a release or upload original ROM or licensed assets.
