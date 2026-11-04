> This is the preserved source-batch explanation. Its original runtime targets and stage-specific status are resolved or explicitly bounded by the v1.07 master and final acceptance register; these source tests are not relabelled as Mesen playthroughs.

> v1.07 supersession: positive Draft8 is OBSERVED_RUNTIME (Troops61→69, reserve34→26); ordinary Tax0/1/99/100/101, Give/Train and labor commit/rollback cases now have native captures. Source-stage statements that these await runtime are historical.

# Genghis Khan NES domestic economic commands — source completion batch

Date: 2026-11-04 UTC. Author: Zhuge Liang/Codex domestic source worker. Scope: original English NES only. This batch executes original ROM 6502 instructions for the arithmetic and state updates; it is **VERIFIED_STATIC**, not a new original-game playthrough, Windows test, or physical controller test. Root owns new Mesen validation and publication.

Private ROM SHA-256: `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`. Baseline SRAM: `evidence/roundtrip/baseline.sram`, SHA-256 `dd22847140cc0397ab8f42e29f1832681628ed52bfa17e95041c288afc4ef773`. No ROM or SRAM copied into this batch.

## Evidence and reproduction

Run from the workspace root: `python batches/domestic_source/test_domestic.py`. Final run: **27 test methods, OK, exit 0**. Exact stdout/stderr retained as `test_domestic.extended_final.*`. `slice_probes.json` contains first-visit VM register/frame/stack traces plus complete cartridge-memory byte changes for representative cases. `slice_probes_fixed.raw.log` gives results. Initial unsupported BVC CPU instruction failure is retained in `slice_probes.raw.log`; `DomesticCpu` adds standard BVC/BVS semantics, leaving original game bytes unchanged. The first complete 17-method suite also remains preserved.

`replay_domestic.py` loads original bank bytes and executes the existing v1.06 6502 interpreter harness. Inputs are synthetic frame/stack and baseline cartridge state. There is no formula substitution for the domestic routines or their arithmetic helpers. At the explicitly logged `$DB9E` random-call boundary only, selected runs inject an exogenous raw random value and resume at `$DBA4`, where the original ROM applies its requested modulus. Thus `rand(n)` below means a controlled draw in `[0,n-1]`; these tests establish the effects of each bounded draw, **not** natural PRNG sequencing/distribution or that the original archived run drew raw 11. UI-rendering/input paths are outside slice tests. Formula assertions are checked against CPU execution rather than substituted for it.

Mutation controls alter original-script operands in synthetic memory: ExtraTax multiplier10→9, gift multiplier30→20, TrainPeople cap50→49, Labor threshold65→66. Results change as expected. Archived comparison independently checks every named country field for ExtraTax1/2/9 and morale for Gold/Food1/8/9. No failures were suppressed.

## Shared fields, widths and helpers

Country1 base `C=$6DCD`; country stride83 bytes. Every field below is unsigned little-endian **word**, except eligibility byte. The country record is packed: civilians is the unaligned word at C+73.

| Offset | Field | Baseline |
|---:|---|---:|
| +20 | Arms |110|
| +22 | Skill |124|
| +24 | Troops |61|
| +26 | Economy |185|
| +28 | Town workers |5|
| +30 | Building value |115|
| +32 | Masons |4|
| +34+2i | Commodity inventory i |varies|
| +54 | Food, inventory index10 |208|
| +58 | Artisans |6|
| +62 | Food workers |27|
| +64 | Gold |439|
| +66 | Morale |118|
| +68 | ordinary tax rate |35|
| +70 | extra-tax-used |0|
| +72 | eligibility BYTE |32|
| +73 | undrafted civilians |34|
| +81 | officer pointer |$65BF|

`N=Troops+Town+Masons+Artisans+FoodWorkers` (baseline103), original helper fixed-bank `$DECD`. `N` is drafted labor, distinct from civilians; do not call it total national population. Total population for these command conservation tests is `N+civilians`.

Fixed-bank helper `$DA67`: unsigned16 arguments, zero-extended32 multiply and division, low-word result; divisor0 returns65535. `$DA8B(a,b)=floor(a*b/100)` through that helper. `$DA49=min` unsigned. `$DA99(pointer,delta)` adds as16-bit then caps at999. `$DAAD(pointer,delta)` subtracts `min(old,delta)`, thus floors at0. `$DB48(x,n,y,q)=min(999,floor((x*n+y*q)/(n+q)))` in tested nonnegative range. None of these helpers should be replaced with floating point.

