# Genghis Khan NES v1.06 — QA disposition

**Scope:** correctness and reproducibility of this economic research checkpoint. **Whole economic mapping:** INCOMPLETE. **Date:** 2026-11-04 UTC.

## Evidence checks

- Original-ROM source and capture suite: 72 methods, OK, exit 0; stdout/stderr preserved beside this document. Breakdown: 55 prior source, 8 new source, 9 captured-evidence methods. This is not 72 completed economic requirements.
- A failed first packaged run caught the invalid Rate190 quote capture. A clean cold-start rerun supplied actual China buyer prices and VM results. The failed log remains.
- Original 167 IDs and all fourteen prior register columns/cells preserved. Current statuses: 62 PARTIAL / 105 NOT RUN; TD01/TD02/TD11/TD15 gain partial evidence, no full parent obligation closes.
- Source mutations and corrupted-capture rejection are tested. Original-game execution is separately documented with screenshots/SRAM/raw requests and read-only traces.

## Independent review

A fresh read-only reviewer reran the complete 72-method suite (5.411 seconds, OK, exit 0), checked original ROM and SRAM hashes, compared the entire register to v1.05, checked table values and hiring deltas, and visually inspected key buyer/seller/Rate190 screens. Verdict: ready as a PARTIAL research checkpoint, subject to separate publication verification.

The review found one important provenance issue: the report initially carried a historical emulator hash. It was corrected to the actual executable's SHA-256, `f02685dafe559addf8dd8d769621a89777137d0c4592436aed65694a7e740676`, matching INPUT_HASHES.json. The ROM and baseline hashes were already correct. Generated Python caches are excluded from publication. Replay instructions were clarified to feed each action record's `value` object, not its JSON envelope.

The reviewer did not rerun complete Mesen navigation or certify unproved item names, Rate bounds, sale-credit generality, full-year economics, Windows/controller behavior or provider publication. These remain outside this checkpoint's verified claims; no completion ruling was made for them.

## Publication checks

The initial Drive ZIP upload failed, and the subsequent candidate was found to be truncated and invalid. A new archive was built, closed, CRC-checked, checked against all 405 evidence hashes, and atomically renamed before replacement under the same Drive file ID. The delivered archive is 1,109,966 bytes with 416 entries; SHA-256 `927028490b1d0bc26d87dadaffa8f43500d2ea8aa8f1e8c73d46da2a08761e2b`. Final provider readback matched this exact hash, passed the ZIP CRC check, and independently matched all 405 manifest hashes. GitHub and Drive text publication are verified separately by complete content readback and exact bytes, plus actual Drive parent and MIME checks.

**Acceptance boundary:** Verified bounded findings and usable continuation documentation; no full economy, game, or port completion claim.
