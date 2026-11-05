# GK1 Windows fidelity: gap-closure execution and implementation plan
**Directive:** ZL-GK1-PORT-RESEARCH-0009
**Version:** 1.00 | **Date:** 2026-11-05 (America/Detroit)
**Author / producer:** Zhuge Liang
**To:** Nobunaga (lead Windows implementation); Cao Cao (independent hostile QA)
**Authority:** Project Director's explicit request for complete NES gameplay, visual and audio fidelity on Windows. Direct Director decisions override this engineering plan.
**Destination:** Bomb-Daddy/NES, main branch, research/Genghis_Khan_NES. Companion numbered handoff: Retro Gaming / Collaboration.
**State at publication:** RESEARCH/IMPLEMENTATION DIRECTIVE PUBLISHED; no claim the developers have read or executed it.

## 0. Success definition: a Windows game, not a stack of reports

Deliver a standalone, playable Windows Genghis Khan I (USA NES) recreation whose Classic mode matches the original player's choices, outcomes, menus, timing, pictures, sound, music, scenarios, AI, combat, persistence and endings. The source code need not resemble the 6502 cartridge. The source ROM is an oracle and must remain unmodified for Classic comparisons. The Windows target must support keyboard and dependable Xbox controller-only play, with a 1280x720 shell and an integer-scaled, visually faithful original-game viewport; do not distort or redraw Classic art without an explicitly separate Enhanced option. Save/quit/cold-load/continue must work.

Every newly closed research issue must produce, in the same work package, a directly callable Windows-engine function, data table, named asset, state transition, UI contract, audio event, or an explicitly bounded implementation-ready specification with a named implementing owner and immediate integration task. A standalone test result without this deliverable is NOT DONE. Do not wait for all 106 gaps to be closed before building: use closed economic mechanics now, implement other subsystems incrementally, and run differential NES/Windows QA continuously.

### Three separate source identities (never merge)
1. **Classic oracle:** private stock Genghis Khan (USA).nes, 262,160 bytes, SHA-256 ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3. Use its pristine records, screens, behavior, audio, and original SRAM as reference. This hash is research-reported and must be rechecked when the exact original binary is retrieved for each research session.
2. **Existing approved mod baseline:** Fonzarelli/Demobilize NES ROM, SHA-256 61ad28688d503231d12537b2cbebb073ae5f91ec1391f825c1d05312436393be. Actual battle/retreat and mod-specific save tests are evidence for those observed paths, not automatic stock-game parity for every rule.
3. **Newer Enhanced candidate:** the Director-authorized shared-BP/Rest NES ROM, SHA-256 4764d46359581be1e7d12bcb85bd375c84cd4497d1084d9d306a6e884afbe7c5. It changes original mechanics. The Rest instruction is 33% per remaining strategic order, using floor(maxBP*33*orders/100), with three orders providing 99%, not an automatic 100%. This content may become optional Windows Enhanced mode, never a replacement for Classic's six-attribute mechanics. Its delivered source is an isolated bundle, not a published GitHub merge.

Research source of truth: v1.07 167-row matrix on main, 61 PASS VERIFIED / 40 PARTIAL / 66 NOT RUN. Its baseline research verdicts remain as written; later narrowly observed mod or graphics results should be entered as separate dated addenda with exact provenance, not silently promoted. The v1.07 11 economic batches are closed for their own scope, NOT for full Windows integration.

## 1. Owners, artifacts, and decision policy

- **Zhuge Liang:** sequence work, retrieve original-ROM evidence, consolidate all open issues, own knowledge-to-engine translation, publish instructions in Collaboration, approve evidence labels and completion register, route inter-agent inputs.
- **Nobunaga:** build the separate Genghis Khan Windows target and integrate each accepted mechanic/asset/audio package; produce executable build, known commit, deterministic test command and native Windows acceptance evidence when run. This is not the ROTK2_NES_Windows or ROTK2_DOS_Windows repository; do not mix source trees. Choose an explicitly named isolated Genghis Windows directory/repository before writing game code and record the source location and commit.
- **Cao Cao:** independently audit calculations, control/path coverage, screen and APU comparisons, negative controls, saves, builds, and claim-to-evidence consistency. Return actionable defects through Zhuge Liang.
- **Director:** controls vision/scope and accepts the playable release; no routine approval delays. Escalate only irreversible design changes, spending, genuine permissions, rights/public distribution decisions, or accepted deviations in Classic fidelity.

Each closure unit MUST publish these five pieces:
1. **Answer:** exact behavior including inputs, outputs, bounds, integer width/rounding, RNG and temporal order, or exact asset and trigger data.
2. **Evidence:** ROM identity, run environment, screen/state before and after, exact controller inputs and frames, source bank/offset when available; preserve raw stdout/stderr, counterexamples and failed attempts.
3. **Port contract:** signature/data shape, state fields touched, UI/audio callouts, and dependencies. Implementer does not need to re-read a research narrative to write the code.
4. **Executable acceptance:** Windows-vs-NES fixture with success/failure/cancel/edge cases as applicable, plus one human-playable path where the player can see the effect.
5. **Status and next task:** verified evidence class and link, actual code commit and build status separately; never label an inferred rule PASS VERIFIED.

Stop investigating when the documented contract is sufficiently decisive to implement and repeatably compare the material branches in Windows; do not demand exhaustive disassembly of unrelated original code. Conversely, a menu screenshot alone cannot close an outcome rule. A Windows-only simulation is not original-game proof.

## 2. Ready-to-use sources and recovery

**Research and economic baseline**
- Full matrix: https://github.com/Bomb-Daddy/NES/blob/main/research/Genghis_Khan_NES/v1.07/GK1_COMPLETE_167_PROOF_MATRIX_v1.07_2026-11-04.csv
- Economic completion and reproduction: https://github.com/Bomb-Daddy/NES/blob/main/research/Genghis_Khan_NES/v1.07/GK1_ECONOMIC_MAPPING_COMPLETION_AND_REPRODUCTION_v1.07_2026-11-04.md
- Original economics: https://github.com/Bomb-Daddy/NES/blob/main/research/Genghis_Khan_NES/v1.07/GK1_ECONOMIC_MAPPING_v1.07_2026-11-04.md
- Ten original-economy appendices, source replayers and native log verifiers under v1.07; 12 private evidence ZIP volumes are in Drive folder https://drive.google.com/drive/folders/1mJ72ezJb9kzIGUHHXPTh3lXtiVxWjEL7 . Reassemble and verify from the recorded manifest; never fake missing captures.

