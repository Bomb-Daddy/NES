# ROTK II battle research — current index

**Research date: 2026-11-05. Owner: Zhuge Liang / ChatGPT.**

Start with [FINDINGS_v1.00.md](FINDINGS_v1.00.md) for the new answers and their evidence boundaries. The [Completion Register](COMPLETION_REGISTER.md) records the remaining work. This research implements the Director's instruction to locate the latest battle guide, publish a plan in GitHub, and execute original-game experiments to resolve its gaps.

The frozen original-NES catalog contains **99 native runs: 93 pass the evidence gates and six retain their failures**. Source analysis resolves the previously incomplete retreat/release and strategic AI handlers. Runtime evidence adds exact food accounting, individual and whole retreat outcomes, missing combat commands, the blinking cursor, and prepared prisoner/ruler branches through committed succession. The principal remaining mechanics question is G09's ordinary-play pending-ally scale transition; historical missing assets and wider runtime coverage remain separate limits.

## Which guide is current?

The latest complete existing mechanics reference found is the **November 1 Battle Answer Matrix v1.4**, in two parts. Read it with the **November 2 stock-NES trace addendum, including its final correction**, and the new November 5 findings in this directory. The November 3 capture branch adds UI observations; the November 5 headless guide explains emulator operation. Neither is a newer complete mechanics matrix.

| Reference | Link | Role |
|---|---|---|
| Battle Answer Matrix v1.4, Q01–Q13 | [Drive part 1](https://drive.google.com/file/d/1r4KOqPC8uN1VDmaNlfpjHa_k0J__GYZZ/view) | Shared battle mechanics |
| Battle Answer Matrix v1.4, Q14–Q22 | [Drive part 2](https://drive.google.com/file/d/1uUNq4BWRie_b-DR3nh7h0jUqiAgw4Mtd/view) | Results, AI, corrections and formerly unresolved details |
| Stock NES trace addendum v1.00, Nov 2 | [Drive document](https://docs.google.com/document/d/1uZFMG5I0uR5GxmTZ1uhkdbmxSD6hRyhg1dogZ0D_lJk/edit) | Read through the final correction; a nine-byte displacement was not an established relocation |
| NES Modding Guide Master v2.4, through Chapter 56 | [Drive document](https://docs.google.com/document/d/1lE4lBWdkwZaYYujpvWrB-3z2WyxbS59PqwXKAbaBbuM/edit) | Broader consolidated reference |
| Headless Mesen operations v1.00, Nov 5 | [Repository guide](../../../reference/MESEN_HEADLESS_CHATGPT_OPERATIONS_v1.00.md) | Existing general operations guide |
| ROTK II isolated headless runtime v1.01 | [Verified runtime instructions](HEADLESS_RUNTIME_v1.01.md) | Exact original-ROM runtime, native checkpoints, memory banks and reproducible experiments |
| Original battle UI captures, Nov 3 | [Pinned capture branch](https://github.com/Bomb-Daddy/NES/tree/c4a6f5a8d3a6f545a72da097d998356a3b852bd8/ROTK2_NES_Battle_UI_Headless_Capture_20261103) | Earlier original-game Normal attack, capture and menu evidence |

## New authored work

- [Implementation plan v1.00](IMPLEMENTATION_PLAN_v1.00.md): published before experiments, commit `9d80c6482a11a1f774635f0466918e95f9b8eddf`.
- [Implementation plan v1.01 / execution disposition](IMPLEMENTATION_PLAN_v1.01.md): completed work, partial coverage and exact next experiments.
- [Findings v1.00](FINDINGS_v1.00.md): reconciled native observations and source analysis.
- [Execution ledger](EXECUTION_LEDGER.md): commands, failures, controls and publication evidence.
- [Completion Register](COMPLETION_REGISTER.md): project, requirement, owner, evidence, status and next action.
- [Q09/Q14 source report](source_findings/q14_q09/STATIC_FINDINGS_v1.00.md): Flee destination writer, Release fallback and corrected record types.
- [Q19 source report](source_findings/q19/Q19_STATIC_CLOSURE_v1.00.md): all five formerly undecoded handlers, transfer versus hostile attack, queue behavior and arithmetic.
- [G09 pending-allied candidate](source_findings/q14_q09/G09_PENDING_ALLY_CANDIDATE_v1.00.md) and [next experiment](source_findings/q14_q09/G09_NEXT_EXPERIMENT_v1.00.md): the remaining troop-scale reachability question.
- [Two-human replay](evidence/twohuman/REPLAY_NOTES.md): exact force reconstruction, controller port, daily food and retained failed attempts.
- [Prepared UI commit review v1.01](evidence/prepared_ui_review/REVIEW_v1.01.md): independently checked persistent ruler deletion and succession, supplementing the preserved ten-case review.
- [Public tool validation](evidence/publication_validation/receipt.json): all 28 harness and catalog tests pass; run `python -B -m unittest discover -v` from `tools/` to reproduce these synthetic evidence-collection checks.
- [Evidence manifest](evidence/manifest.json): authored results and private raw evidence locations.

## Authority and boundaries

The [live authority register](https://docs.google.com/document/d/1qEVhDdiG2oH8LvPz0jan5DCtFFVxHhQWdWRUpduTllQ/edit) and the Director's November 1 ruling treat NES/SNES ROTK II as one battle-mechanics system. Decoded original SNES rules are authoritative for the shared mechanics. Platform addresses and UI wording remain identified by their actual source. This work does not reopen separate NES mechanics gates for already decoded shared rules.

The canonical NES Windows project remains **Google Drive / Retro Gaming / ROTK2_NES_Windows (SUP-0016)**. This GitHub branch contains research, tools, and the requested implementation plan. The DOS restoration and original KOEI-inspired project are outside this evidence set.

Public files contain authored analysis, scripts and manifests. Original ROMs, complete decoded commercial listings, emulator saves, RAM dumps and game screenshots are excluded. The private [research evidence folder](https://drive.google.com/drive/folders/1NlIRYS4k8sCpGgbMSapsmJcR7pmxVF64) belongs to the canonical NES project.

Evidence labels distinguish **VERIFIED_STATIC**, **OBSERVED_RUNTIME**, **INFERRED**, **UNKNOWN**, and **REPORTED_UNREPRODUCED**. No Windows port, physical Xbox controller, campaign acceptance, or full ordinary-play reachability is claimed from Linux emulator runs or bounded original-byte probes.
