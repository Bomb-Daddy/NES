# Original-KOEI Inspired Enhanced Run046 — Q04 real GUI player deployment activation

Scope: **separate Original-KOEI Enhanced** source. Immutable direct parent Run045 SHA256 `e6a171c5a7b1a1e042d18f3998d7e7e2fb8e9910183ff88ac9896684c6f9d4dd` (569 entries). No Classic NES or DOS sources.

## Observable correction

English NES Q04 manual officer placement had already been implemented in `third_branch/q04_manual_deployment.py`, including source-derived 41-province regions at ROM `$28934` (verified bundle SHA256 `9bc5a0c945920e0d6fa50c4fcca3cd4c75ff2b986cd147c912f59aba6edb5aa1`) and hsovl6 original placement validation research `$D0CF`/`$CE5C`. Yet **actual playable GUI campaigns did not enable** that placement path: the core campaign's flag defaulted absent/false. The player saw automatic prepositioned officers instead of the interactive Q04 phase.

This patch enables the already-implemented manual deployment phase at real `StrategicWindow` GUI startup, on an idle saved-game adoption, and upon choosing a ruler. It only applies to 41-province GUI campaigns with no active battle. Explicit core test fixtures, old seven-province games, in-progress battles and the deterministic battlefield region algorithm remain untouched. It changes no combat RNG, troop amounts, economic/marketplace/Body Points logic, save format or map graphics. Upon attack a real human faction now receives the Q04 controller D-pad/A positioning phase before combat.

This is a **source-backed port activation correction**, not a freshly replayed original NES 30-event golden or exact cursor/text/pixel proof. Current native Windows/physical Xbox untested. This does not introduce a second-battle scheduler.

Files: production `third_branch/strategic_game.py`, new `tests/test_run046_q04_gui_activation.py`, and this README. Preserve exact Run045 parent and all previous source.