**Emulator / oracle**
- Portable Mesen/AI bundle: https://drive.google.com/file/d/10a4CW22GYVbOOwAYx4MWPZ1ED496HzjD/view
- Restore instructions: https://drive.google.com/file/d/1iu5Vpd1A0p2n6zeY0VIqUYnivx_M1H5d/view
- Private stock ROM also appears in the Director's ChatGPT Library. Its raw transfer may require the authorized private Drive/archive path; blocked direct materialization in one context is NOT a claim that the ROM or Mesen is unavailable. The preceding session restored the headless emulator and successfully booted/controller-tested the modded ROM. Pin the exact ROM at every new experiment.
- Later mod battle example: https://drive.google.com/file/d/1AbGkCwjaJ2HXXgnbb3I4v-NDzfwsw2q1/view
- Ability-output audit: https://drive.google.com/file/d/1lFS5mRDjKmVYykRBsRde87NnB8jbeCbr/view
- Shared BP delivered implementation: https://drive.google.com/file/d/19Z7HbseZUEDYB37_NTxM0j8eZ5sSdkmA/view

**Graphics**
- Existing separate GitHub branch: https://github.com/Bomb-Daddy/NES/tree/genghis-khan-assets/research/Genghis_Khan_NES/Assets
- 64 original bank 11/12 portraits, 9 static tile sets, 19 attribute tables, 5 observed palette sets, partial loader map, and 151 PNGs are already present. Their exact appearance still needs comprehensive screen/asset routing proof. The 43 computed graphics upload sites, bank 10 faces, nametables, sprites/OAM, fonts, battle/event/terrain screens and audio are explicitly unresolved in that branch.
- Director-approved seasonal map pack: https://drive.google.com/file/d/1e9iFSmUNJGI4-wnoYAUWGVcSoHYn3ebc/view . Keep as optional Enhanced artwork, not Classic oracle.

## 3. Execution method: slice-by-slice, no false finish line

**Iteration A — harvest proof and ship a thin Windows slice.**
Create the dedicated Windows Genghis tree, build a real launchable 1280x720/controller navigable shell, implement one original scenario entry, menu/navigation, one verified economic order, Pass/season progression, and native save/load from the v1.07 economic reference. Obtain one real player-visible build before beginning wholesale documentation of unknown subsystems. Preserve exact Classic game values; a debug fixture cannot replace a played slice.

**Iteration B — resolve a blocking unknown and integrate it immediately.**
For each workstream below: (1) reproduce a meaningful original screen/state via Mesen or validated original-instruction replay; (2) branch through normal action, refusal, cancellation and boundary; (3) map source bytes/RAM/RNG if numerical; (4) deliver Windows-ready contract; (5) implement and produce new playable build; (6) run differential game-state/UI/audio check; (7) let Cao Cao attempt to break it; (8) close or keep explicit defect. Workstreams may overlap, but do not let the engine wait for documentation unrelated to its next missing feature.

**Iteration C — campaign and packaging acceptance.**
The final gates are both original scenario/ruler coverage, all strategic order families, autonomous AI turns, field battles, diplomacy, espionage, heirs/succession, real endings, complete audio and animation, reliable saves/cold loads, multi-year state persistence, true controller-only Windows navigation, and installable/launchable Windows distribution. Linux/headless port tests cannot be called physical Windows hardware PASS.

## 4. Workstream 00: evidence/version ledger and source-to-port scaffolding (P0)

**Existing:** 167-row matrix, main economic reference, private evidence archive, controller scripts, graphics branch, separately patched ROMs.
**Unknown to close:** which source and exact revision currently controls each Classic rule, including conflicts in old v1.04/v1.06 docs, and how every contract is linked to Windows code.

**Actions**
- Snapshot the current GitHub main commit, graphics branch commit, all Drive source hashes and mod variants. Select the *stock* ROM as the sole original-game oracle. Build a searchable per-ID manifest linking v1.07 evidence, original bank/offset, scenario, engine module, test and screenshot.
- Create Genghis Windows project skeleton and explicit modules: GameState, Rules/Economy, StrategicAI, CommandDispatch, Diplomacy/Spies/Dynasty, TacticalBattle, Presentation/UI, NESPalette/Assets, Audio, Persistence, PortParity.
- Define canonical deterministic state serialization with fields for ruler/country, labor, commodities, troops, arms/skill, 6 Classic attributes, prince/kinship, treaties, war/battle state, season/time, AI flags, RNG seed and settings. Never reuse ROM RAM addresses as the Windows save-file schema without an adapter.
- Implement debug-only state import from controlled original SRAM/observations with checksums, plus a side-by-side verifier. Keep original inputs private.
- Maintain a machine-readable closeout ledger: ID, original status, later evidence, source hash, port contract file, code commit, test output, human acceptance, limitations, blocker and next task.

**Output:** Windows executable boots and can load one verified fixture; closure ledger points to executable modules, not merely prose. **Acceptance:** actual launch and a saved stable state reloaded with same values, and one 1:1 economic transaction replay versus the ROM; no unrelated original-game rules invented.

## 5. Workstream 01: scenario starts, world tables, selection, end conditions (P0/P1)
**IDs:** SC01-SC06, SC08-SC11; CF13; SE09.

- Reuse already extracted economic data for all 41 country templates and 15 labeled startup conditions; extract the *remaining non-economic* ruler identities, traits, names, portraits, family links, owned countries, adjacency and access topology, fortifications, troop-unit setup and scenario flags directly from the stock cartridge.
- Walk both Mongol and World starts; cover all legal selectable rulers/difficulties, manual versus automatic setup, invalid/cancel paths, ruler customization, the initial season, menu sequence, starting armies/countries, and UI descriptions. For each distinct start compute canonical GameState hash and compare it to source SRAM/screens.
- Determine turn order and termination: conquest, elimination, vassal absorption, resignation/End, loss, AI victory, no-heir and player succession; obtain at least one representative actual win/loss or source-routine branch replay with Mesen rendering for endings.
- Map every campaign coordinate, province number, ownership overlay, and adjacent-country route. Make data tables work for live conquest rather than screenshots only.

**Implement:** scenario setup factory, immutable country/ruler data, live strategic map and ending state machine.
**Close only when:** the Windows selector covers every NES-legal start; original and port start matrices and ending flows agree at verified checkpoints. A two-scenario title/menu screenshot is NOT a scenario pass.

## 6. Workstream 02: menus, orders, error messages, controller semantics (P0/P1)
**IDs:** CF10, CF11; VW01-VW06; RE03, RE07; portions of TR05, PE08, SE01, OT03, OT06-OT09.

