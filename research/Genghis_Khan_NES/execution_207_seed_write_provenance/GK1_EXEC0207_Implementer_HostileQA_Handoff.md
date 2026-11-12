# GK1 EXEC0207 — Source-executed RNG seed-write provenance, implementer/hostile-QA handoff

**2026-11-11, development candidate, inherits exact complete EXEC0206 C11 app**. This is NOT a new game, standalone prototype, original NES emulation session, or certified Windows release. Predecessor v3.60 master and ledger preserved intact. Original-stock USA ROM private SHA256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`; confidential raw gameplay/evidence private SHA256 `8b757e11b12aeb3efe93e36c10628599ca5f12e91b15c02354cd3ebbd11bf52a`. Neither is present in any public package or GitHub commit.

## New linked C11 code and source provenance

`gk207_seed_write_watch.c/.h` records **COMPLETED** 6502 source interpreter bus writes at NES RAM `$0084..$0089` (also mirrors), with instruction PC, target CPU address, value, mapper-bank knownness/bank, and final cycle count. Six-byte written bitmask is derived only from completed observed stores; no power-on-state inference. The watch is linked to the existing reset interpreter (`gk83_reset_trace.c/.h`) and native CLI (`--audit-stock-seed-writes PRIVATE_ORIGINAL_ROM`) in existing `gk1_native_shell.c`; `CMakeLists.txt` includes new linked test `test_gk207_seed_write_watch.c`. Hostile test checks wrong addresses, RAM mirrors, forbidden fields, atomic invalid-argument rollback, counters, event truncation, and known-ROM original startup bounds. It does not alter campaign, previous RNG or original country templates.

## Actual authenticated stock bytes executed in bounded C11 source interpreter

```
SOURCE_BOUNDED profile=UNKNOWN_RAM_POWER_ON gate=3 pc=$8060 completed_instructions=70 completed_seed_stores=0 written_mask=00 full_seed_written=0
SOURCE_BOUNDED profile=DOCUMENTED_MESEN_ALLZEROS_MODE gate=2 pc=$C5BE completed_instructions=142 completed_seed_stores=0 written_mask=00 full_seed_written=0
NOT STOCK MESEN EXECUTION: source-CPU bounded write provenance only. Unknown seed bytes remain UNKNOWN. Original first Spring is NOT generated.
```

**Very important:** `0 completed_seed_stores` is ONLY for 70/142 decoded instructions before execution gates. Do NOT infer the NES never writes seed or that the complete real NES setup was emulated. The `$0084..$0089` initial values and actual first-Spring RNG draw order remain unknown. **No new OBSERVED_RUNTIME_STOCK** Mesen run; original fresh WorkRAM/archived SaveRAM captures remain separate. The two Spring1175 Gold/Rate conditions (fresh 430/121; archived 439/101), plus World1206 193/114, are each real but are *not* universal constants. Source-authentic first Spring is still OPEN.

## QA executed (not simulated pass claims)

- Linux Ninja Release CTest **170/170**, Debug **170/170**, and independent clean original EXEC0206 ZIP+git integration patch rebuild **170/170**, each exit0.
- Focused compiled C11 test normal and **ASan/UBSan 93 assertions PASS**, input rollback/false-success negative controls, exact original ROM SHA check.
- Counterfeit bit-mutated stock ROM CLI exit8 (expected refusal).
- Actual 1280×720 native Linux X11/Xvfb 120-frame smoke and extracted Debian executable 120-frame smoke PASS.
- Actual existing C11 app imported each distinct authenticated private Spring profile for separate 120-frame controller-flow tests (fresh WorkRAM Mongol1175; archived SaveRAM Mongol1175; archived SaveRAM World1206). Imported states are NOT native generated first Spring.
- `git apply` reproducibility from inherited `GK1_EXEC0206_Complete_Integrated_C11_Source_QA.zip` exact: **712/712 source/QA paths matched** excluding nondeterministic `__pycache__` files; SHA manifest included. ZIP CRC integrity and Debian package CRC/integrity independently checked.
- **NOT RUN:** Windows Win32 binary, NSIS installer, physical Xbox controller, new live stock Mesen trace, full campaigns.

## Reproduce

```sh
unzip GK1_EXEC0206_Complete_Integrated_C11_Source_QA.zip -d inherited
cd inherited
git apply ../GK1_EXEC0207_CopyrightClean_Integration.patch
cmake -S execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
ctest --test-dir build -j8 --output-on-failure
export GK1_ORIGINAL_STOCK_ROM=/PRIVATE/authorized-original-usa.nes
./build/test_gk207_seed_write_watch
./build/gk1_native_shell --audit-stock-seed-writes "$GK1_ORIGINAL_STOCK_ROM"
xvfb-run -a -s '-screen 0 1280x720x24' ./build/gk1_native_shell --smoke-frames 120
```

## Next executable priority and publication

Instrument **actual stock Mesen** from the real reset through player setup, with program counter, six-byte `$0084..$0089` writes, reads, selector `$0050` writes, BRK dispatch and mapper state. Do not assume unknown `$00F5` or PPUSTATUS. Only then source-trace original first-Spring setup/difficulty/character stats/14+27-country seasonal transforms, real 3 orders, merchant schedule, strategic AI, dynasty, diplomacy, battle aftermath, audio/graphics, persistence and full Win32/physical Xbox. Keep original and Enhanced BP/Rest behavior separate, no PRE_SETUP bypass, preserve all old work.

Publish ONLY copyright-clean C11 files/tests/QA and docs in verified Collaboration Drive parent `1d0TA1lgqsVA3AWQQ9o0FZgmvao90oCeT` and public `Bomb-Daddy/NES`; original ROM and licensed snapshots ONLY in private parent `1mJ72ezJb9kzIGUHHXPTh3lXtiVxWjEL7`. Publication is NOT agent read/execute acknowledgement nor release acceptance. Existing hourly execution should remain enabled.

**Classification:** VERIFIED_STATIC=ROM hash, source provenance, patch reproduction; OBSERVED_RUNTIME_STOCK=no new run, earlier fresh/archived separately preserved; TESTED_PORT_LINUX=170/170 each and smoke/QA; NOT RUN=Win32/NSIS/physical Xbox/full campaigns.
