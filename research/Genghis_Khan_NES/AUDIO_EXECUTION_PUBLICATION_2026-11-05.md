# ZL-GK1-DELIVERY-0016 — Audio batches published; native work continued

Version1.00, November5,2026. Author: Zhuge Liang. To: Director; Nobunaga (Windows integration); Cao Cao (independent QA).

## Delivery disposition

**EXEC0013 and EXEC0014 are now PUBLISHED / CONTENT VERIFIED in Drive and GitHub.** Their original report text and verification snapshots retain the historical read-only/NOT DELIVERED status from the turn in which they were created. This receipt supersedes those publication statements only. It does not rewrite their research results or claim that another agent has read/executed them.

**EXEC0015 is also PUBLISHED / CONTENT VERIFIED.** It was newly executed during this publication turn: native invasion, deployment, early retreat, audio-setting controls, and uninterrupted battle-to-Save-to-new-process-Load.

Public authored code, reports, contracts and verification summaries are in GitHub. Private original-game images, sound-bank payloads, SRAM, checkpoints and raw captures are kept in the authorized Drive research folder, not the public repository. No ROM was added to these output packages.

## Canonical links

| Batch | GitHub source and report | Private Drive evidence | Drive handoff |
|---|---|---|---|
|EXEC0013 — Music/Sound and Train Troops|[execution_002](https://github.com/Bomb-Daddy/NES/tree/main/research/Genghis_Khan_NES/execution_002)|[capture archive](https://drive.google.com/file/d/1kF0iuoUj4NoKFh-PrZ7VDB75DmvbvzYq/view)|[report](https://drive.google.com/file/d/1vwnSlpZAxsF_quU78RAOE1Gdt1mJUXPh/view)|
|EXEC0014 — independent sequencer|[execution_014](https://github.com/Bomb-Daddy/NES/tree/main/research/Genghis_Khan_NES/execution_014)|[driver archive](https://drive.google.com/file/d/1bKvDHmw26RaJ-7Xah8D14itfbJowD6OV/view)|[report](https://drive.google.com/file/d/1_k6Ywdxa7C05Y5uw1j7H-Xl2VeWOMRmA/view), [contract](https://drive.google.com/file/d/1cnpJzbO9zZE88t-Go8JdwQqyjWPuzkx5/view), [source import ZIP](https://drive.google.com/file/d/1pYsOGA1elVfOnMl9ewHswxgrHXStycvL/view)|
|EXEC0015 — invasion/retreat and save|[execution_015](https://github.com/Bomb-Daddy/NES/tree/main/research/Genghis_Khan_NES/execution_015)|[new capture archive](https://drive.google.com/file/d/1xMzaOyOeDnAxrOe7xISpLywEGHAvoThd/view)|[report](https://drive.google.com/file/d/1dKwlbaKcqw8acgqYdAhwZayqJ2-3IHP2/view), [JSON contract](https://drive.google.com/file/d/1iNoCGgwP5lqvI8GC1kztV0KZWS6EeT4t/view)|

## Verification performed

GitHub readback established all8 imported EXEC0013/0014 text-file blobs match their local packages, including the final-newline correction in commit `51633d267296166b70922c3c95a4d6b27d1e9412`. EXEC0015's three authored files match their local Git blob hashes at commit `d72207f03dd7cdd3a5be7695228f8061414be6a3`. Only named research files were added; no game source was replaced.

Drive metadata and download readback confirm the actual parent folders and stored formats. Handoffs/contracts/import ZIP are under Collaboration (`1d0TA1lgqsVA3AWQQ9o0FZgmvao90oCeT`); raw capture ZIPs are under Genghis research (`1mJ72ezJb9kzIGUHHXPTh3lXtiVxWjEL7`). Reports are raw Markdown, not Google Docs disguised as Markdown.

Downloaded archive SHA-256:
- EXEC0013 private evidence: `af596a44fde16e6bb3d95b443ecba426099f3f930b380ea7b9bf65ef423ff7c8`.
- EXEC0014 private evidence: `854b3a1c75180ec586924d6deb630f6fa6b84b7f922a9319d75cdab611b4cbc1`.
- EXEC0014 source import: `a65c8ef00ec7dd460eeff641ac6e935e98732064ad435b6cb433cab8ef5df0bd`.
- EXEC0015 private evidence: `62b505124d2c96330061b32c4cac5c09a45bc27b481664dc82cf4627329c6113` (2,168,890 bytes;503 manifested files checked).

Report beginning, substantive middle and ending were read back. The old sequencer was independently rerun against its archived33,419 native calls and55,814 within-call register writes, exit0. It also matched the new21,832 native calls and26,168 within-call writes. These are bounded sequencer comparisons, not Windows acceptance.

## Newly usable findings and integration owner

Nobunaga: consume EXEC0015's event routing and `WarSelf`/`BattleEnterExit`/`SaveGameCodec` contracts. Cues20/48 are Sound-gated invasion effects; cue5 is Music-gated battle background; cue15 is a Music-gated retreat result. BP/resources debit before battle; the observed strategic order is spent on return. The naturally saved post-retreat state cold-loads in a new process with resources/troops/BP/orders and both audio settings preserved. RNG bytes differed and remain explicitly qualified.

Cao Cao: compare Windows against the exact matching modified-ROM captures, including cancel/refusal and audio-setting controls. Do not invalidate modified-ROM proof because it differs from stock by design. Publication is not proof of Windows integration.

Zhuge Liang: next missing native investigations are remaining43 event associations, attack/damage and other combat outcomes, diplomacy/succession, and audible APU/PCM output. The sequencer layer is already usable and should not be re-researched without a concrete mismatch. All emulator processes started for this batch have stopped; no background task is claimed.

**Status:** source/evidence delivery verified; scoped native capture complete; complete audio/game reconstruction PARTIAL; Windows execution NOT RUN; recipient acknowledgment UNKNOWN.

End of verified delivery receipt.
