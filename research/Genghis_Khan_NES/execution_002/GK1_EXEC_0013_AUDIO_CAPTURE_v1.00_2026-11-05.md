# ZL-GK1-EXEC-0013 — Executed NES audio, options and save/load capture

**Version:** 1.00 · **Date:** November 5, 2026  
**Author/owner:** Zhuge Liang  
**Recipients:** Director; Nobunaga (Windows implementation); Cao Cao (independent QA)  
**Project:** Genghis Khan I NES-to-Windows fidelity recreation; approved Shared-BP/Rest modified-ROM reference.  
**Execution:** actual Linux Mesen gameplay and read-only sound-register instrumentation completed. **Whole audio reconstruction: PARTIAL. Windows implementation: NOT RUN.**  
**Delivery:** local artifacts content-verified. GitHub/Drive publication **NOT DELIVERED / BLOCKED** because the actions exposed in this turn are read-only. No remote write or new commit is claimed.

## 1. What this batch actually adds

This is a new native-emulator investigation, not another planning document or a reclassification of earlier tests. Two Mesen processes ran the approved Shared-BP/Rest game. Native controller input entered a fresh Mongol campaign, changed Music and Sound independently, performed matched Train Troops commands, saved the game, shut down the process, and loaded that native save in a new process.

The capture records **15,587 APU-register writes in the first process and 4,626 in the second**, including startup and retained navigation retries. The first process reached frame 22,189. These are writes made by the game's actual emulated CPU to the NES sound hardware—not invented music, a guessed waveform, or a Windows simulation. No PCM recording or listening comparison is claimed.

New implementation-ready outputs:

- Independent Music/Sound flag locations, encodings, observed effects, and native persistence.
- A training-result sound event with **86 register writes**, reproduced with identical relative CPU-cycle offsets, frame offsets, register addresses and values.
- A causal Sound-off control: the identical command produced the same game/RNG result and **zero** sound-register writes.
- A bank-aware manifest of **52 contiguous sound-cue entries**, with **four** actual screen/action associations. This is not a claim that 52 songs have been identified.
- A concrete native-load observation: the tested game fields and audio settings survived, but the volatile RNG bytes differed after cold load. Do not assert universal RNG persistence from a successful save alone.

## 2. Exact source and environment

| Input | SHA-256 |
|---|---|
| Shared-BP/Rest ROM, 262,160 bytes | `4764d46359581be1e7d12bcb85bd375c84cd4497d1084d9d306a6e884afbe7c5` |
| Native Mesen executable | `e777af28bc6cc49d2e61ca0c1a82fbccf13811355332a53ea00749fc37a92761` |
| Corrected MesenCore.so | `0668f6c09bc89a1c8199df6cbf15aeebba243169b30f8116df97127dbfd3d7af` |
| Shared-BP developer archive | `9b2a72c2795bab63dd925bb0cd7dd73a7aa8dd80f0b7c711dfa376b8c802708e` |
| Demobilize-only comparison ROM | `61ad28688d503231d12537b2cbebb073ae5f91ec1391f825c1d05312436393be` |

The captured game was **Spring 1175, Mongol Conquest, difficulty 1**, Fonzarelli; initial Gold423, Food208, Troops61, Reserve34, Skill102, BP100 and three orders. It was a disposable fresh game, not the Director's stopped campaign.

Evidence is **OBSERVED_RUNTIME_MOD** for the captured modified game and **VERIFIED_STATIC_MOD** for inspected source/byte structure. It is valid proof of those modified-ROM behaviors. No new unmodified-stock session or native Windows game was run, and no existing original-game matrix status is silently changed.

The read-only bridge callback records writes to `$4000..$4017`, excluding controller `$4016` and DMA `$4014`. Each record includes ordinal, bridge frame, original CPU cycle, executing PC, physical PRG offset, target register and value. Buffer flush occurs before the normal end-frame bridge wait. The patch is supplied; no ROM code or game memory was altered by it. The existing Lua bridge/client licenses and attribution are retained.

## 3. Music and Sound are independent options

| Option | Native CPU address | Encoding |
|---|---|---|
| Music | `$7BB6` | 16-bit little-endian; On1, Off0 |
| Sound | `$7BB8` | 16-bit little-endian; On1, Off0 |

The actual Other menu presents separate On/Off submenus. Confirmed 240-frame windows:

| Confirmed case | First → last bridge frame | APU writes |
|---|---|---:|
| Music On, Sound Off | 10170 → 10410 | 522 |
| Music Off, Sound Off | 11475 → 11715 | 0 |
| Music Off, Sound On, idle | 12820 → 13060 | 0 |

