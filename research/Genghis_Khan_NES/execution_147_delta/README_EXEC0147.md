# EXEC0147 — source-attested original ten-specialty-goods controller screen

2026-11-10. Standalone inherited native C11 game `1.47.0-dev1` (Linux X11, Windows Win32 target), continuing EXEC0146 without removing earlier work. **Still research-grade; no independent original NES first-Spring generator or full game.**

## Player-facing implementation

Authenticated original-stock Spring checkpoint -> `VIEW` cycles DECODED -> RAW -> FINANCE -> DELTAS -> **ORIGINAL GOODS** -> DECODED. D-pad selects all 14 (Mongol) or all 27 (World) countries. The new 1280x720 controller view shows Fur, Silk, Jewels, Metals, Medicines, Woodworks, Ceramics, Glass, Textiles, Perfume, and separately Food. It reads original goods at `country + 0x22 + 2*i` for `0..9`; Food is `country + 0x36`. Displays are original-source values, never inferred 2026 merchant offers.

The source-bound `gk147_source_goods()` rejects unverified, malformed, tampered, or wrong-campaign input, and validates each displayed field against the authenticated original 83-byte country record and the native game projection. If an Enhanced labor session is active, its replay must also remain source-attested. The method never changes original country bytes, immutable raw SHA, labor state, orders, seasons, merchants, or RNG. `gk1_app_source_goods_view()` links it to the same Gk1App as every prior feature, rather than a detached prototype. No original Trade transaction is enabled.

## Reproducible verification

`cmake -S execution_147 -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j8 && ctest --test-dir build -j8 --output-on-failure`

Public negative controls: `build/test_gk147_stock_goods`. Positive controls require owner-private source-attested 83-byte records and the verified original v1.07 CSV:

`build/test_gk147_stock_goods execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv <fresh-14> <archived-mongol-14> <archived-world-27>`

Integrated Linux GUI `--smoke-stock-goods 30` tested separately with each of three original private source profiles via `GK1_STOCK_SPRING_RECORDS` and `GK1_STOCK_SPRING_PROFILE=fresh|mongol|world`, using Xvfb. It tests the real controller View navigation and renders the goods screen. Test harness pad inputs are **not physical Xbox acceptance**.

## Evidence boundaries

`VERIFIED_STATIC`: source authenticity gates, offset decoding, negative controls, same-app code. `OBSERVED_RUNTIME_STOCK_FRESH_RESET` and `OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED`: inherited real Mesen original fresh Spring1175 versus archived Mongol1175 and World1206; no new Mesen run. `TESTED_PORT_LINUX`: 105/105 CTest, three private positive source-profile runs, three authentic-stock GUI routes (plus old setup regression). `NOT RUN`: native Win32, NSIS installer, physical Xbox controller, complete campaigns, first Spring independent setup.

This execution does not resolve the critical original first Spring initialization, setup RNG, original Tax/Assign/Trade, AI/diplomacy/dynasty, battle aftermath, or seasonal progression. Preserve Classic/Enhanced separation. **Do not publish original ROM, SRAM, states, screenshots, records or audio to public GitHub.**
