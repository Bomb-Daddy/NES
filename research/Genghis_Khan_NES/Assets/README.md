# Genghis Khan (NES, USA) — graphics asset extract (partial)

Source ROM: Genghis Khan (USA), SHA-1 `98b39b610933d0d82a6ccbcb962a28ba7dc71a85`, mapper 1 (MMC1), 16 PRG banks x 16 KB, 8 KB CHR-RAM. **The ROM is not in this repo.** Evidence: ROM bytes (static decode) + headless FCEUX traces of the stock ROM.

## How to identify an asset
File offset in the .nes file = `0x10 + bank*0x4000 + (cpu_addr - 0x8000)` (bank 15 is fixed at `$C000`; its base is `$C000`).

| Folder | What | File name scheme |
|---|---|---|
| `chr/` | 9 static tile sets, raw 2bpp (`.chr`) + grey PNG sheet | `b<srcBank>_<srcCPU>_to_<ppuDst>_n<tiles>` e.g. `b07_B2F4_to_1000_n89` = bank 7, CPU $B2F4, loaded to PPU $1000, 89 tiles |
| `attr/` | 19 attribute tables, 64 bytes each (`.bin`), destination `$23C0` or `$27C0` | same scheme, `_n4` = 4 units of 16 bytes |
| `portraits/` | 64 officer portraits: bank 11 (30) and bank 12 (34) | `b<bank>_<srcCPU>`; `.chr` raw (480 bytes), `.png` 4x, `_1x.png` 1x |
| `palettes/palettes_observed.csv` | 5 palette sets seen on captured screens (hex NES colour indexes, 4 BG + 4 sprite palettes) | `set_id`, first snapshot seen |
| `manifest/static_upload_sites.csv` | every statically decoded tile/attr load: source bank, CPU address, **file offset**, length, PPU destination, tile count, **calling code bank/address** | one row per file in `chr/` and `attr/` |
| `manifest/portraits.csv` | every portrait: bank, CPU address, file offset, tile count, layout | one row per file in `portraits/` |
| `manifest/uploads_static.json` | raw scan of all 75 tile-load call sites (static and dynamic) | produced by `tools/scan_uploads.py` |
| `tools/` | `scan_uploads.py` (finds load sites), `render_tiles.py` (.chr to PNG) | |
| `lua/` | FCEUX scripts used for headless capture: `probe.lua` (snapshots), `trace.lua`/`trace2.lua` (PPU write and uploader tracing), `nav.lua` (scripted button input via env `GK_SEQ`) | |
| `evidence/` | montages and bank sheets used to confirm what was found | |
| `docs/` | findings write-up | |

## How to use
- Raw tiles: NES 2bpp planar, 16 bytes per tile (8 bytes plane 0, then 8 bytes plane 1). For a Windows port, convert `.chr` to your texture format and apply a palette from `palettes_observed.csv`.
- Portraits: 30 tiles, 5 wide x 6 tall, row-major. Palette (BG palette 0): NES colours `0F/3E, 27, 38, 30` = RGB (0,0,0), (255,154,56), (255,231,162), (255,255,255). Render with `python3 tools/render_tiles.py portraits/b11_8004.chr out.png --scale 4`.
- Every `.chr`/`.bin` is a byte copy of the ROM at the manifest `file_offset`, so you can re-extract from your own ROM to confirm.
- Re-find load sites: `python3 tools/scan_uploads.py gk.nes out.json`.

## Mechanism (how the game loads graphics)
No CHR-ROM. The fixed-bank routine at `$C36A` copies 16-byte tiles from a switchable PRG bank to CHR-RAM through `$2007`. Zero page: `$52` source bank, `$54/$55` source CPU address, `$56/$57` PPU destination, `$58` unit count. The VM syscall `$0A` reaches it; bytecode before `E9 88 F9 0A` is `[count][dst][src][bank][flag]`. Portraits load to PPU `$1E20` (tiles `$E2-$FF`) and the order-screen nametable places them at column 10, row 6. Background tiles use pattern table 1 (`$1000`). Full detail: `docs/Genghis_Khan_NES_Graphics_Loader_Findings_20261103.md`.

## Verified
- All 64 portraits equal ROM bytes at their manifest offsets.
- Bank 12 `$8004` portrait vs emulator screenshot: 0 of 1,920 pixels inconsistent.
- Bank 7 `$B2F4`->`$1000` and `$B884`->`$0000` PPU contents equal ROM bytes in the emulator snapshots.

## Not done
43 run-time-computed load sites (battle/event/terrain graphics), bank 10 faces, screen layouts (nametables), sprite/OAM definitions, per-screen fonts, audio (tables, track IDs, trigger sites).
