# ZL-GK1-EXEC-0015 — Native invasion audio, retreat and uninterrupted save/load

**Date/version:** 2026-11-05 / 1.00. **Author:** Zhuge Liang. **Recipients:** Director; Nobunaga (Windows integration); Cao Cao (independent QA).

**Result:** Bounded native modified-ROM discovery complete. New Windows implementation contract supplied; overall game/audio reconstruction remains PARTIAL. No Windows executable or audible PCM comparison was run.

## 1. New behavior captured, not another plan

Three real headless Mesen processes ran the approved Shared-BP/Rest NES ROM (`4764d46359581be1e7d12bcb85bd375c84cd4497d1084d9d306a6e884afbe7c5`). This batch follows actual controller navigation from strategic War into invasion, five-unit deployment, first tactical command, commander Flee, strategic return, native Save and new-process Load. Additional matched checkpoint branches isolate the two audio settings. Modified-ROM evidence is direct proof for this tested build; no pristine-stock or Windows behavior is silently assumed.

Four previously event-unmapped cue entries now have native gameplay associations:

| Cue | Captured trigger | Setting gate | Repeat |
|---|---|---|---|
|20 (`$9387`)|First effect during the observed invasion departure sequence|Sound|No|
|48 (`$97B0`)|Second effect during that departure sequence|Sound|No|
|5 (`$893E`)|Battle overview/deployment and ensuing field scene|Music|Yes|
|15 (`$902F`)|Confirmed commander Flee / retreat-result transition|Music|No|

The first two names are deliberately sequence-specific: no claim they have only one game use or sound like a particular instrument. Their actual game requests, frames and source pointers are captured. Cue0 appears between transitions but is an empty-header request, not a newly named player event or the explicit stop instruction.

## 2. Exact setting behavior for Windows routing

Paired native controller cases produced these requests:

|Case|Observed cue sequence|
|---|---|
|Invasion, Music On / Sound On|20,0,48,0,5|
|Invasion, Music On / Sound Off|5|
|Invasion, Music Off / Sound On|20,48|
|Retreat, both On|0,15,6|
|Retreat, Music On / Sound Off|0,15,6|
|Retreat, Music Off / Sound On|none|

Thus retreat-result cue15 belongs under **Music**, even though it is associated with an action outcome. Do not route every non-background cue through Sound. Strategic cue6 resumes after the tested retreat when Music is enabled.

All compared end-state fields, country and ruler records, and RNG are identical between the three invasion branches, and separately between the three retreat branches. Only the explicitly controlled setting words differ. Audio fields `$7BB6/$7BB8` were changed through logged memory writes for these matched comparisons; actual gameplay actions used controller input. This is not a new test of navigating the already verified audio-option menus.

The battle-music request occurred 1,095 bridge frames after the Sound-on case baseline, but 951 frames after the Sound-off baseline: a 144-frame presentation difference for this input schedule. Do not force identical animation/wait duration while removing the sound. This is a bounded timing observation, not a universal command speed formula.

## 3. A complete invasion/retreat state transition

Scenario: Mongol Conquest, Spring1175, country1/Fonzarelli invading country5/Targutai. The source troop and food values are not assumed to have the same display units as tactical values.

|Checkpoint|Gold|Food|Strategic Troops|Reserve|BP|Orders|
|---|---:|---:|---:|---:|---:|---:|
|Before War|423|208|61|34|100|3|
|After accepting Self invasion|403|198|41|34|25|3|
|After commander Flee and strategic return|403|198|45|34|25|2|
|After native Save, shutdown, new-process Load|403|198|45|34|25|2|

Canceling the Self confirmation with B preserved the compared resources, whole country/ruler records, orders, BP and RNG.