## Ordinary Tax setting

Bank3 `$8A16` menu routes option1 to `$8888` and option2 to ExtraTax `$892F`. Ordinary Tax wrapper `$8824` checks nonzero drafted pool through `$83B1`, accepts a requested rate `r` bounded1..100, treats cancel or identical requested/old rate as no completed command, and calls one of:

- Lower rate, native `$8787`, VM `$878C`–`$87A9`: set `tax=r`; `Morale=min(999,oldMorale+(oldTax-r)*(rand(3)+2))`.
- Raise rate, native `$87AA`, VM `$87AF`–`$880E`: let `d=r-oldTax`, `margin=floor(leaderStat0/5)-r` interpreted signed16. If margin>50, `p=100`; else if margin>0, `p=40+rand(21)`; else `p=10+rand(11)`. Set `tax=min(100,oldTax+floor(p*d/100))`. Reduce morale by `d*(2+rand(3))`, floor0.

The requested increase is **not necessarily the resulting tax rate**. At baseline35→requested40 with both draws0, the original instructions leave tax35, but morale118→108. This is still the successful change path because the requested rate differed. At baseline35→25 with draw0, tax25 and morale138. LeaderStat0 is first word of the six-stat block selected by `$DDD6`; the helper redirects a dependent governor to the base ruler where applicable.

Completed ordinary Tax returns command code0. It changes future rate, not immediate gold. The annual collection engine is the seasonal worker's separate responsibility.

## Extra Tax, including the previously unexplained Food+1

Bank3 native `$892F`. With nonzero drafted pool and sufficient command abilities, calculate maximum requested levy:

```
maxQ = min(Economy,
           1 + floor(floor(Economy*(1+rand(100))/100)
                     * (leaderStat0+Morale+leaderStat3) / 3000))
```

The two leader stats are selected through `$DDD6`. This limit runs `$8944`–`$8981`. If maxQ=0 or the word extraTaxUsed is nonzero, branch to refusal `$8A0B`. Valid prompt range is1..maxQ; cancelling/zero input does not enter the positive action (existing zero runtime capture verifies no change). The reset season for extraTaxUsed is owned by seasonal research.

Successful update slice `$89A1`–`$8A04`, levy `q`:

1. Gold += q capped999.
2. Food += `floor(q*(1+rand(100))/100)` capped999. **This is explicit source behavior at `$89AA`–`$89BC`.** The extra resource is not an unrelated tick.
3. Set Morale to `f(E,oldMorale,q)` below.
4. Let `target=f(E,oldN,q)`. If target<oldN, reduce drafted occupations by `$88B8` until their total≤target, as described below.
5. Economy -=q, floored0. Set extraTaxUsed word to1. Return completed command1.

The exact helper native `$890F`, VM `$8914`–`$892E` is:

```
f(E,x,q) = floor(x/2)
           + floor(floor((((E-q)*x*10) mod 65536) / E) / 20)
```

Both multiply operations are **16-bit VM arithmetic**, not the wide `$DA67` helper. The wrap is necessary for faithful results. For E185, x118, q1, the result is64. A mathematically simplified unlimited-precision percentage is wrong. The tests cover multiple E/x/q bounds and an operand mutation.

Workforce reducer `$88B8` starts with a loss argument `oldN-target`, internally sets a target total, and repeatedly computes `p=floor(target*100/currentN)`. It replaces each of the five drafted occupation words with `floor(oldOccupation*p/100)` until their sum≤target. Independent flooring can undershoot target. It never credits civilian population. If the final drafted total is0, `$DF6A` clears Arms and Skill.

| Levy | Gold | Economy | Morale | Troops/Town/Masons/Food workers/Artisans | Food |
|---:|---:|---:|---:|---|---:|
|1|440|184|64|39/3/2/17/3|208|
|2|441|183|64|39/3/2/17/3|208|
|9|448|176|61|37/3/2/16/3|209|

All country fields match archived actual-ROM captures when the injected food draw is11, yielding percentage12. For levy9, draw11..21 yields the observed extra1. This does **not** identify the historical PRNG raw value; it resolves the causal source path and excludes an unexplained stock drift.

## Give Gold, Food and Item

Bank3 menu wrapper `$8AD4`; core native `$8A80`, VM `$8A85`–`$8AD3`; value helper `$8A50`. Requires nonzero drafted pool, stock>0, and quantity1..stock. Completed command4.

