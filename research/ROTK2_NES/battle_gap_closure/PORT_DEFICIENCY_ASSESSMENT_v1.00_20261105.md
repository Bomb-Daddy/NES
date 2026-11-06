# ROTK II — Classic/Enhanced battle fidelity deficiency assessment v1.00

Date: 2026-11-05 (America/Detroit)
Author: Zhuge Liang / Executive Producer
Audience: Project Director; implementation-plan input only.
Status: SOURCE INSPECTION COMPLETE FOR LISTED GITHUB FILES; CANONICAL ZIP CODE DIFF AND NEW PORT EXECUTION NOT RUN. THIS IS NOT A BUILD ORDER OR RELEASE ACCEPTANCE.

## 1. Decision and scope

The binding goal is original English NES ROTK II functional, behavioral, and player-interface parity in Classic. The Director's November 1 decision accepts the decoded SNES source as authority for shared NES/SNES combat mechanics, while the English NES original remains the source for original UI, addresses, content and observed behavior. Enhanced's default battles must produce the same original combat result from an equivalent battle state and injected random stream; approved Enhanced strategic and economic systems may differ outside the battle kernel. No new game balancing or invented battle commands are authorized.

Three implementations must not be conflated:
A. Canonical NES Windows Classic reconstruction: Google Drive / Retro Gaming / ROTK2_NES_Windows, source-published Classic45 as of last verified status (ZIP ID 1raWTE8eJEl1M80lhzvn4CiJIyF1M4pB6, SHA-256 d0a226041dcae58f89567faa005506690592d5dac8969549651398d3dde2cb1d); candidate Classic46/Run041 not published/merged.
B. Separate Enhanced combat candidate: published Enhanced46 18-part source, combined SHA-256 7eca762acd26410969f26d1964032c853a7eba792e020bb7c8144698b091181e. Enhanced47 and Run041 unmerged/unpublished; Fork4 full-game lineage unaccepted.
C. GitHub Bomb-Daddy/NES/Original_KOEI_Windows_Game/Build40R4: *separate original KOEI-inspired game*, not the canonical NES reconstruction. Its Classic launcher executes a user-supplied English NES ROM in Mesen or a configured external executable, while its Enhanced combat is a Python original-game adapter. GitHub source is useful to diagnose that product's code, but cannot prove a defect in an unseen Classic45/Enhanced46 ZIP.

Evidence statuses: VERIFIED_STATIC = exact source inspected through GitHub; OBSERVED_RUNTIME = original NES native evidence reported and audited in Nov5 research; TESTED_PORT = cited dated developer candidate runs, not independently rerun in this assessment; REPORTED_UNREPRODUCED = older source/build tests read as records; INFERRED = specific hypothesis; UNKNOWN = not proven. No Windows application, physical Xbox device or NES-versus-port trace was run for this assessment.

## 2. Sources and precedence

1. Director one-to-one blueprint v1.00: https://docs.google.com/document/d/1NGeF-eBd71keh6FfSTiWvxIlr8j82v58ftoNW607YZY/edit
2. Unified combat implementation/parity directive: https://docs.google.com/document/d/19vRA_y0q7dcaS7MmuvHtPe_yTmeobBcHqPfKNdX-jF4/edit
3. Battle Answer Matrix v1.4 Q01-Q13: https://drive.google.com/file/d/1r4KOqPC8uN1VDmaNlfpjHa_k0J__GYZZ/view
4. Battle Answer Matrix v1.4 Q14-Q22: https://drive.google.com/file/d/1uUNq4BWRie_b-DR3nh7h0jUqiAgw4Mtd/view
5. November 2 final corrected stock-NES trace: https://docs.google.com/document/d/1uZFMG5I0uR5GxmTZ1uhkdbmxSD6hRyhg1dogZ0D_lJk/edit
6. November 5 research index and 99-run catalog: https://github.com/Bomb-Daddy/NES/tree/rotk2-battle-gap-research-20261105/research/ROTK2_NES/battle_gap_closure
7. November 5 findings: https://github.com/Bomb-Daddy/NES/blob/rotk2-battle-gap-research-20261105/research/ROTK2_NES/battle_gap_closure/FINDINGS_v1.00.md
8. Latest fully Drive-published source and unpublished candidate status: https://docs.google.com/document/d/1UXDXguwS0EGKKTInRWZraSFWabRD6pSuYHq9LKTBjZE/edit
9. Run041 candidate status: https://docs.google.com/document/d/1vg9T9olhhAINTUvx0Z9ap7pqXu1ZyUkZji5GPMjfuqo/edit
10. GitHub Build40R4 source examined here: third_branch/integrated.py, third_branch/officer_battle_adapter.py, third_branch/rotk2_battle_core.py, third_branch/offscreen_q18.py, third_branch/tactical_view.py, third_branch/rom_battle_map.py, and launcher/classic.py at GitHub main, inspected 2026-11-05. Git blob identifiers are exposed in connector readbacks; follow exact file links in Section 4.

