# Enhanced Run031 — Q15 enemy officer View — 2026-11-07

This is NOT a final English NES one-to-one release. Direct source parent: Enhanced Run030 full source SHA `e63609b40ca4b2eb3e450c9dd1db678020573722b1929af20b65fd8829447dee`, 18 parts independently read back from the original-game Drive folder `16mtUI6wWqVMB8w7axPQEa2-nVKIkYLKY`. New complete source ZIP SHA `6f3e6f62cd4bfd167464f727b8a942ef0fb6e0301209951d983330edcfe9c74c`. Only tactical_view.py changed and one test added. All other original-game economy and Body Points systems unchanged; Classic NES & DOS untouched.

**Player-visible problem:** Existing Enhanced View > Officer cycled own officer and did not allow selecting enemy officer at known location. Original Q15 disassembly and observed NES confirm no fog and unrestricted officer inspection. Now View > Officer enters a software controller map cursor; D-pad selects hex, A inspects live current ally or enemy with Skill/Arm/Int/War/Troops using real in-game fields, B backs out, invalid/fled/empty hex rejected without state/RNG/turn/disk mutation. Exact NES prompt strings/pixel UI still not authenticated.

**Executed Linux Python3.13 Xvfb:** pristine Run030 new test 5 expected FAIL / 1 PASS, fixed 6 PASS; wrong ally-only mutant 2 expected FAIL / 4 PASS; broad selected 149 PASS; full extracted source 149 PASS; separate process real disk save and reload fingerprint preserved. Actual Tk screenshot evidence supplied. 30 NES event golden traces and current native Windows/physical Xbox NOT RUN. Unfiltered full suite NOT certified.

To run tests after extracting source:
`xvfb-run -a env PYTHONPATH=.:tests python -m pytest -q tests/test_run0*.py`
Source parts are raw consecutive 8,500,000-byte chunks; concatenate 01..18 in order and SHA-256 verify against accompanying manifest; `ZipFile.testzip()` must return None.
