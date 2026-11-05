# Prepared prisoner UI review — v1.01 supplement

**Reviewer:** Cao Cao / `battle_gap_audit`  
**Date:** 2026-11-05  
**Added scope:** `prepared_ruler_death_commit_001` only  
**Disposition:** **PASS VERIFIED — native Liu Biao deletion and Cai Mao succession record changes observed in the declared prepared fixture.**

This supplement closes the specific record-persistence boundary left in v1.00. The ten-case `REVIEW_v1.00.md`, `summary.json` and their audit files remain unchanged: their statements about the earlier captured frames are still accurate. This is a read-only review of the eleventh retained native run; the reviewer did not execute Mesen or modify source runs.

## Decisive native evidence

The run restores `prepared_ruler_death_continue_001/state_000400.b64`. Its decoded input MSS SHA-256 is `f3027d74dc7e483bdc7d74c0ad646cc7448930b1fe9ae2955d4b147fdd305557`. The decoded parent checkpoint, the probe receipt and the executing script's embedded input all match.

| Record change | Native writer evidence | Full-SRAM confirmation |
|---|---|---|
| Liu Biao name-ID word `$0007→FFFF` at `$67FA–$67FB` | Both byte writes at frame **15** | Deleted marker and no province serving/free membership at frames 50, 100 and 200 |
| Ruler 7 officer head `$67E7→6850` at `$72C2–$72C3` | Both byte writes at frame **15** | Head points to Cai Mao, whose original-ROM name ID is 61, at frames 50, 100 and 200 |
| Province 29 serving head `$67FC→6850` at `$76B2–$76B3` | Both byte writes at frame **15** | Cai Mao leads the list, followed by Kuai Liang `$67FC`, at frames 50, 100 and 200; province owner remains 7 |

The screenshot at frame 200 still displays “Cai Mao has been installed as the new ruler.” Unlike the earlier announcement-only sample, the associated strategic records now contain the replacement and deletion changes.

**Timing qualification:** the schedule contains a single A input at frame 20. The decisive writes occur at frame 15, **before that scheduled input**. The evidence establishes that continued native execution commits these records; it does not establish that the later A press causes the commit.

## Integrity checks

All **19 new products** match their retained-file hashes, receipt sizes/hashes and raw logged payloads: six valid PNGs, twelve memory dumps across four snapshot frames, and one saved-state file. The full PNG validator passes all six images. The run has native exit 0, `LOADED`, `DONE 210`, no `PROBE_ERROR`, and matching original ROM, launcher/core, settings, script and raw-log identities. Regenerating the Lua from its schedule and checkpoint produces identical script bytes.

The full physical SRAM decoder validates all 41 province serving/free lists at each of the four snapshots. The private audit preserves an exhaustive comparison of all 215 officer, 16 ruler and 41 province records between frames 10 and 200, including the additional list and loyalty changes. No new RAM intervention occurs in this run. Its exact checkpoint lineage inherits the original preparation, so its classification remains **OBSERVED_RUNTIME_FROM_PREPARED_FIXTURE**.

Together with v1.00, reviewed coverage is **11 runs, 221 products, 103 PNGs, 35 full snapshot frames and 13 saved-state files**. The added result establishes the bounded strategic-record commit. Ordinary-play reachability of the prepared encounter, an in-game save/cold-load/continued campaign, Windows execution and physical controller acceptance remain untested by this review.

## Added artifacts

`audit_death_commit.py` is the read-only reproduction script. Its exact command, Python version, stdout/stderr and execution receipt are preserved beside it. `death_commit_audit.json` holds the eleventh-case product identities, lineage, snapshots, native writes and full table differences. Its SHA-256 is:

`1567fce4ec4b71d9b94f8b456d58a5ff96ab8aa66a7d8193e75d1b9948d4458f`

`summary_v1.01.json` is the authored public supplement. No original ROM, raw SRAM, MSS payload or private retrieval credentials are included in the public files. The prior static-source ZIP is unchanged; these additions belong to the runtime evidence archive.