- Enumerate every root command and submenu, numeric input, target picker, prompt and view page. Tie exact command code to its displayed label, controller input and cost table; save screenshots and state after navigation. Negative control: cancel, rejected invalid target, unaffordable command, held button, zero quantity and out-of-range quantity.
- Record original cursor origin, direction/wrap, auto-repeat, selection highlight, text reveal delays, confirmation order, prompt overlays, input debouncing, disabled choices and effect of Wait/Display/Animation/Battle settings.
- Extract original text and message tables including punctuation and line breaks; link to event causes and replaceable message IDs rather than embedding anonymous prose in code.
- Preserve authentic 256x240 viewport composition under the Windows 1280x720 shell, with clean integer scaling, no collision between borders, portraits, menus, messages or overlay layers. Xbox controller alone must access title/new/load, every menu and numeric entry; keyboard may remain additionally available.

**Implement:** data-driven command registry, navigation state machine, view screens, message catalog and help for shell-only settings.
**Close only when:** all menu edges reachable in original are reachable in Windows; a route replay yields matching screens/text and correct commit/cancel side effects. Gameplay orders cannot silently execute on mere highlight.

## 7. Workstream 03: complete military organization and strategic war (P0, critical)
**IDs:** AS03-AS05, WA01-WA08, CF07; some MV01 and PE03.

- Use one controlled base checkpoint with variation in troops (0, 1, 20, 61, 999), unit classes and field-unit count. Resolve Assign Army's 1-10 allocations, cavalry/infantry/archer types, size rounding, field command units, lord/prince selection, inventory effects, and order/ability costs. Preserve 1 strategic Troop -> 10 battlefield soldiers only where native evidence proves it.
- Execute War Self and War Prince separately, including adjacent/nonadjacent/ally/vassal targets, unsupported targets, insufficient soldiers, food and gold allocation, order cost, attacker's starting force, defending garrison and castle placement.
- Identify how battle ends return to strategic state: losses, survivors, morale/arms/skill decay, food stock, captured goods, prisoners, occupied country, ruler/vassal replacement, territory ownership, withdraw, game over and order counter.
- The existing mod demonstrated a specific battle/retreat and troop ledger; reuse capture method but do not promote entire stock-war semantics from that one case. Isolate original ROM behavior from Demobilize side effects.
- Build a single **battle-to-strategic round-trip fixture**: War -> deployment -> commands -> outcome -> changed world -> next season -> save -> reload.

**Implement:** strategic war command and combat-state entry/exit adapter; army assignment and cost engine.
**Close only when:** no duplicated/lost troops or goods in representative winning, losing, fleeing and refused cases, with matching NES/Windows state deltas.

## 8. Workstream 04: tactical battlefield renderer, rules and AI (P0, largest mechanical gap)
**IDs:** BT01-BT12; WA05-WA08; CF07; RE05.

**Native research procedure, not a 12-screenshot exercise**
1. Capture actual battle deployment and identify battlefield coordinates, terrain tile IDs, castles/roads/water/forest, flags, cursor, turn/day counters, food and unit stats. Extract map-generation/lookup tables and dynamic graphics loads, not just a baked battle image.
2. Save controlled checkpoints before decisions: move (Entire/Divide), normal attack, duel, arrows, ambush, Pass, Demand, SOS, Raid/Hunt, Flee. For each, capture pre/post troop count, location, damage, success flag, commander changes, terrain modifiers, RNG and AI reply.
3. Isolate combat arithmetic with original code paths and bounded replay when needed. Vary troop type, skill, arms, ruler abilities, terrain, defenders, fortification, supply and days separately. Derive exact integer order, clamp, rounding, RNG calls and morale interactions; reject undocumented equations from FAQs unless tested.
4. Trace path legality and movement reach for every unit type, siege entry/exits, weather if active, ranged range/line-of-sight, anti-softlock, unit splitting and maximum deploy count.
5. Compare human battle AI against autonomous/off-screen prince battle AI. These are not assumed identical engines. Determine day budget (manual mentions 30/60/90) from executed branch; resolve battle endings, fleeing and captured leaders.
6. Make tests adversarial: force zero troops/food, last-day retreat, absent defender, tie, negative unit counts, full garrison, invalid terrain, and same RNG under different single-input mutations.

**Implement:** self-contained tactical battle scene, discrete command/legal-action model, original-like renderer, damage/outcome functions and battle AI. Early milestone: human can start, play and complete a small battle with resulting persistent state change.
**Close only when:** at least one *real playable* offensive and defensive battle, each with a successful command sequence and a failed/refused case, agree with captured NES checkpoints and leave correct persistent campaign state. Full source-code decompilation is not required.

## 9. Workstream 05: strategic AI, vassals and delegated policy (P0/P1)
**IDs:** PL01-PL06; CF03, CF09; SC09; SE09; WA02, WA07.

- Retrieve complete AI turn traces with original ROM for both scenarios, multiple difficulty levels, and one/multi-country rulers. Record state before and after each autonomous action; do not treat a single bounded AI-economy routine as a full AI policy.
- Identify actual action ordering, target heuristics, priorities, army/hire/morale/food thresholds, cheats versus player rules, economy support, trade, diplomacy, spying, uprising response, supply and war selection.
- Acquire a vassal/prince, open Policy menu and verify domestic and foreign policy configuration and subsequent AI execution. Probe zero-policy/autonomy, depleted abilities, unreachable objectives, no-frontier expansion and absence of officers.
- Resolve whether AI consumes the same six Classic attributes and/or command limits, how it replenishes stats between seasons, and which apparently impossible actions have original exceptions.
- Save a deterministic multi-country replay and a stochastic replay; check exact state where RNG seeded/known and property invariants where it cannot be pinned. Document any nondeterminism honestly.

**Implement:** AI decision pipeline with trace logs and vassal policy interpreter. Do not replace source-grounded AI with arbitrary difficulty bonuses.
**Close only when:** the Windows AI independently completes multiple years with wars, production and government changes; controlled original checkpoints match its decisions and state effects within documented RNG bounds.

## 10. Workstream 06: treaty, tribute, espionage, succession and family (P0/P1)
**IDs:** DT01-DT07; SP01-SP06; PE03-PE09; VW02-VW04; CF08.

**Diplomacy:** Map target validity and country adjacency, alliance/vassal/tribute request and refusal, exact charges/refunds, 5-year alliance claim and expiry, leader-stat modifiers, AI response and effects on war/commerce. Execute accepted and rejected negotiations with controlled rulers/RNG and season progression. A message-only success is insufficient: verify relation table and future AI/war effects.

**Espionage:** Implement original View Report, Wreck Havoc, Kill, Search State; record gold fees (guide reports 10/50/100 as unverified), ability/order costs, chance formula, spy detection, victim changes, delayed effects, retaliation and cancellation. Use original false/positive/invalid/insufficient-funds branches and linked memory bytes. Confirm no unintended access to unrevealed foreign data in View screens.

