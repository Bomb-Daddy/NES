# Genghis Khan NES research and Windows implementation

## START HERE — one current guide

**[Unified Knowledge and Completion Guide v2.01](GK1_WINDOWS_MASTER_GUIDE.md)** is the current human-readable source of what is known, what can be implemented immediately, and exactly what remains. It consolidates economics, original/modified-ROM behavior, command data, audio sequencing and event routing, battle/save captures, graphics, emulator recovery, and the remaining implementation plan. Its coverage table reconciles all 167 historical requirement IDs. Version2.01 incorporates the new normal-melee numeric kernel and bounded native evidence from EXEC0018.

**Reuse existing evidence. Do not rerun completed NES investigations.** The guide identifies twelve remaining work packages and their Windows consumers. New Windows integration checks are separate from rediscovering old NES rules. Earlier plans v1.00–v1.02 remain historical; they are not competing active directives.

- [Identical current master in Drive Collaboration](https://drive.google.com/file/d/18Qo3OJqTLXnedoUGJ9TVTINsjNObn45p/view); [preserved v2.00](https://drive.google.com/file/d/1OTd7qMxHAKerHW-CsD56aBFi0lRKmblW/view).
- [EXEC0018 normal-melee code, formulas and native fixtures](execution_018/NORMAL_MELEE_IMPLEMENTATION.md): normal-attack strength, matchup, terrain/role, castle prelude and ordered casualty arithmetic. Nine new controlled native attacks match the kernel. Target legality, commander-zero handling, special commands and full Windows combat remain separate work. [Private evidence](https://drive.google.com/file/d/1The0pqj5hnTLySktLZRW2riDIip8qzC5/view); [source package](https://drive.google.com/file/d/1Jl1tETGdL4Oqe0fwTqYsbpKram5nJTu0/view); [Collaboration handoff](https://drive.google.com/file/d/1VXLhOYT1m16FNw-4oL7nAT77lLejBtOy/view).
- [Recovered command data v1.01](execution_017/command_costs_v1.01.csv): all34 rows,30 named commands. Row6 Train Prince and row7 Train Self were recovered from existing v1.07 data without retesting; only row16–19 caller names remain unresolved. Demobilize reuses row2, not People Demote row12.
- [Complete earlier extraction package](https://drive.google.com/file/d/1kRiBYBfg8MIcq5_CzSUcvqBCf6sEhrxT/view): paired costs,80 patch locations,16 UI surfaces and provenance. Its older UI status labels are reconciled in the master against later native results.

## Existing implementation and accepted evidence

- [Economics v1.07](v1.07/GK1_ECONOMIC_MAPPING_v1.07_2026-11-04.md), [all41 country economic templates](v1.07/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv), [economic completion record](v1.07/GK1_ECONOMIC_MAPPING_COMPLETION_AND_REPRODUCTION_v1.07_2026-11-04.md), and [historical167-row register](v1.07/GK1_COMPLETE_167_PROOF_MATRIX_v1.07_2026-11-04.csv). All eleven scoped economic batches remain accepted. The old61/40/66 count is a historical snapshot, not a current count of untouched mechanics.
- [EXEC0012 extraction report](execution_001/GK1_EXEC_0012_COMMAND_DATA_v1.00_2026-11-05.md).
- [EXEC0013 audio options/training capture](execution_002/GK1_EXEC_0013_AUDIO_CAPTURE_v1.00_2026-11-05.md).
- [EXEC0014 working sound sequencer, verifier and contract](execution_014/).
- [EXEC0015 invasion/retreat and uninterrupted native Save/cold-Load](execution_015/ZL-GK1-EXEC-0015_Invasion_Audio_Save_Report_v1.00.md), with [event/state contract](execution_015/battle_audio_and_save_contract_v1.00.json).
- [Verified audio publication index](AUDIO_EXECUTION_PUBLICATION_2026-11-05.md): overrides original reports' historical NOT DELIVERED wording. With EXEC0018 cue41, ten cue entries now have gameplay associations; the master separately identifies structural cue0 and41 other entries requiring caller classification. Cue41's option gate remains unknown.
- [Original graphics extraction branch](https://github.com/Bomb-Daddy/NES/tree/genghis-khan-assets/research/Genghis_Khan_NES/Assets).

## Existing emulator and campaign tools

- [Headless Mesen operations/recovery](../../reference/MESEN_HEADLESS_CHATGPT_OPERATIONS_v1.00.md).
- [Smoke/checkpoint harness](tools/headless_mesen_campaign_test.py) and [offline tests](tools/test_headless_mesen_campaign_test.py). These are recovery tools, not a requirement to replay settled research during consolidation.
- [Temujin policy script v2.00](campaign/TEMUJIN_UNIFIED_AI_OPERATING_SCRIPT_v2.00.md) and [private campaign folder](https://drive.google.com/drive/folders/1pWASkSQBTPwihklnGZJbetrRZCqyp2g9). Policy, native captures and completed autonomous campaigns are different things; preserve selected checkpoint provenance.

## Preserved history, not current work ordering

- [Gap-closure plan v1.02](GK1_WINDOWS_PORT_GAP_CLOSURE_IMPLEMENTATION_PLAN_v1.02_2026-11-05.md), [v1.01](GK1_WINDOWS_PORT_GAP_CLOSURE_IMPLEMENTATION_PLAN_v1.01_2026-11-05.md), [v1.00](GK1_WINDOWS_PORT_GAP_CLOSURE_IMPLEMENTATION_PLAN_v1.00_2026-11-05.md).
- [Modified-ROM evidence correction](GK1_WINDOWS_PORT_MOD_EVIDENCE_CLASSIFICATION_CORRECTION_v1.00_2026-11-05.md) and [hostile source audit](GK1_WINDOWS_PORT_MOD_SOURCE_HOSTILE_QA_v1.00_2026-11-05.md).
- [Earlier v1.06 research](v1.06/GK1_ECONOMIC_MAPPING_v1.06_2026-11-04.md) and [handoff](v1.06/GK1_EVIDENCE_PROVENANCE_AND_HANDOFF_v1.06_2026-11-04.md), with v1.04/v1.05 records, remain evidence history. No old pass is invalidated merely by a newer document.

Original and approved modified ROMs are authoritative for their respective demonstrated behavior. Neither alone proves a Windows executable. Private ROMs, sound-bank payloads, images, SRAM and checkpoints remain in the linked authorized Drive archives; public GitHub holds authored code, documentation and permitted numeric findings. No full Windows release or recipient execution is implied by publication.
