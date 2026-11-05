# Original ROTK II NES: two-human battle replay

Date: 2026-11-05. Author: runtime subagent for the published battle-gap research plan.
Status: **OBSERVED_RUNTIME / PASS VERIFIED — first-day baseline and Wait-only
day-two food update captured. Historical 190-soldier starting total remains
unreproduced.**

This is the original English NES ROM, not the Windows port. ROM SHA-256:
`11e312691b516e5e29dade3c8ce5434df3561cc69e1aecad9c948acdfddc517a`.
Native Mesen launcher SHA-256:
`f02685dafe559addf8dd8d769621a89777137d0c4592436aed65694a7e740676`.
See `RUNTIME_NOTES.md` and each process receipt for the original core, portable
settings, exact command, environment, stdout/stderr, exit and timing.

## Setup established by captured UI and controller inputs

The cold boot selects scenario 189, two human players, Player 1 Yuan Shu and
Player 2 Liu Biao, and difficulty 2. Player 1 uses NES input port 0; Player 2
uses port 1. A deliberate port-0 attempt at the Player-2 selector had no effect
(case 004), while the port-1 attempt produced Liu Biao's named confirmation
(case 005). Cases 005/006 used a retained local Lua override. Later probes use
the shared harness's explicit `input_port` setting and clear the other port.

Yuan Shu orders Army → War from Province 19 into Province 20. The attacker is
Yuan Shu, Yuan Yin and Ji Ling, with displayed soldiers 100+10+10=120, gold 0,
rice 300. The confirmation is captured at
`experiments/twohuman_019_supplies/f001100.png`. Yu She is selected as the
replacement governor of Province 19 when Yuan Shu personally leads the attack.

Liu Biao declines reinforcements. The defender selector omits the mandatory
commander Liu Biao; the selected rows are Kuai Liang (1), Han Song (3) and
Huang Zhu (4), leaving Kuai Yue (2) unselected. After confirmation, the native
defender field list contains exactly Liu Biao, Kuai Liang, Han Song and
Huang Zhu. The actual count is 4, displayed soldiers 130.

The units are placed through ordinary direction/A inputs. Both humans decline
the separate personal-combat prompts. The first ordinary command menu is
Liu Biao's. Its preserved native checkpoint is
`experiments/twohuman_032_day1_defender_baseline/state_000800.b64`.
No game-memory intervention is used in this chain.

## First-day native census

Primary census:
`experiments/twohuman_032_day1_defender_baseline/census.json`, decoded from the
full 32768-byte physical SaveRAM dump at frame750. It uses physical bank 1
(`$2000–$3FFF`), independently of the currently selected CPU bank.

| Side/list | Officer pointer | Name | Raw battle troops | Position byte | Cell index |
|---|---|---|---:|---|---:|
| Defender field | `$67E7` | Liu Biao | 10000 | `$85` | 73 |
| Defender field | `$67FC` | Kuai Liang | 1000 | `$86` | 86 |
| Defender field | `$6826` | Han Song | 1000 | `$87` | 99 |
| Defender field | `$683B` | Huang Zhu | 1000 | `$88` | 112 |
| Defender resident | `$6811` | Kuai Yue | 1000 | `$FF` | — |
| Defender resident | `$6850` | Cai Mao | 1000 | `$FF` | — |
| Defender resident | `$6865` | Song Zhong | 1000 | `$FF` | — |
| Defender resident | `$687A` | Deng Yi | 1000 | `$FF` | — |
| Defender resident | `$688F` | Wen Pin | 1000 | `$FF` | — |
| Defender resident | `$68A4` | Liu Pan | 1000 | `$FF` | — |
| Defender resident | `$68B9` | Lu Gong | 1000 | `$FF` | — |
| Attacker field | `$672A` | Yuan Shu | 10000 | `$C1` | 25 |
| Attacker field | `$673F` | Yuan Yin | 1000 | `$C0` | 12 |
| Attacker field | `$6754` | Ji Ling | 1000 | `$C2` | 38 |

The field lists and resident list are disjoint and contain only valid 21-byte
officer pointers. Names are read from the original ROM's actual name-pointer
table using each officer's name ID, not its record slot. The position byte packs
the x-coordinate in its high nibble and y-coordinate in its low nibble; the
13-column cell index is `y*13+x`. `$FF` identifies these nonfield residents.

Defender field total is 13000 raw; resident total is 7000 raw; combined total is
20000 raw. Attacker total is 12000 raw. The first-day B summary independently
shows defender 4 generals / 130 soldiers / rice 450 and attacker 3 generals / 120 soldiers / 
rice 300 (`experiments/twohuman_034_day1_summary/f000300.png`). The displayed
defender soldier total excludes the 70 resident soldiers.

| Native CPU-view address in physical bank 1 | First-day value |
|---|---:|
| Defender rice dword `$7C1D` | 45000 |
| Attacker rice dword `$7C21` | 30000 |
| Weather `$7C25` | 2 |
| Wind `$7C26` | 1 |
| Day `$7D1C` | 1 |
| Older claimed day address `$7D25` | 3 |

The terrain, occupancy and region candidate arrays are preserved from native
addresses `$7D58`, `$7DF4` and `$7E90`, each 156 bytes. The occupied defender
cells 73/86/99/112 match the older report. This run's attacker cells 12/25/38
differ from the older 11/24/38; no coordinate was changed in memory to match it.