**Dynasty:** Decode Select/Demote/Promote/Marry/Switch, prince attributes, children, ruler age, spouse/heir choices, death probability, assassination, prince rule, vassal succession and dynasty extinction. Exercise at least one genuine ruler-death -> inheritance/new-ruler/defeat transition, plus prevented/ineligible cases. Preserve original Classic six-attribute identity. The optional Shared BP mod is a different Enhanced rule; do not overwrite classic costs/outcomes from it.

**Implement:** DiplomaticRelations, SpyState and DynastyState engines with durable save fields and AI hooks.
**Close only when:** a working Windows campaign can make, refuse, expire or break an agreement, execute detected/undetected espionage, appoint or lose a ruler, and persist the resulting state with NES-matched deltas.

## 11. Workstream 07: remaining economics, time and cross-system effects (P1, targeted only)
**IDs:** SE01, SE05, SE09; AS03; TR03-TR05; PE08; CF03, CF06, CF08; RE05.

The completed v1.07 economics is the authoritative seed for labor, Trade, merchants, rates, seasonal taxes, yearly ROI, stock, workforce and native integer arithmetic. **Do not restart economic mapping.** Only resolve uncovered interactions:
- Six Classic ruler abilities are not merely spending wallets: source replay demonstrated certain outcomes depend on ability amounts. Map every remaining output reader for Train Self/Prince, diplomacy, war, spy and AI, with same before/after ability values and fixed RNG. Distinguish command charge from outcome modifier.
- Verify all end-to-end Pass variants (0/1/2/3 orders), growth, aging, famine, rebellion, stolen goods, seasonal event order, and how losses/captures affect commodity/worker balances across multiple provinces.
- Resolve unmet manpower conservation after army casualties, multi-country logistics, Move country and tax/rebellion propagation, not just peaceful static economy.
- Translate v1.07 formulas into independent deterministic Windows modules; make a stock-ROM fixture vector for each already closed equation and two distinct starting states, rather than re-deriving it.

**Implement:** original-cost and ability behavior contracts, event scheduler, cross-system conservation assertions.
**Close only when:** the integrated Windows economic effects agree with NES across actual war/government/turn transitions. If a newly found counterexample invalidates v1.07, issue a targeted erratum with a regression, not a silent economic rewrite.

## 12. Workstream 08: original graphics, screen layouts and animation (P0/P1)
**IDs:** CF11, CF13, RE07; BT11 and battle UI rendering; OT06-OT07.

- Start from existing graphics branch, verify exact bank/offset/hash for the 64 extracted bank 11/12 portraits, five observed palettes, 9 static tile sets and 19 attributes. Cross-reference characters and display screens so the same image is not assigned to the wrong ruler. Decode bank 10 additional faces.
- Enumerate all 43 computed CHR upload sites with Mesen/FCEUX actual PPU traces; distinguish dynamic battle, town/event, icon and font patterns. Capture nametables, sprite OAM (priority/flip/position), palette changes, scrolling and original visual timing.
- Build a scene atlas: title/intro, every scenario/ruler picker, strategic map for each season, command/view/policy screen, merchant, family portraits, war entry, all battle terrain and result screens, ending, save/load, error prompt and options.
- Extract pixel-exact source assets into a versioned manifest: original ROM file offset/length, decoded 2bpp data, palette, tile group, atlas coordinates, screen trigger, animation frames, crop, layering and reference screenshot. Preserve pixel-aspect policy in the Classic renderer.
- Record map-owner/flag and sprite overlays as live stateful layers, not a static composite; seasonal map/portrait replacements stay Enhanced optional. Verify toggle does not alter decisions, hit targets or save data.

**Implement:** Classic NES renderer using original assets, dynamic map sprites, correct font widths and overlay priorities; 1280x720 shell uses integer scaling and sidebars only if they do not alter Classic scene semantics.
**Close only when:** a representative screenshot set for every visually distinct screen and all four strategic seasons matches the stock ROM (prefer exact pixel diff for unchanged palette/crop; document justified emulator palette variance) and battle sprites/maps move correctly. Asset-existence counts alone are not closure.

## 13. Workstream 09: complete original audio and option behaviors (P0/P1)
**IDs:** CF12; OT04, OT05, OT08; OT03 and related event triggers.

- Inspect original ROM APU-write routines, music engine tables, instrument/envelope, tempo, loops, sound-effect sequences and track-ID selection. Identify use of both NES pulse channels, triangle/noise and DMC if any; do not assume all five channels active.
- Instrument original gameplay/menus with synchronized APU-register traces and frame-indexed sound capture for title, both scenario starts, ordinary menus, trade, seasonal transitions, weather/events, diplomacy, war declaration, tactical combat, victory, defeat, death, succession and ending. Add deliberately silent/paused and repeated-trigger controls.
- Build a track/FX manifest: source addresses, track ID, triggering event, frame timing, duration, loops, priority/preemption, per-channel envelope/volume/pitch and Music/Sound menu toggle behavior. Identify whether settings survive save/quit.
- Reproduce audio in the Windows engine through a faithful 2A03/APU renderer driven by decoded original event sequences, or identical deterministic playback tables; avoid substituting unrelated MIDI or a recreated soundtrack just because it sounds similar.
- Compare captured NES and port waveforms/notes/events with known normalization of emulator output; test loops, interruptions, overlap, mute/unmute, music/sfx priority and Windows audio device restart.

**Implement:** AudioEventBus, NES soundtrack/FX data, deterministic playback and settings.
**Close only when:** each distinct original track/effect has an identified trigger and usable Windows implementation, every Music/Sound option functions, and recorded cases match timing and sonic behavior to acceptance. No new music composition required.

## 14. Workstream 10: save, load, quit, recovery and Windows input/platform (P0)
**IDs:** OT01-OT09; RE11, RE13; IM03-IM09.

- Reverse-map original menu Save/End/Wait/Display/Animation/Battle options, SRAM persistence and controls. Separately validate stock Save -> emulator shutdown -> cold boot -> load, using a *natural* state transition rather than a memory-restored serialization fixture. Capture whole state and settings before/after.
- Define a new Windows versioned save schema that retains every original gameplay variable plus RNG/event state, owned map, current order, rulers/heirs, armies, diplomacy/spy flags, audio settings. Use checksum and atomic temp-write/rename; corruption and interrupted write must not silently start a fresh game.
- Implement durable saved game slots and migrations. Classic stock saves and Enhanced shared-BP saves require explicit version tagging and non-destructive conversion rules; never silently load a patched NES checkpoint into the pristine baseline.
- Test quitting mid-turn, saving after conquest, after diplomacy, mid-battle if the original allows it, after succession, and after seasonal reset. Physical Xbox controller: every game, text and number entry, save selection and quit without keyboard/mouse. Test Windows 11 installed and cold launched.
- Preserve failures: missing/corrupted save, full disk, wrong version, controller disconnect/reconnect, minimized/restored audio, resizing and display setting toggles.