Orders, BP, resources, country state and RNG were unchanged during these idle comparisons. The last row means there was no effect to play while idle; it does **not** mean Sound On disables effects. The native training experiment below proves the distinction.

**Windows contract:** retain two independent booleans. Music controls the background stream; Sound controls the tested action effect. Do not implement one master toggle that suppresses both. In this captured menu path, changing the setting consumes no strategic order or resources.

## 4. Train Troops: paired sound capture and exact replay

Two branches were restored from our own freshly created full-machine checkpoint. All actual orders and settings changes then used controller input. A third Sound-on branch provided a repeat control.

| Case | Start → committed frame | APU writes | Skill | BP | Orders |
|---|---|---:|---|---|---|
| Music Off / Sound On | 13307 → 14657 | 86 | 102 → 126 | 100 → 95 | 3 → 2 |
| Music Off / Sound Off | 16869 → 18219 | 0 | 102 → 126 | 100 → 95 | 3 → 2 |
| Music Off / Sound On repeat | 18466 → 19816 | 86 | 102 → 126 | 100 → 95 | 3 → 2 |

All three runs had identical starting and ending country/ruler records and RNG at each compared stage. RNG changed from `2212b730fc13` to `19542c4d0061`. Gold423, Food208, Troops61 and Reserve34 were unchanged.

The first audible-effect bus sequence spans frames **14232–14335**; the repeat spans **19390–19493**. Normalizing both at their first write produces an exact match in **CPU-cycle offset, frame offset, register and value**. The private `train_troops_apu_reference_private.csv` contains this replay reference. The last write is 103 frame-count units after the first; that is not a measured PCM duration.

An important presentation boundary was captured: the **“Skill has become 126”** result screen appears with Skill126 but still BP100 and three orders. The sound has already played. A final A acknowledgement debits BP5 and one order. This is a state/presentation timing contract—not permission to double-charge when repainting the message.

**Windows contract:** route this effect to the training-result event, not to the later acknowledgement. Sound Off must suppress the effect without altering command outcome, cost or RNG consumption. Use the private normalized trace to compare the eventual NES-compatible sound renderer. This batch does not contain that renderer or a faithful generated WAV.

## 5. Sound-driver and cue table

The recurring APU writers are in **physical 16-KiB PRG bank14**; the initial `$4015` enable comes from fixed bank15. Bank14 is byte-identical between the Demobilize-only and Shared-BP ROMs inspected here. This is an exact byte comparison of that bank, not a claim that every event trigger or the stock original is identical.

The table begins at bank14 CPU **`$8608`**, headered file offset **`0x38618`**. There are 52 contiguous little-endian pointer entries before the first cue header at `$8670`. Each inspected header contains four channel-stream pointers followed by one timing-control byte. The timing byte is **not** labeled beats per minute without a completed decoder.

| Cue index | Header CPU address | Captured association |
|---:|---|---|
| 17 | `$90D2` | Opening/title sequence |
| 1 | `$8679` | Scenario-selection screen |
| 6 | `$89A5` | Strategic main scene |
| 32 | `$9550` | Train Troops effect |

The requested cue header is stored in zero-page `$B3/$B4`. During the watched training action it changed to `$9550`, then returned to `$89A5`. Stores were observed at `$C4AD/$C4B3`; control-byte writes at `$C4CC/$C4DF` accompany stop/start handling. The separate watch buffer is bounded and **is not a complete write history**; the independent APU CSV is the complete sound-register log for these sessions.

The other **48 entries remain event-unidentified**. Entry0 has an all-zero structural header; it has not been assigned a player-event name. No DMC register activity was seen in this captured subset; the whole game's DMC use remains unestablished. Full song structure, loops, fades, effect preemption and battle/event cue coverage remain open.

## 6. Real native save → process close → new-process Load

After training, both Music and Sound were set Off. The original Save menu displayed **“Data saved”**. Mesen then shut down and wrote an 8,192-byte battery file, SHA-256:

`9fe3a8d2250d64ee432d1c5aa9f523dea4cc4c6aaf8c3fa2456dcca8e2d60cb0`

A new emulator process loaded that exact battery. After a retained navigation retry, native Load restored Spring1175, Gold423, Food208, Troops61, Reserve34, Skill126, all six synchronized BP words95, and two orders. The Other screen visibly showed **Music Off / Sound Off**. A further 240-frame idle window generated zero APU writes. There was **no checkpoint restore or CPU memory-write request in the load process**.

