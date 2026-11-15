# GK1 EXEC0296 — real stock first-draw four-writer provenance

**November 15, 2026 — existing C11 native game; incomplete first-Spring implementation.**

## Newly witnessed in actual original-stock Mesen

We recovered the exact original USA NES ROM from authorized private Drive and
ran the original through the installed headless Mesen (not the same build/core
as the EXEC0076 archived run). A genuine CPU trace demonstrated **948 zero-page
writes to $84–$87**, **289 fixed-bank $C289 calls**, **14 scripted controller
stage markers**, and *zero* witnessed writes to $88 or $89 before first C289.
The prior requirement for six source-written seed bytes was too strong. Their
observed initial contents are not source-writer evidence, nor necessarily
hardware-deterministic power-on RAM. Do not fabricate writes to fill 0x3F.

The old recorder also reported the **post-instruction PC** for writes. All 948
actual mapped stores authenticate to the original ROM when the executing
absolute-store PC is calculated as `reported_PC - 3`. V4 now records and checks
its actual source opcode PC instead of a spurious post-instruction address.

A separate clean original ROM controller replay reached year **1175**, three
orders and actual first Spring. The resulting economics were **not identical**
to either authenticated historical fresh or archived checkpoint values; they
must never be hardcoded. All original screenshots, RAM, private values and
controller transcripts are kept out of this public package.

## Native C11 changes

`gk294_seed_writer_attest.c/h`: new independent `*_masked` authenticated store
attestation that checks an exact source mask; the original six-writer API
remains unchanged. `gk296_stock_seed_origin.c/h`: four witnessed source writers,
explicitly supplied original RAM seed, and one original C289 math draw; reports
`unknown_poweron_mask=0x30` and never claims independent runtime.
`gk1_app_state.c/h`: identical real Classic pre-setup/world integrity gating,
atomic refusal and read-only application bridge, no fake turn unlock. CMake
build integration and `test_gk296_stock_four_writer.c` cover both complete
scenario templates, five difficulty selections and corruption/rollback.

## Source-side Mesen tools

`GK1_EXEC0296_Private_Mesen_SeedTrace_V4.lua` is a **PRIVATE-output** capture
script and contains no original ROM bytes. It checks the actual running ROM
SHA-1, source opcode/PRG bank, captures script-start RAM observation separately,
then logs source writes/C289 and controller stage events. Its stage-memory-bank
mapping can report inconclusive data on this Mesen API: never call zero
an absent country. `gk296_audit_stock_trace_v3.py` authenticates exact private
ROM source code for both legacy V3 (PC+3) and new V4 (true source PC), validates
all source writes and C289 transitions and labels initial RAM as NOT power-on
proof. It independently rejects malformed/altered evidence. Neither creates
player-ready native Spring.

## Reproduction, with original ROM supplied privately by project owner

```sh
cmake -S execution_199 -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j8
ctest --test-dir build -j8 --output-on-failure -E '([Ww]in32|[Ww]indows|[Xx]11)'
build/test_gk296_stock_four_writer execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv /private/original.nes
python execution_296_stock_four_writer/gk296_audit_stock_trace_v3.py /private/original.nes /private/trace_v4.csv
python execution_296_stock_four_writer/test_gk296_audit_source.py /private/original.nes /private/trace_v4.csv
```

**Classification:** VERIFIED_STATIC PASS, genuine OBSERVED_RUNTIME_STOCK_FRESH_RESET
(non-universal single input); TESTED_PORT_LINUX 226/226 selected tests PASS;
TESTED_PORT_WIN32: NOT RUN for this change, prior EXEC0295 218/218 retained;
physical Xbox and complete campaigns NOT RUN. No completed game claimed.
