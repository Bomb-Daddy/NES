# Fork Four Build 07 — Whole-game integrated source (Linux-executed; Windows not yet certified)

**Scope:** Original KOEI-inspired Windows/Linux Enhanced game, NOT ROTK II NES canonical game or DOS restoration.
**Build base:** Full earlier Build06C game source, which incorporates the Build01–Build06C changes onto original full source v1.19, with Build07 direct modifications to live integrated gameplay, not a source delta.
**Director authority:** latest SUP-OG-UI-0092 and later direct clarifications. Historical 41 province Scenario 1; preserve all twelve named AI profiles and three control factions, original 107 active generals and no fictional ruler; controller-first 960×720 4:3 viewport in 1280×720 window.

## Included subsystems and integration checks
- Governor, officer/relation, personnel, real assigned-army cohorts, battle settlement and hostile regression fixtures.
- Original 189 January 107 general army registry, 2,670 troop groups, 100 groups per general maximum, provisional original untouched-state provenance.
- ROTK II researched Skill/Arms registry and Train Troops; complete NES tactical parity NOT yet proved.
- Director 41 province ROM civilian workforce /10, seven equal allocated work categories, zero visible unassigned category, legal monthly inflows.
- Build07 fixes historic Recruit command to debit seven REAL worker jobs, not an unavailable phantom `unassigned`; preserves atomic preview/commit/cold load and original core command journal; changes `disband_troops` and governor autonomous work to restore/transfer real jobs; fixes named AI recruitment candidates and fallback AI feasibility.
- Approved 12-category province-first UI, no permanent officer roster or Quick Orders or fantasy numerical panels; zero map command-button widgets; observer/manual/contextual province TARGET modes.
- Context advisor 20% AI hook, approved economic/market and domestic systems, troop Training/Arms; source includes Windows setup/entrypoint and Linux launchers.
- Both retired commands (`cultivate` direct and `occupy_vacant`) disabled in live Enhanced Campaign UI/command gateway/AI. Older `og/core` standalone test harness and historical tool fixtures retain old core command `cultivate` **inside source development tree**, and need release packaging isolation/retirement; historical evidence cannot be erased or misreported as fully purged.

## Actually run tests (TESTED_PORT_LINUX)
- Full `PYTHONDONTWRITEBYTECODE=1 PYTHONPATH=. python -m pytest -q --tb=short`: **449 passed, 2 xfailed, 3 subtests passed**, exit code 0.
- Targeted historical Scenario 1 full-command interface test: 6 passed (p4/p6/p21 one-unit recruit preview/commit/coldload, cost/worker conservation, 100-unit general cap, disband/no retired bucket, 12 AI style candidate exercise).
- Real 12-month historic campaign seed 100201, advisor weight 2000: exit 0; game events include 93 successful recruit commits, 12 safe real Move Army orders, three attacks, without any retired command. Longer 120-month archive campaign attempts are IN PROGRESS / INCOMPLETE until independent evidence passes.

## Hostile QA — still blocking release
- No proven latest Nobunaga production Round4 merge in Drive; this whole-game source is a distinct Build07 snapshot, not final Windows production.
- User UI still missing historical province artwork, looping seasonal GIF, fully audited 27+ actual transactions, some context workflows, all required sound.
- All 41 actual original tactical maps/season art, troop Skill/Arms parity and player strategy AI expansion not finally accepted. Success of 12-month run does not imply year-10 victory.
- Separate historical standalone `og/core` Cultivate fixture remains in dev/test tree; exclude from shippable runtime when safely migrated.
- Windows installer and physical USB Xbox controller-only acceptance not executed here. Two existing expected failures remain open.

## Run
`python3 -m third_branch.windows_doctor --strict` then `python3 -m third_branch`, or `./PLAY_LINUX.sh`. Windows project entrypoints `PLAY_WINDOWS.bat` / `BUILD_WINDOWS_EXE.bat` require native Windows and their listed build dependencies; source archive is NOT a compiled `.exe`.

**Acceptance:** Linux regression PASS for this hash; full integrated Windows game/PASS player acceptance and all ten independent ten-year campaigns not yet proven. Do not claim otherwise.
