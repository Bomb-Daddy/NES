# ROTK II Battle Knowledge Gap Closure — Implementation Plan v1.00

> **For agentic workers:** Execute this plan task by task using the approved research workflow, preserving the execution ledger and original evidence. Use superpowers:executing-plans or superpowers:subagent-driven-development where applicable. Do not equate research closure with acceptance of a Windows build.

**Date:** 2026-11-05 (America/Detroit)  
**Owner / executor:** Zhuge Liang (ChatGPT), with independent evidence review  
**Status:** IN PROGRESS — source discovery verified; experiments not yet run at publication  
**Goal:** Identify the newest existing ROTK II battle reference, then resolve its genuinely outstanding knowledge gaps using the authorized original game and existing headless-emulator infrastructure.

**Architecture:** Preserve the approved unified NES/SNES battle rules. Reuse the original NES headless capture route in isolated emulator processes, add parameterized read-only observation and controlled experiment scripts, and compare original state changes with the decoded source. Keep each hypothesis separate from its result, with negative controls and exact source hashes.

**Tech stack:** Python, Mesen native test runner, Lua callbacks, Xvfb, existing ROTK II authored capture scripts, authorized original ROM/save states, and original decoded source where available.

**Spec / authority:** The Director's November 5 instruction is to publish an implementation plan in GitHub and follow it to gather remaining battle information. The live [authority register](https://docs.google.com/document/d/1qEVhDdiG2oH8LvPz0jan5DCtFFVxHhQWdWRUpduTllQ/edit) records the November 1 unified NES/SNES mechanics ruling and SUP-0016 repository boundary. This is a research task; it does not authorize gameplay redesign or changes to the faithful Windows build.

## Current source inventory

