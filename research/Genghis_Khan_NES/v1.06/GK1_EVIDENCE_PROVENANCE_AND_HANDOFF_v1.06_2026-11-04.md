# GK1 Economic Research Handoff 006 — v1.06

**Author:** Zhuge Liang / ChatGPT-Codex. **Recipient:** next authorized original-ROM researcher; Nobunaga receives implementation input only after the Director scopes implementation. **Date:** 2026-11-04 UTC. **Project:** original English NES Genghis Khan. **Research status:** PARTIAL / INCOMPLETE. No acknowledgment or downstream execution is claimed by publication.

## Read first

1. [Economic Mapping v1.06](https://github.com/Bomb-Daddy/NES/blob/main/research/Genghis_Khan_NES/v1.06/GK1_ECONOMIC_MAPPING_v1.06_2026-11-04.md): source formulas, actual trades, Rate trials, known errors and acceptance boundaries.
2. [Full 167-obligation register v1.06](https://github.com/Bomb-Daddy/NES/blob/main/research/Genghis_Khan_NES/v1.06/GK1_COMPLETE_167_PROOF_MATRIX_v1.06_2026-11-04.csv): original IDs/requirements retained, with additive current evidence/status columns.
3. [Private v1.06 evidence](https://drive.google.com/file/d/1e_Prnv32dmszNZjhg_nYfF4bnlnFpT-J/view): captures, SRAM, raw action logs, traces, manifest and tools. Do not upload those game assets or the private ROM to the public repository.
4. [v1.05 arithmetic reconstruction](https://drive.google.com/file/d/1Z0WRR6c_lSyMYvOwFYh6sojjyCDAkBmz/view) and [preserved v1.04 master](https://github.com/Bomb-Daddy/NES/blob/main/research/Genghis_Khan_NES/GK1_MASTER_FINDINGS_ALL_OPEN_WORK_v1.04_2026-11-03.md).

## Decisions now supported by evidence

- The table quote routine is connected to actual merchant screens. China row0 and Islam row1 are observed; Uighur row2 identity remains inferred.
- Buyer price in tenths is floor(baseByte*(Rate+100)/100). Seller price is max(1,floor(buyerTenths*90/100)) in the tested source domain.
- Bounded affordable purchase debit rounds the total cost upward to whole gold. The source routine and actual 1-unit/11-unit acquisitions agree.
- One same-Rate buy/resale lost two gold and spent two orders. It does not establish later-season arbitrage profitability.
- Hire8, commodity carryover into Summer and controlled Rate-price probes now have actual game evidence. Complete hire dilution, annual production and stock limits are still open.

## Next executable task

**Owner:** Zhuge Liang/Codex original-ROM researcher. Restore the linked Mesen setup and exact private ROM; use the evidence archive's driver and known cold-start prefix. First finish a source-grounded name/icon/index/stock crosswalk, then purchase a named good, carry it across a natural Rate transition, and sell the acquired amount. Record season, merchant, quantity, quote, gold, stock, orders and abilities at each step; preserve screenshots, SRAM, complete input logs and source hashes. Run the replay/capture suite with the report's command, then add transaction-specific evidence assertions and mutation controls. Follow with matched workforce-history harvest/payroll trials.

**Acceptance:** named-item identity is proved; positive buy and later-Rate sale complete; acquired quantity is conserved or every intervening change is accounted for; all gold and action/ability costs are measured; a second cold start reproduces the result. Source-only arithmetic is not command acceptance. Do not import the custom KOEI game's marginal-volume or Charm rules into this NES mapping.

## Limitations and routing

All 167 original obligations remain open (62 PARTIAL, 105 NOT RUN). The narrow findings in v1.06 are verified but do not close their broader requirements. No Windows/native-controller validation or port implementation was performed. The final sale-credit routine, item names, Uighur conditions, natural Rate bounds, full-year tax/production/payroll and economic interactions remain unfinished. The report lists mislabeled exploratory captures and failed probes explicitly; never infer outcomes from their filenames.

GitHub preserves the previous versions. A matching raw Markdown copy of this handoff is to be delivered to `Retro Gaming/Collaboration`, with folder, MIME and readback verification. Publication is not proof that another agent has read or acted on it.
