# Genghis Khan NES research

## Audio implementation and new native captures — published 2026-11-05

[Verified Drive/GitHub publication index](AUDIO_EXECUTION_PUBLICATION_2026-11-05.md) supersedes the historical NOT DELIVERED text retained in the original EXEC0013/0014 reports.

- [EXEC0015 invasion/retreat and uninterrupted native Save/cold-Load](execution_015/ZL-GK1-EXEC-0015_Invasion_Audio_Save_Report_v1.00.md), with [Windows event/state contract](execution_015/battle_audio_and_save_contract_v1.00.json): four more cue entries identified, 9 of52 now event-mapped;43 remain.
- [EXEC0014 independent sound sequencer and verifier](execution_014/): implemented note/duration/channel/loop/interrupt logic; ordered APU-register output, not PCM or a complete Windows game.
- [EXEC0013 audio settings/training capture](execution_002/GK1_EXEC_0013_AUDIO_CAPTURE_v1.00_2026-11-05.md): independent Music/Sound settings, training-result trigger and native saved settings.

The publication index links the private evidence archives and Collaboration handoffs. Private ROM/game imagery/sound data/raw memory remain off the public repository. Native modified-ROM evidence is valid for the matching modified build; Windows integration remains a separate execution gate.

## ChatGPT headless Mesen execution — verified 2026-11-05

- **[ChatGPT headless emulator operations & recovery guide](../../reference/MESEN_HEADLESS_CHATGPT_OPERATIONS_v1.00.md)** — reproducible setup, source versioning, first-run wizard fix, game-specific asynchronous checkpoint restores, controller pitfalls, evidence and QA.
- **[Executable Mesen smoke/checkpoint harness](tools/headless_mesen_campaign_test.py)** and **[14 offline tests](tools/test_headless_mesen_campaign_test.py)** — original NES ROM restore, 256×240 frame, D-pad pulse, new-save/cold-reload checks passed on Linux in the pinned runtime.
- **[Unified Temujin AI operating script v2.00](campaign/TEMUJIN_UNIFIED_AI_OPERATING_SCRIPT_v2.00.md)** — the Director's strategy, **not** a complete unattended gameplay controller. A real campaign run still requires verified input planning and private savestates.
- Public GitHub does not store the ROM, state files or copyrighted gameplay captures; use the authorized private [Temujin campaign Drive folder](https://drive.google.com/drive/folders/1pWASkSQBTPwihklnGZJbetrRZCqyp2g9).

## Current Windows fidelity execution and hostile QA

- [Latest Windows NES one-to-one gap-closure implementation plan v1.02](GK1_WINDOWS_PORT_GAP_CLOSURE_IMPLEMENTATION_PLAN_v1.02_2026-11-05.md) — the active execution plan, with 106 stock-original open requirements and verified modified-ROM behaviors for matching Windows features.
- [Corrected modified-ROM evidence classification](GK1_WINDOWS_PORT_MOD_EVIDENCE_CLASSIFICATION_CORRECTION_v1.00_2026-11-05.md) — verified modified NES behavior counts as proof for that modified build.
- [Hostile ROM-mod source QA v1.00](GK1_WINDOWS_PORT_MOD_SOURCE_HOSTILE_QA_v1.00_2026-11-05.md) — actual Demobilize and shared-BP developer ZIP inspection, 80-patch integrity review, source provenance and 16 corrected planning gaps.
- [Prior plan v1.00](GK1_WINDOWS_PORT_GAP_CLOSURE_IMPLEMENTATION_PLAN_v1.00_2026-11-05.md) remains historical, not a competing active implementation directive.

**ROM provenance:** Original USA NES alone is the Classic behavioral oracle. Demobilize/Fonzarelli and shared-BP/Rest ROMs prove their tested modified behavior; the corresponding modded ROM is the Enhanced fidelity oracle. This is distinct from stock-game parity proof. Private mod source/ROMs live in the linked Drive archives; do not assume they have been publicly committed to GitHub.

Start with [Complete Economic Mapping v1.07](v1.07/GK1_ECONOMIC_MAPPING_v1.07_2026-11-04.md). All requested economic mapping batches have completed original-ROM research and independent verification. [Completion and reproduction](v1.07/GK1_ECONOMIC_MAPPING_COMPLETION_AND_REPRODUCTION_v1.07_2026-11-04.md) provides the commands, evidence archive and delivery record.

The [economic batch register](v1.07/GK1_ECONOMIC_BATCH_COMPLETION_REGISTER_v1.07.csv) and [167-obligation register](v1.07/GK1_COMPLETE_167_PROOF_MATRIX_v1.07_2026-11-04.csv) distinguish completed economics from broader full-game obligations. All original rows and earlier evidence are preserved. This research does not certify a Windows port, Xbox controls or completion of unrelated game systems.

[v1.06 research](v1.06/GK1_ECONOMIC_MAPPING_v1.06_2026-11-04.md) and its [handoff](v1.06/GK1_EVIDENCE_PROVENANCE_AND_HANDOFF_v1.06_2026-11-04.md), along with v1.04 and v1.05 findings, remain historical records. v1.07 supersedes their unfinished economic batches. Private ROM, screenshots and saved memory are not published in this public repository; authorized evidence links are in the completion document.
