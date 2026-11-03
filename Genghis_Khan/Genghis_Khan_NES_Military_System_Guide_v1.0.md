# Genghis Khan (NES) — Military System Guide — v1.0 (full consolidation; open items in section 11)

Date: 2026-11-03. Scope: original NES *Genghis Khan* (USA), 1-to-1 Windows reproduction. Military counterpart to the economic guide.

**Status (v1.0).** This file consolidates every finding from v0.1-v0.23. Each rule carries its evidence tag. **[V]** rules matched the emulator in randomized trials (counts in section 12): RNG, stat gating, Train (Troops, People, Self, Prince), Draft, Hire, weapon buying, seasonal Skill/Arms decay, Spring food upkeep and population growth, war-launch costs, hex movement and terrain costs, Normal attack, Arrows, Duel transfer and refusal, the battle day loop, daily food, battle end conditions, Raid, Hunt, Pass, AI attack size (1 run), AI target choice and AI war decision. **[S]**-only rules (read from ROM bytecode, not yet run) are marked "not yet run" where they appear. Section 11 lists everything that is still open. Nothing here comes from FAQs; every number was measured in a headless emulator core running the ROM, or read from ROM bytes.

Tags: **[M]** measured by running the ROM. **[S]** read from ROM bytes. **[V]** formula predicted the emulator result in randomized trials (counts given). **[OPEN]** not yet known.

---

## 1. Method and tools

- ROM `Genghis Khan (USA).nes`, 262,160 bytes, SHA-1 `98b39b610933d0d82a6ccbcb962a28ba7dc71a85`, mapper 1 (MMC1/SOROM), 16 PRG banks, CHR-RAM, battery. Vectors RESET `$C000`, NMI `$C0D9`, IRQ/BRK `$C14B`.
- Emulator: own C core (`gkcore.c`) with 6502, MMC1, PPU model, watchpoints, instruction trace, and a **bytecode-VM tracer** that logs every VM instruction (opcode, bytecode address, accumulator, frame pointer, bank). Python wrapper `gk.py`, disassemblers `d6502.py` (6502) and `vmdis.py` (game bytecode), trace harness `cmdtrace.py`, test scripts `traintest.py`, `wartools.py`.
- Cross-check: independent emulator `cynes` gave pixel-identical frames for the intro and scenario select. [M]
- Method for each formula: trace the VM during the command, find the store to the changed field, read the preceding bytecode and its accumulator values, write the formula, then predict the emulator result under randomized inputs.
- Input bits in my core: A=1, B=2, Select=4, Start=8, Up=16, Down=32, Left=64, Right=128.

## 2. Engine: a bytecode VM [S][M]

All game logic (commands, formulas, AI) is bytecode run by a VM; the native 6502 side only provides the VM, I/O and a few services.

- Dispatcher `$EFC9`: opcode fetched from `($06)`, handler address from tables `$F788` (low) / `$F888` (high), `JMP ($0000)`. 163 distinct handlers `$EFE1-$F9A8`. Example captured during Train (v0.1): the store to the skill field ran in handler `$F416` (`JSR $F43D` computes the target address; `STA ($0C),Y` writes the high byte at `$F41B` and the low byte at `$F420`). [M] Bytecode lives in the fixed bank (`$C000+`) and in banks 0, 2 and 13 at `$8000-$BFFF`.
- Registers: `$06/07` bytecode pointer, `$04/05` frame pointer, `$02/03` evaluation stack pointer (pushes go down by 2), `$08/09` accumulator ACC (16-bit), `$0C/0D` register B.
- Opcodes (decoded from handlers and confirmed against traces):
  - `00-0B` ACC=local n; `0C-0F` ACC=argument n; `10-1B` B=local n; `1C-1F` B=argument n; `20-2B` local n=ACC; `2C-2F` argument n=ACC.
  - `30-3F` push local/argument; `40-4F` ACC=n (0-15); `50-5F` B=n; `60-6F` push n; `71-7F` ACC+=n.
  - Immediates (each checked against the ACC value in the next trace step: 209/209, 1/1, 962/962, 1/1, and 951/951, 1508/1508, 388/388, 245/245 for the ones that leave ACC unchanged): `89` ACC=imm8; `8A` ACC=imm16; `8F` ACC+=imm8 (signed); `90` ACC+=imm16; `8B` B=imm8; `8C` B=imm16; `8D` push imm8; `8E` push imm16 (the last four leave ACC unchanged; push versus B is inferred from how the values are used as call arguments and in additions).
  - `A0-AF` + 2-byte address: absolute byte/word load/store (`A4` load word, `A8` store word, `A6`/`AA` other widths).
  - `B0` ACC=word at (ACC); `B1` word at (popped address)=ACC; `D3` ACC=byte at (ACC); `D4` byte at (popped)=ACC; `B3` push ACC; `B4` pop into B. `A4` ACC=word at abs; `A8` word at abs=ACC; `A6` B=word at abs (inferred); `AA` push word at abs (inferred); `8A`/`A0`/`A2` widths other than these are not decoded.
  - `BB` ACC+=B; `BC` ACC-=B; `B5` ACC*=B; `B8` ACC/=B; `B6` signed ACC/=B (v0.9, handler `$F480`), `B8` unsigned ACC/=B, `B9` signed ACC%=B (`$F4B6`), `BA` unsigned ACC%=B (`$F4CE`); `BD` ACC<<=B; `BE` ACC>>=B; `D0` ACC++; `D1` ACC--; `D2` ACC<<=1; `DA` AND; `DB` OR; `DC` XOR.
  - `C0` ==, `C1` !=, `C2-C5` signed <, <=, >, >=, `C6-C9` unsigned <, <=, >, >= (result 0/1 of ACC op B). Corrected in v0.3: v0.2 had the second and third of each group swapped; the order was confirmed from ACC/B values and results in the trace.
  - `D6` jump, `D7` jump if ACC!=0, `D8` jump if ACC==0 (2-byte absolute operand), `D5` range switch (`D5 lo hi n 00`, then a default address and n case addresses), `D9` value switch (`D9 n 00`, then n (value, address) pairs and a default), `AC` call (2-byte), `E9 aa aa nn` call function at aa aa with nn argument bytes, `CF` return, `B7 nn` system call through table `$F9C3` (entries 1-`$2F`); the 32-bit arithmetic ops used by `$DB48`/`$DA67` are `$26` zero-extend ACC, `$14` push ACC32, `$15` pop to B32, `$01` multiply, `$02` signed divide, `$03` add, `$27` truncate to 16 bit (v0.13).
  - Operand lengths used by the disassembler: `80-9F` odd = 1 byte, even = 2 bytes; `A0-AF` = 2; `B7` = 1; `D6/D7/D8/DE/DF/AC` = 2; `E9` = 3. (Confirmed from consecutive-address gaps in a 97,111-step trace.)
- Calling convention: arguments are pushed, callee reads them as `ARG0..` (the last pushed is `ARG0`).
- `E9 88 F9 nn` calls a native trampoline (`$F988`): it copies the pushed bytes to `$4E+` and executes a BRK through the IRQ vector. The BRK handler (`$C14B`) takes the function id from `$50`, and jumps through `$C185 + 3*id`. **Native function 17 (`$C289`) is the random-number generator** (section 3).
- Percent/scale helper `$DA67(a, b, c)` = `a*b/c` with a 32-bit intermediate (implemented with system calls `$26,$14,$15,$01,$02,$27`). Verified numerically: `32*999/1102=29`, `29*100/100=29`. [V]
- Min helper `$DA49(a, b)` = smaller of two unsigned values.

## 3. Random number generator [S][M][V]

- 48-bit state `S` in zero page `$84..$89` (big-endian, `$84` most significant).
- Step: `S |= 1`; `S = (-(S * K)) mod 2^48` with `K = 0xE0900E0187C5` (stored little-endian at `$C2EC`).
- Output (15 bits): `((S>>40) & 0x7F) << 8 | ((S>>32) & 0xFF)`. `rand(n)` = output mod n (`$DB93`, function 17 then `BA`).
- Reset sets `$84=04 $85=D2 $86=04 $87=D2`; every NMI while `$8A == 0` adds 2 to the 16-bit pairs `$84:$85` and `$86:$87`. During play `$8A != 0`, so **the RNG state changes only when the game calls it**. Idle frames and button timing do not matter. Different games differ only because of the state reached on the title/menu screens and the rolls made before.
- Verified: the state after one Train command equalled the predicted next state in 10 of 10 random starting states.

## 4. Command gating by ruler stats [M][V]

Before a command runs, the game checks six minimum ruler stats against a 6-byte row of the table at WRAM `$7E18` (row = command id, stats in the order Leadership, Judgment, Planning, Charm, Body, Battle). If any stat is below its minimum the command has no effect. Rows (copied into WRAM at start; the 144 bytes are present in the ROM file at offset `0x1F1C8` (bank 7), verified byte-for-byte):

| Row | L | J | P | C | Bd | Bt | | Row | L | J | P | C | Bd | Bt |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| 0 | 5 | 0 | 0 | 5 | 0 | 0 | | 12 | 5 | 10 | 5 | 0 | 0 | 0 |
| 1 | 10 | 0 | 0 | 10 | 0 | 0 | | 13 | 5 | 10 | 5 | 0 | 0 | 0 |
| 2 | 5 | 0 | 5 | 0 | 0 | 0 | | 14 | 0 | 10 | 10 | 0 | 0 | 0 |
| 3 | 5 | 0 | 5 | 0 | 0 | 0 | | 15 | 20 | 0 | 0 | 10 | 0 | 10 |
| 4 | 0 | 0 | 5 | 0 | 0 | 0 | | 16 | 0 | 0 | 0 | 5 | 10 | 0 |
| 5 | 5 | 0 | 0 | 0 | 0 | 0 | | 17 | 0 | 0 | 5 | 5 | 10 | 0 |
| 6 | 0 | 0 | 0 | 5 | 0 | 0 | | 18 | 0 | 0 | 0 | 5 | 10 | 5 |
| 7 | 0 | 0 | 0 | 0 | 0 | 0 | | 19 | 0 | 0 | 0 | 5 | 10 | 0 |
| 8 | 0 | 0 | 0 | 5 | 0 | 0 | | 20 | 0 | 5 | 0 | 0 | 0 | 0 |
| 9 | 0 | 5 | 0 | 0 | 0 | 0 | | 21 | 0 | 0 | 0 | 0 | 20 | 0 |
| 10 | 5 | 10 | 5 | 0 | 0 | 0 | | 22 | 10 | 0 | 0 | 10 | 0 | 0 |
| 11 | 5 | 10 | 5 | 0 | 0 | 0 | | 23 | 5 | 5 | 5 | 0 | 0 | 0 |

