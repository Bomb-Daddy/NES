# RUN009 — Q09 primary-slot commander whole-army authority

**Date:** 2026-11-06. **Source:** SUP-0016 canonical NES Classic only. **Status:** TESTED_PORT PARTIAL; Director acceptance OPEN.

## Immutable source and ancestry
Actual immutable parent Run008 complete ZIP on canonical Drive ID `1I3aoVWwOHWALZTXP61k-7IIN0W4O6UTv`, SHA-256 `27f4e35b19ab0a7e7a15f9d6fe75f08acc0d169b384aea64d72224a0bd7331f4` (153 CRC-clean members), itself based on Run007 as documented in the verified Drive handoff 050. Do not conflate with `Original_KOEI_Windows_Game/Build40R4` or the DOS branch.

Source authority: original decoded common gameplay hsovl2 `$D905/$D89D`, as documented in [published STATIC_FINDINGS](../../source_findings/q14_q09/STATIC_FINDINGS_v1.00.md), **VERIFIED_STATIC**. Primary commander alone receives the whole-army prompt; a reinforcement-slot commander is not the primary commander.

## Executed real-game correction
Run008 erroneously accepted `whole_army=True,officer_id=3` from a regular officer and `officer_id=2` from the opposing army. Both actually caused the player's whole army to concede despite not being authorized. Controller UI also offered the whole-army prompt to slot-3 reinforcement commander. Run009 engine validates primary commander of main slot (attacker slot 2, defender slot 0) against explicit selected officer ID; UI offers the whole-army YesNo only to main-slot commander. Original command compatibility without an officer ID is retained when main commander exists. Other mechanics/RNG unchanged.

**Actual RED:** 3 FAIL / 2 PASS on pristine Run008. **GREEN:** 5 PASS. **Software controller/Xvfb group:** 15 PASS. **Applicable broad Linux suite:** 244 PASS / 37 SKIP / 1 DESELECT; clean extracted full ZIP independently 244 PASS / 37 SKIP / 1 DESELECT. Unfiltered non-ROM suite 244 PASS / 37 SKIP / 1 FAIL from old frozen CLASSIC008 manifest (preserved, not concealed). Original-ROM-dependent test explicitly excluded. Engine rejects wrong authority without mutating state or consuming combat RNG. Main commander positive test uses genuine checksummed disk save and cold reload. Hash-guard patch fails on wrong parent/reapplication. This is not new NES Mesen gameplay observation, native Windows or physical Xbox testing.

**New complete source:** SHA-256 `f57e213b7ee8ba25bb17a5af75bdcfd4865ba2d5a624bc1b021daf435c50c7b8`, 4,310,417 bytes, 154 CRC-clean entries, 151 original member payloads unchanged, two production members modified, one test added. **Drive source:** https://drive.google.com/file/d/1DJJ-Dggw3HPM1caN5nKnP-Q4f1lVCJkV/view . **Drive evidence:** https://drive.google.com/file/d/1KZwpOLhbU0ENXA8PIs7nBitTaKmPLLhU/view . Correct parent folder https://drive.google.com/drive/folders/1HMD49wbDrYEwE1swxUDZEMYsMHANKvSe under canonical NES root. Both binary blobs independently re-materialized from Drive and SHA/CRC readback matched; report/register readback also matched. Source ancestry remains unchanged.

## Completion classification and next implementation
Q09 primary authority: source VERIFIED_STATIC / port TESTED_PORT scoped PASS, wider battle PARTIAL. Q09 independent officer r10/r100 escape, failed capture, whole-army per-officer destinations: source VERIFIED_STATIC / port OPEN. New original emulator replay REPORTED_UNREPRODUCED; Windows native and physical Xbox NOT RUN; Enhanced Q03 resident-food OPEN; overall Director release OPEN.

Next patch: derive exact original `hsovl2 $D582` movement/War/training/troops/random score and independent capture update with source-backed fixtures; fail-before/green-after mixed officer escape/capture, no-duplicate lists, save/cold-load, player-visible prompts. Preserve Enhanced as separate lineage and compare default mechanics under identical state/controlled RNG only when able.