Gold uses special index255. Food uses index10. Item uses the country's native commodity index `BYTE[$7692+12*countryIndex]`, where countryIndex=(C-$6DCD)/83. Baseline country1 native item index is0. The third menu item does **not** invite arbitrary inventory selection. Inventory address helper `$DF05` returns C+34+2i.

```
if Gold: V=q; reduce Gold by q
else:
  reduce inventory[i] by q
  avg = floor((sellTenths(row0,i)+sellTenths(row1,i)+sellTenths(row2,i))/3)
  V = min(999, floor(q*avg/10)) + 1
moraleGain = floor(30*V/N) + 1
if country is not recognized as a player country by $DC98: moraleGain +=10
Morale = min(999, oldMorale+moraleGain)
```

The original `$DD01` seller quote is used for each row; the earlier v1.06 arithmetic is executed rather than mocked. Therefore gift commodity value depends on market Rate and all three merchant base-price rows. At Rate101, Food's average quote is18 tenths. q8 gives V15 and morale+5; Goldq8 uses V8 and morale+3. The earlier `floor(food/2)+1` fit was only a local approximation and is superseded by this source formula. All food/gold stock debits are floored0 by `$DAAD`, though the valid UI already bounds q by available stock. Morale cap999 is CPU-tested. Rate extremes beyond the earlier source bounds and nonplayer bonus were inspected but not newly runtime-accepted.

## Assign Labor

Bank2 native `$9EF3`; successful transfer commit `$9F61`–`$9F6B`. Choice1=Town,2=Masons,3=Food workers,otherwise Artisans. Compute `pair=min(999,Troops+chosenOccupation)`; prompt newOccupation in0..pair. Set `Troops=pair-newOccupation` and chosenOccupation=newOccupation. Other occupations and civilian population stay fixed. Thus in valid states with total drafted≤999, this partitions the existing pool and does not create population. Tests move each occupation to0,1 and its full pair limit.

On accepted exit, `$9F91`–`$9FC6` computes `t=floor(100*Troops/N)`. Only if t>65 **and** finalTroops>initialTroops does morale become `floor(Morale*(100-2*(t-65))/100)`. Exact equality65 causes no penalty; raising from65 to66 causes118→115 in a100-person pool. A threshold mutation66 suppresses that result.

The higher wrapper `$A3CE` copies the country record into temporary storage, operates on it, compares the resulting83-byte record with the original, and only copies it back on accepted change. Unchanged/cancelled paths return255. Runtime archive `assign_accepted` has61→57 Troops and5→9 Town, with total103 conserved. Completed Labor code2 charges abilities once. Interface staging, confirmation/cancel behavior, and final copy were inspected; the whole UI wrapper was not emulated in this batch.

## Draft and Train People

Draft bank3 native `$909D`, dilution `$905E`:

```
A = floor(civilians*eligibility/100)
maxQ = min(A,999-N)
```

The positive prompt is1..maxQ. On success q:

- For each of Arms, Skill and Morale: `new=min(999,floor((old*N+1*q)/(N+q)))` through original32-bit weighted helper `$DB48`.
- Troops +=q capped999; civilians -=q floored0.
- Eligibility byte becomes `max(0,floor(100*(A-q)/(oldCivilians-q))-rand(3))`.
- Completed command15. Total `N+civilians` is conserved for valid positive input. Worker occupations other than Troops stay unchanged. This is a different operation from Train People.

Source tests cover civilians1/34/999, eligibility0/32/50, minimum and maximum positive draft, random0/2, and cap999. Baseline Draft8 yields Troops69, civilians26, Arms102, Skill115, Morale109, eligibility7 with draw0 (5 with draw2). This result is **source replay**, not a newly completed Mesen draft; archived `people_draft_reference_v103` remains an incomplete prompt.

Train People bank2 native `$A5B0`, arithmetic/write `$A5E9`–`$A617`. Eligibility is a **byte**, capped50. If eligibility≥50, refusal returns255. With nonzero civilian population:

```
rawGain = floor((leaderStat0+leaderStat5)/600) + rand(2) + 2
factor = 100 - 6*min(5, unsigned16(difficultyWord-$0001))
gain = floor(rawGain*factor/100)
eligibility = min(50,oldEligibility+gain)
```

DifficultyWord is `$7A88`; helper `$A460` provides the factor. In tested levels1/2/6 factors100/94/70. Baseline stats89 and129, level1, draw1 produce32→35, matching the prior Train People observation. Population and all five drafted occupations stay unchanged. Return code8.

