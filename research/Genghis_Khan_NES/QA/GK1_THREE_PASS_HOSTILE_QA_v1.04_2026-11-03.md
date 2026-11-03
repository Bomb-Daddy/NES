# Genghis Khan NES — Three-Pass Hostile QA and GitHub Publication Verification

**Version:** 1.04 · **Date:** 2026-11-03 · **Repository:** `Bomb-Daddy/NES` · **Branch:** `main`
**Disposition:** DOCUMENTATION PUBLICATION VERIFIED. ORIGINAL-GAME RECONSTRUCTION INCOMPLETE.

## QA1 — Completeness and inventory
**19/19 PASS.** Reconciled source v1.03 matrix to output v1.04: all 167 unique ID rows, original status counts 58 PARTIAL and 109 NOT RUN, original requirement and proof text, v1.03 notes, all 14 root commands, all subsystem groups, all nine conditional port rows. Original command crosswalk v1.00 retained verbatim under v1.04 addendum.

## QA2 — Evidence fidelity, contradictions and privacy
**20/20 PASS.** Verified US ROM hash *privately*; no ROM/SRAM selected for publication. Reviewed observed runtime actions vs static and source-only evidence, including tax-vs-market Rate, incomplete China Buy and War, unproven final price, tax rate lower-bound discrepancy, RAM/VM operand and helper references, and no false port acceptance. Local QA assertions had two over-strict matching patterns fixed before the successful run; no verified research findings were altered to make these pass.

## QA3 — GitHub remote readback and negative controls
**20/20 PASS.** Fetched each actual committed GitHub file from `main`; compared its full UTF-8 text with the independently staged Drive source (exact string equality, including 141 KB proof matrix), verified blob SHAs, publication scope, source references, 167 unique IDs, 14 root command sections and negative mutation control.

- [x] Correct repository and default branch
- [x] GitHub writing permission confirmed
- [x] Five distinct research file paths
- [x] All paths confined to separate Genghis research subtree
- [x] GitHub text equals Drive staged source: master
- [x] GitHub text equals Drive staged source: matrix
- [x] GitHub text equals Drive staged source: crosswalk
- [x] GitHub text equals Drive staged source: index
- [x] GitHub text equals Drive staged source: raw
- [x] GitHub blob SHA returned for each file
- [x] Master includes all 167 named IDs
- [x] CSV has 167 CSV rows
- [x] CSV retains all OPEN acceptance states
- [x] Crosswalk retains 14 commands
- [x] GitHub raw QA1 shows 19 of 19 passed
- [x] GitHub raw QA2 shows 20 of 20 passed
- [x] Master still labels overall reconstruction incomplete
- [x] No GitHub paths are game ROM/save/image assets
- [x] Citations to original Drive evidence preserved
- [x] Negative mutation control detects mismatched source

## Published GitHub source files

- [GK1_MASTER_FINDINGS_ALL_OPEN_WORK_v1.04_2026-11-03.md](https://github.com/Bomb-Daddy/NES/blob/main/research/Genghis_Khan_NES/GK1_MASTER_FINDINGS_ALL_OPEN_WORK_v1.04_2026-11-03.md) — blob SHA `2d8c553c0b80d047650e1a59dd5cf5a372f85ec4`
- [GK1_COMPLETE_167_OPEN_PROOF_MATRIX_v1.04_2026-11-03.csv](https://github.com/Bomb-Daddy/NES/blob/main/research/Genghis_Khan_NES/GK1_COMPLETE_167_OPEN_PROOF_MATRIX_v1.04_2026-11-03.csv) — blob SHA `02bc54db11dd8e7b88609803272d6a8386f4cd26`
- [GK1_COMMAND_CROSSWALK_v1.04_2026-11-03.md](https://github.com/Bomb-Daddy/NES/blob/main/research/Genghis_Khan_NES/GK1_COMMAND_CROSSWALK_v1.04_2026-11-03.md) — blob SHA `70e413df185e52b1f8915da5bc749a89b9e1b135`
- [GK1_EVIDENCE_PROVENANCE_AND_HANDOFF_v1.04_2026-11-03.md](https://github.com/Bomb-Daddy/NES/blob/main/research/Genghis_Khan_NES/GK1_EVIDENCE_PROVENANCE_AND_HANDOFF_v1.04_2026-11-03.md) — blob SHA `e09c46b4f5b1496287072960dbb09ce45960b388`
- [GK1_QA1_QA2_RAW_v1.04_2026-11-03.json](https://github.com/Bomb-Daddy/NES/blob/main/research/Genghis_Khan_NES/QA/GK1_QA1_QA2_RAW_v1.04_2026-11-03.json) — blob SHA `d93eac9aae8e425512fde3f6caaa3296ab95c15e`

## Acceptance boundary and outstanding work

- All **167 original register obligations remain OPEN** (58 PARTIAL, 109 NOT RUN), including nine conditional future-port items. This QA does not reduce that count.
- Follow the master, 14-command crosswalk and the accompanying full 167-row CSV for every outstanding experiment and proof requirement.
- The bytecode VM dispatcher, Rate arithmetic, and native helpers have verified **static** evidence, not final buy/sell economics or a full original gameplay traversal.
- No playable Windows port or full NES/SNES mechanics parity was created; this work is a **documentary handoff** only.
- Separate Drive evidence archives retain historical screenshots, SRAM snapshots, tools, logs and failures. Those binaries and private ROM data are intentionally absent from this public GitHub directory.

## QA failure history

The first two source-QA runs stopped on overly strict text-match assertions (case/phrase and combined input-shape checks). The assertions were corrected and both passes reran to full success; no substantive source data was changed. No third-pass remote failures occurred.

**Final QA:** 19/19 + 20/20 + 20/20 publication checks PASS. **Research completion:** INCOMPLETE.