**Implement:** Windows Persistence and Controller service, packaging with all game assets; clearly separate original-game save semantics from added PC safety features.
**Close only when:** a person can start, play battle and seasons, save/quit, cold-load the same state, and finish a session using only the controller on Windows. Headless Linux alone does not establish this.

## 15. Workstream 11: system integration, actual campaigns and player acceptance (P0 gate)

**IDs:** IM01-IM09; SC01-SC11; RE13; CF09, CF10, CF11.

Build milestones (each must generate a real executable + integrated test, not merely a written spec):
- **M0 — reproducible baseline:** stock ROM and all known source/asset hashes pinned, Mesen reproducible, indexed 106-open-gap register, dedicated Windows project selected.
- **M1 — first playable season:** Windows title -> NES scenario -> orders -> verified Tax/Trade/Labor -> Pass -> new season -> save/cold-load; controller-only navigation; original map graphics and at least one authentic sound.
- **M2 — first completed battle:** live unit deployment, tactical commands, original terrain, AI response, win/defeat/retreat -> persistent strategic state and subsequent economic turn; all needed images/sounds.
- **M3 — living world:** AI multi-year campaigns, two scenarios, ruler selection, vassal policies, alliances, espionage, children/heirs, conquest, assassination, death and succession with saves.
- **M4 — full audiovisual fidelity:** all original screens, animations, map overlays, portraits, tracks and SFX with source-to-port manifest; verify audio toggles and event priority.
- **M5 — Windows release candidate:** packaged Windows game launches on real Windows, controller-only complete campaign paths, AI and graphics/audio parity, save/cold-load/recovery, and no gameplay-critical open defects; Director plays and accepts.

At M1 we do not wait for all source questions. At M5 all *material* Classic actions must be implemented and compared. No hidden narrowing of game scope is permitted. An unknown material original rule remains BLOCKED until source-derived, observed in Mesen, or explicitly waived by the Director as a named deviation.

## 16. Prioritization and research stop/go criteria

| Priority | Research slice | First Windows dependency | Stop condition |
|---|---|---|---|
| P0 | Stock ROM/evidence + build skeleton | First playable build | Can reproduce original start and v1.07 economy effect |
| P0 | Strategic war + tactical combat | Complete battle round trip | Combat outcome persists identically |
| P0 | World/scenario + menus/controller | All scenario starts | Player navigates all original legal actions |
| P0 | Audio engine and graphics loader | Complete original aesthetic | Triggers/assets correct; no silent placeholder release |
| P0 | Save/cold-load | Any playable campaign | Crash-safe native persistence, real Windows check |
| P1 | AI + vassals | Multi-year play | Independent simulation respects observed original outcomes |
| P1 | Dynasty/treaty/spy | Complete game systems | One accepted/rejected/expired/terminal scenario per branch |
| P1 | Remaining cross-system economics | Correct campaigns | Integration proof without economy duplication |
| P2 | Rare UI timing and unobservable source implementation details | Finishing polish | Only pursue if observable discrepancy or critical path depends on it |

**No test-for-test's-sake:** before executing an NES probe, write its expected Port Contract field. Cancel an investigation that cannot alter code, data, a visual/audio asset, or acceptance unless it is needed for reproducibility. Focus on branching outcomes and boundary behaviors, not arbitrary accumulation of play hours.

## 17. Evidence and QA gate for each contract

Use labels precisely: VERIFIED_STATIC for ROM bytes/decoded original routines; OBSERVED_RUNTIME for recorded NES input/frame/state behavior; TESTED_PORT for exact executable/commit/command/exit/effect; REPORTED_UNREPRODUCED for an agent claim not independently checked; INFERRED for provisional behavior; UNKNOWN; CONTRADICTED. Runtime capture does not prove universal rules; synthetic CPU replay does not prove original menu reachability; a native original ROM success does not prove Windows port.

Minimum proof per numeric command: hash-pinned original ROM, exact scenario/ruler/difficulty/year/state, initial and final bytes, 6502 mapped code path where applicable, input and frame log, positive and cancellation/invalid cases, boundary/rounding/RNG variation, second independent state, recorded command expense and subsequent season effect. Port fixture must fail under a deliberate mutation and then pass after correction. Preserve all original failed attempts without repackaging excerpts as raw logs.

Minimum proof per visual/audio feature: original scene and state, original trace/frame timestamp, asset/track manifest and Windows output, documented pixel/waveform comparison, trigger absence/disabled behavior, and correct live-state overlay/priority. Tester actually watches/listens where automatic measures cannot settle appearance or quality.

Minimum proof per Windows milestone: exact source commit, build hash, complete real Windows start command, logs, exit/result, controller model/mapping, OS, save path, one real state-changing loop and reproducible negative test. Do not call Linux screenshot simulation or FCEUX-only capture a Windows acceptance.

Cao Cao returns discrepancies with minimal reproduction to Zhuge Liang; Nobunaga fixes same build, reruns acceptance, and producer updates register only after evidence review. Keep the currently passing areas passing under replay and mutation tests.

## 18. First executable work order — start immediately

**Nobunaga / Windows implementer**
1. Choose and identify the isolated Windows Genghis project location/branch. Do not commit this game's source to ROTK2 or the custom Original game. Make an actual Windows-capable executable that starts a game, accepts Xbox controller navigation and displays the original map/menu using existing extracted pixels.
2. Import v1.07 closed Tax/Give/Trade/Assign/Season rules (all 11 economic batches) as versioned data/functions, with the original six Classic stats. Run 3 real gameplay fixtures: a purchase/sale, Labor reassignment, and Spring-to-Summer/annual change, then save and reload.
3. Provide commit, executable build/launch, screenshots, before/after state, limitations, and blocked next function. Do not claim full port complete.

**Zhuge Liang / ROM investigator**
1. Restore original stock ROM and headless Mesen using saved Drive bundle; pin exact identity and event input. The new enhanced-ROM boot in the preceding session proves the emulator route but does not replace original stock identity.
2. Prioritize a single original War -> tactical deploy -> Move/Attack -> result -> strategic return trace. Pair with original APU/PPU capture during battle to yield combat, graphics and audio discoveries in one experiment.
3. Publish first Source-to-Port contracts for Assign Army, Battle Start/End and audio event routing; attach real input screenshots, native SRAM and source offsets.
4. Run one diplomacy acceptance/refusal and one succession/election-of-heir branch only after the battle blocker is removed, unless parallel source analysis can advance independently.

