> This is the preserved source-batch explanation. Its original runtime targets and stage-specific status are resolved or explicitly bounded by the v1.07 master and final acceptance register; these source tests are not relabelled as Mesen playthroughs.

# Genghis Khan NES annual economic engine — source completion batch

Author: Zhuge Liang / Codex source-research subagent. Date: 2026-11-04 UTC.
Owner of original-game runtime corroboration: root research agent. Scope: annual/seasonal economic engine; player Tax/Give/Assign/Draft commands belong to a separate batch.

**Result: the assigned economic calculations and their source order are mapped and original-instruction replays pass. Evidence class: VERIFIED_STATIC.** These are synthetic-memory executions of the private original ROM's 6502 instructions, including native RNG, VM interpreter, nested arithmetic functions, and loops. They are not Mesen executions, gameplay acceptance, Windows-port tests, or proof that all synthetic input combinations are naturally reachable. Root owns the independent runtime comparison and obligation dispositions.

ROM SHA-256: `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`.

## Exact source map

All switchable addresses below are physical PRG bank **1** mapped at CPU `$8000-$BFFF`, unless explicitly marked fixed bank15. A native VM function has a five-byte prologue; VM entries are native entry+5. File offset for bank1 = 16+$4000+(CPU-$8000). VM dispatch runs original fixed-bank instructions at `$EFC9`.

| Behavior | Native entry | Relevant VM source / stop boundary |
|---|---:|---|
| Common end-season engine | `$9022` | `$9027-$90D9`; economic accumulation `$9034-$9079` |
| Season increment and Rate | `$8EAF` | `$8EB4-$8F10`; stop `$8F11` before display calls |
| Spring pipeline | `$8CD4` | Food then tax/payroll calls at `$8D44-$8D4D`; later annual callbacks `$8E2B-$8E3C` |
| Food upkeep / starvation | `$8099` | `$809E-$8104`; stop `$8105` |
| Tax receipts and payroll | `$8106` | `$810B-$8167`; stop `$8168` |
| Annual tax-dependent Morale | `$8989` | `$898E-$89BF`; stop `$89C0` |
| Annual resident-training percentage | `$8003` | `$8008-$8062`; stop `$8063` |
| Allocated-population growth and resident reserve | `$8A23` | `$8A28-$8AD6`; stop `$8AD7` |
| Summer pipeline | `$8E3E` | Skill decay, storm, AI subsidies, possible AI draft |
| Skill depreciation | `$83EA` | `$83EF-$840D`; stop `$840E` |
| Autumn harvest | `$8E50` | `$8E55-$8E9B`; stop `$8E9C` before sickness call |
| Winter pipeline | `$8EA0` | Arms decay, cold wave, AI subsidies |
| Arms depreciation | `$83C5` | `$83CA-$83E8`; stop `$83E9` |
| AI automatic Food purchase | `$82B1` | `$82B6-$832A`; stop `$832B` |
| AI seasonal subsidy | `$81FA` | `$81FF-$8299`; stop `$829A`; early no-human return `$8207` |
| Global merchant offer-mask refresh | `$832C` | `$8331-$83C3`; stop `$83C4` |
| Country merchant status flags | within `$9022` | `$907A-$90CA`; stop `$90CB` |
| Random modulo wrapper | fixed15 `$DB93` | VM `$DB98-$DBA6`; native service17 reaches `$C289-$C2EB` |
| Percent helper | fixed15 `$DA8B` | `floor(a*b/100)` through `$DA67` |
| Add with 999 cap | fixed15 `$DA99` | Read word, add, unsigned min999, write |
| Saturating subtraction | fixed15 `$DAAD` | Subtract min(request,current) |
| Percent reduction | fixed15 `$DAC2` | Subtract `floor(current*pct/100)` |
| Allocated total | fixed15 `$DECD` | Sum of Troops, Town, Masons, Artisans, Foodworkers |
| Country specialty index | fixed15 `$DEF3` | `byte[$7692+12*countryIndex]`, index from `(countryBase-$6DCD)/83` |

## Country fields confirmed by executable access

Country index0 base `$6DCD`, stride83 decimal (`$53`). Words are little-endian. Existing v1.06 runtime labels tie named fields to the original game's panel/transactions; this batch confirms relative accesses and formulas.

