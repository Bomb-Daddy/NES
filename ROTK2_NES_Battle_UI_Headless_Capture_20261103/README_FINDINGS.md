# NES ROM II battle UI - headless capture log (2026-11-03)

Evidence class: OBSERVED_RUNTIME. Stock NES ROM (SHA-256 11e312691b516e5e29dade3c8ce5434df3561cc69e1aecad9c948acdfddc517a), Mesen 2.1.1 Linux testrunner under Xvfb in the cloud container. Not the Mesen for AI bridge (the bridge is not loaded in the desktop app). Nothing was run on the user's PC. Scenario 1, Yuan Shu, invading Province 20 (Liu Biao) from Province 19, 100 soldiers vs 190. Frames: /mnt/user-data/outputs/nes_battle_ui (this session's container).

## Captured flows
1. Pre-battle: duel offer, "Position where?" placement (A accepted only on the marked hexes along the top-right edge).
2. Battle command menu (bottom-left blue-bordered window): 1 Move, 2 Attack, 3 Flee, 4 View, 5 Tactics, 6 Wait, with the ruler name (Yuan Shu) and country below.
3. Move: submenu 1 Normal / 2 Move enemy ("Choose which?"). Normal opens "Which direction?" with "Mobility: N". D-pad picks the direction, A executes one hex step; the map camera follows. A box showing the remaining mobility appears after a step. "Can't move there" shows for blocked terrain. Move enemy shows "Can't move enemy" in these runs. Mobility values 0-5 seen; it resets each turn.
4. Attack: submenu 1 Simult., 2 Normal, 3 Fire, 4 Charge ("Choose which?"), then "Which direction?". An attack at an empty direction shows "Missed!".
5. View: auto-opens at the start of each turn as a panel headed "Jing #20 d/N" (d is the battle day; N rises 2, 3 ... 30) with a wind arrow, army names, generals, soldiers, rice and gold. A "Wait" tag and a "Mobility now N" box follow.
6. Wait and enemy turn: one "<General>'s strategy" box per enemy general (Kuai Liang, Kuai Yue, Han Song, Huang Zhu, Deng Yi, Wen Pin, Cai Mao, Liu Pan, Song Zhong), with a white highlight on the acting hex; some boxes show "Wait". Then the player's menu returns.
7. Flee: "Withdraw whole army (Y/N)?" then "Withdraw where?" listing "29 Open territory", "31 Open territory", "19 Last province" with an arrow cursor, then "Yuan Shu was crushed", then "succumbed to a greater power", then the China map, then the province command menu ("Master Yuan Shu, your orders for Province 29?").
8. Day-30 end: after day 30 the battle closes with "To be continued next month...", the China map with other rulers' turns ("No.11 Cao Cao now plotting strategy", "No.13 Liu Yan", "No.15 Dong Zhuo"), then the province menu for the next month and "This month's orders complete (Y/N)?" at the end.
9. Enemy AI stayed passive for 30 days; rice fell from 443 to about 290.

## NOT captured
- Combat exchange screens (hit, damage, kill messages) and the Attack target step against an adjacent enemy: my unit was blocked by the river and never reached the enemy.
- Battle win and defeat result screens, captured-officer menus, defeated-ruler menu, ruler-death screen.
- Tactics sub-flow beyond the first screen; Simult./Fire/Charge outcomes.
- Why Flee ended in "crushed / succumbed" (ruler gone) rather than a plain withdrawal: not determined; the cursor choice in the destination list was not recorded.
- RAM attempt to find unit coordinates (diff before/after a Move) failed: the second sample landed mid-screen-change.

## Update 2026-11-03 (later runs)
- Four more scripted marches toward the enemy (right/down step patterns, then Attack from turn 6) did not reach combat: the blind input script fell out of step with the menus after turn 1 (the battle day stayed at 1/2 and a "View (Y/N)?" prompt repeated). No combat, result or capture screens came out of them.
- A RAM diff to locate unit coordinates, taken at stable menu frames, was contaminated by the enemy turn (day counter moved 1 to 2). Candidate bytes seen changing with unit activity: $7D1A/$7D1B, $7D2C-$7D2F, $7D11/$7D12 (unconfirmed, not decoded).
- Next method needed: a closed-loop script that reads the screen or RAM state each step, instead of fixed-time button presses.

## Update 2026-11-03 (adaptive-wait run)
- Rebuilt the capture script to wait for each menu to settle before the next button (watching the menu box area). It stays in step for 138 steps.
- New screens: Tactics submenu "1.Re-inforce 2.Switch sides, Choose which?"; "Start where?" prompt (seen after a Move-enemy style entry); mobility counts of 2 and 3 per turn.
- Still NOT captured: any combat exchange, win/defeat result, captured-officer or ruler-death screens. In the six-turn march the unit never reached the enemy (river between). Pressing right from the start does nothing; down moves the unit.