The Nov5 native original catalog reports 99 runs: 93 passed the evidence-integrity gates and 6 retained failures. This is **not** 93 NES-versus-Windows parity passes. Several meaningful capture/succession results use explicitly prepared original-game RAM fixtures. The complete mechanics matrix remains v1.4; Nov2 corrections and Nov5 findings amend it, not replace it.

## 3. What the new evidence has resolved

- G01: Current original NES battle day, weather, wind, rice and gold addresses with mapper-bank discipline and native writer evidence. Earlier +9-offset FCS historical cause remains unknown; do not blindly adjust all addresses.
- G02 / Q03: Daily defending rice includes deployed officers **plus province-resident officers**, not just units visible on map. Original control: 20,000 count, loss 666 hundredths; same checkpoint with one 1,000-troop resident zeroed, loss 633; unrelated strategic officer zeroed, loss remains 666. Main day order is food, then weather, then fire. The older matrix prose must be corrected.
- G03 / Q09: A Flee destination is a selection, **not a guarantee** of successful escape. The original writer checks success, removes officer from battle list and occupancy, converts troop word /100 for a peaceful destination, then inserts into destination's serving list; failure goes to capture. Whole withdrawal has per-officer choices and outcomes. Native P29 whole withdrawal and individual cases support source.
- G04 / Q14: Release helper considers allowed supplied province, then an ordered two-pass neighboring scan, then the ruler's +2 **first linked owned-province head**, potentially not adjacent; if no destination, ordinary officer follows a special free-officer path and a defeated ruler may be offered Behead only. Thus adjacent-only ruler release and arbitrary remote fallback are both incorrect.
- G05/G06 / Q19: Five formerly opaque strategic actions are decoded (diplomacy, joint action, recruitment, covert action, spy activation). The D409 chain is FRIENDLY transfer / occupation of empty territory, **not hostile invasion**. Separate D92B/D98A path is hostile launch. Preserve documented 16-bit intermediate arithmetic; a nonzero dispatch return means queue control, not necessarily diplomatic success.
- G07: Native Normal/Simultaneous/Charge/Fire results, NES actual success/failure messages, white blinking cursor, View versus B summary distinction, and prepared officer/ruler fate/succession records now exist. Prepared Cai Mao succession is a real original-record commit under declared fixture lineage, not proof of natural campaign reachability.
- G09: Normal troop assignment is bounded, but pending allied reinforcements can conditionally return to a strategic list with battle-scale troops. Whether ordinary lawful play reaches that chain is PARTIAL. Do not fix by unapproved clamp or assume bug reachable.

## 4. Deficiency register — evidence and impact