| Offset hex | Country1 address | Meaning in these routines |
|---:|---:|---|
| 14 | 6DE1 | Arms |
| 16 | 6DE3 | Skill |
| 18 | 6DE5 | Troops |
| 1A | 6DE7 | Economy |
| 1C | 6DE9 | Town workers |
| 1E | 6DEB | Defense |
| 20 | 6DED | Masons |
| 22..34 | 6DEF..6E01 | Ten commodity stock words |
| 36 | 6E03 | Food stock |
| 38 | 6E05 | Accumulated commodity production history |
| 3A | 6E07 | Artisans |
| 3C | 6E09 | Accumulated Food production history |
| 3E | 6E0B | Foodworkers |
| 40 | 6E0D | Gold |
| 42 | 6E0F | Morale |
| 44 | 6E11 | Ordinary Tax percentage |
| 46 | 6E13 | Extra-tax-use word; cleared at Autumn harvest |
| 48 | 6E15 | Trained-resident percentage, **one byte** |
| 49 | 6E16 | Resident reserve word, separate from five allocated groups |
| 4B,4D,4F | 6E18,6E1A,6E1C | Three merchant-status words |
| 51 | 6E1E | Lord/person-record pointer |

Global `$65B9` is country count, `$65BB` year, `$65BD` season (0 Spring,1 Summer,2 Autumn,3 Winter); `$7A88` secondary/difficulty word; `$7A8A` common market Rate. Root independently reports baseline `$7A88=1`.

## Operation order and economic formulas

Notation: `floor` is integer truncation for the nonnegative, normal gameplay values discussed here. Basic VM word arithmetic wraps at16bits; tests use bounded values0..999 unless noted. The percent helpers zero-extend operands and use32-bit multiplication/division, preserving intermediate precision. Add-cap helper first adds as a16-bit word then takes min999; invalid huge synthetic inputs can wrap and are not claimed to be valid gameplay.

### Common work before the season changes

At `$9022`, after a message/event-display function, every country receives in this order:

1. `foodHistory = min(999, foodHistory + foodworkers)`.
2. `commodityHistory = min(999, commodityHistory + artisans)`.
3. `economy = min(999, economy - floor(economy*4/100) + town)`.
4. `defense = min(999, defense - floor(defense*4/100) + masons)`.
5. Recompute three merchant status words from updated Economy and current Morale.

After all countries, global merchant offer masks refresh. Then `$8EAF` increments the season, increments the year only when new season0, changes Rate, invokes display/date routines, dispatches seasonal effects, and finally performs AI-only automatic Food purchases. Thus the immediately completed season contributes its assigned workers **before** an Autumn harvest and before Spring food/tax.

The mapped dispatcher does not itself establish every preceding AI action or user Pass command. Root's runtime trace must connect that outer call path. No full-year gameplay claim is made by this source batch alone.

### Rate and actual RNG

Native RNG state is big-endian48-bit bytes `$84..$89`. Each native service17 draw computes:

```
s' = -((s | 1) * 0xE0900E0187C5) mod 2^48
random15 = (s' >> 32) & 0x7FFF
DB93(n) = random15 % n
```

`DB93(0)=0` without drawing/changing the seed. The original source's six-byte multiplication/negation instructions and full interrupt-service wrapper execute in the harness; no returned-RNG stub is used. The formula describes a draw given its incoming state, not an assertion that console timing cannot change incoming state elsewhere.

For new Spring, `Rate = 50 + DB93(94 + 7*difficulty)`. With baseline difficulty1 this yields50..150 inclusive. Source tests use secondary0,1,2,3,4,5,10; this does not establish the UI's accepted difficulty range.

For Summer, Autumn, Winter:

```
p = DB93(9) + 1                        # 1..9
change = floor(oldRate * p / 100)
ignoredDirection = DB93(1)             # ALWAYS 0, but advances RNG
Rate = max(50, oldRate - change)
```

An increase-to-max150 branch exists but is unreachable under the actual modulo1 call. Changing just that source operand to2 allows increases in the negative-control test. Seasonal Rate therefore falls or holds; it does not make a symmetric random walk. The earlier bank2 `$91D0` half-Rate expression is unrelated to this generator.

### Spring Food first, then tax and payroll

Let `P = troops + town + masons + artisans + foodworkers` and `F=Food`.

- If `P <= 2*F`, subtract `floor(P/2)` Food, preserve the five groups.
- While `P > 2*F`, compute `q=floor(200*F/P)`, replace **each group independently** with `floor(group*q/100)`, and recompute P. After any shortage/scaling, finish with Food0. This loop and the round-down of each group matter: replace neither with a single total-population subtraction nor a proportional floating-point result.
- Then compute taxes from the **current** Economy, Tax and Morale (after any earlier sickness). Payroll uses the **post-food-shortage** Troops.

```
a = floor(economy * tax / 100)
f = 12 + min(13, floor(morale/9))
grossTax = floor(a * f / 25)
availableGold = oldGold + grossTax
survivingTroops = min(oldTroops, availableGold)
newGold = min(999, availableGold - survivingTroops)
```

