# GK1 EXEC0187 — Native Classic ASSIGN first executable command (2026-11-11)

This is the complete C11 1.87.0-dev1 continuation of EXEC0186. It **does not** reconstruct original NES setup/RNG or generate a new first playable Spring. A Director-authorized authentic Spring1175/1206 original private checkpoint is required; unattested synthetic checkpoints cannot enter the command screen. Original WorkRAM vs SaveRAM evidence, untouched source records, Classic-vs-Enhanced separation and 1280x720 native controller menus remain preserved.

## Actual player-facing change

In Classic profile, on an authenticated original Spring, select ASSIGN (command 2). A enters the new Classic ASSIGN screen. Up/down selects drafted source occupation, left/right selects drafted target occupation, Start/View changes integer quantity, A executes one labor transfer and consumes one of three native seasonal orders, B returns without consuming an order. The command accepts no transfers between the same occupation or unavailable workers. PASS on the main 14-command menu can finish the remaining Classic orders, with a separate second A confirmation. No next NES season, AI, tax, merchant event, automatic 1175 starting transform or full campaign save is synthesized.

These are **manual-derived** ASSIGN actions using the previously tested native five-occupation planner. The exact original stock NES binary's command-by-command behavior remains **UNVERIFIED**; this work is not an original 6502 command implementation or a claim of ROM parity. Immutable original 14/27-country source data and 83-byte country records are never overwritten; all action state is replay-checked against the original source before or after a commit. A forged decoded field, altered action log, changed original digest or invalid remaining order blocks mutation. Active Classic manual-derived actions intentionally block older stock-only X/Y private save so the user cannot create a save that silently omits turns. New game / private reattach clears derived actions. Persistent storage for these new actions is not yet implemented.

## Reproduce (the original ROM and private RAM records are NOT public)

```sh
cmake -S execution_187 -B build187 -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build187 -j8
ctest --test-dir build187 --output-on-failure
# Private original source checkpoint fixtures, never publish these files:
./build187/test_gk187_classic_assign build187/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv /private/fresh_14x83.bin /private/archived_mongol_14x83.bin /private/archived_world_27x83.bin
xvfb-run -a ./build187/gk1_native_shell --smoke-frames 120
cd build187 && cpack -G DEB
```

The public CTest tests reject unauthenticated input; positive original-source tests require the exact known SHA private checkpoint records. No original ROM, 6502 dump, audio, screenshots, private RAM or save state belongs in public GitHub.

**VERIFIED_STATIC:** original-source state gate, private fixture SHA, C11 replay and cancellation/rollback. **OBSERVED_RUNTIME_STOCK:** inherited archived and fresh source only, no new NES run. **TESTED_PORT_LINUX:** build, CTest, private integration and graphical smoke when reported in external QA receipt. **NOT RUN:** Windows/NSIS, physical Xbox, original NES command parity, full campaigns.
