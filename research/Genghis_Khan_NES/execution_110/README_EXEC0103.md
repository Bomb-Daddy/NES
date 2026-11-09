# EXEC0103 — Clock-driven NTSC PPU gate, source research ONLY

Complete native C11 EXEC0102 application retained with all old source, two new C11 source modules and real CTest. Added `gk103_ntsc_ppu.c/h`, `gk103_ppu_schedule.h`, and `test_gk103_ntsc_ppu.c`, wired into the EXISTING original 6502 reset-source interpreter. Rather than the earlier manually injected NMIs, the new optional trace advances a three-dots-per-CPU-cycle PPU clock, models vblank status at scanline 241 dot1 and its clear at 261 dot1, read-to-clear PPUSTATUS, PPUCTRL NMI latch and interrupt entries. Original stock ROM SHA is mandatory at runtime. Real NES PPUMASK rendering, near-vblank read races, unknown peripherals and absent hardware timing information explicitly fail closed. Old trace APIs and native gameplay unaffected.

**Very important:** Reset frame origin, PPU phase 0..2, approximate 29658-cycle PPUCTRL write warmup, and CPU instruction register read cycle are **hypotheses**. The model does NOT reproduce real Mesen, real reset RNG, first playable Spring, graphics, APU, full CPU/PPU master-clock synchronization, sprite status, odd-frame skip during rendering or real controller events. These restrictions are not optional. No gameplay stats are generated from this source model. Pending original stock parity and player control remain blocked.

Source background (not a substitute for Mesen runtime): NESdev PPU frame timing https://www.nesdev.org/wiki/PPU_frame_timing ; NESdev PPU programmer reference https://www.nesdev.org/wiki/PPU_programmer_reference .

Test in Linux: `cmake -S execution_103 -B build103 -DCMAKE_BUILD_TYPE=Release && cmake --build build103 -j4 && ctest --test-dir build103 --output-on-failure`. Optional authenticated original ROM scope `GK1_ORIGINAL_STOCK_ROM=/absolute/private/stock.nes.bin ctest --test-dir build103 -R GK1_103 --output-on-failure`. Never publish stock ROM, original screenshots, Mesen states or licensed audio.

Windows native/NSIS and physical Xbox are NOT RUN. Keep Classic vs Enhanced separate. Not a complete game. Still missing authentic startup/first Spring and 12 commands, Trade transactions, AI, diplomacy, dynasty, aftermath and campaigns.
