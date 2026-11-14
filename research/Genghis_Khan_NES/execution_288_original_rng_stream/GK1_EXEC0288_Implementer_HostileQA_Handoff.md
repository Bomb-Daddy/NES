# GK1 EXEC0288 — Source-authenticated original C289 multi-draw stream in the inherited game

2026-11-14. This is a source/test increment to the **complete** EXEC0286 C11 Linux game (with EXEC0287 Win32 integration evidence retained). It is not a new isolated game nor the original first playable Spring.

## Implemented code

* New `gk288_original_rng_stream.h/.c`: validated source engine executes 1–64 successive source `$C289` draws from six explicit seed bytes using six coefficients read from the locally held, SHA-pinned **original-stock USA NES ROM** (never redistributed). Every step cross-checks the existing independent source-derived 15-bit generator and state transition; computes difficulty-sensitive `Rate` candidates from the existing source model.
* New native game `gk1_app_original_rng_stream` links to existing `Gk1App`, validates Classic profile, only the original PRE_SETUP world, 14 or 27 country template identity and selected difficulty (1–5), rejects initialized/modified/checkpointed states; leaves app/output exactly unchanged on rejection. Does **not** grant any order or import a historical checkpoint.
* Executable regression `test_gk288_original_rng_stream.c` covers negative stage & rollback, both campaigns, five difficulties, 64-step replay and cross-check against existing single-step rate code using private original ROM at runtime (outside public CTest default).
* All code is copyright-clean, with original ROM and private gameplay data never inside this source archive or GitHub.

## Commands actually executed

```
cmake -S /mnt/data/gk_work_288/execution_199 -B /mnt/data/gk_work_288/build -DCMAKE_BUILD_TYPE=Debug
cmake --build /mnt/data/gk_work_288/build -j8
ctest --test-dir /mnt/data/gk_work_288/build -j8 --output-on-failure -E 'GK1_202_LinuxX11_SmokeFlagsRemainReachable|GK1_220_Linux1280x720_ControllerLaborUndoActualX11|GK1_226_FourSeparateX11_ControllerTitleYSafeUpdate|GK1_255_GeneratedX11Registry_WindowsExclusion_HostileQA|GK1_257_CurrentWin32Workflow_RejectsStaleSourceAndFalsePass'
/mnt/data/gk_work_288/build/test_gk288_original_rng_stream \
 /mnt/data/gk_work_288/execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv \
 /mnt/data/GK1_EXEC0076_Exact_Original_Stock_USA_ROM_PRIVATE_SHA256_ada7066d.nes.bin
```

Linux full C11 Debug build compiled; **228/228 eligible CTests passed**, five inherited exclusions preserved, and **2051 assertions** in an additional separately executed private original-stock-ROM source test passed. Do not count the private-ROM test as a Mesen/gameplay run. Original ROM exact SHA256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3` independently checked; private Mesen evidence ZIP SHA256 `8b757e11b12aeb3efe93e36c10628599ca5f12e91b15c02354cd3ebbd11bf52a` checked without publishing. Source-original artifact includes no original ROM.

## Acceptance boundaries and honest limitations

* **VERIFIED_STATIC**: exact inherited source, source-patch test replay, full archive SHA, private original ROM SHA and public-safe source provenance.
* **SOURCE_EXECUTED_ORIGINAL_STOCK_ROM**: authenticated six runtime coefficient bytes read from owner's ROM for private test. This is conditional source arithmetic, not confirmation of true startup chronology.
* **TESTED_PORT_LINUX**: 228/228 applicable tests and 2051 private-source assertions.
* **TESTED_PORT_WIN32_CI**: NOT RUN for EXEC0288; predecessor EXEC0287 Windows 216/216 preserved.
* **OBSERVED_RUNTIME_STOCK**: prior fresh WorkRAM Mongol and archived SaveRAM Mongol/World observations preserved, NO NEW Mesen run.
* **NOT RUN**: physical Xbox controller, full native campaigns, original NES marriage-age/water/battle-end experiments.

**NOT IMPLEMENTED**: actual first-Spring startup seed provenance/NMI/controller input interleaving, authentic setup/ruler/ability and 14/27 full source-generated country transforms, three real orders; merchant presence, autonomous AI, diplomacy, dynasty, war aftermath, full controller-only campaigns. These cannot be inferred from synthetic seed sequences or one original screenshot. Next focus should be tracing actual RAM seed writers through original CPU setup and reproducing original first playable Spring, not building another generic regression counter.

Source predecessor: latest inherited complete EXEC0286 and EXEC0287 Windows CI. Collaboration parent `1d0TA1lgqsVA3AWQQ9o0FZgmvao90oCeT`. Private source parent `1mJ72ezJb9kzIGUHHXPTh3lXtiVxWjEL7`. Original ROM private and copyright-clean GitHub only.
