> This is the preserved source-batch explanation. Its original runtime targets and stage-specific status are resolved or explicitly bounded by the v1.07 master and final acceptance register; these source tests are not relabelled as Mesen playthroughs.

# Economic dependencies — AI, unrest, and population ledgers

Author: Zhuge Liang / Codex source-research subagent. Date2026-11-04 UTC. This extends `ANNUAL_ENGINE_SOURCE_REPORT.md` following the root agent's request to finish the remaining source dependencies. The earlier report's unreplayed major-epidemic economics, summer AI draft, and AI commodity supplement now have original-instruction replay evidence below.

**Evidence: VERIFIED_STATIC, original6502/VM/callees/native RNG executed in synthetic state.** The suite does not launch Mesen, play battles, validate Windows, or certify natural event frequencies. Source branches into full UI/battle sequences are identified as boundaries, not silently stubbed. Original-ROM runtime corroboration belongs to the root agent.

## Per-country turn order and seasonal connection

Physical bank0 native `$86CD`, VM `$86D2`, processes the shuffled country order from `$7B3A` using `$7B38` (turn slot) and `$7B34` (country index). Unless a separate mode flag`$63EC` bypasses these checks, the source order for each country is:

1. Popular revolt check and possible battle (`$8689`, predicate `$8645`).
2. Vassal/prince revolt processing (`$8577`), unless the prior check already started an event.
3. Provision theft (`$8295`).
4. Low-Morale warning (`$8346`) only when theft did not occur.
5. Set `$7B70` to human-ownership test. Human countries without delegated status101 enter the player command path; other countries enter policy/AI processing through `$8262`.
6. Fixed-bank `$DF6A` clears Arms/Skill if the allocated total is0.
7. Advance the turn slot after event/termination checks.

The bank0 dispatcher `$8003` argument1 invokes native service7 with bank1. Original service7 at fixed `$C407` saves the old PRG-bank selector, switches using `$C5F0`, calls CPU`$8000`, then restores the bank. Physical bank1 starts `JMP $9130`. Its VM `$9135` selects `$9022` when global mode`$6058=0`; this is the common seasonal engine described in the core report. Other mode values take alternate setup/battle-related paths. This resolves the physical-bank connection; it does not claim a complete console run from that static call graph.

## Revolt, theft, and warning thresholds

Let `rnd(n)` be the exact native RNG modulo helper from the core report. All draws below advance RNG in the displayed order except where short-circuit control flow skips them.

### Popular revolt

Bank0 native `$8645`, VM`$864A-$8688`, returns1 when either:

```
Tax > 50 + rnd(210 - 10*difficulty)
```

or, if that comparison did not trigger:

```
Morale + rnd(101) < 25 + difficulty + rnd(16)
```

The source draws the right-side rnd16 **before** rnd101. At equality there is no revolt. High Tax can trigger independently of Morale. The source tests sweep Tax0,1,49,50,51,99,100; Morale0,1,25,40,50,99,100,999; difficulty1,5; and two seeds, checking both return and final RNG state. A threshold-operand mutation changes outcomes. Synthetic extreme settings are not asserted naturally reachable.

On trigger, `$8689` displays civil war and constructs a battle using `$85BB`. The generated rebel troop amount is `floor(oldTroops*(20+rnd(41))/100)`, then+6 if nonzero; if zero, use10+rnd11. If the entire allocated total is0, the code also inserts Foodworkers5..10 and Town/Masons/Artisans1..3 and clears reserve. Battle Food is `floor(rebelTroops*(24+rnd(7))/30)`. Those setup expressions are decoded source; the battle's eventual ownership/resource consequences require battle-runtime evidence and are not replaced with a guessed flat unrest penalty.

### Person-driven revolt

Bank0 `$83FD` tests a candidate person's byte+1A and the average of the six16-bit attributes at person+0E..+18 (`$DF96`). The leader is found through the original same-owner country search `$DF11`; numerical/personality labels are kept literal because this batch does not establish the UI naming crosswalk for byte+1A.