**Cao Cao / hostile QA**
1. Independently validate schema, original source hashes/ROM revision, claims in v1.07, combat and audio capture provenance.
2. Build mutation/cancel/rounding tests against the new Windows slice; flag every unsupported default.
3. Audit controller-only user journey and save replay; report broken routes and source-to-port mismatches, with actual executable evidence.

**Next producer closeout:** update the 106-row appendix and completion register with exact newly verified evidence and real Windows integration. If any task remains not run or blocked, it stays explicit; do not declare milestone complete from a chat summary.

## 19. Source and release references

- GitHub economic-source version: main commit 46b2faf5585074302e31ab376f9e2c6faeb97e03 at planning inspection. Verify again before implementation; do not freeze future development to this historical head.
- Independent economic QA and retained failures: https://github.com/Bomb-Daddy/NES/blob/main/research/Genghis_Khan_NES/v1.07/QA/INDEPENDENT_REVIEW.md
- Graphics research: https://github.com/Bomb-Daddy/NES/blob/genghis-khan-assets/research/Genghis_Khan_NES/Assets/docs/Genghis_Khan_NES_Graphics_Loader_Findings_20261103.md
- Genghis military documentary first pass: https://drive.google.com/file/d/1OHpG-TocfhPzsLVxXmJmNfWQj3Y0MxQu/view
- Latest shared-BP mod is intentionally separate from stock Classic; it is not proof that original six-attribute outputs may be replaced.

## Appendix A — Every v1.07 non-PASS obligation: implementation-linked discovery backlog

**Authority of row state:** the exact 2026-11-04 GitHub 167-row matrix. This appendix reprints all 106 row IDs not already PASS VERIFIED, so no open item is hidden inside a workstream summary. The original v1.07 row status is not automatically updated by later narrowly-scoped mod tests, graphic extraction, or planning. The group workstreams above supply the implementation contract and owner. For each row, the original matrix's required evidence states the minimum research to capture, while the immediate port action is the receiving workstream. If newly verified evidence exists, attach a dated addendum and prove the actual Windows effect.


### Scenario, ruler, country and game progression

| ID / source state | Unclosed original requirement | Required decisive evidence |
|---|---|---|
| **SC01** — PARTIAL | Mongol Conquest: complete at least one full campaign path, not merely start screen | Season-by-season captured playthrough, victory/end conditions and reproducible log |
| **SC02** — PARTIAL | World Conquest: complete at least one full campaign path, not merely start screen | Same complete runtime evidence and end-state capture |
| **SC03** — PARTIAL | Validate every selectable ruler and starting country, including World Conquest 27-country and Mongol Conquest 14-country rosters as applicable | Scenario/ruler/country matrix, starting resources/owners, missing/invalid selectable cases |
| **SC04** — PARTIAL | Validate all difficulty settings | Matched starts and controlled AI/combat/economy comparisons per difficulty |
| **SC05** — PARTIAL | Ruler-stat generation and manual/automatic startup choices | Record all settings, ranges, repeatability, starting effects and cancellation |
| **SC06** — PARTIAL | Map country IDs, capitals, names, rulers, neighbors, ownership and starting stock | ROM table + in-game screen cross-check for both maps |
| **SC08** — PARTIAL | Establish different province economic and military behavior | Same command tested in at least two different provinces, leaders and resource levels |
| **SC09** — PARTIAL | Long-running AI turn progression and no hidden menu softlocks | Recorded multi-year campaign, AI events, error logs, no-step timeout recovery |
| **SC10** — NOT RUN | Campaign conclusion, extinction, succession and game-over outcomes | Original screenshots, persistent state and transitions for representative win/loss |
| **SC11** — PARTIAL | All scenario data and mechanics versioned without mixing Genghis Khan II, DOS, SNES or custom-game assumptions | Per-finding provenance, cartridge hash and exact source version |

### Assign

| ID / source state | Unclosed original requirement | Required decisive evidence |
|---|---|---|
| **AS03** — PARTIAL | Full workforce conservation across countries and between Draft, Hire, war, crisis, Move | Total-population/drafted-ledger conservation with cross-command controls |
| **AS04** — NOT RUN | Assign Army: division among 1–10 field units and unit types | Completed Army command, troop splits/types and battle deployment |
| **AS05** — NOT RUN | Assign Army rounding, troop/skill/arms changes, reallocation constraints | Zero/maximum unit count, odd splits, changed and unchanged cases |

### Train

| ID / source state | Unclosed original requirement | Required decisive evidence |
|---|---|---|
| **TR03** — PARTIAL | Train Self: which ability selected, gain formula, max and repeated use | Cycle all ruler attributes and state boundaries |
| **TR04** — NOT RUN | Train Prince: select target, changed abilities, failure when no prince | Confirmed Prince training on eligible and ineligible roster |
| **TR05** — PARTIAL | All Train branches: action/attribute costs, depleted attributes, no-ops/cancel | Before/after ruler abilities, order counts and failure screens |

### People / succession

| ID / source state | Unclosed original requirement | Required decisive evidence |
|---|---|---|
| **PE03** — PARTIAL | People Switch: reassign vassal prince and base roles | Multi-country executed state; beginning one-country error already observed |
| **PE04** — NOT RUN | People Select: choose/appoint prince, eligibility and succession implications | Successful and rejected selection with dynasty fields |
| **PE05** — NOT RUN | People Demote: remove or transfer prince, troop/country effects | Two-prince multi-country experiment and undo/negative controls |
| **PE06** — NOT RUN | People Promote: heir designation, prerequisites and ruler-death interaction | Promotion and succession tests under controlled leader death |
| **PE07** — NOT RUN | People Marry: spouse/child/heir, diplomacy effects, refusals | Accepted/rejected marriages, children and family state |
| **PE08** — PARTIAL | All People commands order, personal ability and resource costs, canceled actions | Counters and negative controls per subcommand |
| **PE09** — NOT RUN | Aging, heirs, ruler death, prince inheritance and dynastic/AI succession | Multi-year and forced-state test with preserved save/replay |

### View / information

