# EXEC0079 — authenticated STOCK SPRING checkpoint integration — 2026-11-07

This is the same inherited EXEC0078 C11 Win32/XInput + Linux/X11 native application, version 0.79.0-dev1. The `gk79_stock_checkpoint.c/h` module stages externally supplied **exact-SHA-validated original stock NES** Spring 1175/1206 country data (14/27 records decoded by existing `gk77_load`) inside the same Gk1App. Country attributes, goods and population sums come from source records, not hardcoded example Gold/Rate. Source template country identity/name and fields absent in the 83-byte record remain explicitly partial. No licensed ROM/checkpoint bytes or emulator states are embedded in the public files.

## Native controller path (requires private external original capture)

Supply `GK1_STOCK_SPRING_RECORDS` and `GK1_STOCK_SPRING_PROFILE=fresh|mongol|world`, launch game, select Classic campaign/candidate, enter setup and manual source explorer, press A for Stock Spring, VIEW to switch to exact original stock evidence, then A to stage authenticated checkpoint. In the new controller screen DPAD browses the real checkpoint country data, A arms PASS, second A exhausts the manual-verified base three-order allowance, B cancels before confirmation without changing numeric country state. Press B otherwise returns to source readout. **This is a single real game input transaction, not the original full startup or a complete playable Spring.** All further season advance, AI, authentic merchant RNG, battle aftermath, multiplayer, dynastic and complete campaign save/cold-load work is still gated. The app's pre-setup worlds are never overwritten.

## Evidence classification

- VERIFIED_STATIC: source-driven 41 countries, manual order budget/Pass, exact stock profile SHA fingerprints inherited from EXEC0077; no static original-ROM executable bytes distributed.
- OBSERVED_RUNTIME_STOCK: original Mesen fresh Spring1175/archived Spring1175/World1206 records are prior EXEC0076 observations, **not rerun** in this implementation.
- TESTED_PORT_LINUX: new source integrates in real C11 app, tested with synthetic decoded-struct fixture for routing and negative bad file SHA; all three original-stock record files recovered from the private EXEC0076 archive were supplied for an independently executed positive SHA-gated integration run (fresh14, archived14, archived27). Genuine prior Mesen captures were consumed; no new ROM/Mesen run occurred. Synthetic fixtures remain separately named and must not be called original-stock observations. The X11 controller-directed checkpoint and Pass/cancel route ran 120 frames each on fresh14 and archived27, using the exact private source bytes. Version 0.79.0-dev1.
- NOT RUN: Win32 installer and physical Xbox/controller hardware.

## Publication gates

Never publish raw ROM/RAM/screenshot/state. This source archive preserves older execution directories verbatim. EXEC0078 remains the last baseline for original Spring finance. The new checkpoint path imports original stock captured output, **does not reproduce first-year RNG, game setup or character transformations**, and will not be described as having completed those tasks.

The controller **LB/RB** selects one of three source-receipt slots; **X** creates a new 32-byte, integrity-tagged research PASS receipt exclusively, and **Y** reloads it atomically. A cold start must reattach the exact original-stock private capture and enter checkpoint mode first: the public receipt **does not contain or reconstruct source RAM**, so it cannot be used alone as a playable campaign save. Corruption and mismatched rate/player/profile are rejected.

## Optional private-source QA commands (never publish private payloads)

```sh
# Build and run source-bounded unit QA in the extracted complete source tree:
cmake -S execution_079 -B build79 -DCMAKE_BUILD_TYPE=Release
cmake --build build79 -j4
ctest --test-dir build79 --output-on-failure
# Separate authenticated runtime with private original 14/14/27 source files:
build79/test_gk79_stock_checkpoint build79/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv \
  /private/fresh_14x83.bin /private/archived_mongol_14x83.bin /private/archived_world_27x83.bin
GK1_STOCK_SPRING_RECORDS=/private/fresh_14x83.bin GK1_STOCK_SPRING_PROFILE=fresh \
  xvfb-run -a build79/gk1_native_shell --smoke-stock-checkpoint 120
```

**Evidence precision:** These tests validate importing authenticated original-stock RAM and continuing through a single manual-derived Pass. They do NOT implement or verify original startup setup/RNG; the checkpoint has non-RAM identity fields copied from original country templates. An original NES `Pass` gameplay run was not performed to compare subsequent RAM. Both original campaigns remain locked in actual PRE_SETUP. Windows binary/NSIS and physical Xbox not run.
