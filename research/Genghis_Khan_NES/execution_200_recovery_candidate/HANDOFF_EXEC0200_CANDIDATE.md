# GK1 EXEC0200 — Uncertified predecessor-recovery and native controller QA candidate

November 11, 2026. This is actual linked C11 source and executable Linux tests, **NOT** an accepted 2.00 native release, original NES complete first-Spring simulation, or a reconstructed byte-identical official EXEC0199.

## Authority and provenance

Verified newest officially published handoff: EXEC0199. Drive master v3.53 and GitHub `execution_199_delta`. The currently accessible EXEC0196 complete historical archive contains the last byte-exact full predecessor. The EXEC0197 complete source/official delta is absent from accessible Drive and GitHub. EXEC0198 and EXEC0199 real patches were recovered and applied to a conservatively reconstructed EXEC0197 from the *actual complete EXEC0196 native C11 project*, using EXEC0197's published implementer QA handoff. Two shell hunks were manually reconciled to the reconstructed predecessor and no private ROM content included.

The 631-file authoritative 199 SHA-256 manifest comparison finds **619 exact matches, 12 known mismatches, zero missing**. A generated `.pyc` and the 11 197-derived files are not asserted original. Never use this archive to overwrite or falsely certify the independently published EXEC0199 release. The reproducibility verifier rejects unknown mismatches and insufficient manifests; it does not make the 12 divergences original.

## Actual new code and tests

`GK1_EXEC0200_RECOVERY_CANDIDATE_Controller_Tax_Smoke.patch` adds a real native `--smoke-classic-tax-recovery <private-original-stock-capture> 120` controller simulation to existing `gk1_native_shell.c`, not a standalone prototype. It loads original-stock first Spring through existing bank-aware auto-entry, exercises pad MENU → TAX A → change tax via D-pad → A one order → Y Undo restores three orders and source tax → repeat Y no-op → B exits; verifies 83-byte original records and country world are unchanged. The native C11 source continues to refuse forged or pre-setup transaction states. TAX remains manual/port-derived, not original stock tax ability or morale parity.

**TESTED_PORT_LINUX this execution:** release 162/162, debug 162/162, restored original-stock 33 TAX undo + 52 tax forecast assertions, ASan/UBSan focused authentic 33/52 assertions, three 120-frame 1280x720 Xvfb software-pad controller TAX/Undo runs (fresh WorkRAM, archived Mongol SaveRAM, archived World SaveRAM), plus fresh/archived classic ASSIGN→TRADE reference smoke and Debian CPack generation. The exact original ROM and snapshots are not inside this package. Installer is a *research candidate*, internally still labeled 1.99.0-dev1 pending verified complete 197 recovery, not a certified release. The smoke does not prove hardware Xbox or original Mesen behavior.

**VERIFIED_STATIC:** authenticated current 199 handoff/manifest, 619/631 byte-exact candidate files, original private ROM identity from Director's actual source, clean delta replay and hashes. **OBSERVED_RUNTIME_STOCK_FRESH_RESET/ARCHIVED_RESTORED:** inherited original Mesen observations only, not rerun. **NOT RUN:** native Win32/NSIS, hardware Xbox, campaigns, original NES first-Spring startup/RNG source parity, merchant scheduling, AI, diplomacy, dynasty, battle aftermath.

## Reproduce without private files

`cmake -S execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build -j10 && ctest --test-dir build --output-on-failure` from the unzipped full candidate source root. Test requires sibling `execution_034`, `_039`, `_045`, `_047`, `_049`; included in package. `python3 QA/verify_reconstructed_199.py execution_199 QA/EXACT_PUBLISHED_EXEC0199_Source_SHA256_Manifest.json` confirms **619/631 known candidate scope**. Add private source files only for additional stock condition testing. Do not place original ROM/Mesen state/evidence in public GitHub.

## Mandatory next priority

Recover exact original EXEC0197 source (or its integration patch) and make 631/631 reproduce, then implement independent real source-authentic 14/27-country first Spring RNG, player/ability/difficulty and season progression, and source-authenticated real strategic commands. Until that is done, this is a provisional developer reconstruction, not a full game release or agent read/execute acknowledgement. Keep the hourly work active.
