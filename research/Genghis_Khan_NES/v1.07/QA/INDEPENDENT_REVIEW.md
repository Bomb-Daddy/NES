# Independent economic completion review — v1.07

Reviewer: Codex hostile evidence audit. Date: 2026-11-04 UTC.

**Technical disposition: PASS VERIFIED for the scoped economic mapping; ready for publication.** No unresolved arithmetic, native boundary, seasonal-ledger or fresh-log acceptance gap was found in the completed domestic, Trade, seasonal, logistics, scenario-economic, yearly ROI and identified AI-economic batches. This is not completion of all167 original game requirements. Publication is a separate remaining action owned by the parent; RE14 stays IN PROGRESS until its versioned manifest, destination, readback and receipt are verified.

This reviewer independently executed offline source and byte/log audits. The original-ROM native captures were produced by the parent in Mesen; this reviewer did not operate or change either native bridge. Source execution is VERIFIED_STATIC; captured original behavior is OBSERVED_RUNTIME. Neither proves a Windows port, physical controller test or completed campaign.

## Independently executed verification

| Evidence | Independent result and receipt |
|---|---|
| Clean copied original-instruction package | 262 completed unittest methods plus15 setup cases, exit0. Exact command, environment, inputs and complete per-suite stdout/stderr: `source_results/RUN_RECEIPT.json`; top-level `source_run.stdout/.stderr`. |
| Additive Move Base source package | 19 methods, exit0; `base_run_receipt.json`, `base_run.stdout/.stderr`. Counts are methods, not281 played scenarios. |
| Historical native capture audit | 549 snapshot metadata/SRAM pairs matched, 84 native Trade transactions independently recomputed, 12 initialized event controls matched. `CAPTURE_AUDIT.json`. Historical log limitations are retained below. |
| Closed fresh main replay | 490 snapshot metadata/SRAM pairs matched; all84 Trade transactions and12 initialized event controls independently recomputed. `FINAL_REPLAY_CAPTURE_AUDIT.json`. |
| Final integrated native-evidence verifier | Independently executed `batches/native_verification/verify_native.py --evidence-root batches/runtime/final_replay --historic-root batches/runtime/live --boundary-root batches/runtime/boundary_completion --require-complete-log`: 245 accepted cases and1388 integrity checks PASS, exit0. Exact receipt: `../native_verification/run_final_replay_20261104T142006.979207Z/receipt.json`. The explicitly superseded failed repeated-gift attempt remains visibly failed in the report; it is not counted as accepted evidence. |
| AI economic native boundaries | All10 cases independently replayed from captured native entry SRAM/zero page through original6502 instructions; all19 economic words and final48-bit RNG match,381863 original instructions. `AI_NATIVE_INDEPENDENT_AUDIT.json`. |
| Domestic completion | All48 unique selected cases independently pass;281 snapshot records checked, with full83-byte country state where applicable. Original repeated-gift navigation failure remains retained. `DOMESTIC_NATIVE_INDEPENDENT_AUDIT.json`. |
| Actual levy and following year | Six levy/season intervals independently reconcile all mapped fields and ten goods. Both native levy commits match original instructions and final RNG,31477/31483 instructions. `LEVY_YEAR_INDEPENDENT_AUDIT.json`. |
| Second natural Mongol year | Six ledger intervals reconcile. Natural theft original-instruction replay matches all83 country bytes and final RNG,22309 instructions. `YEAR2_NATIVE_INDEPENDENT_AUDIT.json`. |

The 84-transaction check covers three merchants,17 buys and11 sales each. The formulas compare actual before/after Gold, stock/Arms, Judgment and orders; the acceptance does not merely trust producer `checks:true` fields. The integrated verifier additionally checks the negative Trade controls, logistics, later-Rate resale, World year, theft controls, levy refusal and other captured batches.

The first historical metadata audit incorrectly assumed every snapshot's `abilities` field belonged to Temujin. The snapshot writer actually follows country1's current officer pointer, which Move Base swaps. Six apparent mismatches were reviewer-assumption errors, corrected by decoding that pointer. `CAPTURE_AUDIT_initial_auditor_pointer_error.json` remains preserved. The cross-country resale correctly uses explicit Temujin/destination readings instead of country1's replacement officer.

