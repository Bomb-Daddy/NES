# GK1 EXEC0311 — Fix Windows CRCRLF false failure in EXEC0310

Date: 2026-11-15 (America/Detroit). Predecessor: EXEC0310, source SHA-pinned complete native game from EXEC0309.

## Reproduced Windows failure

The initial EXEC0310 Windows Actions run [43746387579](https://github.com/Bomb-Daddy/NES/actions/runs/43746387579) failed in step "EXEC0310 original fresh-title provenance C11 and hostile QA" **before compilation**. Log: 10/11 staging tests succeeded; `test_windows_crlf_source_with_sha_matching_normal_form` failed because `Path.write_text()` already produces CRLF on Windows, and a second `raw.replace(b'\\n', b'\\r\\n')` produced invalid CRCRLF. The production source-normalization gate correctly rejected it with `ValueError: malformed source newlines`; the fixture was wrong, not the runtime game, original ROM or stock RNG.

## Actual source/test change

One UTF-8 production-repository file changed, with GitHub blob-identity verification:

- [EXEC0310/test_stage310.py](../execution_310_original_pristine_title_gate/test_stage310.py) — updated in [commit df9cd1b723a8dadea47dc4688c04c577a556d2c6](https://github.com/Bomb-Daddy/NES/commit/df9cd1b723a8dadea47dc4688c04c577a556d2c6).
- The Windows fixture now uses `m.normalize(existing_cmake_bytes)` before converting LF to CRLF exactly once, proving no `b'\\r\\r\\n'` appears. This works whether the initial fixture was written on Windows or Linux.
- A new **twelfth** hostile test deliberately creates CRCRLF CMake input, checks the production stage refuses the malformed source, and verifies immutable before/after bytes (no mutation).
- `stage310.py` production gates, exact post-EXEC0309 C11 SHA, pristine-title semantics, source audit, rollback, all 14/27 countries, original Classic vs Enhanced behavior and downstream WinMain/NSIS workflow are unchanged.

## Reproduce from the public copyright-clean repo

```sh
python3 research/Genghis_Khan_NES/execution_310_original_pristine_title_gate/test_stage310.py
```

Then inspect [current Windows Actions](https://github.com/Bomb-Daddy/NES/actions/workflows/gk1-native-windows.yml), which on pushes to `execution_310_original_pristine_title_gate/**` runs the exact SHA-pinned complete inherited game through staging, full MSVC CTest, graphical smoke and installed NSIS launch. **Do not claim successful new Windows CI until that run actually succeeds.**

## Acceptance boundaries

VERIFIED_STATIC: identified the precise failing Windows testcase and implemented source/test correction (GitHub). TESTED_HOST_LINUX_PYTHON: original EXEC0310 11/11 passed locally before this fix; new test suite must be rerun separately. TESTED_PORT_WIN32_CI: initial EXEC0310 run FAILED before compilation; latest EXEC0311 run must be inspected. OBSERVED_RUNTIME_STOCK: previously archived/fresh Mesen records unchanged, no new Mesen execution. NOT RUN: physical Xbox, complete campaigns, independent stock first playable Spring. Original private USA NES ROM, original graphics/audio, RAM states, screenshots and checkpoints are never added to public GitHub.

Highest-priority game work still open: derive and generate original stock ruler/character/difficulty/first Spring RNG across all 14/27 countries plus three genuinely playable controller orders. This repair does not pretend to implement those game systems. Recurring work stays enabled until Director stop or verified accepted release.
