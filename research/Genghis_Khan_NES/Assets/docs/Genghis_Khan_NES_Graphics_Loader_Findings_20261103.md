# Genghis Khan (NES, USA) — graphics loader findings (in progress, 2026-11-03)

Status: partial. Evidence class: ROM bytes (static) + headless FCEUX traces of the stock ROM (cloud container).

## How CHR-RAM is filled
- No CHR-ROM; the game copies tiles from switchable PRG banks into CHR-RAM at runtime.
- Fixed-bank uploader writes $2007 from PC $C36A. Parameters in zero page: $52 = source PRG bank, $54/$55 = source CPU address (lo/hi), $56/$57 = PPU destination (lo/hi), $58 = count of 16-byte units. The same routine also writes 64-byte attribute tables to $23C0/$27C0 (count 4).
- It is reached from the bytecode VM by system call $0A. Call-site byte pattern in bytecode, immediately before `E9 88 F9 0A`: `[count][dst][src][bank][flag]`, where count = `8D nn` or `6k`, dst/src = `8E lo hi`, bank = `6k` (k = bank number), flag = `61`. Dynamic sites compute arguments on the VM stack.
- 75 call sites found by scanning all 16 banks; 32 have literal arguments (28 unique). The other 43 are computed and are still to be decoded.
- Verified: PPU contents equal the ROM bytes for bank 7 $B2F4 -> $1000 (89 tiles) and bank 7 $B884 -> $0000 (62 tiles) in 16 of 17 and 17 of 17 snapshots.
- Pattern-table tiles 62 and up in table 0 ($03E0+) at boot are not uploaded from ROM; they are uninitialised CHR-RAM, not assets.
- BG tiles use pattern table 1 ($1000); sprites/other use table 0.

## Portraits
- 30-tile blocks, 5 wide x 6 tall, row-major, 480 bytes each, in PRG bank 11 (30 portraits) and bank 12 (34 portraits), first block at bank offset +4 (CPU $8004), stride 480. Bank 10 holds more faces at a different layout (not decoded).
- Loaded to PPU $1E20 (tiles $E2-$FF); the nametable places them row-major at tile column 10, row 6 on the order screen.
- Verified: bank 12 $8004 (Temujin) decoded against the emulator screenshot — 0 of 1,920 pixels inconsistent.
- Palette on the order screen (BG palette 0): $0F/$3E,$27,$38,$30 (black, orange, pale yellow, white).

## Observed scripted navigation (FCEUX Lua, 5-frame presses, 150 frames between)
start x3, A x3, B, A x6, B x3 reaches the Mongol order menu (1175 Spring). Strategic menus trigger no further tile loads.

## Open
Bank 10 portraits; other dynamic upload sites (battle/event/terrain); screen layouts; sprite definitions; per-screen fonts; audio engine data; three verification loops on the whole package.

## Where the files are
GitHub `Bomb-Daddy/NES`, branch `genghis-khan-assets`, folder `research/Genghis_Khan_NES/Assets/` (see its README.md).
