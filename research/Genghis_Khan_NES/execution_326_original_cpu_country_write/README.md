# GK1 EXEC0326 — original-stock CPU indirect country-writer executable research + native linkage

**Date:** 2026-11-16. **Predecessor:** EXEC0325 (complete inherited C11 app, Windows CI237). **Acceptance: PARTIAL.** This is **not** a first playable Spring generator, a new Mesen capture, or a release. The authentic original-stock 14/27-country transformations, player/difficulty/ability/RNG, actual event queue, three port controller orders, subsequent seasons and full campaigns remain open.

## Source-observed gap and actual implementation

Earlier EXEC0325 established the phases of the original first Spring but did not establish the source CPU writes that generate each country. A fresh stock screenshot/CPU sample at frame 2391 has `PC=$C2BF`, `X=5`. The authenticated **private original USA 262160-byte ROM** at fixed PRG bank15 `$C2BF` has opcode `9D 8B 00` (`STA $008B,X`)—**not** a direct country write to the `$6DCD` 83-byte country-record array. The same fixed-bank ROM at `$C44C` contains opcode `91 00` (`STA ($00),Y`), a **possible indirect writer** whose effective address depends on zero-page pointer `$00/$01` and Y. This program-counter/source-byte observation is **VERIFIED_STATIC**: it does not establish that the `$C44C` instruction wrote country data in the original run, nor the observed pointer/registers then. Original full RAM/CPU event trace needed.

Added `gk326_cpu_country_store.c/h`: a real, bounded NMOS 6502 CPU store **effective-address interpreter**, supporting STA `(zp,X)`, `STA (zp),Y`, `STA abs/abs,X/abs,Y`, `STX abs`, `STY abs`. It correctly wraps zero-page pointers at `$FF`, rejects unofficial opcode `$9E` rather than mislabeling it as STX, maps true 14/27 country records via CPU `$6DCD`, **83 bytes each**, physical SRAM offset `$0DCD`, supports fresh WorkRAM vs separately restored SaveRAM, validates switch/fixed PRG bank, and returns a concrete single-byte original CPU memory update in an **isolated RAM copy**. It does not alter the application turn or the user's save. Wrong backing context, address, opcode or bank are rejected with no writes.

Added `gk326_sha256.c/h`: platform-portable, actual full-byte SHA-256 authentication of an externally supplied original USA ROM to `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`. The strict `gk326_apply_stock_country_store()` refuses a changed ROM before writing anything. Synthetic unit tests use a separately named **unauthenticated structural fixture** function, never assign `original_rom_hash_verified=1` and never claim stock gameplay. No private ROM/source RAM values are shipped.

**Important limitation:** Even a real stock-ROM opcode combined with caller-fabricated A/X/Y/zero-page registers is merely an instruction-decoding test, NOT an original-stock event. The original source trace needs actual Mesen pre-write CPU registers, active PRG bank, and WorkRAM/SaveRAM identification. This module cannot assert `first_spring_generated` or `player_orders_granted` (both zero). It is not a shortcut, clone game, save-game impersonation or ROM emulator.

## Integration / reproducibility

`stage326.py` verifies hashes of the five executable C11 sources/tests, normalizes *only* legitimate CRLF to LF, stages into the **same already authenticated `gk1_app_state` native C11 library** after EXEC0325 and registers a real CTest. It fails closed/rolls back on source hash corruption, stale prior stages, duplicates and injected write failures. Existing source and player saves are not overwritten.

Linux standalone compile/QA commands:
```
cc -std=c11 -Wall -Wextra -Werror -pedantic -O2 \
  gk326_sha256.c gk326_cpu_country_store.c test_gk326_cpu_country_store.c -o test326
./test326
python3 test_stage326.py
cc -std=c11 -Wall -Wextra -Werror -pedantic -O2 \
  gk326_sha256.c gk326_cpu_country_store.c test_gk326_real_stock.c -o test326_stock
./test326_stock /PRIVATE/PATH/EXACT_ORIGINAL_USA_ROM.nes
```

To run full Windows CI, stage `python research/Genghis_Khan_NES/execution_326_original_cpu_country_write/stage326.py current-gk1/execution_199` **after** EXEC0325 and **before** CMake. This adds CTest `GK1_326_6502CountryStoreEffectiveAddressSourceBankAndRollback`; the private ROM test is not registered in public CI. The original game remains intact. Do not interpret passing a source-store decoder as completion of original initialization.

## Verified classification

- **VERIFIED_STATIC:** exact original ROM SHA, fixed-bank opcode `C2BF: 9D 8B 00` and `C44C: 91 00`; correct source address mapping and explicit unsupported unknowns. No raw private ROM bytes committed. The sample `C2BF` is an actual previously captured CPU PC; target decode uses its captured X=5. Synthetic pointer values for `C44C` were **not** observed.
- **TESTED_HOST_LINUX_C11:** Werror Release and ASan/UBSan fixture decoder **64/64 PASS**; genuine private original ROM identity/controlled opcode tests **17/17 PASS**, including one-byte tampered original rejection; portable SHA256 checked against published `abc` vector.
- **TESTED_HOST_LINUX_PYTHON:** 14/14 hostile staging assertions PASS; clean/CRLF/corrupt/rollback.
- **TESTED_PORT_LINUX_FULL:** not newly run at initial source publication.
- **TESTED_PORT_WIN32_CI:** not claimed until an actual Windows Actions run finishes successfully; previous EXEC0325 237/237 remains distinct.
- **OBSERVED_RUNTIME_STOCK_FRESH_RESET / ARCHIVED_RESTORED:** previous recorded source observations re-used but no new Mesen gameplay this execution.
- **Physical Xbox, complete Mongol/World campaigns:** NOT RUN.

The next essential task remains to instrument actual original stock RAM writers in Mesen at country `$6DCD + 83*n` through setup/finance/personnel, recording pre-store active bank, CPU A/X/Y, zero-page indirect pointers and changed bytes; then implement their true general transforms in the existing C11 game rather than using the one witnessed Spring numbers as universal constants. No original copyrighted ROM/data goes to public GitHub or public CI.
