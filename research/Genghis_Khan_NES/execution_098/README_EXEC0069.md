# GK1 EXEC0069 – exact approved-modified NES WorkRAM battle-grid viewer

This extends **the same existing standalone native C11 Genghis Khan I game**, based byte-for-byte on published EXEC0068's inherited source files, rather than creating another executable/game. It preserves both 14- and 27-country PRE_SETUP worlds, Classic/Enhanced profile split, setup wizard, market research, labor planning, Divide, Enhanced Demobilize, controlled Enhanced Rest confirmation, and three exclusive protected pre-setup cold-save slots.

## New controller-executable feature

The **original NES source-captured** 16×10 tactical map from `ZL-GK1-MESEN-GAP-0001` can now be viewed in the native 1280×720 game by supplying the **private** 8192-byte `live_map_capture/battle.workram` as an external local file, through `GK1_MESEN_WORKRAM_FILE=/path/to/battle.workram`. Select title item **Previous Battle Research / Snapshot Viewer**, then use D-pad to move the inspected cursor, **A** to toggle original hexadecimal byte display versus screenshot-grounded raw2/3/4 class abbreviations, and **B** to exit. Xbox X/Y cannot mutate or save the map.

The module checks the **entire 8192 bytes** against SHA256 `dce6b9fb05d47df2b6222789ae2a863d7e789950e16c94622e700f5dd4ff07b1` and only accepts that exact already-observed modified NES capture. This SHA authenticates the *previously archived RAM file*, **not a ROM**, an original-stock run, an arbitrary map, or new source gameplay. The 160-byte map is read at `nesWorkRam` offset `0x015c`. Raw2=plains, raw3=forest and raw4=mountain are narrow source screenshot correspondences. Bytes5/6/7, 0x82/0x83/0x84 and any possible raw0/1 remain **unclassified**, not water; high-bit metadata remains UNKNOWN. No tile bytes or copyrighted ROM data are embedded in source or the Debian application. Supplied original RAM stays outside public distribution.

**Exact local reproduction:**

```bash
cmake -S execution_069 -B build69 -DCMAKE_BUILD_TYPE=Release
cmake --build build69 -j4
ctest --test-dir build69 --output-on-failure
build69/test_gk69_mesen_ram_map /private/battle.workram
GK1_MESEN_WORKRAM_FILE=/private/battle.workram build69/gk1_native_shell --mesen-map-pad-check /private/battle.workram
GK1_MESEN_WORKRAM_FILE=/private/battle.workram xvfb-run -a build69/gk1_native_shell --smoke-mesen-grid 100
```

Invalid/missing/edited/short/long RAM is refused without modifying the map. The game remains **PRE_SETUP**, not original first playable Spring; this is a read-only research view, not tactical movement gameplay, terrain costs, stock ROM parity, natural water discovery, marriage age resolution, new NES emulation, Windows acceptance, or an Xbox hardware test. Full original game scope remains mandatory and OPEN.

## Source lineage and separation

EXEC0068 full source Drive SHA256 `b4fb71e87c4f049765c8a088c45ecca3b30e158e8e83f25868234b36ffbbca23`, 1,315 original entries, verified against the public source-only 1,312-entry ZIP (all common bytes identical; three inherited native-port QA screenshots restored from full ZIP). The original 41-country dataset's SHA256 remains `fa3958f156f09d4b4c15bb559c12aa357fc617a9b1f9217505fe07e8800de581`. The private source-only RAM and screenshots were never copied into the public game source tree. C11 test module, native shell and CMake are versioned 0.69.0-dev1. Exact source, tests, command outputs and negative evidence are in the full archive.