The battle overview showed 200 attacker soldiers against350, attacker Food100 against430, and a90-day budget. Five attacker units contained40 soldiers each (slots0,1,2,4,7). All five were placed through controller input on legal deployment cells. At the first command the leader confirmed Flee;40 soldiers returned as four strategic Troops, while the remaining expedition units were not returned. Reserve and labor remained unchanged. No normal attack/damage equation, victory, or entire combat AI has been established by this retreat case.

**Timing contract:** BP75 and expedition resources are charged before entering battle; strategic Orders remained3 during the battle and fell to2 on return. The Windows implementation must not charge them all together merely because a menu command was chosen. This finding is limited to the captured early-retreat path. Native animation still temporarily displayed old strategic footer values while RAM already held the debit; a screenshot alone is not a complete commit-timing oracle.

## 4. Previous battle/save gap closed for this modified build

The earlier Demobilize report had a real battle followed by a reconstructed serialization fixture after an unsuccessful Save entry. This run **does not use that reconstruction**. After the actual new War commit, deployment and Flee, native Save displayed `Data saved`; the process closed and wrote its battery. A different process cold-loaded that battery without checkpoint restoration or memory writes.

The new battery is8,192 bytes; SHA-256 `aff390ef554415fce338aa4918e03205157ab4928cd61c83546f00d20659c9dc`. Complete compared country/ruler records, resources, troop/worker state, Arms/Skill, BP, orders, year/season, and Music On/Sound On all matched after load.

Volatile RNG bytes again differed: `d14c2f94e0e9` before Save vs `063206320000` after cold Load. Preserve that observation; do not claim exact native RNG continuation. The general source save/initialization rule still needs its own bounded investigation.

## 5. Verification, failures and limits

The existing independently authored sound sequencer was replayed against all three new native traces: **21,832 complete driver calls and26,168 inside-call APU writes matched exactly**, including sequencer state. There were26,171 total writes; three startup writes are outside the driver. No unmatched terminal driver call remains. These are decoder comparisons, not21,832 gameplay features.

All77 SRAM/zero-page snapshot pairs are bound to full raw native read responses. The run, cold-load and paired replay logs contain502,29 and305 complete request/response pairs respectively. No open request remains. The only direct writes were the disclosed audio settings; the cold-load process had none. Own disposable checkpoints were restored for controls; no Director campaign file was used or modified.

Two negative controls were rejected: a changed troop byte even after recomputing its metadata hash, and a dropped sound-register event. All raw controller attempts remain, including unsuccessful quantity/deployment choices. Some exploratory labels reflect intended inputs rather than actual entered values: `war_food_100` is not proof that100 strategic Food was selected; the recorded commit removed10 and tactical display showed100. The exact accepted state and screenshots control. PTY launch was unavailable; FIFO transport worked. A close-wait helper timed out waiting for an acknowledgement the server does not emit; process exit and closed logs were checked independently. These are retained research/tool failures, not hidden successes.

All three batch emulator processes are stopped. No background continuation is claimed.

## 6. Deliverable and next actual work

`battle_audio_and_save_contract_v1.00.json` supplies event IDs, option gates, matched outcomes, state timing and save acceptance for `AudioEventBus`, `WarSelf`, `BattleEnterExit`, and `SaveGameCodec`. Private evidence includes the raw APU/driver logs, input logs, states, screenshots, checkpoint provenance, verifier, retained failures and cumulative private cue manifestv1.02. No ROM or emulator executable is included in these output packages.

**Cue event identification:9 of52;43 remain.** Sequencing was already implemented in EXEC0014 and is reused here, not rediscovered. Next substantive work is additional combat/diplomacy/succession event routing, complete tactical rules, and APU/PCM output; do not extend decoder sweep counts instead of addressing those gaps.

Owners: Nobunaga consumes the contract; Cao Cao compares Windows outcomes with the matching modified-ROM references; Zhuge Liang continues unknown native routes. A publication receipt records actual Drive/GitHub delivery separately; publication is not agent acknowledgment or execution.

**End of EXEC0015.**
