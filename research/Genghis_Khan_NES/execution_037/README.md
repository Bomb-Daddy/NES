# Genghis Khan I — EXEC0037: Friendly join timing audit and source tracing

This folder contains the **actual tested Python audit and negative tests**, a candidate trace analyzer, a **prepared but unexecuted** MesenCE callback watch script, and exact numeric results. It is an additive task after EXEC0036; the cumulative master is [v2.19](https://docs.google.com/document/d/1ClkZ8aabW73PGduVPiM9pCcSNy_BUHegT9x-ylh3v6M/edit).

## New result: two observable stages (not a new NES run)

The private, approved combined-ROM EXEC0033 native evidence was independently reanalyzed. Its 4 cases × 7 snapshot stages passed checksum/identity and field consistency tests. The tested game profile is the combined Shared BP/Rest/Demobilize/Fonzarelli ROM, SHA256 4764d46359581be1e7d12bcb85bd375c84cd4497d1084d9d306a6e884afbe7c5. The original full-machine battle checkpoint SHA256 is e1b6266c963a48d736627812d024910b6d600c2ad8571813eb3b0999e0f2938b.

At the selected destination stage the controlled mover and friendly partner each have 40 soldiers, scratch word $613E is 0, raw $614E is 5 and raw $614C is 14. At after_attempt the mover has 74 soldiers and the partner has 0, both at (8,7), scratch word2 and raw $614E3, **while $614C remains 14**. At after_idle, raw $614C becomes 22 with soldier/word outcomes unchanged. Thus the $614C change is visible later across different sampled VM stages. Its purpose, the actual writer's VM caller and universal joining mechanics are still unknown. Do not mistake this for a source instruction trace or assume word2 means two completed commands. Earlier result remains valid for the tested modified-ROM fixture.

The insufficient-Mobility and enemy-occupied cases preserve mover40, completion word0 and $614E5; exact-Mobility movement results in mover34, word1 and $614E5. These are preexisting verified native controls, not new gameplay runs.

## Reproduction (private bytes stay private)

Run in this folder with Python 3.11+:

    GK1_EXEC0037_PRIVATE=/private/GK1_EXEC_0033_Private_Native_Legality_Evidence_v1.00.zip python -m unittest discover -v -p 'test_*.py'

Result with private licensed ZIP: **21 of 21 PASS**. Without private evidence: **14 PASS, 7 intentional SKIP**. The original prior ZIP must match SHA256 aec170e8661a0011e373024caeb858f2657b716574b5f8555b63703e5cfd28f1.

    python gk1_join_timeline.py --private /private/GK1_EXEC_0033_Private_Native_Legality_Evidence_v1.00.zip --out numeric_findings.json

Outputs 4 cases and 28 verified stage records. Test suite includes deliberately corrupted members, mismatched oracle hashes/phase labels, negative controls, and a recomputed-checksum altered scratch value. Synthetic trace tests verify typed schema, native PC/source separation and invalid event rejection.

## Next actual source trace

The file mesen_join_watch.lua was written for the previously verified **matching developer MesenCE executable/core**, used *as an add-on to* its existing Lua bridge with json and frame_count in scope. Set GK1_EXEC0037_JOIN_TRACE to a private output filename, then load the original approved combined-ROM battle checkpoint, verify the live 40-soldier mover at (7,7), Mobility10, and conduct a controller-driven friendly-occupied movement. The addon watches CPU handler $F1C8/$F1CD, unit writes $6060–$6075, scratch word $613E:$613F and $614C:$614E; outputs actual CPU PC, VM $06:$07 source pointer, PRG mapping and cycle. Verify callback write pre/post semantics, mapper/bank and command boundary. This script is **NOT YET EXECUTED**. Analyze private JSONL with gk1_join_trace_report.py. Do not publish the logs, ROM, SRAM, savestate, screenshots or sound.

**Windows-ready bounded contract:** preserve 74/0 soldiers, zero-soldier partner coordinates, word2 and stage-specific opaque scratch for exactly this approved modified-ROM test state. Do not extrapolate to all friendly units, Entire/Divide, zero-soldier terminal outcomes, AI or other sources. G03 partial, G04 battle terminal open, G11 complete Windows game open, G12 physical Xbox/native Windows/1280x720/installer acceptance untested.

The local copyright-clean ZIP GK1_EXEC_0037_Join_Timeline_Source_v1.00.zip is 12098 bytes, SHA256 d0ffb12fed8540d2f793e8e9fcee733ff937934528c261cdc780d419d4781a4a. Its direct Drive upload failed container_session_unavailable; the source/test files in this GitHub folder are exact tested local Git blobs. This README is independently authored for GitHub; it is not claimed to be byte-identical to the local ZIP README.

See [EXEC0037 handoff](https://docs.google.com/document/d/1Af3OJzHSDWMqpTKU02m42eCOIr2nYHuVGKVtBkWUsvY/edit), [ledger](https://docs.google.com/document/d/13NOcGm5YuBjSPwsQhK7R0F0UiiWRZfHgNjOayMNz0Fo/edit), and [cumulative guide v2.19](https://docs.google.com/document/d/1ClkZ8aabW73PGduVPiM9pCcSNy_BUHegT9x-ylh3v6M/edit).
