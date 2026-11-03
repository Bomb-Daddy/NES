# Original KOEI-inspired Enhanced PC game — Build40R4 (CANDIDATE, not a release)

Separate Original game (not the ROTK II NES restoration). Source tree: [`Build40R4/`](./Build40R4). Derived from Build40R3 (no-music) with Director rulings of 2026-11-02 applied. No music files, ROMs or ZIP binaries are in this folder.

## Director rulings applied
- All windows and screens are **4:3** (strategic map and province screen 1024×768, battle screen 1024×768, dialogs resized to 4:3).
- China map: **no sidebar**. Rescaled bordered map with a one-line button-help strip across the bottom.
- **No "inspect only" menu.** From the province screen: Select / Back to Strategic Map opens the China map; pick a province; A (or click the highlighted province) opens it in the standard province UI. Own province: full commands. Any other province (including vacant): read-only, single command **Exit**. Province-screen left/right no longer cycles provinces. `OrdersWindow` refuses non-player provinces.
- Forge Arms command removed from scope (earlier ruling).

## Defects fixed (hostile QA 2026-11-02)
- Engine: list/dict values in id-like parameters raised raw `TypeError`; now `BAD_PARAMS`, no state change. Non-string actor rejected cleanly.
- Save loader: `null`/non-object documents raised raw `TypeError`; now `BAD_SAVE`.
- Core engine `distribute_food`/`distribute_goods`: now capped at 100 units and scaled by charisma (neutral at 50), matching the approved ECON-MASTER v2.1 override; the two former expected-fail tests now pass.
- Province screen text clipping fixed (nav labels, message word-wrap).
- Selected-province marker on the China map is now clearly visible and drawn for vacant provinces.
- Tests added: duplicate request id checked before any handler runs; stale expected-revision; wrong-owner refusal; journal/revision consistency; courier-event journal contract (Ally/Joint events are their own journal entries, revision +2 by design); view-province flow (6 tests).

## Test status (Linux, KOEI_DISABLE_AUDIO=1, xvfb)
`681 passed, 7 skipped, 0 xfailed` on the full suite. Hostile checks run on this build: ~12,000 random engine calls (both scenarios) with no crash and no state change on refusal; ~2,200 corrupted saves all refused cleanly; ~7,800 random controller-button presses with no exception; 14 mutation experiments (every surviving mutant got a killing test).

## Evidence classes and what is NOT yet claimed
- Layer 1 (engine) evidence exists for all 29 non-battle province orders (preview, commit, duplicate id, bad quote, bad revision, save/reload), class TESTED_PORT. Layer 2 (real launcher UI per command) and layer 3 (cross-mode replay per row) are NOT RUN per row. The command gate remains NOT RUN / PARTIAL; nothing here is PASS VERIFIED, and the three-pass verification has not been completed on this build.
- Battle-screen layout was rebuilt for 4:3 and checked by screenshot only.
- Not a release: geographically accurate China overview, complete province art, 300 matched simulations and full combat parity remain open.

QA scripts used are in [`Build40R4/qa_tools_20261102/`](./Build40R4/qa_tools_20261102).