| ID / source state | Unclosed original requirement | Required decisive evidence |
|---|---|---|
| **VW01** — PARTIAL | Own State view: exact fields, units, page navigation and no cost | Screenshot complete panel and unmodified counters |
| **VW02** — NOT RUN | Own Ruler and foreign Ruler intelligence permissions | Compare own/foreign before and after Spy View Report |
| **VW03** — NOT RUN | Allies view: treaty list, duration and vassal relationship visibility | Compare before/after treaty and five-year expiry |
| **VW04** — NOT RUN | Prince and Child view: stats, family ties, selection, leader changes | Known roster versus screens, after changes |
| **VW05** — NOT RUN | Stock view: item types, count, local commodity and potential display caps | Record all stock pages against RAM known holdings |
| **VW06** — NOT RUN | Verify foreign-state intelligence costs, spy prerequisite, cancel and error | Positive and negative target cases with orders/abilities |

### Move / multi-country logistics

| ID / source state | Unclosed original requirement | Required decisive evidence |
|---|---|---|
| **MV01** — PARTIAL | Move Base after acquiring at least two countries | Base selection, ruler location and subsequent order allotments |

### Policy / delegated government

| ID / source state | Unclosed original requirement | Required decisive evidence |
|---|---|---|
| **PL01** — NOT RUN | Acquire/appoint autonomous vassal and open functioning Policy menu | Multi-country vassal state and completed policy change |
| **PL02** — PARTIAL | Domestic order controls: tax, labor, construction, relief, recruitment | Record policy flags and next AI-driven resource changes |
| **PL03** — NOT RUN | Foreign policy controls: war, treaty, espionage and expansion | Policy change followed by witnessed AI action |
| **PL04** — NOT RUN | Cost/prerequisites/limits of each policy and who can change it | Negative control for nonvassal and unqualified ruler |
| **PL05** — PARTIAL | Vassal order counts, economic autonomy and province-state persistence | Cross-season independent vassal state versus direct control |
| **PL06** — PARTIAL | Starting one-country No vassal states message is a precondition only | Already observed no-op; confirm subsequent success |

### Treaty / diplomacy

| ID / source state | Unclosed original requirement | Required decisive evidence |
|---|---|---|
| **DT01** — NOT RUN | Become Vassal: target selection, success and failure effects | Accepted/refused outcomes, tribute/ownership and AI response |
| **DT02** — NOT RUN | Pay Us/tribute: amount, transfer timing, ability costs and refusal | Before/after two countries and diplomacy counters |
| **DT03** — PARTIAL | Ally: duration, source-reported five-year term, renewal and expiry | Successful alliance + multi-year countdown and boundary |
| **DT04** — NOT RUN | Diplomatic success probabilities and leader stats/military strength | Matched repeated controlled tries and mapped RNG/code |
| **DT05** — PARTIAL | Geographic eligibility, neutral/nonadjacent targets and target validity | Target prompt observed; completed accepted and rejected examples missing |
| **DT06** — PARTIAL | Diplomatic command order/ability/gold cost and cancellation | Successful and rejected attempts with save/replay |
| **DT07** — NOT RUN | Diplomatic relations effects on warfare, spying, trade and vassal control | Compare same commands under allies/enemies/vassals |

### Spy / intelligence

| ID / source state | Unclosed original requirement | Required decisive evidence |
|---|---|---|
| **SP01** — NOT RUN | Spy View Report: paid intelligence, target validity and enabling foreign View | Before/after access check, funds and orders |
| **SP02** — NOT RUN | Wreck Havoc: Economy/Defense damage, detection and failures | Successful/refused tests, mapped source/target SRAM |
| **SP03** — NOT RUN | Kill: target ability/leader consequences, death and probability | Controlled attack with success/failure/succession |
| **SP04** — NOT RUN | Search State: identify/remove enemy spies, cost and timing | Positive/negative spy-present states |
| **SP05** — NOT RUN | Source-reported 10/50/100 gold prices, command/ability costs and affordability | Cost matrix verified against real menu and accepted action |
| **SP06** — NOT RUN | Espionage AI response, retaliation, and cross-season persistence | Spy action with turn advancement and adverse-response controls |

### War strategic

| ID / source state | Unclosed original requirement | Required decisive evidence |
|---|---|---|
| **WA01** — NOT RUN | War Self: initiate invasion, target selection and starting force constraints | Actual battle entrance, troop/food drawdown, valid/invalid target |
| **WA02** — NOT RUN | War Prince: delegated auto-resolve and prince stats/quality effect | Complete AI-led battle, victory/failure and state conservation |
| **WA03** — NOT RUN | Declare war against ally, vassal, nonadjacent and neutral targets | Validity matrix and original error messages |
| **WA04** — NOT RUN | Full war food, gold, troop/casualty and captured-stock accounting | Prebattle/postbattle state of both countries |
| **WA05** — NOT RUN | Combat damage and victory/loss equations across troop types and commanders | Repeated matched battle/ROM writer traces |
| **WA06** — NOT RUN | Postwar conquer/annex, ruler/prince capture and regional government | Player conquest state and next-season operation |
| **WA07** — NOT RUN | AI-initiated invasion, defense, relief, and outcomes | Observable independent AI actions and persisted combat result |
| **WA08** — NOT RUN | War command costs, canceled invasions and retreat/failure handling | Counter snapshots on each end state |

### Tactical combat and battle UI

| ID / source state | Unclosed original requirement | Required decisive evidence |
|---|---|---|
| **BT01** — NOT RUN | Deploy 1–10 units, cavalry/infantry/archer types and starting positions | Original battle screenshot and unit stats |
| **BT02** — NOT RUN | Tactical Move Entire versus Divide | Actual position changes, bounds, stamina and per-turn cost |
| **BT03** — NOT RUN | Attack Normal versus Duel | Battle results, damage/casualties, commander effects and replay |
| **BT04** — NOT RUN | Attack Arrows | Ammo, range, terrain, lethality, failure controls |
| **BT05** — NOT RUN | Attack Ambush | Preconditions, surprise modifiers, skill and outcomes |
| **BT06** — NOT RUN | Battle Pass | Turn/round progression and opposing AI response |
| **BT07** — NOT RUN | Other Demand / surrender | Success/refusal and resulting province/army state |
| **BT08** — NOT RUN | Other SOS / reinforcements | Reinforcement availability, travel, resource changes |
| **BT09** — NOT RUN | Other Raid / Hunt | Food/supply changes, weather/terrain and success limits |
| **BT10** — NOT RUN | Flee / retreat | Flee target, casualties, province state and game-over branches |
| **BT11** — NOT RUN | Terrain/siege/weather/fortification/archer paths and limits | Battle map matrix and representative battles in each environment |
| **BT12** — NOT RUN | Battle duration (source mentions 30/60/90-day possibilities), battle food drain and terminal states | Explicit game-time counters and exhaustion tests |

### Pass and seasonal simulation

