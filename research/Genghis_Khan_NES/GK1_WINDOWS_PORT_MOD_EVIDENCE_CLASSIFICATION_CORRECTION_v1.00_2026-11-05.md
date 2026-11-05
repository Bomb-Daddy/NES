# ZL-GK1-PORT-HQA-0011 — Correction: modified NES ROM is authoritative evidence for modified NES behavior

**Version:** 1.00 | **Date:** 2026-11-05 (America/Detroit) | **Author:** Zhuge Liang  
**To:** Nobunaga (Windows implementation), Cao Cao (independent QA), Project Director  
**Applies to:** v1.01 Genghis Windows gap-closure plan and HQA-0010. Corrected active execution plan: **v1.02**.
**Disposition:** Evidence-classification defect CONFIRMED and documentation CORRECTED; no new gameplay testing or Windows port execution claimed.

## 1. Root cause and correction

The previous wording said that modded ROMs/source were merely research leads, or evidence that did **not** count as proof. That collapsed two independent propositions:

1. A modified NES ROM is **direct primary evidence for its own implemented and actually executed modified behavior**.
2. A modified ROM alone does **not** establish that its altered behavior was in the *unmodified stock* NES game.

The first is true and must be adopted without equivocation. The second remains true but was overextended. Original and modified ROMs can each be an **authoritative behavior oracle within their exact named build/mode and tested circumstances**. Future Windows development should use both, rather than treating mod evidence as second class. The exact purpose of the modifications is to alter the game; naturally their behavior can differ from stock.

**Evidence semantics (effective immediately):**
- **VERIFIED_STATIC_MOD:** independently inspected hash, patch source/bytes, manifest and original-code execution for the *modified* build; supports exact applied implementation, binary identities and bounded routines, not an untouched stock game.
- **OBSERVED_RUNTIME_MOD:** actual native NES Mesen controller/screen/SRAM/operation captures from the *modified* ROM; directly proves that gameplay event for that build and state.
- **REPORTED_UNREPRODUCED_MOD:** a specific prior agent's reported native ROM test that was not independently rerun in the current audit. It is genuine existing evidence to preserve and use, with honest provenance; no gratuitous retest just because the reviewer changed.
- **TESTED_PORT:** the independently built Windows program was run and produced its recorded result (separate achievement, not implied by the ROM).
- Stock original evidence keeps its own VERIFIED_STATIC / OBSERVED_RUNTIME labels. Use exact build hash, exact test case and scope instead of using bare "not proof" language.

No obligation to retest **all** known passing mod behavior before port implementation. Consume existing captured original/Mesen evidence, source, tests and fixtures as the implementation reference; only investigate uncovered paths or discrepancies. Where the Director wants an altered feature faithfully carried into Windows, its *modified* ROM and passing results are the correct fidelity oracle.

## 2. Evidence already established by delivered ROM modifications