If Gold cannot pay one Gold per troop, the troop count falls to availableGold and Gold becomes0. Gross receipts are added before payroll and the999 cap is applied afterward. Ruler attributes and the other worker groups are not inputs to this isolated tax/payroll routine; negative controls vary them and preserve the result. Their indirect effects through Economy, Morale, or earlier events remain real.

Later in the Spring pipeline, after per-country ruler/death processing and ancillary callbacks, annual Tax-dependent Morale reduction occurs:

```
pct = 2 if tax < 50 else tax - 48
morale = morale - floor(morale*pct/100)
```

This happens **after** that Spring's revenue calculation. Even Tax0 has the annual2% reduction. It is separate from the immediate Morale effect of changing the Tax setting.

### Spring population and resident reserve

After annual Morale and trained-percentage update, the five allocated groups grow.

For the five-group total `P=min(999,total)`:

- `P<20`: add1 specifically to Troops.
- Otherwise, `gain=min(999,floor(P*(103+bonus)/100))-P`.
- Human-owned countries: bonus0.
- AI countries with a lord marked status100 (leader), or a player-owned neighboring country: `bonus=min(3,difficulty)`; other AI countries: bonus0. The source ownership and neighbor functions execute in the controls; synthetic neighbor terminator is used for the no-neighbor test.
- Add `gain//5` to every group. Distribute `gain%5` leftovers in order **Troops, Masons, Town, Artisans**. Foodworkers receive no leftover unit.

Separately, `residentReserve=min(999,residentReserve+floor(residentReserve*5/100)+1)`. This reserve is not included in P.

Before that reserve increase, the one-byte trained-resident percentage updates. If old reserve<20, it becomes `20+DB93(11)`. Otherwise:

```
a = floor(reserve*(oldTrainedPercent+3)/100)+1
b = floor(reserve*105/100)+1
newTrainedPercent = min(50, floor(100*a/b))
```

Full original loops run for both14-country and27-country configurations in tests, with synthetic records and a relocated harness frame to avoid corrupting country memory.

### Autumn harvest and history

Each country performs:

```
Food = min(999, Food + foodHistory)
i = byte[$7692 + 12*countryIndex]
commodity[i] = min(999, commodity[i] + commodityHistory)
foodHistory = 0
commodityHistory = 0
extraTaxUsed = 0
```

This is a **sum of accumulated per-season worker counts**, not a direct division-by-four of final staffing. Histories themselves are capped999 each season. Four identical staffing values25 produce history100; histories10,20,30,40 also produce100;0,0,0,40 produces40 despite the same final staffing40. All three execute the original accumulation and harvest code and produce the expected distinct outputs.

The routine does not clear current Artisans or Foodworkers. Adjacent non-specialty goods stay unchanged. Existing Food/goods carry over until999 saturation. Root's item-name crosswalk must map specialty indices to names; this batch establishes the exact index/address mechanism.

**Register correction:** the existing assumption that Extra Tax permission resets in Spring is contradicted by this reset at Autumn harvest. Player command checking of this word belongs to the command-source batch; source of the reset itself is verified here.

### Summer, Winter and adverse-event effects

Summer first sets `Skill=floor(Skill*93/100)` (different from subtracting floor7%, by one for nonmultiples). Winter first sets `Arms=floor(Arms*97/100)`.

Storm and cold effects use `D=DB93(6)+(humanOwned ? floor(3*difficulty/2) : 0)`, separately drawn for each D call. Reduction `reduce(x,p)=x-floor(x*p/100)`.

- Summer storm: Defense reduces by25+D%; Economy by a separate25+D%; resident reserve and trained percentage are each multiplied by separately drawn80..90% and floored.
- Winter cold: both production histories become0; Economy reduces by20+D%; reserve/trained each multiply by independently drawn80..90%.
- Ordinary sickness (called in Spring and after Autumn harvest): Economy reduces10+D%, Morale20+D%, and all five allocated groups15+D% (one common group reduction percentage); then the reserve/trained80..90% reductions.
- Major epidemic effect source `$8785`: Morale reduces45..55%; all five allocated groups reduce by a separately drawn45..55%; reserve and trained percentage become0; Economy reduces by a further independent45..55%. Ruler/family effects accompany this path. The extension report now includes an original-instruction replay of the bounded economic effect; family/UI interactions remain outside that replay.