**Source edge requiring fresh runtime validation:** If civilians=0, the function displays a refusal-like message but branches to `$A62A` and returns code8, rather than255. The outer command handling may therefore consume the normal ability/order despite no eligibility improvement. The zero-population branch and return were inspected; no new original-game result is claimed.

## Ability costs and order distinction

Cost table is byte array `$7E18+6*commandCode`. Bank3 charge `$8242`, eligibility `$827B`; bank2 duplicates `$9863`/`$989C`. Charging subtracts each cost from a corresponding unsigned word, saturating0. Costs are charged through outer command processing, separately from stock/morale arithmetic; sufficient ability is checked before normal execution.

| Command | Code | Costs at six-stat offsets +0,+2,+4,+6,+8,+10 |
|---|---:|---|
|Tax rate|0|5,0,0,5,0,0|
|Extra Tax|1|10,0,0,10,0,0|
|Assign Labor|2|5,0,5,0,0,0|
|Assign Army|3|5,0,5,0,0,0|
|Give|4|0,0,5,0,0,0|
|Train Troops|5|5,0,0,0,0,0|
|Train People|8|0,0,0,5,0,0|
|Draft|15|20,0,0,10,0,10|

The stat block starts at officer+14. In the baseline these are89,77,112,61,170,129. Established labels include position0 Leadership, position2 Planning, position3 Charm; avoid assigning unverified labels to positions1/4/5. The table and original charge routine are replayed. `Assign Army` is included for cost cross-reference, not newly closed behavior. Train Troops is extended below.

Order counter `$7B36` is used as a **word** by original dispatch, even though observed values fit one byte. Existing completed Tax/Give/Labor/Train captures show one order cost; slice tests stop before outer UI dispatch and do not claim fresh order acceptance. Gift source debits and morale can happen before acknowledgment/order decrement, as prior incomplete capture already demonstrated.

## Remaining genuine unknowns and integration cautions

- Natural PRNG sequencing and distribution; raw11 in tests is a controlled input, not a recovered historical seed.
- New Mesen acceptance for ordinary rate raise/lower, high morale and cap boundaries, positive Draft, labor penalty boundary, TrainPeople50%/zero-civilian cases, native Item gift identity and nonplayer gift bonus.
- Whole-UI cancellation/rollback and complete order charging for every branch are inspected but only prior archive cases have runtime evidence.
- Cross-country/scenario/state reachability outside the tested valid bounds; synthetic corrupted over-cap states are not accepted campaign rules. The source's16-bit operations must still be retained wherever explicitly shown.
- Annual rate collection, extra-tax reset, growth, disasters, harvest/payroll are separate seasonal work. Trade identity and price generation are separate trade work.

These are bounded source closures and runtime-test targets. They do not prove full economic acceptance, a Windows implementation, all167 obligations, or delivery to an outside agent. Full investigative disassembly text is private working material; publish the authored report/scripts/logs/trace outcomes only after root review, without ROM, SRAM, or bulk extracted source.

## Extension requested by root: Train Troops and population ledger

**Train Troops** native bank2 `$A4E4`. It checks command5 ability and nonzero drafted pool (not specifically nonzero Troops). Update slice `$A4F9`–`$A50D` writes the persistent country Skill word C+22 directly, then UI/reporting and completed code5 follow. Gain is:

```
raw = 15 + rand(11) + floor((leaderStat0+leaderStat3+leaderStat5)/40)
sizeAdjusted = floor(999*raw/(N+999))
factor = 100-4*min(5,unsigned16(difficultyWord-1))
gain = floor(sizeAdjusted*factor/100)
Skill = min(999,oldSkill+gain)
```

`raw` is bank2 `$A28A`, including dependent-governor/base-ruler resolution; size adjustment `$A2BE`; difficulty factor `$A47D`. Legal player bounds N≤999 and stats≤999 avoid word overflow in the displayed sum. Baseline raw21 with draw0 yields19 points, Skill124→143. Factor is100 at difficulty1,80 at difficulty6. Tests vary N, difficulty, RNG and Skill0/124/998/999. At999 the arithmetic stays999 but still returns completed code5; runtime order/ability charging at cap should be checked. Five repeated source updates carry forward actual Skill and deduct the5 Leadership before the next raw calculation. Exact cartridge-byte changes establish that only the Skill word changes inside the update slice; Gold, Food, population, Arms and Morale are untouched there. Leadership/order charging occurs outside it.