- Byte0: no revolt and no random draw.
- Byte2: draw t=rnd51. If candidateAverage>leaderAverage+t, return rnd100<3; otherwise return rnd101==0.
- Other nonzero byte (tested1): draw t=50+rnd101. If candidateAverage>leaderAverage+t, return rnd100<1; otherwise return rnd501==0.

These exact branches and RNG state are replayed. The surrounding `$845E` only considers delegated-status101 lords; human-owned delegated countries use this candidate predicate, while AI delegated countries instead have a1/251 branch. `$84EF` selects eligible human-family candidates from five records; `$8577` invokes the same predicate for a selected candidate. Full family selection, civil-war UI, and battle resolution remain outside the economic predicate replay. They are explicitly separate from popular Tax/Morale revolt.

### Provision theft

Bank0 `$8295`, VM`$829A-$8345`, reduces **Food**, not Gold or production history. Let `P` be allocated total, `F`Food, `M`Morale, `T`trained-resident byte, `d`difficulty.

```
if floor(M*T/100) + rnd(201) > 16:
    if floor(F/90) < floor(P/90) + rnd(176-5*d):
        return no theft
if rnd(100) > 70+6*d:
    return no theft
F = floor(F*(55+rnd(21))/100)
F = max(0, F-(5+rnd(11)))
return theft
```

This is a two-stage risk test: stock relative to allocated population also matters, so a single Morale cutoff is not an equivalent rule. Difficulty1 gives the last gate successes0..76 inclusive. Tests include positive and negative seeds, Food0/1/5/100/999, population0..999, trained0/50, difficulty1/5, exact final RNG state and unaffected Gold/history controls. The text pointer`$B21C` identifies stolen provisions; filenames/test labels do not supply that identity.

Low-Morale warning `$8346` draws `40+rnd31` and shows the uproar message only if Morale is **below** that threshold and the country is eligible for player-facing messages. This warning routine itself makes no economic-field write.

## AI economic actions

Physical bank2 native `$8481` is the autonomous country-turn path. Its economic ordering is army formation when required, Tax adjustment, a separate espionage/political path when `$7B70=0`, labor allocation, then conditional Hire/Give/weapon purchasing. Ruler relocation/diplomacy/training branches follow. Delegated policy path `$80B7` uses an explicit desired Tax byte and independent policy flags to select Hire/Give/weapons, then diplomacy/transfers/training. Those political and battle choices are not recast as economic formulas.

### Labor allocation conserves allocated population

Bank2 `$8172`, VM`$8177-$8226`, takes old total P, overwrites four worker groups with floors of these percentages, then assigns the exact remainder to Troops:

| Row | Town | Masons | Foodworkers | Artisans | Selection |
|---:|---:|---:|---:|---:|---|
|0|5%|1%|8%|1%|Special condition below|
|1|20%|5%|30%|5%|Economy<300|
|2|18%|7%|30%|5%|Economy300..499|
|3|15%|8%|30%|7%|Economy≥500|

Special row0 requires rnd5==0, Gold>150, P>150, season!=Winter, and`$7B70=0`. Every call draws rnd5 before the other short-circuit tests. The routine returns0 for row0,1 otherwise; main AI invokes Hire/Give/weapons only on return1. Floors plus the troop remainder preserve P exactly. Arms and Skill are not recalculated by this allocation routine. Tests cover all thresholds, tiny/zero totals, P999, both source branches and a human-flag negative control.

### Tax adjustment and free Winter reset

Bank2 `$828F`:

- If Winter,`$7B70=0`, and P>600: directly set Tax=50+rnd6 and return. **No Morale reduction** in this branch.
- Otherwise Tax35..50 inclusive is left unchanged with no draw.
- Outside that band, choose desiredTax=38+rnd6 and call the increase/decrease routine. Delegated policy may instead request a different desired Tax through `$8003`.

Bank2 `$A1DB` lowering: set Tax to desired, add `(old-desired)*(2+rnd3)` Morale capped999.

Bank2 `$A1FE` increasing: let delta=desired-old and `gap=floor(attribute0/5)-desired`, where attribute0 is the first lord16-bit attribute at person+0E (the test variable named `judge` is an operator label, not independent proof of its UI name).