| ID / source state | Unclosed original requirement | Required decisive evidence |
|---|---|---|
| **SE01** — PARTIAL | Pass when all three orders vs one or zero; no-op and command cost | Repeat for 3,2,1,0 orders in distinct states |
| **SE05** — PARTIAL | Births, aging, population increase, sickness/starvation | Distinct seasonal events with probability/replay and new population |
| **SE09** — PARTIAL | Full seasonal operation order: AI actions, growth, tax, wages, food, harvest and market | Observed event ordering with watchpoints and code trace |

### Other and cartridge services

| ID / source state | Unclosed original requirement | Required decisive evidence |
|---|---|---|
| **OT01** — NOT RUN | Save native in-game game file | Save menu and persistent battery-backed data |
| **OT02** — NOT RUN | Quit emulator/cold-load original save with equal game state | Cold-run checksum, scenario/province/ruler/turn/stock comparison |
| **OT03** — NOT RUN | Wait text speed changes and persists | Toggle screenshots, settings state and reboot behavior |
| **OT04** — NOT RUN | Music option toggle and volume/track behavior | User-visible UI plus audio playback test |
| **OT05** — NOT RUN | Sound option toggle and effects behavior | Audio event and off/on comparison |
| **OT06** — NOT RUN | Display toggle/setting effects | Screenshot-level comparison and controller navigation |
| **OT07** — NOT RUN | Animation toggle/setting effects | Equivalent battle/turn captures with mode on/off |
| **OT08** — NOT RUN | Battle option settings and consequence in combat | Game option change and actual subsequent battle |
| **OT09** — NOT RUN | End/quit behavior, unsaved-state handling and reload | Exit/restart negative and positive save cases |

### Cross-cutting numerical and content fidelity

| ID / source state | Unclosed original requirement | Required decisive evidence |
|---|---|---|
| **CF03** — PARTIAL | Multi-year and multi-country economic conservation for currency, Food and goods | Independent ledger from original state transitions |
| **CF06** — PARTIAL | Morale cap, rebellion/robbery outcomes, tax and relief effects | Threshold sweep, negative controls and event traces |
| **CF07** — PARTIAL | Weapon quality Arms and Skill interaction with troop strength and losses | Buy/Train/War intersecting state changes |
| **CF08** — PARTIAL | Different rulers' abilities and command feasibility/costs | Stat-probe matrix and deliberate insufficient-ability failures |
| **CF09** — PARTIAL | AI economics: tax, hiring, allocation, trading, diplomacy and cheat/fairness rules | Controlled AI province observation + code paths |
| **CF10** — PARTIAL | Exact menu input handling, confirm/cancel, numerical input and event timing | Controller sequences that always reach same state; timeout recovery |
| **CF11** — PARTIAL | Every game text/status/error message, portraits/sprites/assets and source mapping | Complete script/asset extraction and ROM-to-screen crosswalk |
| **CF12** — NOT RUN | Music and sound assets and when invoked | Audio extraction or original output and event evidence |
| **CF13** — PARTIAL | Map topology and province/country numbering across both campaigns | All mapped country/adjacency records with validated screen IDs |
| **CF14** — PARTIAL | Confidence classification separated across manual, guide, emulator, source and proposed designs | Evidence-linked register covering every resolved and unresolved rule |

### ROM reverse engineering, repeatability and forensic QA

| ID / source state | Unclosed original requirement | Required decisive evidence |
|---|---|---|
| **RE03** — PARTIAL | Recover all command handler entry points and call graph | Code/data coverage and bank-aware call table for 14 commands |
| **RE05** — PARTIAL | Reproduce every numeric formula from code, then validate in a second independent start | ROM arithmetic, test vectors, boundary/rollback, no exceptions |
| **RE07** — PARTIAL | Full graphics frame evidence: exact actual screenshot per menu/submenu | Screenshot+input manifests and absent-menu gap coverage |
| **RE09** — PARTIAL | Preserve all timed-out/incomplete/negative test logs and failed controls | Full raw logs and failure status, not curated proof |
| **RE11** — NOT RUN | Save/reload cartridge RAM faithfully and test corruption/recovery | Cold-load state, battery save file format and negative corruption control |
| **RE13** — NOT RUN | Verify SNES counterpart only if supplied and relevant; NES evidence remains NES-specific | Separate ROM hash and execution evidence rather than platform assumption |

### Reimplementation and human acceptance (separate from research)

| ID / source state | Unclosed original requirement | Required decisive evidence |
|---|---|---|
| **IM01** — NOT RUN | Decide exact target: standalone faithful Genghis Windows reproduction versus research input to custom KOEI game | Director scope/authority before modifying any separate game repository |
| **IM02** — PARTIAL | Implement Genghis economy independent deterministic reference functions when exact rules are known | Current fixtures cover observed amounts only; no complete economy engine |
| **IM03** — NOT RUN | Enforce model conservation of population, workforce, Food, gold and items | Unit, property, mutation and save/replay tests against original vectors |
| **IM04** — NOT RUN | Integrate merchant networks, Rate, weapons, hiring and seasonal economics in authorized codebase | Actual UI-controlled gameplay and direct NES-to-port parity vectors |
| **IM05** — NOT RUN | Integrate 14 command families, AI autonomy, succession, war and battle only if faithful target authorized | Real executable gameplay, not menu-only or fixture passes |
| **IM06** — NOT RUN | Build complete campaigns and static scenario data if a faithful port is in scope | All source-defined starts and end states, cross-scenario regression tests |
| **IM07** — NOT RUN | Test native Windows launch, controller-only navigation, save/quit/cold-load and packaging if port approved | Hardware-specific witnessed acceptance, not Linux emulator simulation |
| **IM08** — PARTIAL | Separate original NES rules from custom game's approved Body Points, unified merchant market and 500-unit pricing | No unapproved cross-import; independent source and design labels |
| **IM09** — NOT RUN | Human playability and final director sign-off for any release, not for research alone | Player run-through, regression log and explicit acceptance |

## Appendix B — completion register field contract

Each non-PASS source row gets a ledger entry with: ID; parent group/workstream; original v1.07 status; dated later evidence and classification; source ROM hash; host/Mesen version; capture IDs and full logs; associated byte/VM routine where known; exact Windows module/API/data/asset/track ID; implementing owner; code commit; tested Windows build/hash and launch command; pass/fail including negative controls; user-visible behavior; remaining limitation; blocker; next executable task. A row may become research PASS VERIFIED while its Windows implementation remains NOT RUN, or Windows TESTED_PORT while native original behavior remains UNKNOWN. Never conflate the two axes.

**End of plan. Research closure is subordinate to delivering the complete, enjoyable, faithful Windows game.**