Event-selection gates inspected: summer storms are bypassed in the non27-country scenario; otherwise a60% global gate, region-table eligibility, then5% per eligible country. Winter cold uses60% global gate and5% per eligible country; the non27-country scenario bypasses the region-table restriction. Ordinary sickness uses80% global gate and5% per country. The special major-epidemic attempt applies only when year differs from1176, year is divisible by12, season0, and5% draw succeeds; it replaces ordinary sickness for that call. These gate descriptions are VERIFIED_STATIC decoding, not event-frequency observations, and the display branches are not reproduced as console gameplay.

### Explicit AI economic assistance

The source has AI-only economic support; this must not be mistaken for player output.

Automatic Food purchase after every seasonal dispatcher is skipped for human-owned countries. If `DB93(100)<20*difficulty`, targetFood=`floor(troops*150/100)`. With positive deficit and positive `surplusGold=gold-floor(troops/2)`, buy `q=min(deficit,2*surplusGold)` Food, capped999; subtract `floor(q/2)` Gold. This fixed conversion does not consult Rate or the merchant price table. All gates and boundary cases execute in source tests.

Subsidy function `$81FA` exits when there are no active human slots. Let H be total human-owned countries and A active human slots. Called with parameter k=0 in Spring, k=3 in Summer and Winter:

- 14-country scenario: `base=floor(H/2)+k+2`.
- 27-country scenario: `base=floor(floor(H/A)/3)+k+3`.
- Each AI country gains `(DB93(3)+1)*base*difficulty` Gold, capped999. Human countries are skipped.

Both whole-country loops execute in tests. Summer also calls an AI commodity supplement: if the sum of ten goods is at most349, add50..100 to a randomly selected index0..9, capped999 (`$81B7`). The extension report replays this supplement and the summer AI draft gate/effect at `$84B6`/`$844E`. These are AI assistance, separate from player production; neither report claims a complete political/battle AI model.

### Merchant refresh exact expressions

For each merchant m=0..2, `$907A-$90CB` sets country word+4B+2m to:

```
(DB93(100) > threshold)
threshold = word[bank1:$B1C6 +54*m +(N==27 ? 2*remainingCount :0)]
            + floor(updatedEconomy*Morale/20000)
```

**remainingCount counts N,N-1,...,1** in this loop. It is not the zero-based country index. The source-expression tests preserve that actual indexing. Whether flag1 is displayed as unavailable is a separate runtime/display interpretation.

Global masks `$7A8C` (3×11 words) and `$7ACE` (3×17 words) refresh every season. For each merchant and each mask, choose a forced index via DB93(columns), set it1, and for each other index set `DB93(100)<chanceTableWord`. Chance tables are bank1 `$AFB2` and `$AFF4`, respectively; word position is `2*(merchant*columns+item)`. Forced indices do not consume a subsequent chance draw. Source tests execute both complete masks and verify exact RNG sequence.

## Verification and reproducibility

Run with Python3 and the private authorized ROM; there are no third-party dependencies:

```
GK_ROM='/private/Genghis Khan (USA).nes' python test_annual.py
```

The final core source suite has26 test methods. The later `ECONOMIC_DEPENDENCIES_SOURCE_REPORT.md` and24-method extension add verified original-instruction replays for the previously decoded major-epidemic economics, AI draft and AI commodity supplement, plus AI and unrest dependencies. `annual_source_replays.json` retains241 verbose selected-case records, original VM register checkpoints and memory writes; it includes2,159,832 original CPU instructions across those records. Many additional grid/country-loop cases run without verbose recording. Logs name their exact stage; earlier passing logs are retained. Do not equate241 records or26 test methods with241 gameplay experiments or26 fully closed register obligations.

Negative controls include source mutation of Rate's modulo1 operand to2, source mutation of Economy depreciation4% to5%, unchanged RNG result after changing an adjacent non-seed byte, isolated tax independence from ruler/non-troop population, no-human AI subsidy exclusion, player exclusion from AI Food purchases, non-specialty-stock preservation, final-staff-matched production-history differences, and explicit low/high/cap/rounding boundaries.

`source_cpu.py` interprets authentic legal6502 instructions. It rejects unmodelled mapper writes, illegal opcodes and instruction-budget exhaustion. `inspect_source.py` verifies the private ROM hash and loads bank-aware source. `vm_disasm.py` is a diagnostic disassembler: only specified function/code ranges are meaningful; it is not a whole-ROM code/data classifier. Replays do not replace gameplay logic with formula stubs. Expected formulas are separate assertions against executed instructions.

Potential register associations: TX04; reset portion of TX06; production portions of TD15/TD16; SE03-SE09; arithmetic/ledgers/development parts of CF01-CF05 and CF09. Their complete acceptance criteria may include runtime, UI, named-item, multi-year or other-agent evidence outside this source batch. Root must merge evidence without silently closing those broader criteria.