D00 — P0 — Source and product identity / unmerged candidates — VERIFIED_STATIC documents, source ZIP not freshly inspected
Classic45 and Enhanced46 are latest fully Drive-published candidates; subsequent Classic46/Enhanced47, Run040/Run041 were local, with failed uploads. GitHub Build40R4 is a different original-game product. A parity patch against the GitHub original game does not update canonical Classic. Block implementation until immutable exact parent, parts, ZIP CRC, entrypoints, mode adapters and rollback are pinned. Do NOT destroy local/unpublished Run041 work merely because publication failed.
Acceptance: independently hash canonical source; identify selected production tree and exact commits; build a lineage/delta census; reject mismatched ancestry before patching.

D01 — P0 — One kernel versus emulator Classic — VERIFIED_STATIC
GitHub Build40R4 launcher/classic.py chooses a configured Classic executable or launches Mesen+NES ROM. Enhanced uses the original-game Python officer adapter. These are not a proven shared code kernel. Two-mode launcher existence is not evidence that the native Classic rewrite and Enhanced share Q01-Q22.
Acceptance: create explicit mode architecture diagram, with a pure battle-kernel API and adapters. For the native NES port, verify source-level shared rule implementation or immutable equivalence; do not claim same code when Classic is the ROM emulator.

D02 — P0 — Officer lists, side slots, day phases — PARTIAL on canonical candidate, VERIFIED_STATIC Build40R4 shortcut
Original has four side slot lists 0/1 defenders, 2/3 invaders, active-unit order, reserve and resident distinctions. GitHub adapter uses an array and simplified CPU phase; full original slot/head/linked-list/event order not established. Simultaneous and outcomes can change based on which officers act and which slot holds them.
Acceptance: four-slot normalized state with distinct fielded, pending, resident, captured, withdrawn; replay two-human and both invasion directions; test every slot with empty/occupied/pending cases, immutability and single ownership.

D03 — P0 — Deployment/map and battle entry — VERIFIED_STATIC for exact 41 ROM grids; PARTIAL UI
GitHub rom_battle_map.py validates 41 source maps and zones. officer_battle_adapter.initialize() picks first cell for a human, explicitly says interactive placement unimplemented; CPU random ordering and original stacking choices not matched. Legacy seven-province synthetic terrain is intentionally nonoriginal.
Acceptance: region-correct interactive Position where?, defender castle, up to all original officers and valid stacks, blocked/invalid cells, AI seeded candidate path, reverse P19/P20, P1 and P41. No generic terrain fallback in Classic.

D04 — P0 — Food, weather, day 30 and month transition — OBSERVED_RUNTIME original, VERIFIED_STATIC Build40R4 mismatch
GitHub officer adapter food sums fielded defenders and slot-1 reserves; no demonstrated resident province troop traversal. It increments day and settles DEFENDER when day>30. November 5 source describes 30-day continuation/pending-force serialization and resume path, while old Q13 describes defender holding on timeout. This is a **source-flow reconciliation gate**, not permission to invent endless battles. Correct original call order: food -> weather -> fire.
Acceptance: compare original states at day 1/2, 29/30, a continuing active battle with pending allies, month progression and cold-load; resident-zero/unrelated-officer negative controls; distinguish defender held province from whether battle state persists across months.

D05 — P0 — Withdrawal and escape — VERIFIED_STATIC discrepancy in Build40R4
officer_battle_adapter._withdraw immediately sets every chosen unit fled and one destination, then settles whole army. Original hsovl2 D648 performs per-officer escape success/capture checks; individual and whole use legal lists and per-officer results; peaceful versus attacked destination changes scale. Native P29 whole withdraw leaves Yuan Shu in P29 with troop word 100 despite severe defeat wording.
Acceptance: same original checkpoint and deterministic test RNG; selected valid destination can fail/capture; cancellation and no-destination are non-mutating; whole withdraw multiple officers may produce mixed fates; verify destination officer linkage and exact once-only troop ownership, scaled/unscaled transitions.