## Difference from the older 190-soldier food case

The older matrix reports 130 deployed plus 60 resident soldiers. This controlled
two-human replay actually has 130 deployed plus 70 resident. That older total is
**not reproduced**, and the extra officer is not deleted or altered.

The same 11-officer Province 20 list, including Lu Gong, already exists in
`twohuman_010_begin_campaign/memory_000800_save0000.bin`, during the initial AI
rotation, and again in `twohuman_013_ack_message/memory_000650_save0000.bin`,
before the invasion order. The campaign troopwords total 200 at those points;
after battle preparation they total 20000. This establishes that the extra
officer predates the invasion/defender-selection steps. It does not establish
why the older run had 190 soldiers or whether earlier AI actions differ.

For this actual roster, a daily cost based on all defenders predicts
`floor((13000+7000)/30)=666` raw rice, versus 433 for deployed defenders alone.
The attacker prediction is `floor(12000/30)=400`. Both predictions were then
observed in the Wait-only day advance below.

## Verified day-one to day-two food update

The four defenders and three attackers each select Wait through the ordinary
controller menus. No unit moves, fights, flees or loses troops. The three
attacker steps are cases 042/043/044. Case 044 restores the unchanged Ji Ling
checkpoint, presses down five times then A, and records the resulting daily
update. Native exit is 0; execution elapsed 10.600918650001404 seconds.

| Field | Day 1, case 044 frame 10 | Day 2, case 044 frame 1100 | Change |
|---|---:|---:|---:|
| Defender rice dword `$7C1D` | 45000 | 44334 | −666 |
| Attacker rice dword `$7C21` | 30000 | 29600 | −400 |
| Day `$7D1C` | 1 | 2 | +1 |
| Older claimed day `$7D25` | 3 | 3 | 0 |
| Weather `$7C25` | 2 | 3 | +1 |
| Wind `$7C26` | 1 | 1 | 0 |

Defender field 13000, resident 7000 and attacker 12000 troopwords, all officer
memberships and all unit positions are unchanged. All four case 044 snapshots
record native BANK offset 8192, memory type 50 (`nesSaveRam`), mapper bank1 and
PRG mode 3. The CPU `$6000–$7FFF` dump agrees with full SaveRAM physical
`$2000–$3FFF` in each paired snapshot.

Read-only memory callbacks capture attacker-rice writes at local frame 317,
defender-rice writes at 318, and day 1→2 at 329. The rice writes identify native
CPU PC 61965, bytecode pointer 47090 and converted PRG offset 120818. These raw
decimal values are retained in the full stdout, without requiring any inferred
symbol name. The day writer has native PC 59332 and bytecode pointer 33428.

The final B-summary image independently displays date 1/2, defender 130 soldiers / 
rice 443 and attacker 120 soldiers / rice 296, with the rain icon and upward wind
arrow: `experiments/twohuman_045_day2_summary/f000300.png`.

This excludes a deployed-defenders-only food charge in this case: that would
be 433, while the observed decrement is 666. Combined with the separately
inspected resident-list food routine, it supports charging the resident troops.
It does not recreate the older report's exact 633 decrement or establish why
that older starting list had one fewer 10-soldier officer.

The pre-action attacker fixture remains available for independent experiments:
`twohuman_041_wait_huangzhu_attacker_baseline/state_000500.b64` (Yuan Shu), and
`twohuman_042_wait_yuanshu/state_000350.b64` (Yuan Yin after Yuan Shu Wait).
The day-two defender fixture is
`twohuman_044_wait_jiling_day2/state_001100.b64`. All are under `experiments/`.

## Preserved unsuccessful and diagnostic attempts

Case 015 issued destination input during a transition; case 016 established that
the destination prompt still awaited input. Case 017 completed it after the
screen was ready. Case 026's A-only placements stayed on the occupied Liu Biao
cell; case 027 used down/A and captured the actual placements.

Case 031 reached the first menu but hit its 55-second host timeout before the
final checkpoint. Case 032 replayed from the unchanged source and completed.
Case 037 hit the native 90-second limit with exit 255 after showing Huang Zhu's
menu; case 038 hit its 75-second host limit after a frame 350 snapshot. Their
missing final checkpoints are not described as successfully saved.

Case 033's first Wait worked; an unnecessary later A opened the next officer's
Move submenu. Case 035 selected Normal without entering a movement direction.
Case 036 cancelled with B. Its defender names, troopwords, position bytes and
raw mobility bytes equal the case 033 values, and Kuai Liang's main menu is
visually confirmed. Subsequent Wait probes omit the extra A.

Every generated schedule, Lua script, process command, raw log, screenshot,
native checkpoint, RAM dump and receipt remains under its original experiment
directory. Execution success is distinct from successful UI selection.
The later authorized storage cleanup only replaces identical completed-run
launcher/core copies with content-verified hardlinks; it preserves their
paths, bytes, modes and owners. Its audit is
`evidence/runtime_dedup_001/actions.jsonl`. Fresh launches still get independent
copies, and no active process or Genghis Khan campaign is attached or modified.

## Scope

This work establishes original-emulator observations and reproducible private
fixtures. It does not verify the Windows implementation, physical controllers,
historical FCS extraction offsets, or exact replay of the older 190-soldier
starting state. The current MSS extraction and conditional Nestopia 8+1-byte
header explanation are separately documented in `RUNTIME_NOTES.md`.
