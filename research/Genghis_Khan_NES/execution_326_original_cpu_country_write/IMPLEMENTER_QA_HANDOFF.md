# EXEC0326 reproducible implementer + hostile QA handoff — November 16, 2026

**Project:** existing original-stock Genghis Khan I NES-to-native C11 Windows/Linux game. Do not confuse with ROTK II or inspired game. **Parent:** EXEC0325, Windows CI237 accepted. Preserve all previous complete native app/source, private ROM/gameplay and Director requirements. Not a game release, not agent acknowledgment.

## Actual executable result

`gk326_cpu_country_store.c/h` now decodes a single observed NMOS-6502 STORE instruction effective address and applies it to the *separate provided research RAM copy* with fail-atomic output. Fixed PRG bank15 vs switch bank, genuine fresh WorkRAM vs archived SaveRAM, country count 14/27, 83-byte record layout, correct goods offset +0x22 and zero-page wrap are verified. `gk326_sha256.c/h` refuses any supplied ROM except exact 262160-byte original stock USA SHA256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3` in production function. No original ROM bundled.

**Static new original-source observations:** fixed bank15 C2BF bytecode `9D 8B 00` is STA $008B,X and not an original country write (frame2391 PC=C2BF X=5), C44C `91 00` is a candidate STA ($00),Y. Its real runtime pointer was NOT captured; test supplies hypothetical pointer purely for verifying CPU semantics, not observed Spring operation. This is an interpreter/source-tracing building block, not original country initialization, not an active controller menu or playable 3 orders.

**Local executed commands/results:**

```
cd execution_326_original_cpu_country_write
cc -std=c11 -Wall -Wextra -Werror -pedantic -O2 gk326_sha256.c gk326_cpu_country_store.c test_gk326_cpu_country_store.c -o test326
./test326  # PASS 64 assertions
python3 test_stage326.py  # PASS 14 assertions
cc -std=c11 -Wall -Wextra -Werror -pedantic -O2 gk326_sha256.c gk326_cpu_country_store.c test_gk326_real_stock.c -o test326_stock
./test326_stock /PRIVATE/authorized/USA_ORIGINAL.nes  # PASS 17 assertions
```

ASan+UBSan on both C11 tests also PASS. The real-stock private test is manually invoked by authorized tester only, not public CI and not archived with the ROM. Bad original hash rejects without RAM or receipt mutations; source register examples deliberately synthetic; source assertion flags generated=0, player orders=0, stock authenticated if exact only.

**Staging:** `python stage326.py current-gk1/execution_199` after 325 and before CMake. Full app library compiles these sources and registers `GK1_326_6502CountryStoreEffectiveAddressSourceBankAndRollback`. Hostile stage SHA, duplicate, CRLF/CRCRLF and rollback controls must pass. Actual Win32, full Linux suite and physical controller statuses are separate and cannot be inferred from local tests.

**Privacy:** Public repository contains only original new copyright-clean C11/Python tests, no ROM or raw original gameplay bytes. Original evidence/ROM maintained privately in Genghis research Drive `1mJ72ezJb9kzIGUHHXPTh3lXtiVxWjEL7`. The Collaboration destination is `1d0TA1lgqsVA3AWQQ9o0FZgmvao90oCeT`.

**Next:** acquire actual PRE-WRITE CPU zero-page, current mapper PRG bank and active RAM backing, between source original setup/difficulty/ability and Spring commands for all 14/27 countries, and implement true transforms inside live mutable Gk1App with real controller and source RNG. Follow with original Trade scheduler, autonomous AI, war aftermath, diplomacy/dynasty, complete saves/cold-load, licensed-free faithful art/audio, Windows and physical Xbox full campaigns. Keep recurring development enabled; no release acceptance.