D06 — P0 — Prisoner Release, ruler decision and succession — VERIFIED_STATIC discrepancy in Build40R4, PARTIAL canonical Run041
GitHub ruler_release_destination() requires adjacent defender-owned province and blocks nonadjacent head fallback. Source's two-pass search also accepts certain unowned/attacked destinations; zero helper return for a normal officer is a free-officer path, not an arbitrary no-release dead end. Build40R4 ruler Behead clears ruler pointer then may mark faction dead; native prepared continuation installs Cai Mao into ruler/province lists. Run041 reported forged-choice and menu guards but ruler succession, owner-linked head and full capture-list order remain PARTIAL.
Acceptance: table-driven five source destination branches, only legal menu choices; Recruit/Release/Behead for ordinary prisoner; ruler Release/Behead; zero-helper behavior; Cai Mao successor and ruler deletion; save/quit/cold-load before and after settlement; no duplicate treasury or officers.

D07 — P1 — Reinforcement origin, delay and pending allies — VERIFIED_STATIC Build40R4 policy shortcut; original ordinary-play edge PARTIAL
GitHub _queue_reinforcements accepts caller-provided arrival_day; tactical UI sets day+1. CPU help request selects a sorted first eligible officer, delay +2. These are explicit PORT policies rather than exact original source routing. Pending allied troops and contested-region cell availability matter; source G09 shows a conditional scaled pending list return that might affect subsequent battle.
Acceptance: source-derived delay and candidate eligibility, slot-1/3 pool conservation, no-room deferral, no double arrival, alliance ends between month update/resume, attacked vs peaceful return; separately execute legitimate-play G09 reachability research without changing Classic arithmetic in advance.

D08 — P1 — Tactical attack/charge/simultaneous/fire and Move Enemy — VERIFIED_STATIC partial code
GitHub rotk2_battle_core.py contains 13x12 neighbors, movement, exchange and fire primitives; adapter adds per-officer handlers. Numeric tests and mode implementation are promising but not event-by-event NES parity. Exact charge multi-exchange/breakthrough, simultaneous multi-target order, per-step passive forest reactions, Move Enemy target drag and fire/wind ordering require fixed-input goldens. The Nov5 original first-exchange snapshots provide high-value checks.
Acceptance: first-exchange and post-turn independent golden fixtures for Normal, Simultaneous, Charge and Fire; original messages and RNG-consumption order; one adjacent and multiple-adjacent opponents; mutation and rollback tests. Do not equate identical result winner with same mechanic.

D09 — P1 — Tactical AI Q16/Q17 — VERIFIED_STATIC heuristic remaining
GitHub _auto_ai() picks closest/distance targets and a deterministic fire exit, and holds a commander under 3000. Its comments explicitly say original ranking and retreat branch remain unknown. Nov5 Q19 source closure is STRATEGIC AI, not proof of exact tactical officer targeting. Preserve original non-optimal quirks where known.
Acceptance: original-source trace fixtures for at least the first 10 per-officer CPU choices, burn evacuation, commander 2999/3000, occupied target, defender first and no-legal-order; before/after full ledger and RNG stream.

D10 — P1 — CPU versus CPU Q18 off-screen resolver — VERIFIED_STATIC partial
GitHub offscreen_q18.py explicitly labels partial; 6 rounds exist but original intermediate integer widths, retreat/capture roll, remaining-rice event order and final decisions are not exact verified. Pending reinforcements currently rejected rather than resumed. A passing tactical unit test is no proof of this separate resolver.
Acceptance: original byte-derived per-round score/troop/rice golden vectors at exact boundaries, 6:1 and 7:1, shortage, zero, overflow, retreat refuge, and no generated or lost officer records; never route CPU-only battle into a player tactical fallback to hide gap.

D11 — P1 — Numeric precision, original quirks, RNG and edge cases — VERIFIED_STATIC source; PORT parity NOT RUN
Use explicit signed-truncation vs Python floor, u16 multiply/wrap, troop times 100 and division, caps and fire money/rice penalties, stable random draw **consumption order**. Hardware-timed NES RNG cannot be claimed identical to a deterministic PC generator; test matched **supplied random streams** instead. G09 656-plus troop reachability is an outstanding research question, not authorization to silently clamp.
Acceptance: 0,1,100,655,656 troop boundaries; negative numerator, zero rice, max pools, wrap/cap mutation; replay same battle in both modes with identical logical input and injected RNG stream.

