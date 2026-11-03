# Fork Four — Officer relationships, loyalty, sworn brothers and rewards

**Isolated experimental fork; NOT ROM-parity-certified; production/older 300-arm experiment unchanged.**

This is an additive, executable feature branch on `Fork_Four_Enhanced_Advisor_AI_Batch_2026-10-31.zip`. It introduces `third_branch/officer_relations.py` and wires six personnel orders into the campaign bus, preview/commit, GUI order list, context help, save/load, validation, and 400-test suite.

| UI command | Engine command | Who may execute | Supported behavior |
|---|---|---|---|
| Reward Officer | `reward_officer` | Ruler or local governor | Local officer receives gold, horse or writings. Gold/horses raise loyalty, writings raise INT by one with appointed advisor ≥2 INT above target. |
| Buy Horse | `buy_horse` | Ruler or local governor | 100 gold per horse, real treasury debit; item inventory persisted; temporary specialty market adapter. |
| Appoint Advisor | `appoint_advisor` | Ruler | Locally present officer with INT >80; persisted post, required for writings. |
| Swear Brotherhood | `swear_brotherhood` | Ruler | Co-located target with loyalty ≥95, up to 9 sworn (5 if explicitly configured hardest); locks loyalty to 100 while bound. Oath removed on capture/change of faction. |
| Feast | `feast` | Ruler or local governor | 10 gold per present officer; restores up to 5 BP and increases eligible subordinate loyalty by 2. |
| Rest | `rest` | Ruler or local governor | Restores up to 10 BP per local officer, no gold cost. |
| Appoint Governor — Personnel focus | `appoint_governor` | Existing permission model | Monthly automatic governor use of 3 BP and 10 gold to raise lowest-loyalty local officer by 3 if below 85, with an exact audit entry. |

**Exploit guard:** Feast and Rest share a once-per-province-per-month gate, preventing unlimited BP recovery. Gold and items are never created by rewards. All new values other than the source-stated 100-gold horse purchase price are provisional balance parameters.

**Important incompleteness:** ROTK II loyalty/recruitment/defection probability calculations, Bandit Kings compatibility/IMC/personality-based differences, annual loyalty decay, service-time/blood-family anti-betrayal, a brother's *exclusive* direct-control of another prefecture, the delegation authority distinction, enemy officer recruiting, appointed advisors' counsel logic, and exact loyalty effects are **not fully implemented**. These are requirements for follow-up ROM/Mesen oracle work and full feature parity. Existing Fork Four UI already permits direct player control over all owned provinces; the game must introduce governor appointment remains direct; do not add brother-only province permissions before the Bandit Kings brother privilege has its historical consequence. The multi-station army capture bug remains separately unresolved.

**ROM provenance:** The NES ROTK II manual describes Person → Reward Gold/Horse/Writings, high-charm ruler/governor effectiveness, advisor 2+ INT gap for writings, and 100 gold per horse. The NES Bandit Kings guide describes Heroes → Brother, Give → Hero, Feast, Rest, loyalty threshold 95, permanent brother loyalty 100 until captured, and direct control of a sworn brother's prefecture. These mechanical *classes* are grounded; this fork's **formulas and policy values are not**.

## Installation

Best: use the full package ZIP `Fork4_Officer_Relationships_Experimental_v1_2026-10-31.zip`. Unzip to a new folder and do **not** overwrite the production Enhanced game. Alternatively apply `Fork4_Officer_Relations_Source_Overlay_v1_2026-10-31.zip` over a verified extract of `Fork_Four_Enhanced_Advisor_AI_Batch_2026-10-31.zip`.

To test: from the `Fork_Four_Enhanced_Game` directory run `python -m pytest -q tests`. At 2026-10-31: `400 passed, 2 xfailed, 3 subtests passed` (Linux Python 3.13). To launch: use the existing `PLAY_WINDOWS.bat` or `PLAY_LINUX.sh`, subject to their normal dependency requirements. The commands appear in `All Orders` under their names above. Saved games record the optional new fields only once the commands are used.

Do **not** combine advisor-weight experiment histories across pre- and post-feature revisions without verification that the relevant trajectories remain parity-identical. The new commands are player-facing; existing faction AI does not yet autonomously consider most of them. Only the appointed Personnel governor policy is automatically applied monthly.


Director correction (2026-10-31): **Do not implement brother-only province-control privileges.** Governors can be directly appointed. Sworn brotherhood affects officer relationships/loyalty, not the right to command provinces. The separate Reward Governor ruler-only order shows a roster of governor, province and loyalty and rewards from the ruler's treasury.
