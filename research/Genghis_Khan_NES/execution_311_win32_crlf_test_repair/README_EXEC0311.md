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

## Final source-provenance correction and Windows CI226 acceptance (supersedes earlier pending status)

An additional review of the *real* inherited source proved that EXEC0310's proposed C11 gameplay fix was **already present in EXEC0277**. The correct exact source includes both world and pristine setup guards:

```c
if(a->setup_stage[0]==GK53_SETUP_PLAYERS)
  return a->active_world==0u && pristine_title(a,0u);
```

Exact source SHA256 **04c5d36bb85462610f5728c8f8a22c82a007b123604c7dafcbfede5b6622803a** from post-EXEC0309 was preserved **byte-identical**. This supersedes any preceding implication that EXEC0310 or EXEC0311 originally implemented this production behavior. We fixed an actual Windows CI test/provenance failure and added an executable native C11 regression without overwriting already-correct source.

- Initial 43746387579 failed because malformed CRCRLF was manufactured by the test on Windows. This was fixed in `test_stage310.py` (normalizing before CRLF conversion), with a dedicated negative control against genuinely malformed CRCRLF.
- 43747227167 passed 12/12 hostile staging tests, but stale one-line source patch expectation rejected original post-EXEC0277 source.
- 43747719876 passed **13/13** hostile staging tests and SHA-pinned staging, but Windows workflow had a stale assertion demanding an incorrect one-line replacement. Corrected workflow requires the real two-line guard, checks original source SHA unchanged, and rejects the unsafe old shortcut.
- **Final Windows CI SUCCESS:** [GitHub Actions run 43747883967](https://github.com/Bomb-Daddy/NES/actions/runs/43747883967) at exact workflow commit `349dc48fe409861dc5518cab3a2b7494240338f5`, completed **2026-11-15 18:40:36 UTC**. MSVC completed the full inherited game build; **226/226 native Windows CTests PASS**, including `GK1_310_OriginalFreshTitleSetup_IsPristine`; graphical WinMain/software-controller simulation, NSIS setup, silent installation, installed executable launch and QA manifest all PASS.
- Exact verified QA artifact ZIP: GitHub Actions artifact **14175540003**, **579136 bytes**, SHA256 `e1d097ed53d97752e86dca614f128e7fabd06cd21147283029b6f712cb3ca5b0`, archive integrity PASS. Inside: Windows installer **612729 bytes**, SHA256 `0ebde3913caec6e8cbbcf05ccf2b19c989ace875b14af51498a38b95636530e4`. QA manifest explicitly says `release_accepted=false`, `physical_xbox_tested=false`, `full_campaign_tested=false`.
- Correct [Collaboration Drive EXEC0311 numbered implementer/QA handoff](https://docs.google.com/document/d/1a22UXKLFUXriV18b00wTqyIVCoq7NFIbQW0wB8JJo5Y/edit), master/ledger appended without deleting predecessor content. GitHub stores copyright-clean source only; original NES ROM/country RAM/assets/checkpoints remain PRIVATE.

Classification: **VERIFIED_STATIC PASS**; **TESTED_PORT_WIN32_CI PASS 226/226, WinMain/NSIS**; **TESTED_PORT_LINUX new full suite NOT RUN**; **OBSERVED_RUNTIME_STOCK fresh/archived prior source preserved, not rerun**; **physical Xbox, full campaigns and authentic independent original first playable Spring NOT RUN**. The game remains QA-only, not an accepted release. Continue existing recurring work until Director explicit stop or verified full release.