D12 — P0/P2 — Original battle UI/controller-only path — OBSERVED_RUNTIME NES and VERIFIED_STATIC Build40R4 difference
GitHub tactical_view.py draws 1024x768 header/right information panel with 17 Tk buttons; a separate Toplevel; raw dict result messages; B outside target mode closes window; target picking is coordinate cursor, not original per-prompt NES command flow. The Nov3 source comparison showed the original six-command lower-left menu (Move, Attack, Flee, View, Tactics, Wait), nested options, scrolling map, one-step direction confirmation, Day/View panel and battle result screens. Nov5 research resolves previously missing Fire/attack text, white cursor, prisoner/ruler UI, and B summary versus View.
Acceptance: 1280x720 controller-only NES Classic content with source-accurate proportions/menus, no scrolling outside viewport, A/B/Start/Select behavior, all prompts/nested actions and results; real native application run and visual frame-by-frame comparison. Original-game Build40R4 4:3 ruling does not supersede NES Windows 1280x720 acceptance.

D13 — P0/P2 — Settlement atomicity and campaign persistence — PARTIAL
Source Q14 transfer chains span battle slots, officer/captured/prisoner lists, province and ruler ownership, supplies, allegiance and roster succession. Build40R4 has transactions/saves but provisional distant-garrison and retreat policies, making successful test counts inadequate for exact source. Canonical Run041 has local green tests but unpublished and unmerged.
Acceptance: complete save/quit/cold-load before decision, after each prisoner choice, after battle, after month; same event ledger, exactly one owner per soldier/officer, correct ruler succession and province histories; failure/invalid-choice leaves byte-equivalent save and state.

D14 — P2 — Tests, shipping and acceptance — REPORTED_UNREPRODUCED
Nov2 selected Classic and Enhanced Linux tests and one Xvfb software-controller order are recorded. These are not Windows application/exe, physical Xbox, native NES comparison or extended full campaign proof. New Nov5 99 original runs strengthen source oracle only. Avoid percent-complete claims or promotion based on unit count.
Acceptance: actual Windows launch, both modes and all six scenarios, 41 provinces, menu/controller-only battle path, 30 matched event traces with source plus injected RNG, CPU-only results, saves, 2-human cases, physical Xbox and installer cold-start; independently inspect every skip/failure.

## 5. Priorities and dependency order for the forthcoming implementation plan

P0 Gate 0 — Source and identity: verify Drive source hashes and parent ancestry, recover or explicitly classify unpublished Run041 and the unmerged Enhanced47 fork, choose a single reproducible baseline for EACH product; no copy from GitHub original game to canonical NES repository without explicit port adapter and review.
P0 Slice 1 — Normalize battle state/slots and one playable battle on source map; demonstrate a legal human Move, attack, end day, food, retreat and resulting campaign ledger with genuine controller menu.
P0 Slice 2 — Settlement corrections now source-closed: implement Q14 release helper, ruler succession, Q09 per-officer escape and destination writer; first design fail-before assertions on the existing candidate.
P0 Slice 3 — Resident food, day/continuation and allied pending force state; regression from original P19/P20 and month boundary.
P1 Slice 4 — Attack/movement/fire event parity, Q16/Q17 first decisions, Q18 offscreen; enforce same default battle kernel between native Classic and Enhanced on equivalent fixture with injected RNG.
P2 Slice 5 — Original NES UI and controller-only prompts integrated into actual Windows installer, then full campaign acceptance and visual QA. Tests and negative controls attach to EVERY slice; do not defer all QA to final phase.

