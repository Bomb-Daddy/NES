# RUN005 — Q09 open-territory officer withdrawal preserves faction & claims empty province
2026-11-06 | Handoff SUP-NES-BPF-20261106-048 | **NES Classic only**

## Authority / actual parent
Canonical NES source: Google Drive Retro Gaming/ROTK2_NES_Windows (SUP-0016). Run005 is a byte-reversible derivation of **Run004 full-source ZIP** SHA-256 `fcddf9c486d31c190952e7d89b5273e4913de2b2a7990d883e0b10494c4b277c` (4,303,156 bytes, 149 entries), now uploaded to canonical Run004 Drive folder `1gfmnX36aMBvMhH9DJRfwC8eRpXE4mObk` and independent bytes read back SHA matching. Earlier immutable ancestry: Run002 full Drive source ID `1JX1s68-TcNZnTBoOj3ob3u2MfLjOqBd-` SHA `2c95615d8f1d75f29eae42153be08a31896fb0b0ac879201bdbe7c78cded3851` -> Run003 Q14 patch -> Run004 Q14 siege flag -> Run005. Do NOT treat separate Original_KOEI_Windows_Game Build40R4 or DOS as source.

The original English NES P20->open P29 withdrawal sample is **OBSERVED_RUNTIME historical**, not freshly rerun here. Successful Yuan Shu flee retains officer allegiance, claims P29 and converts 10,000 battle troops to 100 strategic, with result/reason 0/6. The decoded original shared source (SNES hsovl2 D648 -> root 5154/507C) is **VERIFIED_STATIC**. See [research findings](../../FINDINGS_v1.00.md) and [Q09 writer audit](../../source_findings/q14_q09/STATIC_FINDINGS_v1.00.md).

## Actual code changed, not a mock
In real `src/rotk2/battle/engine.py` Run004 incorrectly cleared `officer.ruler_id` when a successful Flee target was unowned; target province stayed owner=None. Run005 instead keeps existing allegiance and assigns `province.owner=officer.ruler_id` for the legal empty destination after battle settlement. Existing troop /100 and complete battle transaction intact. All prior unrelated members byte-identical, one engine changed, one new test. Exact old engine SHA `2a2b50fb3b57fedd4ca6105d3785aeee3dd79860731a2616a1ff3f8009a7c532`; new engine SHA `015b09d61c4d2392270f8e8eca6bfd5e186d0504a119e76703eb6cbbf9a57090`. Hash-pin reproduction with [patch](run005_apply_q09_open_territory.py), [regressions](test_run005_q09_open_territory_ownership.py).

Full tested Run005 source ZIP: SHA-256 **e6bf01a7c96787985a6dd690006ecc54b4514f983c6b0b092fc72bf6c8d7acd9**, 4,304,529 bytes, 150 entries, ZIP CRC verified. QA ZIP SHA **af62732b8cb61602bcbbe76119a4b96f566756046f3fd7ca3105c480811ee3e9**, 14 files. These are local tool artifacts, not yet Drive-published (container_path error).

## Executed tests
- Prepatch source targeted 1 FAIL (expected: open destination owner None), 2 PASS; postpatch 3 PASS. Actual fail-before/green-after.
- Q09 conservation/withdraw neighbors: 12 PASS; clean extract of full Run005 ZIP: 12 PASS.
- Broad applicable Linux `pytest tests src/tests`: **227 PASS, 37 SKIP**; excludes obsolete Classic008 frozen bundle manifest and single test requiring unavailable original ROM. Their separate failure/collection evidence preserved, not claimed PASS.
- Linux software/controller-visible Xvfb UI: **24 PASS**. NOT Windows native / physical Xbox.
- Cold save before settle and after settle; province ownership, faction, enemy destination rejected with unchanged state, unrelated siege flag and `GameState.validate()`: PASS.
- Hash guard rejects unexpected parent SHA. Source ZIP reassembled independently CRC PASS.

## Publication / blockers
Run004 Classic ZIP and combined Run004 QA ZIP were successfully uploaded to canonical Run004 Drive folder, source file ID `1sSWBp3tGL-xIHWUJNBgRm1fmlvEY1aVF`, QA file ID `1K8VuGh0EhmV8sAXne_-2pRf8T21FKDvC`. Run004 Enhanced full ZIP was **BLOCKED file_too_large** (non-retryable as one file); split into <=8 MB parts before retrying. Run005 canonical folder ID `1h6IBz43W-TN7L5QsmlokzCRUvuSzpO7I` was created correctly under NES root. Upload of *new container-path* files was BLOCKED `container_session_unavailable` on first attempt. GitHub patch source and tests are published, but are not substitutes for full canonical Drive source. Binary ZIPs available as conversation artifacts. No Director release claim.

## Status
- Q09 source writer: **VERIFIED_STATIC**.
- Original NES case: **OBSERVED_RUNTIME historical**; fresh run **REPORTED_UNREPRODUCED**.
- Canonical Classic partial Flee settlement: **TESTED_PORT partial** (one verified defect closed).
- Original four-list officer transfer, chance/capture RNG, midbattle capture and full Flee/Withdraw UI, 30 matching original traces: **OPEN**.
- Enhanced Q03 undeployed defender food, default matched RNG parity: **OPEN**. Enhanced Run004 intentionally unmodified, no cross-project merge.
- Native Windows and physical Xbox new-build tests: **NOT RUN**.

## Exact next implementation
Implement officer-by-officer Q09 Flee with source-ordered rand(10) and rand(100) score, enemy occupancy, failed capture and successful insertion into running-battle lists; original Yuan Yin failure and Ji Ling success controlled trace, cancel atomicity, mixed results, cold saves, no duplicates and full Xvfb/Windows tests. Separately split and upload Enhanced Run004 ZIP into 18 checked parts and read back/reassemble.