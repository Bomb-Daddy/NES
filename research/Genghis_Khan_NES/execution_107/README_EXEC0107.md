# EXEC0107 — PPU write-only reads and mirror-fail-closed source research

Built on the COMPLETE inherited EXEC0106 native C11 application. Version
1.07.0-dev1, not a separate emulator or first Spring shortcut.

New `gk107_ppu_ports.c/h` recognizes exactly $2000/$2001/$2003/$2005/$2006
and their 8-byte mirrors as reads that yield the PPU I/O bus latch value,
**not** the CPU open bus. It rejects $2002 status, $2004 OAMDATA, $2007
PPUDATA as distinctly side-effectful ports; accepts a known last full CPU
write only within a deliberately conservative 3000 CPU-cycle interval,
refusing potentially analog-decayed latch values. Unknown bus, invalid port,
clock reversal and old reads do not mutate output. The bounded original-stock
trace has an optional gated implementation integrated into its pre-existing
PPU source mode. No synthetic Spring stats or universal reset phase.

Provenance: NESdev `Open_bus_behavior` and `PPU_programmer_reference`.
These describe behavior, not original-Genghis/Mesen reproduction.

Source test: `test_gk107_ppu_ports`, all inherited CTest in Linux Release,
Debug, sanitizer. Optional authenticated private stock ROM from external
GK1_ORIGINAL_STOCK_ROM path, never included in source or public archives.

UNKNOWN/NOT RUN: exact Mesen PPU bus decay, emulator frame parity, original
setup/RNG, first playable Spring, autonomous AI, trade and full campaign,
Windows/NSIS/physical Xbox controller. Do not declare complete release.
