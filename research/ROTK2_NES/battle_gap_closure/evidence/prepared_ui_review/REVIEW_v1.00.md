# Prepared prisoner and defeated-ruler UI review — v1.00

**Reviewer:** Cao Cao / `battle_gap_audit`  
**Date:** 2026-11-05  
**Project:** ROTK II NES Windows — original NES evidence only  
**Disposition:** **PASS VERIFIED for the bounded original-game UI and record changes below. Ruler Behead persistence remains PARTIAL.**

This is an independent read-only review of ten completed native Mesen runs. The reviewer did not execute an emulator, alter the retained runs, or publish remotely. These are **OBSERVED_RUNTIME from an explicitly prepared RAM fixture**, with prepared ancestry carried through every descendant. They do not establish ordinary-play reachability of that fixture, Windows implementation correctness, controller hardware acceptance, or a completed campaign/save/reload cycle.

## Preparation and ancestry

The root run `prepared_ruler_capture_001` restores the exact decoded MSS payload from `forest_ready_001/state_002200.b64`. At frame 10 its declared interventions change four bytes:

| Record / field | Address | Before | After |
|---|---|---:|---:|
| Liu Biao position | `$67F9` | `$85` | `$B4` |
| Deng Yi position | `$688C` | `$B4` | `$85` |
| Liu Biao troop word, low byte | `$67F3` | `$10` | `$64` |
| Liu Biao troop word, high byte | `$67F4` | `$27` | `$00` |

Thus the preparation swaps the two officer position fields and changes Liu Biao's raw troop word from 10,000 to 100. The raw native `INTERVENTION` records match the declared addresses, mapped physical SRAM offsets, old values, requested values and read-back values. Normal original-game controller input then produces the capture and subsequent prisoner choices. No descendant adds a RAM intervention.

The ordinary Recruit/Release/Behead branches all restore the root's frame-2000 checkpoint. The Release branch continues through Song Zhong, Liu Pan and Lu Gong to the ruler menu. Both ruler choices restore `prepared_release_next_004/state_000260.b64`. The final succession-announcement run restores `prepared_ruler_behead_001/state_000300.b64`. Every edge was verified using the **decoded MSS payload SHA-256**, compared both with the parent checkpoint and with the input embedded in the executing Lua script. Base64 text hashes were kept separate from decoded-state hashes.

## Observed results and exact limits

| Case | UI evidence | Independently decoded strategic records | Disposition |
|---|---|---|---|
| Prepared capture | Root frame 650 reads “Liu Biao was captured”; frame 2000 shows Han Song's **1 Recruit / 2 Release / 3 Behead** menu | Liu Biao's troop word changes 100→0 at frame 616, position `$B4→FF` at 617, and result bytes `$7D0B/$7BF4` become `1/1`. By frame 2000 P20 owner is 4 and P29 owner is 7. | Observed from preparation |
| Han Song Recruit | `prepared_recruit_001` advances to Song Zhong's menu | Han Song `$6826` enters P20's serving list. Allegiance **officer+7** changes 7→4 at frame 154; loyalty changes 80→54 at 155. Record/list state is present at frames 200 and 900. | Record/list effect verified |
| Han Song Release | `prepared_release_001` advances to Song Zhong's menu | Han Song enters P29's serving list, retaining allegiance 7 and loyalty 80. Frames 200 and 900 agree. | Record/list effect verified |
| Han Song Behead | `prepared_behead_001` advances to Song Zhong's menu | Name-ID word at `$6839–$683A` changes `$0030→FFFF` at frame 154. His record is absent from all 41 province serving/free lists at frames 200 and 900. | Deletion marker verified |
| Further ordinary Releases | Continuation reaches the ruler-specific menu at `prepared_release_next_004/f000260.png`: **1 Release / 2 Behead** | Song Zhong, Liu Pan and Lu Gong each join P29's serving list in their respective continuation. All retain allegiance 7. | Record/list effects and ruler menu verified |
| Liu Biao Release | `prepared_ruler_release_001/f000140.png` reads “Liu Biao was set free” | At frame 109 P29 serving head changes `$67FC→67E7`; Liu Biao's next pointer becomes `$67FC`. He is first in P29's serving list with valid name ID 7 and allegiance 7 at frames 120, 200 and 300. | Record/list effect verified |
| Liu Biao Behead, first segment | `prepared_ruler_behead_001/f000300.png` displays Liu Biao's death text | Through frame 300, Liu Biao's name ID remains `$0007`; his officer record, ruler-7 record and P29 record are unchanged. There are no writes in that run's watched officer/P29 ranges. | Death UI observed; deletion/succession not yet observed |
| Death-message continuation | `prepared_ruler_death_continue_001/f000400.png` reads “Cai Mao has been installed as the new ruler” | The sole difference across all 215 officer records, 16 ruler records and 41 province records from frames 10→400 is **ruler 7 byte +7: `$00→01`**, with a native write at frame 270 (`$72C9`). Ruler 7's officer head still points to Liu Biao `$67E7`; Liu Biao's name ID remains `$0007`; P29 serving head remains `$67FC`. | Succession announcement observed; committed head replacement/deletion remains PARTIAL |