**First decisive executable task** after source selection: implement a source-trace red test for Q14 release fallback (supplied province; neighbor pass 1; neighbor pass 2; ruler-owned head fallback; zero-helper), against BOTH actual selected battle trees. Confirm it fails against the current candidate; patch one shared source-backed helper; rerun and compare with original source/fixture. This is smaller and less ambiguous than beginning with AI or a complete renderer rewrite. Before any mutation, pin source and preserve rollback.

## 6. Hostile acceptance tests: reusable fixture names

F-01 D1/D2 P19->P20 resident food: field 19000, resident 1000, total 20000 -> rice -666; resident zero -> -633; unrelated officer changed -> -666. Class OBSERVED_RUNTIME original.
F-02 P19->P20 whole withdrawal to P29: choose original popup, verify source/field list empty, destination owner and head, source troop word 100 after /100, result0/6, visible harsh defeat text, same campaign roster at following screen.
F-03 Individual Flee: same allowed province selection, one success to friendly P19, one fail/captured and one cancel; watch no fabricated teleport.
F-04 Q14 release: source five-way destination test; ordinary Recruit/Release/Behead; no matching destination free-officer handling.
F-05 Prepared ruler death: original Liu Biao name ID -> FFFF and Cai Mao leader head in ruler/province records; preserves explicit RAM editing ancestry; one observer-positive and one stale-precommit negative.
F-06 First exchange: identical original day5 checkpoint Normal/Simultaneous 9608 vs 692 and Charge 9641 vs 856 at bounded first completed exchange, not later enemy phase; Fire creates/clears occupancy with observed English NES messages.
F-07 Q19 strategic: D409 friendly transfer or empty occupation, never conquest; queue nonzero not synonymous with success; low-word arithmetic test 6000*200/(600+1) -> 33, not wide-product 1996.
F-08 Day30 and month continuation with in-transit ally, preserve original source branch and cold-load; do not choose winner based on an unqualified 30-day rule.
F-09 Q18 actual NPC six-round intermediate arithmetic and fallback on missing valid refuge.
F-10 Complete player path at 1280x720, B = prompt back/summary where appropriate, not exit from battle; prepared prisoner UI choices; physical Xbox separate.
Each fixture requires: original source bytes hash, checkpoint lineage, emulator/input/frames, original observed state, exact port version/command/environment, RNG fixture, full pre/post, stdout/stderr/exit, skipped and mutated negative tests. New author-created parity JSON belongs private NES source/evidence; commercial ROM bytes and raw copyrighted assets remain private.

## 7. Evidence exclusions / risks

- No canonical ZIP was downloaded, extracted or executed in this review. Claims about Classic45 and Enhanced46 internals outside published docs are REPORTED_UNREPRODUCED, not freshly verified source.
- GitHub Build40R4 source *was* directly inspected and code-location observations are VERIFIED_STATIC, but it is the original KOEI-inspired game.
- Nov5 prepared original fixture branches do not prove natural encounter frequency; original emulation evidence is distinct from a port pass.
- G09 ordinary pending-ally scale transition remains not proven reachable. Historical FCS +9 cause and exact older 130/60 roster remain unavailable; they do not stop using newly verified live fields.
- The 30-day continuation versus defensive timeout wording needs source/control-flow and runtime reconciliation before implementation; do not decree a new rule based on UI text alone.
- No fresh Test-Port, Windows, ROM-vs-port, physical Xbox or installation tests were conducted for this report. Avoid PASS VERIFIED for those gates.

## 8. Producer disposition

Status: AT RISK / PARTIAL, NOT RELEASE READY. The new November 5 research materially reduces the knowledge bottleneck (Q09, Q14 and Q19 especially); the remaining bottleneck is faithful implementation, canonical source-promotion and player-facing verification. No new Director design decision is necessary to preserve 1:1 Classic behavior or equivalent default Enhanced combat. Next operational step is the hash-pinned source selection and Q14 golden red regression, not another broad research project. This is a planning-input assessment and not evidence that Nobunaga or Cao Cao received, acknowledged or executed an order.