Row 5 is **Train → Troops** (Leadership ≥ 5): measured, Leadership 0-4 give no gain and 5, 6, 10+ work. Rows tied to commands by the row number the code passes (v1.0): 5 Train → Troops (measured), 6 Train → Prince, 7 Train → Self, 8 Train → People, 9 Hire and Buy, 15 Draft, 24 Policy, 25-27 the three Treaty choices [S]. Other rows are not tied yet.

## 5. Train → Troops [M][V]

Effect: Skill (`$6DE3` for country 1) increases; 1 order used; message "Skill has become N".

- `pop` = Troops + Town + Masons + Artisans + Food workers (country fields `+04,+08,+0C,+24 copy at $6E07,+2A copy at $6E0B`; at game start 61+5+4+6+27 = 103).
- `T = floor((Leadership + Battle + Charm) / 40) + rand(11) + 15`
- `gain = floor(T * 999 / (999 + pop))`
- `gain = floor(gain * (100 - 4 * min(level - 1, 5)) / 100)` where `level` is the game level byte at `$7A88` (1-5).
- `Skill = min(Skill + gain, 999)`.
- Verified: 24 of 24 randomized trials (random RNG state, troop count 0-999+) and level 1-5 checks matched exactly; Skill cap 999 confirmed.
- This resolves the earlier two-game discrepancy: the only difference was the RNG state.
- Judgment, Planning and Body do not enter. Economy, Defense, Arms, Morale, Gold do not enter.

## 6. Train → People (mobilization proportion) [M][V]

"Proportion available for mobilization is N%": the draft rate, stored as a byte at country offset `+$34` (`$6E15` for country 1).

- Fails (no change) if the proportion is already ≥ 50 (`$32`).
- `gain = floor(((Leadership + Battle) / 600 + rand(2) + 2) * (100 - 6 * min(level - 1, 5)) / 100)`
- `proportion = min(proportion + gain, 50)`.
- Gate: stat-table row for People (row id not yet matched, see section 4).
- Verified: 20 of 20 randomized trials matched exactly.
- The proportion caps the Draft: max draft = floor(pool × proportion / 100) (section 8, [V]).

## 7. Train → Self and Train → Prince (v0.15) [S][M][V]

Train menu (bank 2, jump table `$B983`): Troops, Prince (`$A52A`), Self (`$A577`), People (`$A5B0`). Self and Prince share `$A49A(mode, c, person)`: a "Skills" menu of the six stats (Leader, Judge, Plan, Charm, Body, Battle) picks index k (0-5), then
```
gain = floor(base * (100 - 4*min(level - 1, 5)) / 100)      ($A47D, same level factor as Train -> Troops)
stat_k(person) = min(999, stat_k + gain)                     (person stat words start at person + 14)
```
- **Self** (gate row 7): person = the ruler (country word +`$51`); `base = floor((Judgment + Planning + Body) / 40) + rand(11) + 15` of the ruler (`$A263`).
- **Prince** (gate row 6; `$9913` must allow it; a prince is picked from the list): person = that prince; `base = floor((Leadership + Battle + Charm) / 40) + rand(11) + 15` of **the ruler** (`$A28A`, the same base as Train → Troops; when `$DE1A(c)` is true the ruler of the overlord `$DF11(c)` is used).
- Prince 1's stat words are at `$6912-$691D` in the test game (prince records `$6904 + 145*n`).
- **[V]** Self 18/18 exact (all six stats, random stats 20-400, level 1-5, RNG state); Prince 12/12 exact (all six stats, gain from the ruler's L+Bt+C).

## 8. Recruiting: Draft, Hire, buying Arms (v0.13) [S][M][V]

Country record `c` as the strategic code sees it = `$6DCD + 83*country` (same base as section 10f; country 1 = index 0). Fields used: +`$14` Arms, +`$16` Skill, +`$18` Troops, +`$40` Gold, +`$42` Morale, byte +`$48` mobilization % (Train → People raises it, section 6), word +`$49` draft pool. `pop` = Troops + Town + Masons + Artisans + Farmers (`$DECD`). Global word `$7A8A` = the "Rate" shown under Orders left.

**Weighted average `$DB48(X, P, w, N)`** (32-bit arithmetic): `min(999, floor((X*P + w*N) / (P + N)))`; 0 if P = N = 0. New soldiers enter Arms, Skill and Morale with value `w`, weighted by their number against the existing population.

### Draft (Orders → People → Draft, bank 3 `$909D`)
- Gate: stat-table row 15 (`$827B(15)`).
- `max = min(floor(pool * pct / 100), 999 − pop)`; 0 → "Further recruiting is impossible". Prompt "Draft how many soldiers (1-max)?".
- For N drafted (`$905E`): Arms = `$DB48(Arms, pop, 1, N)`, Skill = `$DB48(Skill, pop, 1, N)`, Morale = `$DB48(Morale, pop, 1, N)` (all with pop before the draft); Troops = min(999, Troops + N); pool = pool − N; pct = `max(0, floor((max − N) * 100 / pool_after) − rand(3))`.
- **[V]** 20/20 exact (random pct 5-50, pool 20-400, Arms/Skill/Morale/Troops, RNG state).

### Hire (Orders → Trade → merchant → Hire, bank 3 `$8D2E`)
- Gate: row 9. The merchant must be available (word `$7ACE + 34*m + 2*i`; "Merchant of … is unavailable"); in the test game only the Uighur merchant was.
- Price per soldier in tenths of gold: `p = floor(60 * (Rate + 100) / 100)`, shown as "The going rate for troops is p/10.p%10".
- `max = min(999 − pop, floor(Gold*10 / p))`; 0 → "No more soldiers can be recruited". Prompt "Hire how many".
- For N hired (`$8683`): Arms = `$DB48(Arms, pop, 100, N)`, Skill = `$DB48(Skill, pop, 100, N)`, Morale = `$DB48(Morale, pop, 50 + rand(11), N)`; Troops = min(999, Troops + N); Gold = `floor((Gold*10 − p*N) / 10)`.
- **[V]** 20/20 exact (random Gold, Arms, Skill, Morale, Troops, Rate 0-200, RNG state).

### Buying weapons (Trade → merchant → Buy, bank 3 `$8C34`)
- Gate: row 9. The 18-cell goods grid (3 columns × 6 rows); the six weapon icons in the right column are goods i = 11-16. Availability word as for Hire; unit price in tenths `p = floor(B[$7DE5 + 17*m + i] * (Rate + 100) / 100)` (shown as p/10.p%10; the same formula prices all goods).
- Weapon strength `K_i = B[$7DE5 + i]` (the first row of the price table): 20, 80, 100, 40, 60, 150 for i = 11-16.
- `max = min(999, floor(pop * (999 − Arms) / K_i), floor(Gold*10 / p))`; 0 if Arms = 999 ("You can't buy any more").
- For N bought: Arms = min(999, Arms + floor(9 * K_i * (N + 1) / pop)); Gold = `floor((Gold*10 − p*N) / 10)`.
- **[V]** 18/18 exact (all six weapons, random Arms 0-900, Gold, Troops).

## 8b. Seasonal changes to military fields (v0.15, bank 1) [S][M][V in part]

Season word `$65BD` (0 Spring, 1 Summer, 2 Autumn, 3 Winter) is advanced at `$8EAF`; the switch at `$8F18` then runs one handler for the new season, followed by `$82B1` every season. All countries (`$6DCD + 83*c`, `$65B9` countries) are processed.
- **Summer (`$8E3E` → `$83EA`):** Skill = floor(Skill × 93 / 100). **[V]** 4/4.
- **Winter (`$8EA0` → `$83C5`):** Arms = floor(Arms × 97 / 100). **[V]** 4/4.
- **Spring (`$8CD4`), per country:** food upkeep `$8099`: while pop > 2 × Food, every population group (Troops, Town, Masons, Artisans, Farmers) is scaled to `floor(2*Food*100/pop)` percent; then Food −= min(Food, floor(pop/2)); if any scaling happened Food = 0. **[V]** 3/3 exact without starvation; 3/3 with starvation give the scaled Troops + 1 (the +1 is Spring population growth, below). Then gold income `$8106` (Economy × tax, morale factor; economic side, not reproduced here) and further country updates; the mobilization % is recomputed (`$8004`): if pool < 20 → 20 + rand(11), else `min(50, floor((floor(pool*(pct+3)/100) + 1) * 100 / (floor(pool*105/100) + 1)))`.
- **Spring gold and soldier pay (`$8106`).** Income = `floor(floor(Economy*tax/100) * (min(13, floor(Morale/9)) + 12) / 25)` is added to Gold; then if Troops > Gold, Troops = Gold (unpaid soldiers leave); then Gold = min(999, Gold − Troops). [S][M] Measured: in 9 of 10 trials the result fits once about 10 extra gold is credited before the pay step (the Economy used for income changes earlier in the same Spring update; that economic part belongs to the economy guide and is not modelled here); the 10th trial was the invasion case below.
- **Spring population growth (`$8A23`, after upkeep and pay).** `P = min(999, pop)`. If P < 20: Troops + 1. Otherwise `L = min(999, floor(P*(103 + g)/100)) − P`, where g = 0 for the player's countries and `min(3, level)` for countries that pass `$DDFE` or `$89C1` (AI countries grow faster at higher levels); each of Troops, Town, Masons, Artisans (+`$3A`) and Farmers (+`$3E`) gets floor(L/5); the remainder r = L mod 5 adds +1 to: r=1 Troops; r=2 Masons, Troops; r=3 Town, Masons, Troops; r=4 Artisans, Town, Masons, Troops. Draft pool = min(999, pool + floor(pool*5/100) + 1). **[V]** 11/12 exact for all five groups and the pool (player country). The 12th trial (Troops 3) and one pay trial (Troops 13) are **explained (v0.17)**: with so few Troops the country became the weakest neighbour, and during that season pass the Tatars (ruler Tog Temur) invaded it (screen: offense 270 = 27 soldiers × 10, "30 days left", the player asked to place units). The test script kept pressing A through the battle, the defence was wiped out (Troops 0), and the game never reached the Spring update. This matches the AI target rule (weakest defence, 9c) and the 30-or-60-day campaign length (9b).
- **Autumn (`$8E50`):** Food = min(999, Food + word +`$3C`); the goods slot `+$22 + 2*$DEF3(c)` += word +`$38`; then words +`$38`, +`$3C` and the raid word +`$46` (section 10f) are cleared. [S] Because +`$3C` is cleared here, it is a food-production word that equals the Farmers number only at game start (v0.1 saw both +`$3C` and +`$3E` = 27); the Raid in 10f takes from this word, so "Farmers" in 10f means this production word.
- **Rate `$7A8A`** (hire and goods prices): at the start of each year (Spring) Rate = 50 + rand(94 + 7×level) and year `$65BB` + 1; in every other season Rate = max(50, Rate − floor(Rate × (1 + rand(9)) / 100)) (a `rand(1)` test that would pick an increase always gives 0, so the rate only falls during the year). Observed 118 → 108 Spring→Summer. [S][M]


### Assign (Orders → Assign) and army composition (v0.21, bank 2) [S] (not yet run)
- **Assign (`$9EF3`):** pick one of town builders (Town +`$1C`), masons (+`$20`), food producers (+`$3E`), specialized laborers (+`$3A`). `pool = min(999, group + Troops)`; "How many will you make (0-pool)" → the group becomes N and **Troops = pool − N**, so Assign moves people between the army and a work group. On confirming the whole distribution, if Troops is now more than 65 % of the population and Troops grew, Morale = `floor(Morale * (100 − 2*(pct − 65)) / 100)` where pct = floor(Troops*100/pop).
- **Army composition (`$9FF3`, used by War at `$A3C1` and `$A74F`):** a list of up to 10 unit entries of (unit type, percent of the soldiers); "Which unit's strength do you want to change" / "Make what percentage" assigns percentages out of a remaining 100 %, "Unit type" picks cavalry, infantry or archery. These are the units placed in the battle (section 10).

### Treaty and Policy (player orders, v0.23, bank 2) [S] (not yet run)
Bank 2 `$A2F5` dispatches part of the order menu through a value switch on word `$7B76`: 2 → `$A3CE`, 4 → Train (`$A62C`), 10 → `$AA96`, 11 → `$ABDC`, 13 → `$AC36`, 20 → the AI/delegated country turn (`$A34B`).
- **Treaty (`$ABDC`)**: three choices, each with its own stat gate (`$989C` rows 25, 26, 27): (1) demand vassalage → the same `$915E` as the AI (an AI target answers by the rules in 9c; a price may be asked); (2) demand tribute → the same `$9315` (an AI target accepts when Troops ratio > 140 + rand(31) and rand(100) < 30; it then pays 10 % of Gold, Food and goods); (3) alliance (`$938C`; "That isn't our enemy's base" if the target fails `$DDFE`; "Should we extend our period of alliance" when already allied). So the player's diplomacy and the AI's use identical acceptance formulas.
- **Policy (`$AA96`)**, gate row 24: hand a country to a ruler's control ("Do you want %s to set policy for country %d", Yes/No); Yes with existing orders in the country's plan record clears them; otherwise `$A9C4` sets up the delegated plan. Delegated countries are then run by the plan record (`$80B7`, section 9b).

## 9. War: launching an attack [M]

Order of prompts when choosing **War**:

1. Map with the 14 flags: "Attack which country?" (cursor selects the target).
2. "Soldiers (1-N)?" with N = all current troops. Digits are entered with Up (+1 to the last digit) and Right (adds a new last digit).
3. Message "N gold paid as war expenditure": **gold cost = 1 per soldier** (431 → 411 for 20 soldiers).
4. "How much food should we take (1-3N)?": maximum is **3 food per soldier** (10 soldiers with 31 food was refused; 10/30, 7/21, 3/9 and 20/60 were accepted).
5. Message "These supplies will last D days": D = 90 for 60 food and 20 soldiers; this matches the battle's days-of-food formula `min(90, floor(food*30/soldiers))` (10f, `$80FD`), but the launch routine itself was not read. The battle header showed "90 days left" for 30, 60 and 100 food alike, so the battle clock starts at 90 whatever the food.
6. "Who is going to command the expeditionary army?" Leader menu: Self or Prince.
7. "Is everything all right?" Yes/No.

On Yes the game deducts **gold −N, food −F, troops −N** for N soldiers and F food (measured: 20/60 gave gold 431→411, food 208→148, troops 61→41; 7/21 gave −7, −21, −7), shows the departing soldier ("We will be awaiting your return") and then the battle screens.

## 9b. Strategic AI: launching an attack (v0.13, bank 2 `$85BC`) [S][M] (one run captured, v0.18)

Called from the AI country turn `$80B7` (per-country AI plan record `$DCB7(c) = $77D2 + 19*index`; byte +13 = target country, 255 = none) and from `$847C`. Arguments: attacker record a, defender record d, a ruler/leader value, and a flag F (1 from `$80B7`).
- Preconditions: `$DE28(a, d)` and `$98DD(32)` must both be true (meanings not read).
- Defender strength `D = $8562(d)`: 0 if d's Troops or Food is 0, or Food < floor(Troops*10/100); otherwise Troops.
- Attacker capacity `A = $852E(a) = floor(min(Troops, Food, Gold) * k / 100)` with k = 70 when word `$7B72` > 1, otherwise 90.
- Required margin `m` (percent of D): if `$7B70` = 0, `$DE0C(a)` is true, the defender is the player's country (`$DC98(d)`) and `rand(2)` = 0: `m = 50 + rand(71)`; otherwise `m = (F ? 70 : 125) + rand(101)`.
- Campaign length: `L = 1 + rand(2)`; days remaining for the battle (`$6043`) = 30*L (30 or 60 days).
- The attack goes ahead only if `A > floor(D*m/100)` and `A > 20`. Soldiers sent = A; food taken = `min(L*A, floor(Food*70/100))`; both must be nonzero.
- Stance code for the battle: 100 if `$DDFE(a)` is true, else 101 (101 = AI-played side, section 10f).
- If the player is involved or watching (`$7B70`, `$DC98(d)`, `$7BBA`), the message "%s of country %d attacked country %d" is shown; then `$9DE3(a, d, leader, soldiers, food, stance)` runs the battle.

**Captured run (v0.18):** frame-by-frame trace of the Winter AI turn with the player at 3 Troops. Attacker country index 1 had Troops 43, Food 90, Gold 39 when `$85BC` was entered; `$7B72` = 4, so k = 70: A = floor(39*70/100) = **27** soldiers (battle size 270). After the launch: attacker Troops 43 → 16 (−27) and Gold 39 → 12 (−27, 1 gold per soldier as for the player, section 9); Food stayed 90 (the AI's food is not deducted at launch); battle food 270 = 27 × 10, i.e. food taken = min(L*A, …) = 27 with L = 1; battle clock `$6043` = 30. Target = the player (Troops 3, the weakest neighbour). All values match the rule above (`aiattackverify.py`). (v1.0: target choice and the war decision are decoded and [V] in 9c; the routines still not read are listed in section 11, item 2. After those fixes the whole pass was run again as an address check (every `$xxxx` in v0.1-v0.23 searched in v1.0) and a stale-phrase scan. It found and fixed: the menu cursor `$05A6` and record 15 `$726B` (restored in Appendix A), the Train store-handler example `$F416` (restored in section 1), and a stale line in section 10 saying combat was not decoded. The remaining unmatched addresses are v0.1 offset-view field addresses (covered by Appendix A's offset table), 6502 listing addresses, addresses earlier versions listed together with a routine v1.0 documents (`$9998` with `$99AE`, `$A306` with `$A308`), the v0.2 approximate grid address `$6160` (exact: `$615C`) and general memory-map bounds.)