- If signed gap>50: responsePct100.
- Else if gap>0: responsePct40+rnd21.
- Else responsePct10+rnd11.

Set `Tax=min(100,old+floor(delta*responsePct/100))`. Subtract `delta*(2+rnd3)` Morale, saturated0. The Morale cost uses the **full requested delta**, even if rounding/poor response barely changes Tax. Tested example attribute0=0, Tax0→requested40 gives actualTax4 and Morale100→0 for the recorded seed.

Player Tax helpers in bank3 duplicate this logic but are owned by the command-source batch; this report does not replace that UI/ability proof.

### Hiring

Bank2 `$8A98` acts only when Gold>2*Troops. It uses the actual hire quote `priceTenths=floor(60*(Rate+100)/100)` and hires:

```
q = floor(min(999-P, floor(10*Gold/priceTenths))/2)
```

If q>0, Arms and Skill are weighted against newcomer value100 using **old allocated total P**, not just old Troops. Morale uses newcomer value50+rnd11. For each value v, `newV=floor((P*v+q*newcomerV)/(P+q))`, capped999 by the common helper. Then Troops+=q and `Gold=floor((10*Gold-q*priceTenths)/10)`.

Recorded source vector: P100 (Troops61,Town39), Gold439, Rate101, Arms110, Skill124, Morale118 → hire18, Troops79, Gold223, Arms108, Skill120, Morale107. This is an AI source vector; it must not overwrite the separately observed player Hire8 experiment.

### Weapon purchase

Bank2 `$8B29` requires P>10 and Arms<300 for status100 leaders, Arms<200 otherwise. It requests buyer row0,column14 from the actual fixed quote routine. Let `w=byte[$7DF4]` (kept as the exact table input, independently varying from the quoted column):

```
maxUnits = min(999, floor(P*(999-Arms)/w))
q = floor(min(maxUnits, floor(10*Gold/quoteTenths))/2)
if q>0:
    Arms=min(999,Arms+floor(w*9*(q+1)/P))
    Gold=floor((10*Gold-q*quoteTenths)/10)
```

The `q+1` and separate w input are literal source behavior. No claim that all AI merchants/items are chosen symmetrically follows. A synthetic table test with row0,column14 base40, w60, Rate100, P100, Arms100, Gold999 gives q62, Arms440, Gold503. Source names for the six weapon columns remain the trade batch's responsibility.

### Gifts and AI-only bonus

Bank2 `$8AE4` acts when Morale<150, selects the largest of ten commodity stocks (earliest index wins ties), and if that stock is positive and at least floor(P*10/100) with P>0, gives the **whole stock** via `$A187`.

For a commodity gift, compute each merchant's original seller quote; average the three quoteTenths by integer division3; `value=min(999,floor(averageTenths*quantity/10))+1`. For Gold gift index255, value=quantity and Gold is decremented. Then:

```
MoraleGain = floor(30*value/P)+1
if country is not human-owned: MoraleGain += 10
Morale=min(999,Morale+MoraleGain)
```

The explicit extra10 is verified by paired human/AI synthetic ownership tests. The numeric gift effects are separate from the player command's officer/order costs.

### Free training branch

Bank2 `$8A38`:

- If Troops>0 and Skill<350, Skill gains `floor(999*g/(P+999))`, where `g=floor((attribute0+attribute3+attribute5)/40)+rnd11+15`; cap999.
- If the floor-average of all six lord attributes is<150, select the lowest attribute (first on ties), add `floor((attribute1+attribute2+attribute4)/40)+rnd11+15`, capped999.

This routine makes no Gold debit. Original-source tests verify Skill/attribute outputs and unchanged Gold. Political/strength comparisons decide whether a country reaches this routine; those outer decisions are not treated as random-free guaranteed training every turn.

### Subsidies, Food conversion, and summer draft

Core report tests verify the separate annual Gold subsidy, automatic AI Food conversion, and human exclusions. Added replay closes the summer commodity supplement `$81B7`: with ten-stock total≤349 add50+rnd51 to index rnd10, cap999; total≥350 returns before any RNG use.

