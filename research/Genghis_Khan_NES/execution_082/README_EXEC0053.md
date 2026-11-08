# Genghis Khan I — EXEC0053 original-manual-constrained setup wizard

**Standalone native C11 Genghis Khan I Windows-port source**, successor to EXEC0052. This is not the separate ROTK II or original KOEI-inspired game. Inherit the pre-setup country factory and controller/Divide/replay/snapshot components without overwriting the originals.

## Verified historical source and deliberate gate

The original English NES manual (DigitPress `Genghis Khan.pdf`, printed page 9) describes a 1-player Mongol Conquest; 1–4-player World Conquest; difficulty levels 1–5; choosing original World rulers; and automatic or manual +100-point stat allocation for one's lord and four princes. This is historical documentation, **not** a completed ROM-based setup trace. World lords among the exact 27 original pre-setup records are Genghis (index 0), Richard I (6), Alexious (10), Yoritomo (18); Mongol Temujin is index 0. No invented ability values or first Spring economic changes are applied.

## New implementation

The **same native 1280×720 executable** has an Xbox-style START input on the country candidate screen to enter a controller-only staged wizard: player count, difficulty, unique historical lords, auto/manual abilities; then an explicit blocked source-stat/season boundary. B backs out one step. Stock Classic vs approved modified Enhanced are preserved as separate settings, and original 14/27 pre-setup country records remain isolated.

New portable little-endian **2,654-byte setup draft** extends the previous 2,618-byte format with versioned manual choices, selected lords, CRC32 and atomic invalid-data rejection. Existing 2,594/2,618-byte saves import unchanged. Exclusive-create saves avoid overwriting campaigns; separate application runs verify save/restore. No complete campaign or playable Spring is claimed.

## Build and test (Linux only)

From the ZIP root:

```
cmake -S execution_053 -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug -j4
ctest --test-dir build-debug --output-on-failure
```

One can rerun with `-DCMAKE_BUILD_TYPE=Release`; ASan/UBSan via `-DCMAKE_C_FLAGS='-fsanitize=address,undefined -fno-omit-frame-pointer' -DCMAKE_EXE_LINKER_FLAGS='-fsanitize=address,undefined'`. `--manual-setup-export path 2 7 2 5 manual` creates a sample World draft (Richard I + Genghis); `--manual-setup-import path` loads it; `--smoke-setup 120` exercises the native 1280×720 X11 screen under Xvfb.

Evidence classes: **PRIMARY_PRINTED_MANUAL** for constraints; **TESTED_PORT_LINUX** for controller software simulation, state machine, byte-format, clean builds and X11 window; **NOT OBSERVED NES Mesen** for player/setup stat/campaign transformation; **NOT TESTED WINDOWS/PHYSICAL XBOX**. No copyrighted ROM, save state, imagery or sound included.

Port TODO: actual source-traced setup allocation for all four princes; seasonal transitions; complete strategic orders/AI/dynasty/diplomacy; verified battle terminal states; actual sound/graphics; full campaign persistence; real Windows installer/controller acceptance.