| Source | Verified edition / location | Use |
|---|---|---|
| Battle Answer Matrix, part 1 | [v1.4, November 1, Q01–Q13](https://drive.google.com/file/d/1r4KOqPC8uN1VDmaNlfpjHa_k0J__GYZZ/view) | Current detailed rules |
| Battle Answer Matrix, part 2 | [v1.4, November 1, Q14–Q22 and corrections](https://drive.google.com/file/d/1uUNq4BWRie_b-DR3nh7h0jUqiAgw4Mtd/view) | Current detailed rules and remaining gaps |
| Stock NES trace addendum | [v1.00, November 2, including final correction](https://docs.google.com/document/d/1uZFMG5I0uR5GxmTZ1uhkdbmxSD6hRyhg1dogZ0D_lJk/edit) | Conditional memory observations |
| NES Modding Guide | [Master v2.4, through Chapter 56](https://docs.google.com/document/d/1lE4lBWdkwZaYYujpvWrB-3z2WyxbS59PqwXKAbaBbuM/edit) | Later additions and supersession |
| Latest stock trace report | [NOB-ROUND2-ROM-TRACE-20261102-001 v1.2](https://docs.google.com/document/d/1pzK5VWi3zQNH80J_INXcV6zXOxvlc_gxIIIvp2PA5Jg/edit) | Reported Mesen run; raw evidence must be checked |
| Headless operations guide | [v1.00, November 5](../../../reference/MESEN_HEADLESS_CHATGPT_OPERATIONS_v1.00.md) | General infrastructure; GK1-specific bootstrap/decoder excluded |
| ROTK II battle captures | [branch nes-battle-ui-captures-20261103](https://github.com/Bomb-Daddy/NES/tree/c4a6f5a8d3a6f545a72da097d998356a3b852bd8/ROTK2_NES_Battle_UI_Headless_Capture_20261103) | Original game scripts and later normal-attack UI evidence |

Research branch starts from main commit `5e98092b1170a9213831c0b82f662813194d1688`. Existing capture branch head is `c4a6f5a8d3a6f545a72da097d998356a3b852bd8`. The November 2 Run041 port-completion register is a partial implementation record, not a replacement battle reference.

## Global constraints

- Original English NES ROM: 524,304 bytes; SHA-256 `11e312691b516e5e29dade3c8ce5434df3561cc69e1aecad9c948acdfddc517a`.
- Decoded SNES reference ROM SHA-256: `815bfcf4fd6eb23a20c2e50dde023c210b273ffb6cd86a93909d803c3643ce46`.
- NES/SNES are one Director-authorized battle-mechanics system. Source-specific offsets and evidence origins remain identified accurately.
- Canonical NES game code remains Google Drive / Retro Gaming / ROTK2_NES_Windows. This branch holds research tools, conclusions, and the requested plan.
- Do not alter another active emulator or the ongoing Temujin campaign. Use unique output directories and isolated processes/configuration.
- Preserve source saves. Experimental state manipulation, if needed, is explicit, isolated, and never represented as ordinary gameplay.
- Publish authored code, summaries and evidence manifests to this public repository. Keep commercial ROMs, private saves, full memory dumps and game captures in authorized private evidence storage.
- A pending restore, screenshot alone, synthetic model or passing Python test is not proof of the claimed original-game behavior.
- Mark each item NOT RUN, IN PROGRESS, PASS VERIFIED, PARTIAL, BLOCKED, or SUPERSEDED, and classify evidence as VERIFIED_STATIC, OBSERVED_RUNTIME, REPORTED_UNREPRODUCED, INFERRED, UNKNOWN, or CONTRADICTED.

## Review focus

1. Wrong-game or modified-ROM assets must fail hash preflight before emulation.
2. A nine-byte arithmetic coincidence must not silently become a relocated-memory-layout claim.
3. A participant counted for food must be traced through actual lists and values, not inferred from a displayed rounded total.
4. An officer leaving battle must have a verified destination/list and troop conservation; absence from the screen is insufficient.
5. Later screenshots and port tests must not overwrite older source evidence or claim unobserved result/AI paths.

## Gap register at plan publication

| ID | Question | Existing evidence / boundary | Decisive next evidence |
|---|---|---|---|
| G01 | Actual battle day, weather and wind fields and visible value coding | Q21 versus November 2 candidates conflict; nine-byte relocation is unproved | Native CPU-memory reads, visible day/wind/weather, exact writer/caller trace in controlled setups |
| G02 | Extra 60 food-accounted defending soldiers | Q03 derives 190 food-accounted soldiers versus a reported 130 fielded | Enumerate actual battle/officer lists and daily rice arithmetic from the same checkpoint |
| G03 | Individual Flee/Withdraw destination and officer-list write | Destination popup already known; internal destination update remains open | Before/after native state, selected destination, writer path and conservation |
| G04 | Q14 Release when adjacency search has no hit | Exact return field not established | Decode original helper/caller and exercise no-neighbor fallback with controls |
| G05 | Five Q19 strategy-overlay routines | Named handlers incompletely decoded | Source-grounded routine/call analysis with branch conditions and effects |
| G06 | Q19 invasion-handler attack versus transfer meaning | Dispatch inputs described, game-level result open | Follow the complete call/state transition and compare attack and transfer cases |
| G07 | Remaining battle UI/result observations | Nov3 normal combat captured; Fire/Charge/Simultaneous, result/capture/ruler flows and white marker coverage incomplete | Reuse battle checkpoint; capture each missing flow, reconcile with v1.4 rules |
| G08 | Earlier Flee capture producing crushed/succumbed | Destination and causal setup not recorded | Controlled withdrawal branch with exact selection and post-battle state |

G07 is a capture/evidence gap; it does not reopen formulas already decoded in v1.4. If later source evidence already closes an item, record that supersession and avoid redundant experiments.

## Task 1 — Recover and validate an isolated original-game harness

**Files:** create `tools/battle_probe.py`, `tools/test_battle_probe.py`, `EXECUTION_LEDGER.md`, and `evidence/manifest.json` under this directory. Reuse relevant authored scripts from the pinned capture branch with source attribution.

**Interfaces:** CLI takes explicit ROM, emulator, input checkpoint, output directory and experiment; produces receipt JSON, append-only action log, hashes and private captures. No fixed historical workstation paths.

- [ ] Inventory local authorized runtime, ROM, original decoded source and battle checkpoints; record hashes.
- [ ] Read upstream/local emulator API instructions before invoking unsupported callbacks.
- [ ] Add only necessary harness checks: wrong ROM hash, missing/corrupt checkpoint, occupied output directory, and asynchronous restore completion.
- [ ] Run a cold original-ROM launch or verified battle-state restoration, capture a real 256×240 frame, and test a bounded input pulse with release.
- [ ] Cold-load a newly saved disposable checkpoint and compare meaningful original state.
- [ ] Record every failed attempt and resolution. Do not claim runtime success on offline tests.

**Acceptance:** original ROM identity verified, real original screen inspected, native state read, input proven, repeatable checkpoint restoration; exact commands/stdout/stderr/exit captured.

## Task 2 — Resolve G01 and G02 from one causal battle sequence

**Files:** `tools/battle_probe.py` experiment modules; `FINDINGS_v1.00.md`; private per-run memory/capture artifacts.

**Consumes:** verified harness and baseline checkpoint. **Produces:** field map and food/participant ledger with source boundaries.

- [ ] Reproduce the documented Yuan Shu P19 → Liu Biao P20 setup where assets allow; record difficulty, player count, sent generals/troops/rice/gold.
- [ ] Capture the same day transitions from native CPU RAM and visible panels.
- [ ] Trace actual day/weather/wind writers and correlate values with visible labels/arrows; preserve identical-value writes.
- [ ] Compare one-human versus two-human setup with other variables fixed; check older extraction/container offsets before proposing dynamic layouts.
- [ ] Enumerate all participant/list records used by daily food and compare exact pool deltas with displayed totals and rounding.
- [ ] Test one changed troop count or participant membership as a causal control.
- [ ] Document supported answers and any narrower unresolved field separately.

**Acceptance:** fields have observed meaning and an identified writer or controlled causal validation; food total reconciles from actual participants and exact arithmetic.

## Task 3 — Resolve G03, G04 and G08 retreat/capture edge cases

**Files:** isolated experiment scripts, findings, manifests; original source remains unchanged.

**Consumes:** battle checkpoint/source and G01 field map where needed. **Produces:** retreat/release destination decision table and conservation evidence.

- [ ] Branch the same checkpoint into individual retreat and whole-army withdrawal; record every menu choice.
- [ ] Read before/after ownership, province officer lists, troops and pools, then inspect the original destination writer/caller.
- [ ] Exercise valid friendly and permitted empty destinations; preserve rejection/no-destination controls.
- [ ] Trace Q14 Release helper return semantics when no adjacent destination matches, including caller behavior.
- [ ] Reconcile the old crushed/succumbed capture with the exact causal choices; do not infer a general withdrawal rule from one undocumented run.

**Acceptance:** source-supported destination algorithm and original state evidence where reachable; every unsupported boundary remains explicitly open.

## Task 4 — Resolve G05 and G06 original strategic AI paths

**Files:** `tools/source_probe.py` only if needed; `FINDINGS_v1.00.md`; source manifest.

**Consumes:** original source/disassembly matched to identified ROM. **Produces:** routine-by-routine effects, gates, call chain and attack/transfer classification.

- [ ] Identify all five unresolved Q19 routines from the current matrix.
- [ ] Decode complete reachable branches and callees; name inputs, outputs, persistent writes and randomness.
- [ ] Follow the invasion-handler dispatch through its destination/result changes.
- [ ] Cross-check at least an attack and a non-attack/transfer control where a runtime fixture is practical.
- [ ] Mark static decoded findings VERIFIED_STATIC and observed executions OBSERVED_RUNTIME; do not imply unexecuted AI frequency validation.

**Acceptance:** all five routines and dispatch semantics accounted for, or exact unavailable source/caller boundaries recorded with concrete next tests.

## Task 5 — Complete remaining UI evidence and publish reconciled results

**Files:** `FINDINGS_v1.00.md`, `EXECUTION_LEDGER.md`, `evidence/manifest.json`, updated plan checkboxes and gap register.

- [ ] Reconcile November 3 combat captures with the full guide before new runs.
- [ ] Capture reachable missing attack and result/capture/withdrawal flows from controlled checkpoints.
- [ ] Investigate the white marker with active-unit/cursor/troop state controls and an original writer if necessary.
- [ ] Run independent review against the source documents and raw evidence; correct unsupported claims.
- [ ] Save private evidence to the authorized project destination and verify readback.
- [ ] Commit authored tools, source index, findings and the final completion register to this research branch; verify GitHub readback.
- [ ] Report the latest reference links, newly established findings, and any genuine remaining blockers. No all-complete claim while a required item remains open.

## Execution rule

Continue through recoverable environment/input problems and all independent research paths. A genuine missing private source, unavailable compatible runtime, or unsupported tool boundary is recorded precisely after recovery attempts; it is not replaced by simulated evidence. No routine approval pause is needed for the work the Director has requested.

## Publication record

This plan is to be committed before experiments begin. Commit ID and subsequent execution evidence belong in `EXECUTION_LEDGER.md`. The plan's existence does not mean the experiments have run.
