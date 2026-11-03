# Genghis Khan (NES, USA) — graphics asset extract (partial)
ROM SHA-1 98b39b610933d0d82a6ccbcb962a28ba7dc71a85. Evidence: ROM bytes + headless FCEUX traces.

## Contents
- chr/        28 static loads (9 CHR tile sets as .chr raw 2bpp + grey PNG sheets). Names: b<bank>_<srcCPU>_to_<ppuDst>_n<tiles>.
- attr/       19 attribute tables (64 bytes each, dest $23C0/$27C0) from fixed loads.
- portraits/  64 officer portraits, bank 11 (30) and bank 12 (34); 30 tiles, 5x6 row-major, 480 bytes; .chr + 1x PNG, palette 0F/27/38/30.
- palettes/   palettes_observed.csv — 5 palette sets seen in captured screens.
- manifest/   static_upload_sites.csv, portraits.csv (bank, CPU address, file offset, PPU destination, calling code address).

## Mechanism
Fixed-bank routine $C36A copies 16-byte tiles from a switchable PRG bank to CHR-RAM. ZP: $52 bank, $54/55 src, $56/57 PPU dst, $58 count. VM syscall $0A; bytecode pattern before E9 88 F9 0A: [count][dst][src][bank][flag].

## Not done
43 run-time-computed load sites (battle/event/terrain graphics), bank 10 faces, screen layouts, sprite definitions, audio. Not verified by three independent passes.

## Verified (one check)
Bank 12 $8004 portrait vs emulator screenshot: 0 of 1,920 pixels inconsistent.
