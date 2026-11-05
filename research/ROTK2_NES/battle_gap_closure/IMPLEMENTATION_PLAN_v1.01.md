# ROTK II battle gap plan — execution disposition v1.01

**Date:** 2026-11-05. **Owner:** Zhuge Liang / ChatGPT.  
**Supersedes:** the execution status of [v1.00](IMPLEMENTATION_PLAN_v1.00.md); preserves that pre-experiment plan and its publication commit.  
**Goal and authority unchanged:** resolve genuine battle-knowledge gaps through original source and original-emulator evidence, preserving the unified NES/SNES rules and canonical Drive NES project.

## Completed work

- [x] Located and compared the latest full matrix, trace correction, master guide, capture branch and headless guide.
- [x] Published v1.00 in GitHub before original-game experiments.
- [x] Recovered exact original NES/SNES images and the historical Mesen executable; verified hashes.
- [x] Ran an isolated native headless route without Xvfb; verified cold checkpoint restoration, complete SRAM and PNG readiness.
- [x] Added necessary fail-closed gates and preserved meaningful negative controls, including the reviewer-found truncated-PNG and retained-executable defects.
- [x] Observed live day/weather/wind/food fields, writer traces, HUD values and mapper-bank transitions.
- [x] Reconciled current defender food with deployed and resident lists; executed resident-zero and unrelated-officer controls from the same checkpoint.
- [x] Reconstructed the named two-human force; preserved its actual 130 fielded + 70 resident population rather than forcing the older reported 60 reserve.
- [x] Decoded the Flee/Withdraw destination writer and Release fallback; observed a successful whole withdrawal through persistent campaign state.
- [x] Branched an ordinary multi-officer battle to observe failed individual escape and capture, successful individual return to friendly P19, and cancellation without roster changes.
- [x] Decoded all five Q19 handlers and the transfer/occupation chain; executed bounded original-byte queue/arithmetic probes with negative controls.
- [x] Exercised Normal, Simultaneous, Charge and Fire on the original NES; separated first-exchange results from later AI changes.
- [x] Identified the fresh white cursor via original NES rendering code, initializer writes, OAM writes and repeated frame captures.
- [x] Reached original captured-officer menus in an explicitly prepared state fixture and branched menu choices with raw before/after state.
- [x] Continued defeated-ruler beheading through the actual record commit: Liu Biao deleted, Cai Mao installed in both ruler and province lists; independently reviewed all eleven prepared cases.
- [x] Performed independent hostile review; corrected the defeated-ruler helper-zero predicate and other reporting boundaries.

The actual publication and final verification records belong in [EXECUTION_LEDGER.md](EXECUTION_LEDGER.md) and [evidence/manifest.json](evidence/manifest.json). Individual source questions and runtime coverage are tracked in the [Completion Register](COMPLETION_REGISTER.md).

## Remaining executable work

### Priority 1 — G09 legitimate pending-ally scale transition

Use the [exact experiment specification](source_findings/q14_q09/G09_NEXT_EXPERIMENT_v1.00.md). First obtain a legal allied commitment from a peaceful source province with a resident belonging to the same ruler. Preserve at least one pending officer through a genuine 30-day battle continuation; legally end the alliance before resume; capture original hsovl3 $D900 insertion and the subsequent ordinary ×100 operation. Keep intact alliance, available entry cell and terminal normalization controls.

**Acceptance:** replayable unmodified campaign evidence shows the pending officer origin, unit scale, uniqueness in lists, peaceful return, and exact later wrap. A prepared RAM fixture alone is insufficient. If a required step is blocked by the original, identify the guard and revise the candidate; do not introduce a cap to avoid the question.

### Priority 2 — historical source recovery

Recover the old FCS blob and the two named extraction scripts to determine the +9 cause. Recover the exact old 130 fielded / 190 food-counted state to identify its 60 resident troops. Recover `_sheet_attack.png` or an exact November 3 S-02 frame to decide whether it shows the freshly identified cursor. Current findings already provide usable live fields, food mechanism and cursor behavior; unavailable historical bytes remain an explicit boundary.

**Acceptance:** compare actual original bytes and metadata. A guessed header position, similar-looking picture, or edited roster is not historical reproduction.

### Priority 3 — additional runtime edge coverage

The saved multi-officer human battle now covers individual Flee to a friendly province, cancellation, failed escape/capture, and successful escape. Additional coverage can exercise no reachable province and multi-officer whole withdrawal. For combat, obtain a genuine multiple-adjacent-enemy Simultaneous case and Charge removal/breakthrough variants. Prepared fixtures can isolate a specific original branch if every alteration and descendant lineage is declared. They cannot prove ordinary campaign reachability.

**Acceptance:** every case has a verified prestate, exact input, observed mode/target, native writers or list changes, and a poststate that distinguishes success/failure. UI text alone is insufficient for persistent fate.

## Stop conditions and evidence discipline

The work already executed resolves the documented source gaps and supplies current runtime evidence. Remaining entries are specific research boundaries, not a reason to relabel verified shared mechanics as unknown. No Windows acceptance is inferred. Source ROMs, full original code, state files, RAM and game images remain in private authorized evidence; public GitHub holds authored tools, reports and manifests.

The current test framework validates evidence collection. Passing it does not mean that every battle branch, every campaign scenario, or the Windows game is complete.
