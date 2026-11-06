# Enhanced game battle mode — 1:1 English NES ROTK II player-behavior mandate, Run005

Date: 2026-11-06 | Producer: Zhuge Liang | Status: TESTED_PORT PARTIAL, not an accepted game release.

## Binding Director rule
The **entire battle mode** of the separate Original KOEI-inspired Enhanced game must behave 1:1 like original English NES Romance of the Three Kingdoms II. Not source-code duplication, not only numeric damage equivalence, not an approximate alternative battle game. Preserve the approved Enhanced economy, marketplace, officer BP and strategic additions **outside** combat, but the battle's original commands/eligibility/turn cycle/terrain/movement/damage/AI/flee/prisoners/campaign aftermath and default battle presentation are locked to original observable mechanics. No extra Enhanced-only combat rules unless the Director later explicitly changes this mandate. The playable Windows game must use 1280x720 controller-only menus and source-faithful UX scaled to screen; no side-panel 17-button substitute or mouse requirement.

## Actual Run005 implementation
Baseline: **separate original-game Enhanced** Drive folder `1IRMW3YM7jhg0jpjNugaPScAiUSKWhA5U`, source `ROTK2_Enhanced46_BPF_Run004_Q14_Remote_Refuge_Source_v1.03.zip`, exact 18-part reassembled SHA-256 `7b2891d3c7d7b6ef371b2e06926fce59bda1b0f480160ccb8fc32c9e286ec229`. 517 source entries, CRC clean. Parent recovered and each part SHA verified before mutation.

A reversible local source patch changes ONLY:
- `third_branch/officer_battle_adapter.py` — adds `_q03_resident_defenders_troops100(s,b)`, counting actual officer-owned station troops in the besieged province that are **not** in the defender's field/queued officer lists. Excludes remote, fled, allies, deployed and double-count. Uses no phantom troop reserve. Legacy aggregate battles return zero and retain their original behavior.
- `third_branch/army_cohorts.py` — exact army conservation now accepts fielded defender troops **plus** identified, posted, genuine resident troops, rather than treating every off-field defender garrison as a corrupt state.
- `tests/test_bpf_q03_resident_enhanced.py` — real Campaign/army-ledger test creates a posted nondeployed defender, advances a real battle day, checks food deduction, validates armies and save/cold-load; guards against deployed double-charge and unrelated province count; source numeric boundary 19000+1000 -> 666 rice hundredths, 19000 -> 633.

Full new local immutable source candidate: `ROTK2_Enhanced_Run005_Q03_Resident_Battle_Parity_CANDIDATE.zip`; SHA-256 `b550f12de820e383965102e24ffb9873776dd41229424cb3e649411081341a4b`, 148,704,302 bytes, 518 CRC-clean entries. 515 of 517 old member contents unchanged; 2 code files changed and 1 test added. **Full 18-part Run005 source has NOT been published/read back in Drive; do not promote as canonical.**

## Tests executed
- Pristine actual Enhanced Run004 RED: three new Q03 cases failed, one passed; exit 1.
- Initial green five passed; full relevant run exposed legacy aggregate double-charge regression. Repaired it, with no weakening of source-backed army conservation.
- Q03 + preexisting aggregate Q08 compatibility focused after correction: **13 passed**, exit 0.
- Broad battle/army/tactical tests including Xvfb for GUI-sensitive tests: **184 passed, 2 skipped**, exit 0.
- Same broad scope from a **cold-extracted full Run005 source ZIP**: **184 passed, 2 skipped**, exit 0.
- Full unfiltered suite: **NOT GREEN**. Private ROM environment and historical rollback import block collection, earlier broad run timed out; initial temporary aggregate regression observed and corrected. Do not count 2 skips or incomplete suite as pass.
- No native Windows original-to-port parity, physical Xbox controller, actual original Mesen replay or Director acceptance for this candidate.

## Verified persistent patch/evidence
- Separate original-game Drive Run005 folder: https://drive.google.com/drive/folders/12Cfsw4iJRswTdZwC68NoDjB82kXL68HL
- Q03 source diff, exact red/green, first-fail logs, broad 184 pass, manifest/README: https://drive.google.com/file/d/1GaqenIBYWQfZ_4H9QcC85OIuA79yWvRK/view
- Readme: https://drive.google.com/file/d/1FMure69N-Be3z9rnJHDfPpxtjevNPwZq/view
- Original NES battle reference and 99 original emulation evidence runs: https://github.com/Bomb-Daddy/NES/tree/rotk2-battle-gap-research-20261105/research/ROTK2_NES/battle_gap_closure
- Canonical **NES Classic** latest as of Nov6 11:35 UTC is separate Run014 Drive, SHA `ddb6ee5693e6dce140f4e216d14d241e8c1239aa8ddfb66fd1b7257d46ade398`; do not conflate Classic Run014 and Enhanced Run005.

## Exact subsequent implementation priority
0. Deliver the complete Enhanced Run005 source as 18 numbered split parts into the existing separate original-game Run005 folder. Full SHA/part hashes/parent/CRC readback mandatory. Library staging alone is NOT canonical Drive source.
1. Original Q09 **individual Flee/whole Withdraw**: live per-officer escape success/capture with source order, failed-escape prisoner list, destination officer insertion, troop /100 for peaceful province, retained scaled state when destination under attack. Reference Classic Run014 and original hsovl2 $D648-$D6C8. Red-before, green-after in same current Enhanced source and controlled RNG, both human sides.
2. Four battle slot lists 0–3 and original offensive/defensive deployment (41 source terrain/region tables), actor-by-actor phases. Keep off-field resident defenders separate and source-correct, not manual player cheat.
3. Original Q05/Q06/Q07/Q08/Q10/Q11 and Q13/Q14 native gameplay and all messages, including AI outcomes Q16/Q17/Q19 and distinct six-round Q18 CPU-only battles. Match 30+ original-to-Enhanced golden traces. Preserve original quirks.
4. Original NES six-command in-game battle UI (Move, Attack, Flee, View, Tactics, Wait) with nested menus and exact A/B/Start/Select behavior at a single 1280x720 controller-only view, no scroll. Original messages/results and save/quit/cold-load.
5. True native Windows release, physical Xbox and Director player acceptance; no releases on passing only synthetic/offscreen/Linux tests.

Owners: Nobunaga codes and verifies the actual Enhanced source; Cao Cao independently hostile-QAs; Zhuge Liang tracks authority, reports exact statuses, and publishes numbered instructions in Google Drive Collaboration. No agent read acknowledgement implied by publication.
