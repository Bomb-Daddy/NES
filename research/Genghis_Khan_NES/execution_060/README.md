# Genghis Khan I EXEC0048 — Integrated original-modified-ROM path controls

**Project**: standalone English NES-to-Windows Genghis Khan I port; not ROTK II.
**Classification**: TESTED_PORT_LINUX (new C11/X11 work), SOURCE-BOUNDED using OBSERVED_RUNTIME_MOD cases from EXEC0045. Not a full game, stock-ROM proof, new NES gameplay, compiled Windows program or physical Xbox acceptance.

Reuses exact previous EXEC0044 complete source, preserving older research viewer/replay/controller and two locked campaign menu entries. **New EXEC0045 C11 source is linked directly** into the executable, not merely tested in isolation. New path case selection adds three controlled source profiles: unchanged raw7 second tile (reject/error dismissal), controlled raw2 second tile (two accepted steps/B completes), controlled same-side ally40 on forest (join produces 74/0/word2). From the menu select the third entry then scenario; D-pad moves the cursor on a 16x10 grid, A commits the selected supported move, B dismisses error/completes pending or returns after done, X saves the 64-byte path research receipt, Y loads it. Unsupported moves leave soldiers/unit/RNG unmodified. The cursor and unit positions display independently.

**Important evidence limit**: 0045 `gk1_observed_turn` implements only the controlled approved combined Shared BP/Rest modified-ROM checkpoint. No general pathfinding, Divide quantity/split, hostile AI, two playable campaigns, audio, terminal state, stock-English Classic parity, installed Windows build or physical controller acceptance. Full 1280x720 X11 GUI was executed, rendered and screenshot-checked on Linux. Windows GDI/XInput branch was authored earlier but **not compiled or run on Windows here**.

## Build/test on Linux

```
cmake -S execution_046 -B build46 -DCMAKE_BUILD_TYPE=Debug
cmake --build build46 -j4
ctest --test-dir build46 --output-on-failure
./build46/gk1_native_shell --path-export /tmp/raw7.gkp 1 12bb
./build46/gk1_native_shell --path-import /tmp/raw7.gkp
./build46/gk1_native_shell --path-export /tmp/raw2.gkp 2 12b
./build46/gk1_native_shell --path-import /tmp/raw2.gkp
./build46/gk1_native_shell --path-export /tmp/join.gkp 3 1
./build46/gk1_native_shell --path-import /tmp/join.gkp
Xvfb :108 -screen 0 1280x720x24 &
DISPLAY=:108 ./build46/gk1_native_shell --smoke-path 25
```

## 64-byte research path receipt GK1OP046

Byte 0..7 magic `GK1OP046`; byte8 format v1; byte9 source fixture 1/2/3; byte10 event count 0..4; byte11..14 logged actions (1=first step,2=second step,3=B); byte15 reserved zero; byte16..47 is exact approved combined modified-ROM SHA-256 **label**; byte48..59 reserved zero; byte60..63 little-endian CRC32 ISO-HDLC. Decode replays all actions from fresh EXEC0045 source state, rejects impossible scripts, malformed fields/length, wrong-ROM provenance label or CRC, and commits app state only after all checks pass. File creation is exclusive, no silent overwrite; cold read refuses truncated/oversized files. SHA label/CRC do not prove that the actual ROM executed and the receipt is **not** a campaign save. Existing 48-byte EXEC0039 research replay remains unchanged and incompatible by deliberate version distinction.

## Proven numerical oracle used for regression only

- Original raw7 second tile: forest first step 40→34, (8,7), Mobility10→6, then second attempted (9,7) rejected; first B dismisses message, next B finishes path at (8,7), Mobility10, word1.
- Controlled raw2 second tile: 40→34 forest, then (9,7), Mobility6→3, unchanged RNG, B finishes Mobility10/word1.
- Controlled ally40 at (8,7): forest 40→34; then join 34+40=74, partner zero at sampled completion, word2, Mobility10.

No original-ROM native test was repeated in this increment. No Divide quantity split is invented. Tests use synthetic controller samples + exact source-bounded fixtures; each is labeled in test source.

## QA and publisher rules

Do not interpret GitHub ZIP visibility or Drive presence as Windows execution. The included archive contains copyright-clean sources only, not licensed ROM, emulator, raw checkpoint, native screenshot or campaign files. Preserve v2.27 and EXEC0045 evidence. When source and native captured behavior differ, stop promotion and investigate profile and phase before changing oracle.