| Modified build | Behavior and evidence that count | Scope / genuine remaining proof |
|---|---|---|
| **Demobilization + Fonzarelli**; ROM SHA-256 \`61ad28688d503231d12537b2cbebb073ae5f91ec1391f825c1d05312436393be\` | Completed native People → Troops → Demobilize transfer: q41, Troops61→20, reserve34→75; required order/L5/P5 debit, original Draft preserved, cancellation/refusal conditions, hiring/labor/demobilization interactions. Native War displayed 200 men as five 40-man units; Flee returned four strategic Troops, with preserved reserve, labor ledger and strategic order effects. Fonzarelli rename and saved-name migration are implemented/tested in the reported package. | These are **proof of this mod's tested paths**, not a speculative design. A post-battle serialization fixture was restored after a failed Save entry before a later cold-load check; it is not an uninterrupted battle→natural Save capture. Other combat branches/AI remain separate unknowns. |
| **Shared BP / Rest + Demobilization + Fonzarelli**; ROM SHA-256 \`4764d46359581be1e7d12bcb85bd375c84cd4497d1084d9d306a6e884afbe7c5\` | 43 source/instruction tests; 80 nonoverlapping, guarded patches, independently rebuildable source; nine actual native Rest cases including 170→829 with two orders, 1/2/3 remaining-order variation, cancellation and held-input controls; native Train/Help/war quote UI; real native Save→process close→new-process Load→continue (160 BP, 1 order, Troops50, reserve45, subsequent Rest489 BP). Additional twelve isolated *native routine fixtures* for AI, duel, victory, espionage, epidemic, exhaustion/death; 43 named World-scenario full records synchronized; HD main/Rest/Train images verified. | **Proven for tested modified-ROM paths.** Natural AI selection of training was not observed; some Treaty/foreign View/Policy/Move/Child/full battle routes were not successfully navigated. The isolated-event fixtures prove bounded routine behavior, not natural full-campaign trigger frequency. No native Windows game has been executed. |
| **Seasonal HD graphics package** | Pack contains Director-approved seasonal artwork and modified-compatible trigger files; packaged images preserved across the shared-BP copy and its specific main/Rest/Train presentation was native-captured/reviewed. | Four-season map transitions, live province overlay priority and all gameplay scenes still need complete runtime coverage. This does not invalidate the verified pack/affected-screen behavior. |

Source records: [Demobilization MOD-0005](https://drive.google.com/file/d/1AbGkCwjaJ2HXXgnbb3I4v-NDzfwsw2q1/view), [Shared-BP NOB-0008](https://drive.google.com/file/d/19Z7HbseZUEDYB37_NTxM0j8eZ5sSdkmA/view), [Shared-BP developer source/QA archive](https://drive.google.com/file/d/16NysbtEJGeCaVbLzSuEPRWOt8kUvijq9/view), and [graphics handoff](https://drive.google.com/file/d/1YJSCcMTl-HZS6eYoOxkwQfvJg_LHv6Sf/view).

## 3. Required change to Windows implementation workflow

- **Treat proven modified-ROM behavior as a reusable, authoritative feature specification** for the corresponding enhanced Windows variant. Import exact costs, state transitions, previews, new menu navigation, Rest recovery rules, save migration and captured screen effects into port modules immediately. Do not send these to a new "research discovery" phase.
- **Add parallel parity tracks:** (A) stock Classic Windows vs stock NES and (B) feature-modified Windows vs the *exact modified NES candidate*, each with its own ROM SHA. These do not have to be the same gameplay design. Explicitly name what features enter the released Windows build; preserve Director approval and distinguish compatibility with saved games.
- For the combined Shared-BP mod, use the Director's confirmed **33% per remaining order**: \`gain = min(maxBP-currentBP, floor(maxBP * 33 * remaining_orders/100))\`; 1/2/3 remaining orders yield 33/66/99% before caps, not an automatic full refill. The source's 999 BP cap remains an implementation choice, not a newly asserted Director-approved universal balance.
- Source written but not game-executed is established implementation/source behavior; actual native gameplay outcome is established when supported by the original Mesen logs/fixtures. Keep those two evidence labels distinct, not dismissed.
- Do not require stock behavior to prove **how a new invented command is supposed to work**. Stock comparison is necessary only to say which surrounding game mechanics were preserved or changed.
- Each accepted modified behavior maps to a named Windows function and a portable acceptance case, regardless of whether Classic originally had that function. A row may be **MOD PASS VERIFIED / WINDOWS NOT RUN** without contradiction.

## 4. Concrete corrections to prior HQA

- HQA-01/02/06/07/08/09/12/15: keep the byte-mapping and Classic-versus-Enhanced distinction, but remove any implication that passing mod tests are only provisional clues. They are *accepted modified-ROM implementation/runtime evidence*.
- HQA-15 originally said "mod source can serve as a pointer, not a broad PASS." Correct wording: **verified mod source proves its recorded patches and bounded instruction behavior; executed modified-ROM Mesen tests prove the captured modified game actions; neither alone makes the entire Windows port TESTED_PORT**.
- For HQA-05, "isolated AI native fixture" is valid evidence for the modified AI **routine**, while natural scheduler selection is a separate unresolved question. Both can be true simultaneously.
- Preserve source-specific uncertainties exactly: do not call the entire battle system verified from one successful Flee; do not describe the Save fixture as uninterrupted; do not infer every UI screen works from the verified Rest screens.
- The 167-item November 4 source matrix remains a stock-original research register (61 PASS / 40 PARTIAL / 66 NOT RUN). Its count is not a refutation of later modified NES evidence. Maintain **stock original status**, **modified NES status**, and **Windows port status** separately.

## 5. Immediate owners and acceptance

**Nobunaga:** Use the verified Demobilize and Shared-BP implementation records to implement their respective approved Windows features directly; first deliver controller-operable Rest/Demobilize screens and state changes in the Windows engine, each compared to the matching modded ROM. Continue separate Classic parity for unchanged gameplay. Version-tag saves.

**Zhuge Liang:** Reclassify captured mod evidence under correct build identities and link known mod outcomes to Windows feature contracts. Remove unnecessary retesting and close research steps already answered by executed native modified-ROM evidence.

**Cao Cao:** Hostile QA the Windows-versus-modded-ROM parity of actual actions with their original captured Mesen fixtures, while separately checking Classic-versus-stock fidelity. Do not issue a defect solely because a modified mechanic differs from stock by design.

**Acceptance:** A corrected evidence register identifies modified mechanics as proven for tested builds, preserves explicit unknowns, and pairs each proven feature with its Windows consumer. Actual native Windows verification remains outstanding until executed.

**End. This corrects evidence classification, not the user's design choices or previously completed ROM implementation.**
