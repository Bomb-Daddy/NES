# EXEC0323 — Stock Spring 83-byte record integration (C11 implementation, not completed first Spring)

Authority: Existing GK1 native app EXEC0322/CI233; Human Director; original stock runtime EXEC0076. The separately developed Original KOEI-inspired game and ROTK II are out of scope.

- Added bounded source-authentic stock recorder decoder `gk323_original_spring.c/h`: parses 14/27 real 83-byte country records supplied by caller from the appropriate original NES memory bank. Checks exact ROM SHA textual identifier and scenario/bank/year/season/3-order observational provenance, validates numeric ranges and offset +0x22 traded goods. Fails atomically; never uses fixed example Gold/Rate as universal first Spring.
- Added `gk323_app_stock_bridge.c/h`: tied to existing C11 `Gk1App` active Classic scenario, requiring source identities, all original countries and original PRE_SETUP phase; does NOT mutate source world/phase or turn counter. No fake first Spring or native controller orders.
- Added two actual C11 tests and SHA-pinned fail-closed atomic staging; tests guard wrong bank/source, wrong scenario, missing source, rollback, unrelated source, duplicate integration.
- HOST ONLY local decoder C11 34/34, ASan+UBSan 34/34, bridge C11 with mock source-authored header 15/15, stage 19/19 PASS. PRIVATE original-stock earlier RAM evidence checked 14+14+27=55 original records and 1393 checks PASS, without new emulator execution. No private original ROM or RAM content in this source/hand-off.

PENDING: native Windows CI and Linux full suite until separately verified; original-stock actual first Spring authoring remains OPEN (ruler, difficulty, character, RNG/season and three playable controller orders), as do Trade scheduling, autonomous AI, battle endings/aftermath, diplomacy, dynasty, authentic sound/art, physical Xbox and complete campaigns. Do not mark source-claim read/execute ACK or finished release. Continue recurring development.

## Reproduce

```
python research/Genghis_Khan_NES/execution_323_original_stock_ram_import/test_stage323.py
python research/Genghis_Khan_NES/execution_323_original_stock_ram_import/stage323.py current-gk1/execution_199
cmake -S current-gk1/execution_199 -B <build-dir>
cmake --build <build-dir>
ctest --test-dir <build-dir> --output-on-failure
```

Do not put proprietary original USA NES ROM, original RAM 83-byte records, Mesen states, art/audio or source checkpoints into public GitHub or public artifact.
