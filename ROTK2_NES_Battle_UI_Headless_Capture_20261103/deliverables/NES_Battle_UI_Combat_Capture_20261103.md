# NES ROM II battle UI - combat capture addendum (2026-11-03)

Addendum to claude/NES_Battle_UI_Headless_Capture_20261103.md.

## Update 2026-11-03 (Dong Zhuo vs Yuan Shu, Province 19, combat captured)
Method as above (savestate branching, cursor-detecting controller). Dong Zhuo attacked Yuan Shu in Jing Zhou #19 with a cursor-driven march (Move down each day, Jia Xue Wait). Evidence class OBSERVED_RUNTIME, stock ROM, cloud container only.
1. Attack submenu again confirmed: 1.Simult. 2.Normal 3.Fire 4.Charge, "Choose which?", then "Which direction?". Only Normal was exercised.
2. A white arrow on the map shows the chosen direction. Attacking an empty tile gave a "Missed!" box over the direction prompt.
3. Attacking an adjacent enemy (Yuan Yin, 50 soldiers, below Dong Zhuo's 98): a matchup box "Dong Zhuo 98 / Yuan Yin 50", then the map count of the defender changed 50 -> 41, then an empty message box.
4. Enemy turn: box "Jia Xue was captured by the enemy". Battle panel "Jing Zhou #19 1/7" then "1/8" (Defense Yuan Shu 8 generals 200 soldiers vs Attack Dong Zhuo 2 generals 103 soldiers; rice 404/278; gold 263/0), with a down arrow. Later 7 generals / 198 soldiers vs 1 general / 98.
5. A "1.Normal / 2.Move enemy" box appeared once when the cursor was not visible; treated as the Move submenu, not confirmed which item opened it. The earlier Tactics reading ("Re-inforce / Switch sides") is unchanged.
Still NOT captured: Simult./Fire/Charge outcomes, win/defeat result screens, captured-officer and defeated-ruler menus, ruler death, white tile marker meaning. Not yet compared with Build40R4: rows C-01..C-06 in the Drive matrix v2 have NES side only.
Frames: container /mnt/user-data/outputs/nes_dz_battle (dz_combat_seq_*, dz_attack_down_seq_*, _sheet_attack_adjacent_enemy.png, _sheet_attack_sequence.png).
