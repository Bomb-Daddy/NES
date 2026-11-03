# Genghis Khan I NES — Evidence, Provenance and Reproduction Index v1.04

**Date:** 2026-11-03; owner Zhuge Liang. **GitHub:** `research/Genghis_Khan_NES/`. **Purpose:** Direct the next operator to original reference authority, actual tested evidence and unknowns. **Status: original-game research INCOMPLETE.**

## Inputs and boundaries

- This repository is public; **never push the Director's NES ROM, cartridge RAM, savestates, artwork/screenshot dumps, commercial game assets, passwords, or private-game files**. Only authored research documents, code-independent fixtures and noncopyrighted metadata summaries are included.
- Original ROM SHA-256: `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3` (input required privately). No actual ROM is in GitHub.
- For source authority: [KOEI current authority/supersession register](https://docs.google.com/document/d/1qEVhDdiG2oH8LvPz0jan5DCtFFVxHhQWdWRUpduTllQ/edit). Keep original Genghis NES research segregated from ROTK II NES, DOS, and Original KOEI Windows game.

## Prior versioned original research and testing

| Date/version | Artifact | Scope and evidence class |
|---|---|---|
| 2026-10-26 | [Genghis NES Master](https://docs.google.com/document/d/1MCWq6KnvkV-u0GlDWnA8Ct47BiFmfQtQmEECVKNljLA/edit) | Manual/guide-derived mechanics, distinct from executable proof |
| 2026-11-03 v1.00 | [First live Mesen captures, historic in later archive](https://drive.google.com/file/d/15Jaeo6bKCAaY6IuxdmGZ1HTRNJLywgyI/view) | Both scenario starts, merchant pricing and one commodity sale; OBSERVED_RUNTIME limited |
| 2026-11-03 v1.01 | [Live ROM investigation](https://drive.google.com/file/d/1KtDHN3C3MzLCoWy15BtRSTf9BTsJEbtL/view) | Tax rate, Assign Labor, Give Gold, Pass, original RAM field watch |
| 2026-11-03 v1.02 | [Live ROM investigation](https://drive.google.com/file/d/1ltE8ST9LZKYMXc4uVX9nVKb4wCAPVkjb/view) | Give Food, Extra Tax, Train Troops, China buy menu only, failed War navigation |
| 2026-11-03 v1.02 | [Full command crosswalk and prior captures](https://drive.google.com/file/d/1M4vObtuZrQ35fbJHn-53ffg4xETIo7LA/view) | 14 root commands documented, only some tested |
| 2026-11-03 v1.01 | [AI source-only QA](https://drive.google.com/file/d/11oqOimIwqnikvnUZITiLTyNCUOa6vIDA/view) | Archived regression and source byte comparison, original-game acceptance unchanged |
| 2026-11-03 v1.02 | [Original ROM static map](https://drive.google.com/file/d/1W1QJfMX9_c4EILz6eCxmWaa67PP_wTob/view) | 256 VM dispatch entries and opcode examples; VERIFIED_STATIC |
| 2026-11-03 v1.03 | [VM arithmetic and source/static QA](https://drive.google.com/file/d/1Z-qQjW1eBCxJuGA7UupSZBJ5l2CUIGGk/view) | Rate/adjacent word and VM handler operations; bounded 6502 replay, not whole original runtime |
| 2026-11-03 v1.03 | [Runnable bounded VM analysis evidence](https://drive.google.com/file/d/1EL5YbWuWPTg9rjDeQI3EbqehDarb7_xq/view) | 47 archived technical tests, test fixtures and output logs; no game binaries |
| 2026-11-03 v1.00 | [Outstanding proof register](https://drive.google.com/file/d/1r6UODVd97ws3fjCn1dcc1eiOe4i4qI4B/view) | Every one of 167 open requirements, including 9 conditional port rows |

## Supplemental recovered local research not previously published as the latest public summary

- `gk_static_v1_03/GENGHIS_KHAN_NES_STATIC_VM_RESEARCH_SUMMARY_v1.03.md`: branch `$91BA→$91D0`, Rate-half, `B7` 48-helper map, three Rate+100 scripts.
- `gk_static_v1_03/GENGHIS_KHAN_NES_NATIVE_HELPER_ADDENDUM_v1.03.md`: 48-entry helper-pointer table and complete observed B7 helper ID sequence (original-ROM bytes).
- `gk_static_v1_03/GK1_167_item_STATIC_VERIFICATION_MATRIX_v1.03.csv`: authoritative latest archived row-by-row static analysis snapshot incorporated into v1.04 GitHub CSV.
- The GitHub master, command crosswalk and 167-row matrix incorporate all substantive findings from these recovered local files. No original ROM or saved RAM bytes are distributed.

## Reproduction gates

1. Verify ROM hash and proper MMC1 bank mapping from private authorized source.
2. Re-run v1.02 or v1.03 archived pure-source Python tests with exact inputs; distinguish a passed test from original NES game parity.
3. For `Trade`, trace actual gameplay execution to the VM Rate arithmetic plus callbacks; test completed buy/sale, price rounding, vendor differences, negative controls.
4. For `People/Draft`, complete nonzero draft and compute real population and workforce change with cancel and overflow tests.
5. For season, capture every phase through next Spring, repeat varied staffing and document actual harvest, tax and payroll.
6. Preserve source hashes, exact commands, stdout, stderr, exit codes, RAM/ROM bank mapping and screenshots in the Drive evidence source; do not claim Windows or Xbox acceptance from Linux testing.
7. Close each of the 167 CSV IDs only after independent source-appropriate evidence; preserve all previous negative tests and regressions.

## Source authority hierarchy

Director decisions > latest current authority/supersession register > original NES ROM and reproducible Mesen outputs > original official manual and correctly identified original-NES guides > bounded static inference > new-game inspirations. Source authority does not mean a reported approximate formula is exact original code.