## Native obligations completed after the preliminary review

| Requirement | Accepted evidence and boundary |
|---|---|
| AS01 | Native Town0/66/67-invalid, Masons0/65/66-invalid, Artisans0/67/68-invalid and Foodworkers0/88/89-invalid; conserved P103/reserve34, exact debit and no-charge invalid paths. Shared preview/cancel/no-op proof retained. |
| GV03 | Allten controlled specialty indices atq8/Rate101; Jewelsq1/101 andq8/50; P50/P200 controls. Exact chosen-stock and morale changes, other stocks unchanged. The explicit specialty-byte control does not claim ten naturally visited countries. |
| GV04/GV05 | Allthree gift modes at empty stock, overstock, cancel, exact stock and morale cap; Planning4 refusal. Matched Morale998/Gold8 no-gift control versus two actual sequential Gold1 gifts, followed through Summer. Repeated-gift failure and corrected rerun remain separate. |
| PE02 | Draft0 rejection,10 acceptance,11-invalid, eligibility0, P998 maximum1, P999 refusal; actual TrainPeople49→50 then Draft17. Resident conservation, dilution, original eligibility rounding and costs independently checked. |
| TX04/TX06/TX07 | Actual levy1, Summer, Autumn flag reset, successful Autumn reuse, Winter and following Spring all reconcile. Spring1176 ends Gold463/Food248/Troops28/reserve36/use1. The separate Ally transfer contributes10 Gold. Both levy commits match native RNG; loss does not restore reserve. |
| TX06 message | Original refusal reaches bytecodeEB5D with argument9DC0; Gold, abilities and orders unchanged. Reviewer visually inspected `levy_denial_message_frame16.png`: “Conditions in this country make it impossible to collect extra tax”. Its SHA256 is `4d14327f1996c2446db5394743820efcf9d93f8e5f2da40643192aca553fe90b`. Autumn resets the permission; Spring does not. |
| CF09 economic subset | Native labor Economy299/300/500, Winter tax P600/601, AI/human Food assistance and no-surplus control, hire Gold200/201 with100 troops. Normal Pass reaches the original bounded routines; country fields, RNG and NMI guard controls are explicit. No PC/VM-frame/ROM patch manufactures the result. These are bounded AI observations, not whole autonomous-turn coverage. |
| CF03/SE03 economic subset | Second natural Mongol year: second harvest contributes108 Food and24Fur; actual theft346→214 is traced at original entry/positive DE0C with no RAM intervention. Gold449 stays unchanged. Spring1177 gross tax57 minus payroll62 gives444 before treaty10,454 after; Food161/Fur115/Troops63. No unexplained country1 residual remains. |
| Logistics and destination sale | Controlled ownership allows actual Jewel transfer26/29→25/30, normal Move Base, fresh destination orders, Islam sale Gold66→71 and destination stock30→29; source25 remains. Body170→150 and actual Temujin Judgment72→67/orders3→2 are separated from swapped-officer metadata. Natural acquisition/conquest is not claimed. |
| Later-Rate resale and World year | Fresh complete replay reproduces natural Jewels purchase7 at Rate101 and sale5 at Rate91, signed trading result−2; intervening treaty10 is separate. World year reproduces Rate114/113/108/102/118, sickness and separate treaty income, ending Gold175/Food402. |

TD16 asks for a whole-year ROI simulation grounded in real costs. Ten explicitly conditional modeled paths with50 original-instruction checkpoints meet that requirement. They must remain labelled modeled policies, not ten played campaigns; additional native campaigns are not invented as an acceptance condition.

## Raw-log closure and retained failures