## 9c. Strategic AI: country turn, war decision, target choice, recruiting (v0.14, bank 2) [S]; war decision and target choice [V] (v0.19)

**AI country turn `$8481(c)`**, in order: if byte c+1 < 10 or word `$65BD` = 1 → `$8227(c)`; `$828F(c)` (tax rate, word +`$44`); if `$7B70` = 0 → `$89FA(c)`; if `$8172(c)` → `$8A98(c)` (hire), `$8AE4(c)`, `$8B29(c)` (weapons); then `$82EC(c)` (move the ruler to another of its countries); if that did nothing → **war decision `$83D3(c)`**; finally treaty handling (`$8C0E`, `$86DB`). When the country is under the player's delegation (`$7B70`, `$DE0C`) and its plan record (`$DCB7`) has orders, `$80B7` carries those orders out instead (section 9b).

**War decision `$83D3(c)`:**
1. If the ruler's average stat (`$DF96` of the ruler at c+`$51`: mean of the six stats) < 50 → no war (`$8A38`).
2. Target `t = $8C7E(c)` (below); none → no war.
3. `A = $852E(c)` (section 9b capacity), `D = $8562(t)`, ratio `r = floor(A*100/(D+1))`.
4. r < 50 → `$8340(c, t, 2)` (diplomacy, mode 2); 50 ≤ r < 110 → no war; 110 ≤ r < 125 → `x = $8340(c, t, 1)`; 125 ≤ r < 170 → `x = $8340(c, t, 0)`; r ≥ 170 → x = 0 (no diplomacy).
5. If r ≥ 125, or t is the player's country (for every r outside 50-109, including r < 50 after the mode-2 diplomacy call): attack (`$85BC(c, t, …, 0)`, section 9b, which applies its own margin test) when x = 0, or when x ≠ 0 and `rand(100) < 20`.
   (`$8340`: see the diplomacy paragraph below.)


**[V] (v0.19):** both rules were checked against the game's own decisions, read from a frame-by-frame bytecode trace of two season passes (one normal start, one with the player at 3 Troops): **target choice 18/18** reproduced (all candidates eligible; 1 case took the "player's country, rand(5) = 0" shortcut), **war decision 18/18** reproduced (ratios 35-675; `r = floor(A*100/(D+1))` matched every logged A and D, e.g. 27*100/70 = 38, 27*100/4 = 675). Two r = 35 decisions against the player's country did call the attack routine, which then refuses at its own margin test (section 9b). The eligibility test `$DE28`, the random draws and the diplomacy results were taken from the trace, not recomputed (`targetverify.py`, `warverify.py`).


