# Genghis Khan I — EXEC0312: original stock CPU-PC provenance (C11)

Date: 2026-11-15. New numbered code, after verified EXEC0311 Win32 CI226. This patch applies to the **same inherited C11 Gk1App** and depends on prior actual `gk302_stock_seed_timeline` and `gk304_stock_store_value` source modules. No original ROM is inside this repository or archive.

## Concrete behavioral increment

`gk312_verify_original_event_pc()` rejects a claimed chronological original startup trace unless **every** claimed event carries a matching CPU-cycle, PC and mapped PRG-bank witness. NMI gating requires the original fixed-bank `$C106` gate read; the original source RNG C289 event requires `$C289`; source STORE requires the claimed STORE opcode PC/bank/cycle to agree with its gk304 A/X/Y register sidecar. It then replays the prior full source-validating `gk304` and `gk302` checks. Bad provenance, contradictory register sidecars, wrong bank, absent events, malformed ROM and count mismatch fail without modifying the output. The app-level wrapper rejects invalid stock setup, non-Classic mode and mutable original country state. This does **not** independently prove an authentic Mesen runtime event occurred.

Production app result deliberately retains `observed_stock_runtime=0`, `independently_generated_first_spring=0`, `playable_orders_granted=0`. There is **no first-Spring unlock, no three real orders and no complete campaign** in this increment. 14 Mongol +27 World source template countries remain intact. It is one bounded RNG source dependency, not a full game claim.

## Reproducible staging of complete current native source

After the existing exact source-authorized EXEC0311 Win32 build staging steps (including 302, 304), run:

```bash
python3 research/Genghis_Khan_NES/execution_312_original_event_pc_witness/stage312.py current-gk1/execution_199
python3 research/Genghis_Khan_NES/execution_312_original_event_pc_witness/test_stage312.py current-gk1/execution_199
cmake -S current-gk1/execution_199 -B build-gk1-win32 -A x64
cmake --build build-gk1-win32 --config Release
ctest --test-dir build-gk1-win32 -C Release -R GK1_312_OriginalSourceEventPcWitnessProvenance --output-on-failure
```

The tested Linux run used the **complete inherited EXEC0300 copyright-clean native source tree**, with the verified 302 and 304 native modules added, and then the new 312 source/app integration. This is **not** a claim that the exact current EXEC0311 Win32 workflow has accepted EXEC0312; Windows CI must run independently. The staging code refuses missing 302/304 predecessor, extra module, duplicate CTest, corrupted source hashes, malformed CRCRLF and nonmatching source anchors. Its injected failure tests confirm byte-identical rollback.

## Test evidence performed in this run

- `cmake -S /mnt/data/gk_work/src/execution_199 -B /mnt/data/gk_work/build -DCMAKE_BUILD_TYPE=Release` **PASS**.
- `cmake --build /mnt/data/gk_work/build -j8` full inherited native Linux Release **PASS**.
- `ctest --test-dir /mnt/data/gk_work/build -j8 -E '^GK1_257_CurrentWin32Workflow_RejectsStaleSourceAndFalsePass$'` **237/237 PASS**, including executable 1280x720 X11 smoke. Omitted one test only because the unpacked base has a stale embedded GitHub workflow version, not an original-game conflict. A preliminary unfiltered run was 237/238, with exactly that workflow test failing.
- `python3 test_stage312.py .../execution_199`: **12/12 hostile staging assertions PASS**, including fail-after-write rollback at six positions, corrupted CRCRLF, duplicate refusal and CRLF normal form.
- `ctest -R '^GK1_312_OriginalSourceEventPcWitnessProvenance$'`: **1/1 native Release PASS**.
- `test_gk312_stock_event_pc` with exact privately materialized original USA ROM SHA256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`: **33/33 source-static/negative assertions PASS**. Its positive example is constructed from source-defined oracle values and is expressly **NOT a fresh emulated stock CPU trace**.
- `cmake --build /mnt/data/gk_work/build_asan --target test_gk312_stock_event_pc -j8` and targeted `ctest`: **1/1 PASS with ASan/UBSan**. Attempt to build *every* sanitizer test target hit the runner's time limit; no unearned full sanitizer suite claim.

## Classification

VERIFIED_STATIC: source stage/test SHA, private original ROM SHA checked separately, no private bytes published. TESTED_PORT_LINUX: the complete inherited 0300+302/304+312 native Release binary, 237/237 eligible tests; 312 targeted ASan/UBSan. OBSERVED_RUNTIME_STOCK_FRESH / ARCHIVED: prior 0076 evidence retained, no fresh Mesen run. TESTED_PORT_WIN32_CI: **NOT RUN for EXEC0312**; last accepted predecessor EXEC0311 Win32 226/226 remains separately valid. PHYSICAL_XBOX / COMPLETE_CAMPAIGNS: NOT RUN. First playable source-authentic Spring OPEN.

GitHub public: copyright-clean C11 implementation, test and stage source only. PRIVATE ROM, RAM, graphics, audio, screenshots, save states and original campaign assets MUST remain PRIVATE. No source publication is an agent read/execute acknowledgement; do not disable recurring task until explicit Director stop or verified complete release.