| Log | Audited result |
|---|---|
| `runtime/final_replay/events_final.jsonl` | 18898050 bytes; SHA256 `938b36181b9497f58b33eb1341226d764b2f03467ffbd6304fc827ea6eb1eec5`;32100 records,16036 complete request/response pairs, no structural error/trailing request, all490 snapshot files bound to exact raw8192-byte read responses. `MAIN_LOG_FINAL_AUDIT.json`. |
| `runtime/boundary_completion/events_boundaries.jsonl` | 14569016 bytes; SHA256 `59441da49a431704de194203c870a75531298e88069415fe694c83c16ae8233d`;14632 records,7303 complete pairs, no structural error/trailing request,349 snapshot files. One historical prompt is explicitly superseded below; no unexplained binding gap remains. Six failed driver attempts remain present. `BOUNDARY_LOG_FINAL_AUDIT.json`. |
| Initial `runtime/live/events.jsonl` and `events_part2.jsonl` | At the earlier audit,5658/439 records and3349131/255634 bytes, respectively, each ending on an unmatched request. These are incomplete historical logs. Their later screenshots/SRAM are separately identifiable evidence, not proof of a complete raw input history. Fresh replay does not repair missing historical bytes. |

The original `domestic_completion_20261104T135839435701_gift_gold_cap_give_prompt_entered` lacks its own durable raw-read binding. The complete rerun `domestic_completion_20261104T140904002096_gift_gold_cap_give_prompt_entered` has the identical SRAM SHA256 `7670a85811a5b373d19d7851ccb9cb5127f9a0600c99aaa1df1e62af6f08155b`, its own complete raw binding, and a full rerun of the case. The exception is explicit in the audit; it is not silently backfilled.

Retained failures include wrong-menu Uighur and levy probes, misentered Tax100, session expiration, wrong EB58 read-breakpoint timeouts, original treaty interruptions, restore/checkpoint waits, and the repeated-gift Assign Labor navigation failure. NativeEB58 is a JSR prologue; the bytecode read entry isEB5D. The corrected case and frame16 rendered denial are accepted; wrong-menu or menu-only screenshots are not called refusal-message proof. Source/model corrections preserve merchant presence divisor20000 and actor index7B34. Six boundary driver failures remain in the final raw log; the corrected repeated-gift and Gift Gold cap reports are selected by explicit rerun labels.

## Scope, document and register audit

The master now explicitly distinguishes source templates from modified startup state; natural play from controlled fields/offers/seeds; deterministic source models from native observations; and scoped economic completion from all game actions that can change economic fields. It correctly accounts for treaty income and natural theft. Its expanded sections10–12 describe the native evidence and historical failures without claiming Windows, physical Xbox, battery-save recovery or complete campaigns.

The published-copy appendices03,08 and09 now have explicit v1.07 supersession notes for formerly pending Draft, the formerly unexplained10 Gold, and Autumn levy reset. Their source-stage text remains historical instead of being silently rewritten. The master/index's20 local links all resolve to current files (`PUBLIC_MASTER_LINK_CHECK.json`); remote publication and archive receipt remain the parent's final delivery work.

The additive proposal preserves all167 IDs, original row order and all19 earlier column values. `MATRIX_VALIDATION.json` verifies this invariant. `PROPOSED_GK1_COMPLETE_167_PROOF_MATRIX_v1.07.csv`, built by `build_proposed_matrix.py` plus `final_status_updates.json`, currently has60 PASS VERIFIED,40 PARTIAL,66 NOT RUN and RE14 IN PROGRESS. Of94 populated economic-status entries,93 are PASS and RE14 is IN PROGRESS. RE09 records PASS for the complete current economic evidence provenance while preserving PARTIAL for irrecoverable historical raw-log omissions.

Broad rows remain honest: CF03 requires attribution of other countries' complete AI/political/battle trajectories; CF09 requires wider diplomacy/full-turn interaction; AS03 includes war casualties; SE05 includes births/aging; CF06 includes resolved civil war; TR05/PE08 include untested command branches. Spy fees/Wreck Havoc, Pay Us tribute, battle/captured stocks, tactical Food drain, succession, complete campaigns and Windows/Xbox acceptance are unchanged separate obligations. None is made complete by this scoped economic result.

## Remaining action

There is **no identified in-scope technical acceptance blocker**. The parent must finish the authorized versioned publication, archive/hash manifest, destination verification and readback, then update RE14 and the master/index working-status lines from actual publication evidence. Keep RE09 PARTIAL for the explicit irrecoverable historical raw-log gap; do not relabel old logs complete. This internal review is technical QA, not proof that outside agents received, read or implemented a handoff.