**Diplomacy used by the war decision (`$8340(c, t, mode)`, v0.20) [S] (read, not run).** If `$A2DA(c, t)` is true and mode ≠ 0 → returns 1 (no war). Otherwise by mode: 0 → "Become our vassal or we invade" (`$915E(c, t)`); 1 → tribute demand (`$9315(c, t)`); 2 → 9 in 10 times nothing, else (1 in 10, `rand(10)` = 0) an alliance offer to the player's country when `$7B70` = 0, `$DDFE(t)` and Gold(c) > 10 (`$938C`); mode 2 always returns 1. The return value is the x of step 5 above.
- When the target is the player's country, the player answers (`$8E75`).
- When the target is an AI country (`$9024(c, t)`): realm strength = total Troops of all countries under the same overlord (`$8D3F`). `R = floor(realm(c)*100 / (realm(t)+1))`. If `R > 160 + rand(31)`, the loyalty test `$8FB0` runs: with the two overlords' rulers (`$DF11` → `$DDD6`), `L8 = floor((L_t + C_t + 1)*100 / (L_c + C_c))` (Leadership + Charm), compared with `(100 if t's relation byte +$1A = 2 else 150) + rand(51)`; if L8 is greater and `rand(4)` = 0 the test is true. If the test is not true, `$8D96`: `floor(Troops_c*100/(Troops_t+1)) > 150 + rand(31)` and `rand(100) < 30` gives result 1, else 2. **Mapping inside `$915E` (v0.21):** `$9024` = 0 (realm test and loyalty test both true) → the target offers to submit for a price `P = 50 + rand(21) + 5*level + floor(Rate/2)` gold: an AI demander pays when its Gold ≥ P + 50 (Gold −= P; the target's overlord byte becomes the demander's, its stance byte = 101, relation byte +`$1A` = 2; "Country %d became a vassal of country %d"); a player demander is asked Yes/No and needs Gold ≥ P. `$9024` = 1 → accepted (x = 1, no war). `$9024` = 2 → refused (x = 0, the war decision then attacks). When the target is the player's country the player answers. **Tribute demand (`$9315(c, t)`, v0.22):** if t is the player's country the player answers (or, under delegation, the plan record's stored answer is used). An AI target accepts when `floor(Troops_c*100/(Troops_t+1)) > 140 + rand(31)` and `rand(100) < 30` (`$8DCC`). On acceptance (`$8F11`) t pays c 10 % of its Gold, 10 % of its Food and 10 % of its specialty-goods stock (each `$DAC2(…, 10)`, added to c with the 999 cap); a relation-change message follows (`$8F8D`). Accepted = x 1 (no war); refused = x 0.

**Target choice `$8C7E(c)`:** walk the country's neighbour list (`$DE89(c)`, up to 7, ended by 255). Only neighbours with `$DE28(c, n)` true count (`$DE28`: the two countries have different overlords (`$DBBF`), and either both are AI countries (`$DC1A` ≥ 4) or the treaty table `$79E6 + 27*slot + overlord` has no entry between them). The first one is the default; each later one replaces it if its defence `$8562(n)` is lower (weakest defence wins; a country short of food counts as 0). For each neighbour, if `rand(5)` = 0 and n is the player's country and `$DE0C(n)` is false, n is returned at once.

**AI hiring `$8A98(c)`:** if Gold > 2 × Troops: price p = `$9C92()`; N = floor(max/2) where max = `$9C7E(c, p, 999 − pop)` (as Hire's max, section 8); then the Hire stat update (`$9CA4`), Troops += N, Gold paid (`$9C65`).

**AI weapons `$8B29(c)`:** if pop > 10 and Arms < (300 if `$DDFE(c)` else 200): buys goods item 14 from merchant 0 at `$DCE5(0, 14)`, with strength byte `$7DF4`; the purchase amount continues past `$8B7F` (not fully read).

## 10. Battle: set-up screens and unit table [M]

1. Header screen "Country N / Offense vs Defense": names (the Mongols / Temujin vs the Tatars / Tug Temur), troop and food totals, "Unit View" with units 0-9.
2. **Men = 10 × soldiers.** Offense with 20 soldiers shows 200 men and 600 food (= food × 10); the defender (Tatars, 63 troops) shows 630 men and 900 food.
3. **Each side is divided into exactly 5 equal units** in slots 0, 1, 2, 4, 7. Unit size = total men / 5 = `2 × soldiers` (measured for 1, 3, 5, 7, 10, 20, 30, 61 soldiers; defender always 126).
4. The slots are three cavalry (type 0), one of type 1 and one of type 2. Type 0 = cavalry, 1 = infantry, 2 = archery (named by the game's own messages "Not an infantry unit!" for Ambush and "Not an archery unit!" for Arrows, and the Hire/SOS unit-type lists).
5. Battle block in WRAM (bank 0): `$6005` offense men (word), `$6007` offense food ×10, `$6020` defense men, `$6022` defense food ×10. Unit records are **11 bytes**, 10 per side: offense from `$6060`, defense from `$60CE`. Fields: `+0` size (word), `+2/+3` position (`FF FF` = not yet placed), `+4` type (0/1/2; `FF` = empty slot), `+5` mobility (10c; Pass caps it at 15/13/13), `+6` hidden-in-ambush word, `+8` hunting countdown, `+9` arrival countdown (0 = on the field), `+10` helping country id for SOS units (10f).
6. Terrain: see section 10b (16 × 10 grid at `$615C`). Cell size on screen is 32×32 NES pixels (cursor block measured 33×32). Terrain codes are defined by their effects (10c cost table, 10d defence table): 7 impassable, 6 castle (10f), 5 the Raid hex, 3 the Hunt terrain; their on-screen names were not confirmed.
7. The next stage is "Lord Temujin please position cavalry unit 0" with a cell cursor that moves one cell per press. Placement is in 10b; movement and combat rules are in 10c-10f.


## 10b. Battle input and map layout (v0.3) [M]

- Battle input poll: native function 6 (via `$F988`) returns the raw button bits in ACC: A=1, B=2, Select=4, Start=8, Up=16, Down=32, Left=64, Right=128 (Select/Start auto-repeat). The poll loop at `$91C1-$921A` blinks the cursor and returns the key; the dispatcher at `$B1F3` compares it (A is turned into code `0x30`).
- Battle map: **16 columns × 10 rows**, one byte per cell at `$615C + row*16 + col` (terrain code in the low 7 bits, mask `$FF7F`; bit 7 marks special cells). A second layer of the same shape is at `$61FC`. Terrain code `2` = plain (most cells). The placement code compares the terrain code with `7` and skips placement when equal (read from the bytecode, not tested). Meanings: see section 10 item 6 (v1.0).
- The placement cursor starts at cell **(7, 5)** and moves exactly one cell per direction press. The view is a window onto the map; `$614E` (x) and `$6150` (y) hold its origin, initially (5, 3). The bytecode compares the cursor with origin+2/+3 (x) and origin+4 (y) and calls scroll routines at `$8D74`/`$8D50`; the window size is inferred from those constants, not measured.
- **Placement rule (solved in v0.5).** Pressing A (key `0x30`) runs, in the bytecode at `$B252-$B2B6` (bank 5): (1) terrain code at the cursor cell must not equal 7; (2) the cell must be free (occupancy scan over the 20 unit slots); (3) the **second map layer** at `$61FC` (16 × 10 bytes, same layout as the terrain) must equal the side's code (offense = 0, the byte at `$6004`; the test is `(layer2 == code) XOR (side == 1)`; the defender branch also needs `$7BC0 != 0`). If all pass, the cursor cell is written into the next unit slot (`+2` = x, `+3` = y) and the next unit is requested.
- Layer 2 of this battle (rows 0-9, columns 0-15, `FF` = no zone): offense (code 0) = row 0 columns 11-14, row 1 columns 11-14, row 2 columns 12 and 14. Other codes present: 5 (columns 0-1, rows 1-5, plus (2,3)), 6 (rows 6-9, left side), 2 (columns 13-15, rows 6-9 and (15,6)). Zone codes other than the deployment codes are country ids: SOS reinforcements arrive on hexes whose zone equals the helping country, and Flee needs a hex whose zone is a friendly country (10f).
- Verified by two independent runs: placing at (11,1), (12,1), (13,1), (14,1), (11,0) filled slots 0, 1, 2, 4, 7 in that order; a fresh run accepted (14,2) and (12,2) (code 0) and rejected (10,1) and (13,2) (code `FF`) and every cell passed on the way. Pressing A twice on one cell can put two units on it (seen at (11,1), (12,1), (14,2)).
- After the offense is placed the defenders deploy automatically (this battle: (9,5), (10,6), (8,6), (10,5), (9,4) for slots 0, 1, 2, 4, 7), and the screen shows "Move where? (Mob 10)". Units are drawn with their men / 10 (offense 4, defense 12), and the cavalry slots carry mobility 10 (unit record `+5`), the other two 8.
- Movement was solved in v0.6 (section 10c: one hex per A press, Left/Right ring, terrain costs).

## 10c. Battle orders, movement and target search (v0.6) [S][M][V]

**Orders menu** (opens with B twice from the "Move where?" prompt, header "Lord <name> unit N awaits orders"): Move, Attack, Pass, Other, Flee, View, Recon. **Attack** opens "What attack do you want?": Normal, Arrows, Duel, Ambush. **Move** opens "What kind of movement": Move, Entire, Divide.

**The battle map is a hex grid.** 16 columns (0-15) × 10 rows (0-9). Neighbor routine at `$EE70` (native 6502, fixed bank; arguments: direction, pointer to x, pointer to y; returns 1 on success, 0 if off the map). Directions:

| Dir | Step |
|---|---|
| 0 | x-1; if new x is even, y+1 |
| 1 | y+1 |
| 2 | x+1; if new x is even, y+1 |
| 3 | x-1; if new x is odd, y-1 |
| 4 | y-1 |
| 5 | x+1; if new x is odd, y-1 |

Limits: x 0..15, y 0..9 (fail if the step would leave them).

**One move = one hex.** Move-commit function (bank 5 bytecode `$9B36`): reads unit x (+2) and y (+3); calls `$EE70` with the chosen direction; writes the new x, y; looks up the terrain cost `cost[terrain]` from the byte table at bank 5 `$BBB8` (terrain = map layer 1 at `$615C` masked `$FF7F`); subtracts it from mobility (record +5). Cost table, index = terrain code 0-15:

`8, 6, 3, 4, 5, 4, 5, 255, 7, 5, 255, 2, 5, 255, 255, 5` (255 = impassable: codes 7, 10, 13, 14). Plain (code 2) costs 3: mobility 10 → 7.

**Direction selection ring (Move where?).** After the first A, Left/Right rotate a selection through the six neighbors in the order `0,1,2,5,4,3`: Left = next, Right = previous, starting at 0. A with no Left/Right pressed since the prompt does nothing. A confirms the step. Up/Down do nothing here.

**Attack target search.** After "Attack where?" is shown (string `$B3A6`), the game scans the attacker's six neighbors with `$EE70` (function `$969F`: returns the direction index 0-5 at which a given enemy hex lies, 255 if none) and an enemy unit counts only if its size word ≠ 0 and its flag byte (+9) = 0 (`$9689`). Attack where? uses a map cursor with digit key codes ('1'-'3', '7'-'9' range checks at `$9254`), not the six-way ring.

**Still open in 10c:** how Attack where? maps the keys to a cell, the damage/casualty formula, morale, Arrows, Duel, Ambush, Flee, Recon, Other, Pass, turn order and day count.

## 10d. Normal attack resolution (v0.7) [S][M][V]

Verified on a Normal attack. Flow: Orders → Attack → Normal → "Attack where?" → direction ring → A. The ring for Attack starts at direction 5 (not 0 as in Move) and rotates in the same cyclic order (Left = next in `0,1,2,5,4,3`, Right = previous); A with no Left/Right does nothing and returns to the menu ("Nothing to attack"). The target hex must hold a live enemy adjacent to the attacker (see 10c).

Bank 5 bytecode. Attack function `$81E9(att, def)`; resolver `$815F(att, def, k)`; strength `$8061(unit)`; defense factor `$8024(unit)`; casualty function `$811E(unit, share)`.

**Terms.** `side` = 0 offense, 1 defense. Per-side words: `Skill_s = W[$6037+2s]`, `Arms_s = W[$603B+2s]` (copies of the country's skill (+2) and arms (+0) fields). Side block at `$6001+27*s`: word +0 = pointer to the ruler's six-word stat array at pointer+14 (Leadership L, Judgment J, Planning P, Charm C, Body Bd, Battle Bt, in that order); byte +2 = stance code (100, 101, 102 …).

**Strength of a unit** (size = unit size word):
```
sk   = Skill_s/10 + 1
ar   = Arms_s/5 + 1
base = ( floor(size*(100+sk)/100) + floor(size*(100+ar)/100) ) >> 1
f    = 1090                              if stance code == 102
     = Bt + (L>>1) + (Bd>>2) + (J>>1) + 1000   otherwise