**RNG qualification:** before save, zero-page `$84..$89` was `19542c4d0061`; after cold Load it was `063206320000`. The stored country/ruler records match, but those RNG bytes do not. This is a measured case, not a universal conclusion about every save route. The port must not promise exact native RNG continuation until the original/modded save contract is resolved. A PC-specific deterministic-save extension must be labeled as such.

## 7. Verification and retained failures

The offline analyzer independently re-read the actual CSV, SRAM, zero-page snapshots, metadata and raw request/response logs. It checked the paired outcomes, repeated cycle-level sequence, settings persistence, source cue structure and exact source hashes. It exited0.

- **55** first-process SRAM/zero-page snapshots and **9** cold-load snapshots are bound by hashes to the actual logged native memory-read responses.
- First log: **630 request/response pairs**. Second: **82 pairs**. No orphan or trailing request; no direct game-memory write requests.
- Negative controls reject a dropped effect event, an altered register value and an incorrectly marked Sound-off flag.
- The first process intentionally restored its own checkpoint twice for matched experiments. Bridge frames continue while CPU clocks return to checkpoint time; compare within named cases or normalized trace offsets, not a false monotonic global CPU timeline.

Failures remain in the evidence. Some early exploratory screenshot labels describe an intended menu rather than the menu actually reached; the confirmed cases named in this report control. The first cold-load navigation used overly long Down presses and entered a new game; a reset and corrected five-frame pulses reached native Load. An initial battery-discovery loop also copied a same-name archived fixture over the new copy. That wrong copy is retained as superseded; the newly written battery was recopied from its explicit native path, hashed and used for the load. These are recovered capture/provenance mistakes, not silently successful first attempts.

## 8. Files and reproduction

The private evidence bundle includes full native request/response/action logs, both APU CSVs, every captured SRAM/zero-page/screen, own checkpoints, actual saved battery, retained failed captures, session environment/settings/logs, bridge instrumentation patch, scripts and manifests. It excludes the ROM and emulator binaries, which remain in the separately authorized source archives.

Implementation data:

- `contracts/audio_cue_manifest_v1.json`: all 52 structural entries, bank-aware addresses, channel pointers, timing bytes and four event mappings.
- `contracts/audio_options_and_training_contract_v1.json`: detailed paired cases and native-load observation.
- `contracts/train_troops_apu_reference_private.csv`: private normalized 86-write effect reference.
- `contracts/apu_writer_sites_v1.json`: every observed APU writer PC/register with source offsets.
- `contracts/verification_receipt_v1.json`: run hashes, log binding, checks, negatives and scope.
- `public/audio_options_training_port_contract_v1.00.json`: authored portable contract, safe for GitHub; no ROM, game images or sound-sequence payload.

To rerun the complete extraction locally, place the authorized Shared-BP archive under `bp/` and the Demobilize archive under `demob/` of the extracted capture root, preserving their archive layouts. Then run:

```sh
python3 scripts/analyze_audio.py .
```

The analyzer verifies both actual ROMs and regenerates the contract data; it does not run the emulator. The native reproduction uses the supplied capture driver/batch scripts and exact recorded controller requests. Replay either the high-level batches or their lower-level action history, not both. The absolute paths in environment/launch records describe this run and must be rebased for a different host. No Director campaign save is an input.

## 9. Completion and executable handoff

| Requirement | This batch's state | Next consuming action |
|---|---|---|
| Music/Sound flag behavior | PASS VERIFIED, bounded modified-ROM cases | Nobunaga: two independent Windows settings and free menu changes |
| Train Troops effect timing/repeat | PASS VERIFIED for captured fixture | Nobunaga: training-result event; Cao Cao: compare exact private APU sequence |
| Native saved audio options | PASS VERIFIED for Off/Off native cold load | Nobunaga: save codec; extend combinations only where useful |
| 52-entry cue catalog | PARTIAL: four event identities | Zhuge Liang: identify remaining event/cue mappings and decode stream semantics |
| Full audio fidelity | PARTIAL | Complete remaining music/FX, loops/preemption, actual PCM validation |
| RNG persistence contract | PARTIAL: observed differing bytes | Trace source save/init path before asserting exact continuation |
| Combat/new strategic AI | NOT RUN in this batch | Continue the existing prioritized gaps; no false combat completion |
| Windows runtime | NOT RUN | Integrate the data and verify actual executable output |
| Remote publication | NOT DELIVERED / BLOCKED | Authorized connector upload/commit actions required; no new remote commit exists |

No global stock-source matrix row is automatically promoted. Existing modified-ROM evidence is accepted as valid for the tested modified build. The new results should eliminate guesswork in the Windows audio/settings implementation; they do not justify another generic planning cycle.

**End of executed capture report.**