Persistence is not permanent value retention across seasons. Bank1 `$83EA` iterates countries and applies93% to Skill using leaf `$8089`. A source chain of Train→that decay leaf gives124→143→132. This proves the decay calculation and its field; **the seasonal worker/root must establish actual call cadence**. No headless slice substitutes for save/cold-load persistence testing.

Population ledger, with exact country1 addresses:

| Quantity | Storage / derivation | Draft8 example |
|---|---|---|
|Troops|word `$6DE5`|61→69|
|Town|word `$6DE9`|5→5|
|Masons|word `$6DED`|4→4|
|Artisans|word `$6E07`|6→6|
|Food workers|word `$6E0B`|27→27|
|Drafted N|sum of the five words; `$DECD`|103→111|
|Undrafted civilians C|unaligned word `$6E16`|34→26|
|Eligibility percentage|byte `$6E15`|32→7 with draw0|
|Eligible undrafted headcount|derived `floor(C*eligibility/100)`; local in Draft, no separate persistent word identified|10 before;1 after|
|Total resident population for these commands|derived N+C; no separate persistent total identified|137→137|

The eligible headcount is recalculated from a rounded percentage after drafting, so it need not equal oldEligible−q exactly: baseline10−8=2, but newpercentage7% of26 rounds to1. This extra rounding loss is faithful source behavior; the people themselves remain in the civilian count. Random decrement0..2 further changes the percentage, not civilian headcount. Assign changes only drafted partition, TrainPeople only eligibility, TrainTroops only Skill, Give only stock/Morale. Emergency Tax destroys drafted headcount without adding civilians, so total residents drop by the actual rounded occupation losses (baseline137→98 at q1, 137→95 at q9).

The requested economic cost subset now tests **every** listed code0/1/2/3/4/5/6/7/8/9/15/22/23: exact-cost sufficiency, each nonzero requirement one point short, charge-at-zero saturation, and exact-cost depletion. Tests enter the original eligibility/charge routines, stopping before the insufficient-ability message renderer. There are no resource writes in the gate paths. An added table-byte mutation changes Give Planning debit5→6 and the replay detects107→106.

Additional cost rows:

| Command | Code | Six position costs |
|---|---:|---|
|Train Prince|6|0,0,0,5,0,0|
|Train Self|7|0,0,0,0,0,0|
|Trade (buy/sell/hire)|9|0,5,0,0,0,0|
|Move People|22|10,0,0,10,0,0|
|Move Gold/Food/Item|23|5,5,5,0,0,0|

`command_cost_matrix.csv` preserves all32 inspected rows plus known command identities and economic-test scope. Unnamed rows are explicitly unmapped; the matrix does not claim other command behavior is accepted.

## Exact suggested original-game inputs for root

These begin at the archived cold-start frame3615 with Tax selected. Each press in the archived path held5 frames and released50 frames. Navigation must be checked against the actual heading if the current session has a different cursor position.

- **Train People**, historically completed: Down×3, A, Down×3, A, wait200 frames, A acknowledgment, wait180. Archived result32→35 eligibility,3→2 orders, Charm61→56. For a cap boundary, explicitly intervene eligibility49, preserving civilians>0 and Charm≥5, then repeat and observe50. Eligibility50 should refuse. Zero-civilian behavior should be tested separately because of the code8 return described above.
- **Draft**, historically reached prompt: Down×5, A, Down×5, A, wait100. Archive screenshot says `Draft how many Soldiers (1-10)? 0`. The old incomplete probe pressed A while the number stayed0. Use **Up once**, verify displayed1, then A and acknowledge the result. Numeric Up→1 is independently proven by prior GiveFood1/ExtraTax1. For8, Down×2 should give8 using the Gold8-proven digit wrap, but verify the displayed quantity before confirming. Baseline max10 derives from floor34×32/100. A valid positive result must show Troops↑, civilians↓, quality dilution, eligibility recomputation, cost[20,0,0,10,0,10], and order decrement.
- **Train Troops**, first Train option: Down×3,A,A, then acknowledge result. Record Skill and Leadership before/after; subsequent seasonal comparison must use a matched untrained control because the Skill decay path exists.

The source lower bound for ordinary Tax is **1**, not0: `$8824` invokes fixed `$D3CC`; `$D3D2` pushes literal1 as numeric lower bound. This resolves the prior manual1 versus master-paraphrase0 dispute. The lowering leaf itself can be synthetically called with0, but that is not evidence of a legal UI choice; final tests use legal1/10.