Summer AI draft gate `$84B6` is restricted to non-human countries. Morale must exceed140+rnd41, and eligibleReserve must exceed floor(P*30/100). It calls `$844E` with `q=min(999-P,floor(P*30/100))`.

The draft effect weights Arms/Skill/Morale against newcomer value**1** using allocated total P, adds q Troops, subtracts q from reserve, and writes:

```
trainedByte = floor(max(0,oldEligible-q)/(oldReserve-q))
```

There is **no multiplication by100** in this AI routine. It usually writes0. This is executed literal source behavior, not a suggested repair. Example P100,reserve500,trained50: q30; Troops130,reserve470,trained0; Arms110→84,Skill124→95,Morale118→91. The player Draft formula below is different.

## Resident reserve, eligibility, and Extra Tax aftermath

There are two separate population stores:

- Allocated total P: sum of five16-bit groups; resident reserve is excluded.
- Resident reserve R at country+49 and trained percentage T(byte)+48. Nominal eligible count E=floor(R*T/100).

Player Draft's accepted headroom is `min(E,999-P)` (`bank3 $90A2-$90C2`). After accepted quantity q, the source effect `$90EA-$9124`:

1. Weights Arms/Skill/Morale against newcomer value1 using P.
2. Adds q Troops.
3. Subtracts q reserve.
4. Sets trainedByte=`max(0,floor((E-q)*100/(R-q))-rnd3)`.

The player and AI draft rules are therefore not interchangeable. Source tests execute the effect boundary after selection, with original callees and RNG, and avoid pretending to test the UI confirmation or ability gate.

Extra Tax's population-loss helper (bank3 `$88B8`) calculates target=P-requestedLoss; while currentP>target it calculates floor(target*100/currentP) and floors each group's percentage separately. Rounding can remove more people than the requestedLoss. It does **not** increase resident reserve or trained percentage. The common `$DF6A` call afterward clears Arms/Skill only when P0; it does not touch reserve.

Recorded source control starts allocated groups[700,100,50,50,90] (P990), reserve500,trained50, requestedLoss100. It ends[623,89,44,44,80] (P880), reserve500,trained50 unchanged. The displayed Draft headroom rises9→119 because `999-P` increased, despite no resident restoration. This explains how a derived eligibility display can appear to recover after a levy.

The command-source batch owns levy amount selection and its immediate Economy/Morale/Gold/Food formulas. Direct source inspection additionally shows the levy credits Food with `floor(quantity*(1+rnd100)/100)` at bank3`$89AA-$89BC`, then decreases Economy, sets+46=1, and consumes the command. These state changes have ordinary later consequences through the already mapped food upkeep, revenue, growth headroom, theft and revolt predicates. No extra hidden delayed-levy penalty variable was found in the inspected levy/annual paths; this is a scoped finding, not a whole-ROM absence proof. The explicit+46 permission word clears at Autumn harvest.

## Acceptance evidence and boundary

Run:

```
GK_ROM='/private/Genghis Khan (USA).nes' python test_dependencies.py
```

Final extension:24 test methods, all passed. `dependency_source_replays.json`:73 selected verbose source records;816,949 recorded original CPU instructions. The core suite contributes26 methods and241 records. Combined source methods50; combined selected records314; selected instruction count2,976,781. Additional grids run without verbose records. Counts are not original-game experiment counts.

Retained logs: `dependencies_initial.*` (20-method pass), `dependencies_expanded.*` (24-method pass); core logs are likewise retained. No failure was hidden or relabeled. The compiler-free harness uses no UI/action substitutes, RNG stubs, or output-formula shortcuts; expected formulas are assertions against authentic instruction execution. The only deliberate source mutations are named negative controls.

Boundaries requiring other evidence: full Mesen trajectories in both scenarios; temporal AI choices and political/battle resolution; actual menu semantics and command failure/cancel/ability costs; table/name/icon crosswalks; physical Xbox/Windows acceptance. The user authorized the complete economic research, and root should integrate these source results with runtime and other batches before making any broader completion claim.