strength = ( base + floor(size*f/1000) ) >> 1
```
**Stance value** `st(side)` = 2 if code = 100, 1 if code = 101, else 0.

**Matchup** `M[att type][def type]`, type 0 cavalry, 1 infantry, 2 archery, bytes at bank 5 `$BBE0`: `3,5,5, 4,3,3, 4,4,3` (row = attacker, column = defender). `k = M[..] + st(attacker side)`.

**Defense factor** (defender's hex terrain `t`, 0-5): `df = Tbl[type*6 + t] + st(defender side)`, with `Tbl` at bank 5 `$BBCE`:
```
cavalry : 1 1 3 2 1 5
infantry: 1 2 3 4 5 5
archery : 1 2 3 4 5 5
```
Terrain 6 uses a different branch (walls), see open items.

**Share and casualties.**
```
A = strength(att) * k
D = strength(def) * df
p = clamp( floor(100*A/(A+D)), 1, 99 )          (50 if A+D = 0)
cas(size, q) :  r = rand(24)
                L8 = floor(size*(87+r)/100)
                L7 = floor(L8*(100-q)^2 / (q^2 + (100-q)^2))
                loss = (L7>>1) + floor((100-q)/q) + 1       (capped at size)
attacker loses cas(att size, p), then defender loses cas(def size, 100-p)
```
The attacker's `rand(24)` is drawn first, the defender's second (section 3 RNG). Both sides always take casualties in one exchange. A unit that reaches 0 goes to a removal routine (`$8DA3`).

**Example (traced and measured):** archery unit size 40 attacks cavalry size 126 on plain: strengths 48 and 161, k = M[archery][cavalry] 4 + stance 2 = 6, df = Tbl 3 + stance 2 = 5, A = 288, D = 805, p = 26; attacker lost 22, defender lost 7.

**Still open in 10d:** terrain-6 wall branch (damages a wall/defense counter at `$95CE(1)+30` before the exchange), Arrows, Duel, Ambush, meaning of stance codes, what sets them, morale effects after casualties.

## 10e. Arrows and Duel (v0.8) [S][M][V]

### Arrows
Bank 4 `$846A(unit)`, damage bank 5 `$820E(shooter, target)`.
- Shooter must be an archery unit (type 2), else "Not an archery unit!". Arrows left = side block byte +9 (`$6001+27*s+9`); 0 → "You have no more arrows". Menu shows "Attack where? (N)" with N = arrows left. In this battle the offense side had 5 and the defense side 6 (origin of the starting count not decoded).
- Direction is chosen with the same Left/Right ring as Attack (start direction 5). The target hex is the hex **two steps** from the shooter along that direction (measured: the shot hit a unit exactly two hexes away in the chosen direction and no unit at 1, 3, 4, 5 or 6). A shot into an empty hex still spends one arrow (measured, count 5 → 4). A direction whose first hex is impassable (terrain 7) does not fire.
- Damage (casualties to the target unit), `rand(16)` drawn once:
```
q    = min( Skill_s/10 + 1, 30 )            (Skill_s of the shooter's side, section 10d)
Y    = floor( floor(size_shooter*q/100) * (30 + rand(16)) / 100 )
loss = floor( Y * (18 - df) / 19 )          (df = defense factor of the target, section 10d)
```
If the target stands on terrain 6, the shot also damages a wall counter first (`rand(3)` + `floor(size*(100+q)/100/90)` + 1 to the word at `$95CE(1)+30`). Wall branch not yet measured.

### Duel
Bank 5. Order "Duel" is accepted only from the Command Unit and only when the enemy Command Unit is adjacent ("Can't find the enemy Command Unit" otherwise). Each side may dare once (side block byte +11; "You may only dare once").
- Refusal rule for an AI-controlled challenged side (`$9EA8`): let `stat3(s)` = ruler word at pointer+20 (fourth stat). If `stat3(challenged) > stat3(challenger)`, the challenged side refuses when `rand(100) < 50`; otherwise it accepts. Measured: with stats 50 vs 300 two of three random draws refused; with equal or lower challenged stat, 27/27 draws accepted.
- Refusal penalty (measured and in the code): the refusing ruler's first stat (Leadership) and sixth stat (Battle) are halved (156 → 78, 196 → 98).
- Winner roll (`$9C83`): `ds(s) = Judgment + Body + Battle` of side s's ruler. Challenger side C wins when `ds(C) > floor(ds(other) * L / 100)` with `L = 75 + rand(76)` (plus `2*level` when the stance conditions `$97D2` and `$97A8` hold).
- A second roll (`$9CD3`): `rand(101) < floor(100*strength(command unit of s)/(sum of both command units' strengths))`. Result either sets a flag (`$63D4 = 1`, the command unit's fate: "<ruler> was ..." messages) or moves army size from loser to winner.
- Army transfer (`$9D06`, measured 10/10 exact in randomized duels): every live loser unit loses `floor(size*(20 + rand(10))/100)` (one draw per unit in slot order); with `T` = total lost and `n` = number of live winner units, **each** winner unit gains `floor(T/n)` (cap 9990 total).
- In 14 of 24 randomized duels no sizes changed (the flag branch).

**Still open in 10e:** exact meaning of the flag branch (capture/death of the command unit and what ends the battle), the order of the two rolls relative to the RNG stream, the stance conditions in the winner roll.

## 10f. Special orders, battle end conditions, day loop and battle AI (v0.9, extended v0.10-v0.11) [S][V]

**10f is read from ROM bytecode. These parts were then checked against the emulator and passed (rows 10f-1 to 10f-3 in section 12): the day loop (day counter, days remaining), the daily food use with remainder carry, the food-out and dead-Command-Unit end conditions with their winner and tie order, and the Pass mobility rule (one trial).** Everything else in 10f is read from bytecode only and not yet run. Where a routine is named but not read, it is listed as open.

### Side block (confirmed by reading code)
`$6001 + 27*s` (s = 0 offense, 1 defense): word +4 = total army size (written by `$952A`), word **+6 = food (provisions)**, byte +8 = food remainder, byte +10 = ambushes left, byte +2 = stance code, byte +9 arrows, byte +11 dare flag.

### Battle state words
`$6045` day counter (set to 1 before the loop, +1 each day). `$6043` days remaining (−1 each day). `$613C` side currently acting. `$613E` unit slot being processed by the AI. `$63D4` battle-over flag. `$603F` winning side. `$6142` surrender flag. `$6041` withdrawn flag. `$6140` AI mode word (0 or `$FF`). `$7A88` difficulty level. `$7BC0` auto/demo flag. `$6144/$6145` and `$6146/$6147` = x,y of the terrain-6 hex and the terrain-5 hex found by the map scan at `$95E7` (corrected in v0.12: terrain code, not zone; RAM shows (9,5) and (10,6), the only hexes with terrain 6 and 5 on the test map).

### Day loop (bank 4 `$96DD`–`$974E`)
```
repeat:
    day = day + 1                        ($6045)
    $92A3()                              (daily village effect, not read)
    for side in 0,1 until $63D4 != 0:
        if $97D2(side) then $8E5B() (player turn) else AI_TURN() (bank 5 $B048)
    if $63D4 != 0: break
    if $92FF() != 0: break               (food check, below)
    daysRemaining = daysRemaining - 1
until daysRemaining == 0
if $63D4 == 0 (days ran out):            $603F = 1, $6041 = 1 (offense has withdrawn), $63D4 = 1
```
`$97D2(side)` is true when the side is player-controlled (`$97B6` nonzero or `$7BC0` set with side 0 makes it false; otherwise it asks whether the side's country is the player's).

### Daily food (`$92FF`)
For each side s: `T` = live total army (`$9704(s)`). Food use per day = `T / 30` (integer); remainder `T % 30` is added to the side's byte +8, and when byte +8 ≥ 30 one more food is used and 30 is subtracted. Food (word +6) is reduced by that amount, not below 0. If food is 0 afterwards the other side wins (`$603F = s xor 1`, `$63D4 = 1`); side 0 is checked first and only one loss is recorded.

### End message (`$940B`)
Priority: `$6142` set → "has surrendered"; else `$6041` set → "has withdrawn"; else food of the loser side = 0 → "is out of provisions"; else total army of the loser = 0 → "was exterminated"; else "is in ruins!". Followed by "The army of …" and "Hit any key".

### Helper predicates (bank 5)
| Routine | Meaning |
|---|---|
| `$9602(u)` | side of unit pointer u = ((u−$6060)/11)/10 (signed divide) |
| `$9611(u)` | slot of u = ((u−$6060)/11) mod 10; slot 0 is the Command Unit |
| `$95A6(s)` | side block pointer `$6001 + 27*s` |
| `$95DE(slot, side)` | unit pointer `$6060 + 11*slot + 110*side` |
| `$9704(s)` | sum of sizes of the side's live units (flag +9 = 0) |
| `$97C4(s)` | side stance code (`$95B4`) equals 102 |
| `$A531(s)`, `$A53D(s)` | Command Unit pointer of side s, of the other side |
| `$A54B(u)` | unit is ready: word +6 = 0, size ≠ 0, flag +9 = 0 |
| `$A5A5(s)` | `$9704(s)/3 > $9704(other)` (side is more than 3 times stronger) |
| `$A5BD(s)` | `$9704(other)/5 > $9704(s)` (side is under one fifth of the other) |
| `$A5D5(s)` | 0 for side 0; `$A5BD(s)` for side 1 |
| `$A5E6(s)` | false if side has exactly one live unit; else `Command Unit size * 100 / $A596(...)` < 30 |

### Special orders (Orders → "Other", bank 4 `$8B9A`; menu items 1–4 = Demand `$85F0`, SOS `$881F`, Raid `$8905`, Hunt `$89AE`)
All four require the acting unit to be the Command Unit (`$9611` = 0) except where stated.
- **Demand surrender (`$85F0`).** Needs the enemy Command Unit adjacent ("Enemy Command Unit not found"). `own` = side of the acting unit, `en` = other side. Refused with "They are still too strong" if any: enemy side is player-controlled (`$97D2(en)`); `$97C4(en)`; `$97C4(own)`; `6*strength(Command Unit own) / (strength(Command Unit en) * defenseFactor(Command Unit en)) <= 5`; or `$9704(own) / $9704(en) <= 4` (signed integer divide). Otherwise, with `E` = enemy ruler word +24 + word +20 + 1 and `O` = own ruler word +24 + word +20 + 1 (ruler pointer = side block word +0, stat words from +14): success needs `floor(E*100/O) <= 70 + rand(81)` and `rand(100) < 100 − 10*level` (`$7A88`). Success prints "gave up" and sets `$603F` = own side, `$63D4 = 1`, `$6142 = 1`; failure prints message `$B5D1`.
- **SOS (`$881F` → `$8753`; menu label "SOS").** Command Unit only. Fails with "There is no country that can send troops" if `$9852(side)` = 0; if the zone code under the unit is not a country (`$953B` = 255) or that country fails `$DF83` with the side's country (`$8068` message); "You can't lead any more soldiers" if there is no free unit slot (`$9666`) or the side's total size incl. arriving units (`$972E`) ≥ 9990; "Extra troops won't arrive in time" if days remaining ≤ 3; "That country can't send troops" if the helper's Gold or Troops is 0 or the side-block word at +13 + 2*i is 0, where i is the helper's position (0-6) in the 7-entry country list returned by `$DE89` for the defender (`$8725`). Then: unit type chosen from a 3-item menu; `max = min(9990 − total, Gold*10, floor(Troops*10/2))` (helper country); "How many soldiers will you summon" (1-max) → size N; helper Gold −= ceil(N/10) and Troops −= ceil(N/10); new unit: size N, flag +9 = 3 (arrives after 3 day ends), word +6 = 0, byte +10 = helper country id.
- **Raid (`$8905`).** Only side 0 ("You can't do that in your own country!" for side 1); unit must stand on a terrain-5 hex (`$9527` returns the terrain code; v0.9-v0.11 wrongly said zone). With `c` = the defender's country record (VM base, see below): fails ("The people have nothing left to give") if word `c+0x46` ≠ 0 or the amount is 0. `amount = floor(Farmers * (30 + rand(11)) / 100)` is removed from Farmers (`c+0x3C`); Morale (`c+0x42`) = `floor(Morale * (55 + rand(21)) / 100)`; side 0 food (`$6007`) = min(9999, food + 10*amount); the code then stores 1 through a local meant to point at `c+0x46`, but **in the emulator that word is never written** (WRAM diff before/after shows only food, Farmers and Morale changing), so the "already raided" block never triggers. **[V]** row 10f-4: 20/20 exact (Farmers, Morale, food). message "Raiding has yielded %d food from the enemy" shows `10*amount`. The AI routine `$A924` performs the same raid with the same formulas.
- **Hunt (`$89AE`).** Unit must stand on a terrain-3 hex; fails with "You can't do that now" if `$9819` is true; otherwise unit byte +8 = 2 ("Hunting"; AI units with byte +8 ≠ 0 skip their turn).
- **Pass (`$8B7D` → `$9C36`).** Unit mobility (byte +5) becomes `min(mobility + 1, max)` with max = 15 (cavalry), 13 (infantry), 13 (archers) from the table at bank 5 `$BBB5`; then "Mobility is now %d" is printed.
- **Ambush (`$8506`).** Unit type must be 1 (infantry); not the Command Unit; side block byte +10 must be nonzero ("Troops have reached their limit" otherwise); the target hex is chosen with the ring and must pass `$9941` (unless direction code `$35`) and `$9964` ("Can't ambush here", "Enemy unit too close for ambush"). On success unit word +6 = 1 (hidden), and byte +10 is decreased by 1; if the code is not `$35` and `$9B2C`, `$99EC`, `$9188` pass, word +6 is set back to 0 immediately. Hidden units are processed by `$AD9A` in the AI.

### Battle AI (bank 5 `$B048`, one call per side per day)
For slot = 0..9 (`$613E`): unit `u = $95F2()`. Skip if size = 0 or flag +9 ≠ 0. Then, in this order, the first matching branch runs, then `$99AE` is tested and the loop ends if it returns nonzero:
1. unit word +6 ≠ 0 (hidden): `$AD9A(u)`.
2. otherwise `$A50C(u)` (cursor focus) and, if unit byte +8 ≠ 0: print "Hunting", skip.
3. `$6140 = 1` and side = 1: `$AF98(u)`.
4. `$6140 = 0`: `$AEFE(u)`.
5. `$A5E6(side)` true: `$AEDC(u)`.
6. `$A5D5(side)` true: `$AEC3(u)`.
7. side = 0: `$AD05(u)` (attack posture); side = 1: `$AD38(u)` (defense posture).

`$80FD(s)` = days of food = `min(90, food_s * 30 / $9704(s))` (food word +6; total army of live units). It is the same ratio as the daily food rule (use = army/30 per day).
`$AD05(u)` (side 0): `A = $80FD(0)`, `B = $80FD(1)`. If `A > B` and `B < daysRemaining` and `B < 2` → `$AC11(u)` (hold: the defender starves within 2 days); otherwise `$AB29(u)` (advance and attack).
`$AD38(u)` (side 1): `A = $80FD(1)`, `B = $80FD(0)`. If `$A5A5(side)` (own army more than 3 times the enemy's) or (`A < daysRemaining` and `A < B`) → `$AB29(u)` (attack: it will starve before the battle ends). Else on day 1 with `A >= 10` it tries `$AC53(u)` (ambush) and stops if that succeeds. Else `$A5BD(side)` (own army under one fifth of the enemy's) → `$AC11(u)`; otherwise `$AAAC(u)`.
`$AC53(u)` (AI ambush): requires infantry, not the Command Unit, ambushes left > 0; tries up to 7 directions starting at `rand(7)` through the table at bank 5 `$BE4E`, using the same hex checks as the player's ambush (`$9941`/`$9964`); on success the unit word +6 = 1 and byte +10 −1.
`$AEFE(u)` (`$6140 = 0`): if the unit is side 0's Command Unit and the enemy Command Unit is adjacent (`$9809`): when `$9894(side)` is true and `rand(100) > 130 − 15*(7 − level)` (= `25 + 15*level`), the AI demands surrender and succeeds: `$603F` = own side, `$6142 = 1`, `$63D4 = 1`. If the enemy Command Unit is not adjacent it first tries `$A6A8` (move toward it), otherwise looks through its other units for one that `$A82D` accepts and attacks with `$9BF7`; if none, it sets `$6140 = $FF` and passes (`$9C36`). Other units and side 1 use the `$AF88` path (move toward the own Command Unit's target via `$A6A8`, else pass).
`$AF98(u)` (`$6140 = 1`, side 1): contains a withdraw branch that sets `$603F` to the other side, `$6041 = 1`, `$63D4 = 1`, and otherwise adjusts a word at ruler-record offset +0x18 by `min(999 − …, size/10)` and clears the unit; branch conditions not fully read.

### AI path search (bank 5 `$A1C6`, `$A0A2`, `$A322`, `$A42E`) [S]
- Distance map: 160 words (16 × 10 hexes, index `y*16 + x`) initialised to `$7575` (unvisited); the target hex gets 0. A flood fill with a 160-entry ring-buffer queue expands to the six neighbours (`$EE70`), adding the movement cost `$BBB8[terrain & $FF7F]` (the same terrain table as player movement) to the distance of the hex being expanded; it stops when the unit's hex is reached.
- `$A42E` marks impassable hexes (terrain code 7) with 30000 (`$7530`) before the fill; `$A123` adds 1 to the word of every cell whose terrain code is 7 and `$A14A` adds 1 to the word of every hex occupied by a live unit (own-side units skipped when its third argument is nonzero); what these increments are used for is not worked out (reading uncertain).
- `$A0A2(u, map)`: for the six neighbours of the unit finds the one with the smallest distance lower than the unit's own hex; if `$9941` accepts the step it is made (`$9B2C`), then `$9188`, `$9ABE`, `$9AA4` are checked and the step repeats while the unit can still move. This is the AI's "move toward target".
- Targets are chosen by the callers: `$A6A8(target, u)` (approach a unit), `$A777` (first live enemy unit), `$A7A0` (an enemy unit adjacent to or attackable from the Command Unit; exact rule not worked out), `$A7EE` (a ready enemy unit whose size is at most one third of the unit's size).

### Castle hex (terrain 6) and Morale in battle (v0.20) [S] (not yet run)
- **Defense factor on terrain 6** (`$8024`): `df = $DAD9(floor(Defense/7) + 1) + 3 + st(defender)`, where Defense is the defending country's Defense (+`$1E`) and `$DAD9` is a 32-bit bit-by-bit routine read as an integer square root. On terrains 0-5 the table of section 10d is used instead.
- **Normal attack against a unit on terrain 6** (`$815F`, before the casualty roll): defending country Defense −= min(Defense, `floor(floor(size_att*(100 + ar)/100) / 30) + rand(4) + 5`), with ar = Arms/5 + 1 of the attacker's side.
- **Arrows against a unit on terrain 6** (`$820E`): Defense −= min(Defense, `floor(floor(size_shooter*(100 + q)/100) / 90) + rand(3) + 1`) (q as in 10e; this is the "wall counter" of 10e, now identified as the defender's Defense).
- **Morale has no effect on battle results.** The only reads of the country Morale word in the battle code are the display (`$BA60`), Raid (10f) and castle occupation (10f), which lower it.

### Battle-AI helper routines (v0.20) [S] (read, not run)
| Routine | Rule |
|---|---|
| `$A66B(u, t)` | archers with arrows (`$99FE`) shoot if t is exactly 2 hexes away in a direction (`$A06D` → `$A016`, `$820E`); otherwise a Normal attack if adjacent (`$9FFE` → `$81E9`) |
| `$A68A(u, t)` | if t is not adjacent, take one path-search step toward t (`$A308`) |
| `$A6A8(u, t)` | `$A68A`; if it moved, mobility is reset from the table at bank 5 `$BBB2` = 10 (cavalry), 8 (infantry), 8 (archery), the same values as a unit's starting mobility (`$99EC`) |
| `$A6BF(u, t)` | for archers: look for a hex two steps away along any of the six directions that is passable (terrain ≠ 7) and free, and move there to get a shooting position; otherwise `$A68A` |
| `$A777(u)` | first ready enemy unit (slot order) |
| `$A7A0(u)` | an enemy unit that is adjacent to the own Command Unit, or that the own Command Unit can shoot (`$A016`); the last such unit adjacent to u is preferred |
| `$A7EE(u)` | attack (`$A66B`) a ready enemy unit whose size is at most one third of u's size |
| `$A82D(a, u)` | true if a is adjacent to u and of the same type (merge candidate) |
| `$A84D(u)` | merge u into the own Command Unit if `$A82D` holds (`$9BF7`) |
| `$A870(u)` | merge into the Command Unit when the Command Unit is larger than 15 % of the enemy's army, or u's size < 10 |
| `$A8A7(u, t)` | if own army > 50 % of the enemy's and t is not adjacent: find another own unit adjacent to u, same type, adjacent to t, and merge |
| `$A915(u)` | try a Duel (`$9F4B`, 10e) |
| `$A99A(u)` | try Raid (`$A924`); else attack the first ready enemy (`$A6A8`, `$A66B`); else pass |
| `$A4E9(u)` | build the impassable-hex map (`$A42E`) and take one path step toward the best target hex (`$A0A2`); if it moved, reset mobility (`$99EC`) |

### More battle-state rules (v0.10, bank 4/5 bytecode)
- **Battle-over test `$99AE`** (called after every unit action and every AI step): if `$63D4` is not set and, for either side s, food(s) = 0 or the side's Command Unit (slot 0) has size 0, then `$603F = s xor 1` and `$63D4 = 1`. A destroyed Command Unit therefore ends the battle at once.
- **`$9894(side)`** is false only when `Command Unit size(side) / 2 < Command Unit size(other)` and `army(side) / 2 < army(other)`; i.e. true when the side has at least twice the enemy's Command Unit size or army. The AI mode word `$6140` is set to 0 (demand-surrender mode, `$AEFE` for every unit) when neither side is player-controlled, `$7BC0 = 0` and `$9894(0)` is true; otherwise `$FF`.
- **Hunting payout (`$912B`, corrected in v0.12; v0.10/v0.11 wrongly called this reinforcement arrival).** At each day end, every unit with byte +8 > 0 (set to 2 by Hunt) has it decreased by 1; when it reaches 0 its side gains food: `x = floor(size*(2 + rand(19))/100) + 1`, `x = floor(x * (101 − floor(20*level/3) + rand(5)) / 100)`, food = min(9990, food + x). The unit skips its turns while byte +8 ≠ 0. **[V]** row 10f-5: the payout arrives at the end of the second day and lies in the formula's value set in 20/20 trials (10 with a fixed RNG state, which all gave the maximum r1 = 18, r2 = 4; 10 with random RNG states).
- **SOS arrival (`$9273`, `$91E7`).** Each day every unit with flag +9 > 0 has it decreased by 1 (SOS sets it to 3); when it reaches 0 the unit is placed on the first free hex (scanning y 0-9, x 0-15) whose zone code equals the unit's byte +10 (the helping country's id), and "Extra troops are here" is shown; if no hex qualifies, "Extra troops left" (`$B8D2`, `$B8E8`).
- **Merge (`$9BF7(a, b)`).** Unit a takes b's coordinates; both sizes are set to 0 and the sum is written into the unit with the lower record address.
- **Player turn (`$8E5B`).** For slot 0..9: skip empty/not-live units; select the unit (`$88D0`); if byte +8 ≠ 0 print "Hunting" (`$B89B`); else if word +6 ≠ 0 call the ambush prompt `$8DB7` ("… stay in ambush?"); else the command prompt `$8DF4`. After each unit `$99AE` is tested.

### Battle-side country record, Flee, Orders dispatch and other v0.12 findings [S] (Flee, castle and stance parts not yet run)
- **Country record as the battle code sees it:** `$DBA7(c) = $6DCD + 83*c` (c = country index 0-13; byte +3 of the side block holds it). This base is 20 bytes before the v0.1 table's base, so VM offset = v0.1 offset + `$14`: +`$14` Arms, +`$16` Skill, +`$18` Troops, +`$1A` Economy, +`$1C` Town, +`$1E` Defense, +`$20` Masons, +`$36` Food stock, +`$38/$3A` Artisans, +`$3C/$3E` Farmers, +`$40` Gold, +`$42` Morale, +`$46` raid word. `$DECD(c)` = Troops + Town + Masons + Artisans(copy) + Farmers(copy) (the population sum also used by Train → Troops).
- **Battle size = 10 × soldiers.** Flee returns `size/10` soldiers; the war launch sends 20 soldiers and the offense army in battle totals 200.
- **Orders menu (bank 4 `$8DF4`, 7 items):** Move `$8ADC`, Attack `$8B21`, Pass `$8B7D`, Other `$8B9A`, Flee `$8BEF`, View `$8C7B`, Recon `$8D11`. Other submenu (screen labels): **Demand** `$85F0`, **SOS** `$881F`, **Raid** `$8905`, **Hunt** `$89AE`.
- **Flee (`$8BEF`).** The unit's hex must carry a zone code that is a country (`$953B` ≠ 255) and that country must pass `$DF83` with the side's country (same value of `$DBBF`, i.e. byte +`$1D` of their records); else "You can't withdraw from here". If the side's stance code is 100 and the unit is the Command Unit, the confirm "The commander from the withdrawing side has deserted his men. Do you really want to withdraw?" (`$8A22`) is shown; Yes sets `$6041 = 1` (battle ends as withdrawn). The unit's soldiers go home: country Troops (+`$18`) += `min(999 − population, size/10)`; the unit is removed (size 0).
- **View (`$8C7B`) and Recon (`$8D11`)** only change the display (map views and a scroll cursor); no game data is written.
- **Castle occupation (`$92A3`, start of each day).** If the offense Command Unit stands on the terrain-6 hex and the defense Command Unit is not adjacent, defender Morale = `floor(Morale * 93/100)` − 3 (not below 0).
- **Side stance byte +2 (values 100/101/102), effects found in code:** 100: Flee by the Command Unit asks the desertion confirm; attack stance bonus +2 (10d). 101: `$97D2` treats the side as not player-controlled, so the AI plays it; stance bonus +1. 102: strength factor fixed at 1090 (no ruler stats, 10d); a Demand by or against such a side is refused ("too strong"). What the game calls these three cases is not decoded.
- **Battle maps (`$9033`).** Terrain (160 bytes) and zone (160 bytes) layers and the 176-byte unit set-up are copied per defender country from bank tables: terrain `$8B4E + 160*c`, zones `$A4EE + 160*c`, set-up `$976A + 176*c`; when word `$65B9` = 27 a second set is used (`$940E`, `$ADAE`, `$A10A`).
- **AI hunting (`$A739`).** If days of food < days remaining, days remaining > 10, the unit stands on terrain 3 and no enemy unit is adjacent (`$9819`), the AI sets byte +8 = 2; otherwise it passes.

**Open in 10f:** the emulator check of the helper table and of the castle rules, the meaning (names) of stance codes 100/101/102.

## 11. Still to be done (v1.0)

**Not decoded at all**
1. Player orders Give, Move and Spy (bank 2/3 handlers not read). Tax, View and Other were not read either; from their strings they are economy or display.
2. AI economic routines in the country turn: `$8227`, `$828F` (tax), `$89FA`, `$8172`, `$8AE4`, `$82EC` (ruler move), the purchase amount at the end of `$8B29`, the helpers `$98DD`, `$DE0C`, `$DDFE`, `$DC1A`, the treaty handlers `$8C0E` and `$86DB`, the delegated-orders routine `$80B7`, and the branches `$8003` and `$8029`.
3. What happens after a battle: survivors returning, ruler capture or death, the duel "flag" branch (`$63D4` with no army transfer), and the post-battle code `$936A`/`$939E`.
4. The names the game uses for side stance codes 100/101/102 (their effects are decoded, 10f).
5. The extra ~10 gold of Spring income (economy side, section 8b).

**Decoded but not yet run against the emulator**
6. Battle: Demand, SOS (limits, arrival), Ambush, Flee, castle rules (terrain-6 defence and Defense damage), the battle-AI posture rules and all helper routines (10f), the AI path search.
7. Strategic: Assign, army composition, Treaty, Policy, vassal and tribute acceptance (9c), AI hiring and weapons, Rate drift, Spring mobilization % recompute, Autumn harvest.
8. AI attack launch has only 1 captured run (9b); more runs would strengthen it.

**Kept elsewhere**
9. Graphics: separate project doc `Genghis_Khan_NES_Graphics_Loader_Findings_20261103.md`.

## 12. Verification log (three independent passes)

| Pass | Method | Result |
|---|---|---|
| 1 | Train → Troops: formula from bytecode vs emulator, 24 random RNG states and troop counts, plus levels 1-5 | 24/24 exact; levels 1-5 exact |
| 2 | Train → People: formula from bytecode vs emulator, 20 random trials (stats, level, proportion incl. cap and ≥50) | 20/20 exact |
| 3 | RNG: constant read from ROM file bytes equals the constant used; emulator state after one command equals the predicted next state, 10 random states | 10/10 exact; `K` matches ROM bytes |
| Placement (v0.5) | Zone rule re-tested on a fresh load: 2 zone cells accepted, 2 non-zone cells rejected; 5 placements in zone filled slots 0,1,2,4,7 | as stated in section 10b |
| 10c-1 (v0.6) | Hex step rule vs game: 60 moves (10 start cells × 6 directions, plain map, Left/Right ring) | 59/60 exact; the one miss is a move into a cell occupied by our own unit 4 (blocked, as expected) |
| 10c-2 (v0.6) | Terrain cost table vs game: all 16 terrain codes placed on the destination cell, mobility 10 | 16/16: cost equals table; codes 7, 10, 13, 14 refused |
| 10c-3 (v0.6) | Selection ring: all 31 L/R sequences up to length 4 vs predicted ring position | 31/31 |
| 10c-4 (v0.6) | Neighbor routine read from ROM vs values the ROM returned in a trace (attacker at (11,1): directions 0-3 gave (10,2),(11,2),(12,2),(10,1)) | identical |
| 10d-1 (v0.7) | Normal-attack model vs game, 40 random trials (sizes 5-500, types, terrain 0-5, stance codes, skill/arms words, ruler stats, RNG state) | 40/40 exact (attacker loss, defender loss) |
| 10d-2 (v0.7) | Same model, wide ranges (sizes 1-998, stats 0-999), fresh seed | 40/40 exact |
| 10d-3 (v0.7) | Same model with a different defender record (infantry slot 4 moved to the target hex), fresh seed | 30/30 exact |
| 10d-4 (v0.7) | Intermediate values read from the bytecode trace vs model for one attack (strengths 48/161, 288, 805, 26) | identical |
| 10e-1 (v0.8) | Arrow damage model vs game, 2 × 30 random trials (shooter size 100-900, target size, type, terrain 0-5, skill, stance, RNG) | 60/60 exact |
| 10e-2 (v0.8) | Arrow range: defender placed on every hex within 6 of the shooter, two directions | hit only at exactly 2 hexes in the chosen direction |
| 10e-3 (v0.8) | Duel army transfer: 24 randomized duels, loser loses floor(size*(20+r)/100) per unit with r in 0-9, winners gain floor(T/n) | 10/10 of the duels that transferred army fit exactly; 14 changed nothing (flag branch) |
| 10e-4 (v0.8) | Duel refusal: 9 stat pairs × 3 RNG draws; halving of refuser's stats read from RAM after the message | refusal only when challenged stat > challenger stat; stats 156/196 → 78/98 |
| 10f-1 (v0.11) | Daily food: set random sizes (30-900 per unit), food (400-3000) and remainder byte (0-29) for both sides, Pass through the player's last unit, read RAM at the day change; model `use = T/30 (+1 if T%30 + carry >= 30)` | 16/16 trials, both sides (32 side-days); day counter 1→2 and days remaining 90→89 each time |
| 10f-2 (v0.11) | End conditions: food 3 for side 0 / side 1 / both, and defense Command Unit size 0, then one day | winner 1 / 0 / 1 (side 0 recorded first) / 0, `$63D4 = 1` each time |
| 10f-3 (v0.11) | Pass mobility: unit 7 (archer) 8 → 9 | matches `min(m+1, 13)` (one trial; cap not tested) |
| 10f-4 (v0.12) | Raid: Command Unit poked onto the terrain-5 hex, random Farmers (0-999), Morale (0-300), food (0-9990), random RNG state; Orders → Other → Raid | 20/20 exact for Farmers, Morale and food; raid word +`$46` never written |
| 10f-5 (v0.12) | Hunt: Command Unit on a terrain-3 hex, random size 50-2000, other units parked; food change per day minus the daily use | payout only at the end of the 2nd day, 20/20 inside the formula's value set (10 fixed-RNG, 10 random-RNG) |
| 10f-6 (v0.12) | Terrain-6/terrain-5 hex scan vs RAM `$6144-$6147` | (9,5) and (10,6) = the only terrain-6 and terrain-5 hexes |
| 8-1 (v0.13) | Draft vs emulator, 20 random trials | 20/20 exact |
| 8-2 (v0.13) | Hire vs emulator, 20 random trials | 20/20 exact |
| 8-3 (v0.13) | Weapon purchase vs emulator, 18 random trials over all six weapons | 18/18 exact |
| 7-1 (v0.15) | Train → Self vs emulator, 18 random trials over all six stats and levels 1-5 | 18/18 exact |
| 7-2 (v0.15) | Train → Prince vs emulator (WRAM diff), 12 random trials | 12/12: only the chosen prince stat changed, by exactly the predicted gain |
| 8b-1 (v0.15) | Seasonal decay: Summer Skill and Winter Arms, 4 random values each | 8/8 exact |
| 8b-2 (v0.15) | Spring food upkeep, 6 random Food values (3 starving) | 3/3 exact food; starving Troops = scaled value + 1 in 3/3 |
| 8b-3 (v0.16) | Spring population growth, 12 random group sizes and pools | 11/12 exact; the 12th was interrupted by an AI invasion (v0.17) |
| 8b-4 (v0.16) | Spring soldier pay, 10 random Gold/Troops | rule fits 9/10 with about 10 extra gold of income; the 10th was interrupted by an AI invasion (v0.17) |
| 9b-1 (v0.18) | AI attack launch captured in the emulator (Winter AI turn, player at 3 Troops) | soldiers 27 = floor(min(43,90,39)*70/100), Troops −27, Gold −27, battle food 270, 30 days: all as predicted (1 run) |
| 9c-1 (v0.19) | AI target choice vs game decisions (trace, 2 runs) | 18/18 reproduced |
| 9c-2 (v0.19) | AI war decision branches vs game decisions (trace, 2 runs) | 18/18 reproduced; ratio formula exact on all 18 |
| QA (v0.4) | Fresh checks of guide claims: stat table vs ROM file; Train gate at Leadership 3-6; war costs and unit sizes at 3, 7, 10 soldiers; food limit; opcode immediates against 3 traces | Table found in ROM at `0x1F1C8`; gate = 5; costs and sizes as stated; 31 food for 10 soldiers refused; immediates 100% consistent. Errors found and fixed: wrong status line, unsupported "identical in both games" claim, `8C/8D/8E` mislabeled, stale terrain note, overstated placement test wording. |

Overall status (v1.0): the rules listed as [V] in the status paragraph at the top are verified by the counts above. The guide as a whole is **not complete**: section 11 lists what is not decoded and what is decoded but not run.

### v1.0 consolidation QA (three independent passes, 2026-11-03)

Each pass checks against a different source: pass 1 against the earlier guide versions, pass 2 against the emulator, pass 3 against the ROM bytes.

**Pass 1 — completeness against v0.1-v0.23.** Every content line in v0.1-v0.23 was compared with v1.0. 97 lines had no counterpart. Each was either stale (superseded by a later finding) or a fact that had been dropped. Stale lines were replaced with the current finding: gating rows 5-9, 15, 24-27; Proportion used by Draft; supplies-days formula; unit types; unit record fields +5/+6/+8/+9/+10; terrain codes 3/5/6/7; zone codes = country ids; movement; opcode meanings. Dropped v0.1 facts were restored in Appendix A (country record offsets, order menu, 3 orders per turn). A second read-through found three more stale items, all fixed: the 9b note calling target choice "not yet read"; the old "not complete and not verified" summary under the table above; and missing routines `$8C0E`, `$86DB`, `$80B7`, `$8003`, `$8029` in section 11, item 2.

**Pass 2 — regression rerun against the emulator.** Every verify script was run again on this version. Results:

| Check | Result |
|---|---|
| Train Troops / RNG (`traintest.py`) | 0 mismatches of 24 |
| Normal attack (`combatverify.py`, `combatverify2.py`) | 40/40, 30/30 |
| Arrows (`arrowverify.py`) | 30/30 |
| Duel transfer (`duelverify2.py`) | as predicted |
| Hex step (`hexverify.py`) | 59/60; the miss is the same blocked move into our own unit |
| Terrain cost (`costverify.py`) | 16/16 terrain codes |
| Day loop / daily food (`dayverify.py`) | 16/16 |
| End conditions (`endverify.py`) | all cases as expected |
| Raid (`raidverify.py`) | all formula values match; the script still flags the raid flag at +46, which the game never writes (section 10f) |
| Hunt (`huntverify.py`) | 10/10 within the possible range |
| Draft, Hire, weapons | 20/20, 20/20, 18/18 |
| Train Self, Prince | 18/18, 12/12 |
| Summer Skill, Winter Arms (`seasonverify.py`) | 8/8 |
| Spring food upkeep | 4/4 |
| Spring growth (`growthverify.py`) | 11/12 (the 12th is the AI-invasion case) |
| Spring pay (`payverify.py`) | 9/10 fit: 7 runs are exactly +10 gold, 1 shortfall run fits with +10, 1 run matches with no extra gold, and 1 is the AI-invasion run; the script's exact test reports 1/10 because it leaves out the extra gold |
| AI attack launch (`aiattackverify.py`) | Troops 43→16, Gold 39→12, Food 90 unchanged, 30 days: same as v0.18 |
| AI target choice (`targetverify.py`, default and Troops 3) | 13/13 + 5/5 = 18/18 |
| AI war decision (`warverify.py`, default and Troops 3) | 13/13 + 5/5 = 18/18 |

**Pass 3 — against the ROM bytes (rerun on the final text).** ROM text is plain ASCII, with line breaks and control bytes between words. Every quoted span in the doc was searched in the ROM text, after joining those line breaks. Of 91 spans: 71 are game messages and every one was found. 11 are the doc's own wording, or false matches where the search paired quote marks from two different quotes. 9 are labels read off the screen that are not stored as one plain string: "30 days left" / "90 days left", "Country N / Offense vs Defense" (Offense and Defense are stored as separate words), "Lord Temujin please position cavalry unit 0" (built from "please position %s"), "Orders left: 3", "Other", "SOS", "Skills" (the ROM stores "Skill"), "Unit View". The first run of this pass reported 77 strings and "every game message found". That count came from an earlier text and missed the label distinction; the counts above replace it. Tables re-read from bank 5: `$BBB2` = 10,8,8; `$BBB5` = 15,13,13; `$BBB8` = 8,6,3,4,5,4,5,255,7,5,255,2,5,255,255,5; `$BBCE` = 1,1,3,2,1,5 / 1,2,3,4,5,5 / 1,2,3,4,5,5; `$BBE0` = 3,5,5,4,3,3,4,4,3. All match the doc. `$BBB2` was added during this pass, to the battle-AI helper table in section 10f (`$A6A8`).

## 13. Files

`gkcore.c`, `gk.py`, `d6502.py`, `vmdis.py`, `cmdtrace.py`, `nav.py`, `wartools.py`, `traintest.py`, `vm_opcode_table.txt`, `vm_handlers.txt`, `train_troops.json`, save-states `state_orders.bin`, `state_warmap.bin`, `state_warsold.bin`, `state_battle0.bin`, `state_battle1.bin`, `state_s5.bin`, `state_orders_attack.bin`, `state_attackwhere.bin`, `hexverify.py`, `ringmap.py`, `costverify.py`, `combat.py`, `combatverify.py`, `combatverify2.py`, `state_preatk.bin`, `arrowverify.py`, `duelverify2.py`, `state_duel1.bin`, `state_afterpass.bin`, `dayverify.py`, `endverify.py`, `raidverify.py`, `huntverify.py`, `state_day2_u0.bin`, `draftverify.py`, `hireverify.py`, `armsverify.py`, `selfverify.py`, `princeverify.py`, `seasonverify.py`, `growthverify.py`, `payverify.py`, `aiattackverify.py`, `targetverify.py`, `warverify.py`.

GitHub: `Bomb-Daddy/NES`, folder `Genghis_Khan/military_tools/` (ROM not included; see its README).

## Appendix A. Country record and order list (carried forward from v0.1, v1.0)

The player gets **3 orders per turn** ("Orders left: 3"); the menu cursor is the byte at `$05A6` (1 = Tax, 4 = Train). Order menu: Tax, Assign, Give, Train, Trade, People, View, Move, Policy, Treaty, Spy, War, Pass, Other. [M]

Country records: 14 records of 83 (`$53`) bytes; record 15 (`$726B`) is all zero. Two address views of the same bytes:

| Field | v0.1 offset (base `$6DE1` for country 1) | Battle/strategic code offset (base `$6DCD + 83*index`, index 0 = country 1) |
|---|---|---|
| Arms | +`$00` | +`$14` |
| Skill | +`$02` | +`$16` |
| Troops | +`$04` | +`$18` |
| Economy | +`$06` | +`$1A` |
| Town | +`$08` | +`$1C` |
| Defense | +`$0A` | +`$1E` |
| Masons | +`$0C` | +`$20` |
| Food stock | +`$22` | +`$36` |
| Artisans (two copies) | +`$24` / +`$26` | +`$38` / +`$3A` |
| Food production / food workers (two copies) | +`$28` / +`$2A` | +`$3C` / +`$3E` (+`$3C` is cleared each Autumn, 8b) |
| Gold | +`$2C` | +`$40` |
| Morale | +`$2E` | +`$42` |
| Tax rate | — | +`$44` |
| Raid word | — | +`$46` |
| Mobilization % (byte) / draft pool (word) | +`$34` / +`$35` | +`$48` / +`$49` |
| Ruler pointer | — | +`$51` (ruler stat words from ruler + 14: Leadership, Judgment, Planning, Charm, Body, Battle) |

Country 1's ruler stats are also visible at `$65CD` (six words in the same order). Prince records: `$6904 + 145*n`, stats from +14.
