# Genghis Khan I — EXEC0275 (2026-11-14)

This is an additive fix to the **existing complete native C11 Windows/Linux game**, inherited from EXEC0274. It does **not** implement independent original NES first-Spring setup/RNG.

## Actual defect and fix

The real `gk166_enter_original_fresh_json` original-stock loader admitted one authenticated first-Spring WorkRAM sample regardless of selected player/difficulty/ruler/ability wizard. That could mislabel the recorded Spring of one original Mesen history as the user's different new game's generated start.

The updated `gk166_fresh_entry.c` refuses an already completed incompatible wizard **before file access or mutation**, covering wrong difficulty, ruler, player count, ability method, and hidden bonus points. A title-screen source checkpoint continuation remains available without claiming the port generated that history.

Code hashes:
- Predecessor gk166 SHA256: `e3c796e02d7db070bd746bf459dc025fc3d3127223e344827b15685736e56ba2`
- This revision gk166 SHA256: `0c0a392ceac9a3c8d479f1c6ec1d85a8f22a86c0404acfd37bb0df162d3fd05b`
- New C test SHA256: `014b02b13bbccdc8f6800df5290cc05efd98a20f008831e74ff614abf129408f`

## Reproduction

On the full inherited source:
```sh
cmake -S execution_199 -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
ctest --test-dir build -R 'GK1_(193|166|162|275)_' --output-on-failure
```

Native Release: PASS. New regression: 34 public/no-private and 40 private authenticated-source assertions PASS. Targeted 4/4 CTests PASS. Broader bounded Linux 222/222 applicable CTests PASS; excluded historical obsolete GK1_257 and hanging multi-process GK1_226, neither marked passed.

The new executable CTest is registered by the exact modified `CMakeLists.txt` in the complete source ZIP. Live Windows workflow stages these two exact GitHub C sources and registers the new CTest in compiled Windows C11 build: [workflow](https://github.com/Bomb-Daddy/NES/blob/main/.github/workflows/gk1-native-windows.yml), commit `c70748f5b6cb3d7202f304ea4688a2979b0b7a6e`. **Native EXEC0275 Windows CI not independently confirmed yet**.

**Private original NES ROM, raw Mesen RAM/checkpoints, screenshots and audio are not included in this public folder.** The first real self-generated original-stock Spring, autonomous AI, Trade scheduling, diplomacy, dynasty, battle terminal and full Xbox-controller campaigns remain open.

Full-source archive SHA256: `a06bad8235fd3d26ec5d82a762671c670d6f64bea844b03b08a762a57061e87f` (5,399,726 bytes), attached to conversation; raw Google Drive egress was rejected. Complete source, not this README alone, is required to build.