The last row must not be summarized as a completed persistent installation of Cai Mao. The captured UI announces it, but the retained strategic records have not yet performed the corresponding head replacement or officer deletion. No claim is made about what occurs after the last captured prompt.

“Record/list effect verified” means the native game changed the relevant strategic records in full physical SRAM, with the values retained in the bounded run's snapshots/checkpoints. It does not mean an in-game save, cold restart and campaign continuation were tested.

## Integrity and native execution gates

All **202 declared products** across the ten runs match their receipt SHA-256/size and their original logged payloads. These comprise **97 complete PNGs**, **93 memory dumps across 31 snapshot frames**, and **12 saved-state files**. Each PNG passed the strengthened native validator: correct 256×240 RGB/RGBA structure, chunk CRCs/order, complete bounded zlib image data and valid scanline filters. Selected capture, prisoner-menu, result and succession screenshots were also visually inspected.

For each run, the audit verified native exit code 0, the requested `LOADED` and exact `DONE` frame, no `PROBE_ERROR`, the original ROM identity, the retained copied launcher/core identities, and the recorded script/settings/stdout/stderr hashes. The script regenerated from each schedule and embedded input checkpoint is byte-identical to the executed Lua. Every scheduled product is present exactly once in the raw log and equals its retained file.

| Identity | SHA-256 |
|---|---|
| Original NES ROM | `11e312691b516e5e29dade3c8ce5434df3561cc69e1aecad9c948acdfddc517a` |
| Native Mesen launcher | `f02685dafe559addf8dd8d769621a89777137d0c4592436aed65694a7e740676` |
| Native Mesen core | `9d21269be65268c1f56543b83e2f10650f3a28ef2566a4acd861615ecc6ed189` |

The record decoder uses physical SRAM bank 1 (`$2000–$3FFF`, logical `$6000–$7FFF`), rather than assuming that the current live CPU window maps that bank. At every one of the 31 snapshots, all 41 province serving/free lists have aligned, bounded, cycle-free and unique officer pointers, with allegiance matching the owning province or the free-officer sentinel. Officers outside those lists are not automatically classified as dead; the ordinary Behead finding uses the explicit name-ID deletion marker.

## Reproduction and evidence package

`audit_prepared_ui.py` reproduces the first nine audits; `audit_death_followup.py` reproduces the tenth without replacing the initial results. Exact commands, Python version, stdout/stderr and execution receipts are retained beside them. These scripts only read the original ROM and retained runtime evidence and create new audit outputs; they do not launch Mesen.

The private `audit.json` records every product identity, native write, checkpoint link and decoded snapshot used in the first nine cases. `death_followup_audit.json` records the final case and the exhaustive officer/ruler/province table comparison. The public `summary.json` contains authored findings, run references and hashes; it contains no ROM, raw SRAM, MSS payload or private retrieval credentials.

| Audit result | SHA-256 |
|---|---|
| Initial nine-case `audit.json` | `c738c54fb0c1146a2e17acf13aab1bf33dfab0aec8514e440ae3b43cbf4a52ec` |
| Tenth-case `death_followup_audit.json` | `ff0486baf425e2ff64c0772c20ac541e1e32de32d87ad32bc164285204422624` |

The completed review belongs to the runtime evidence archive. The already delivered private static-source ZIP is unchanged. Publication and Windows acceptance are separate gates owned by the parent delivery workflow.
