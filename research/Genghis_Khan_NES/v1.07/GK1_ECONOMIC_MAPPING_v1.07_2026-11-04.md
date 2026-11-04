# Genghis Khan NES — complete economic mapping v1.07

Canonical rendered copy: [GitHub](https://github.com/Bomb-Daddy/NES/blob/main/research/Genghis_Khan_NES/v1.07/GK1_ECONOMIC_MAPPING_v1.07_2026-11-04.md). Relative document links resolve there or in a local checkout.

**Date:** 2026-11-04 UTC. **Owner:** Zhuge Liang / Codex. **Research status: PASS VERIFIED — all scoped economic batches complete.**

This completes the requested domestic, Trade, seasonal, logistics and economic-AI mapping batches for the original English NES game, replacing their unfinished sections of v1.04–v1.06. The versioned earlier findings remain preserved. It covers Trade, tax, relief, labor, drafting and training relevant to the economy, production, upkeep, development, economic events, transfers, economic AI and scenario economic data. The separate 167-row full-game register retains its broader campaign, battle, succession, asset, port and hardware requirements; economic evidence does not certify those unrelated requirements.

## Evidence and authority

The original private ROM is 262,160 bytes, SHA-256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`.

Mesen binary SHA-256: `f02685dafe559addf8dd8d769621a89777137d0c4592436aed65694a7e740676`. Native Linux Mesen executes the actual ROM; it is separate from the authored bounded 6502 source-replay harness. Full-machine checkpoints preserve CPU, PPU, mapper and memory state for matched experiments. Controller inputs, interventions, failures, screenshots, SRAM, zero-page snapshots, read-only traces and source replay logs are retained privately. Public files contain authored documentation, tests and numeric findings, not the ROM, save data or commercial artwork.

Source authority is the Director's instructions and [current authority register](https://docs.google.com/document/d/1qEVhDdiG2oH8LvPz0jan5DCtFFVxHhQWdWRUpduTllQ/edit), then original cartridge bytes and observed original-game behavior. Original USA manual NES-GX USA supplies the merchandise icon names. SNES, DOS, Genghis Khan II and the separate custom KOEI economy supply none of these rules.

**VERIFIED_STATIC:** inspected bytes or original-instruction execution with stated synthetic inputs. **OBSERVED_RUNTIME:** original ROM executed in Mesen, including explicitly identified controlled RAM/RNG cases. **MODELED_SOURCE_CONDITIONED:** a policy ledger checked against original source operations under stated event/price assumptions. **INFERRED:** an unexecuted interpretation, never substituted for proof. No result is TESTED_PORT or Windows/Xbox acceptance. The separate full-game register still includes espionage fees/destruction, diplomatic tribute, war supplies and captured-stock outcomes; these are not silently declared mapped by this batch completion.

## 1. State, widths and population

Country base is `$6DCD + 83*i`, where i is zero-based. Country1 is `$6DCD`; Country2 `$6E20`. Unless labelled byte, fields are little-endian unsigned16. The packed record's eligibility and resident fields must not be combined.

| Relative offset hex | Field | Country1 baseline |
|---|---|---:|
|14 / 16|Arms / Skill|110 / 124|
|18 / 1C / 20 / 3A / 3E|Troops / Town / Masons / Artisans / Foodworkers|61 / 5 / 4 / 6 / 27|
|1A / 1E|Economy / Defense|185 / 115|
|22+2*i, i0..9|Ten named goods|See item table|
|36|Food, inventory index10|208|
|38 / 3C|Commodity / Food production histories|6 / 27|
|40 / 42 / 44 / 46|Gold / Morale / Tax / extra-tax-used|439 / 118 / 35 / 0|
|48 **byte**|Undrafted eligibility percentage|32|
|49 **unaligned word**|Undrafted resident reserve|34|
|4B / 4D / 4F|China / Islam / Uighur presence words|State-dependent|
|51|Officer pointer|`$65BF`|

Define **P** = sum of the five drafted occupation words. Baseline P103 excludes the34 undrafted residents. Total residents for conservation accounting are P+reserve137. The displayed eligible headcount is derived from reserve and its byte percentage; it is not an independently stored extra population. Food upkeep, morale gift effectiveness, hire dilution and several other rules use P, not P+reserve.

Globals: `$65B9` country count14/27, `$65BB` year, `$65BD` season0Spring/1Summer/2Autumn/3Winter, `$7A88` difficulty, `$7A8A` common market Rate, `$7B34` active ability-actor country index, `$7B36` order **word**. `$7B66` is not the ability actor. Ruler abilities are six words at officer+0E in Leadership, Judgment, Planning, Charm, Body, Battle order; dependent governor resolution may redirect to the base ruler.

Plain VM word arithmetic wraps at65536. Wide helper `$DA67` multiplies in32bits before division; percent helper `$DA8B(a,b)` is floor(a*b/100). Add helper `$DA99` adds as16bits then caps999; subtract helper `$DAAD` subtracts min(request,current). Preserve the operations actually used: replacing all calculations with unlimited-precision algebra changes Extra Tax.

## 2. Merchandise and market prices

Names and icon positions are resolved for all17 columns using the USA manual's printedp21 and original item-layout/address instructions. The same named column is shared across all merchants. Weapons have no corresponding inventory words; they raise the shared Arms field.

| i | Item | Country1 stock address | China base | Islam base | Uighur base |
|---:|---|---|---:|---:|---:|
|0|Fur|6DEF|10|4|8|
|1|Silk|6DF1|16|37|25|
|2|Jewels|6DF3|32|28|25|
|3|Metals|6DF5|18|22|10|
|4|Medicines|6DF7|14|8|15|
|5|Woodworks|6DF9|11|14|7|
|6|Ceramics|6DFB|13|15|18|
|7|Glass|6DFD|8|5|20|
|8|Textiles|6DFF|20|15|31|
|9|Perfume|6E01|18|29|38|
|10|Food|6E03|10|11|11|
|11|Daggers|Arms6DE1|20|15|25|
|12|Swords|Arms6DE1|80|75|82|
|13|Axes|Arms6DE1|100|110|90|
|14|Lances|Arms6DE1|40|40|35|
|15|Pikes|Arms6DE1|60|70|65|
|16|Bombs|Arms6DE1|150|200|250|

For merchant m and item i, `T=BYTE[$7DE5+17*m+i]`. Quote values are integer tenths of Gold:

```
buyTenths  = floor(T*(Rate+100)/100)
sellTenths = max(1,floor(buyTenths*90/100))
hireTenths = floor(60*(Rate+100)/100)
```

The buyer routine is fixed bank15 nativeDCE5/VM DCEA; seller DD01/DD06; bank3 hire8671/8676. In natural tested bounds Rate+100 fits the word. Province does not supply a separate Rate: one global Rate is shared; merchant base values and offers differ.

For valid quantity q:

- **Buy:** stock+=q, Gold=`floor((10*oldGold-q*buyTenths)/10)` = oldGold−ceil(q*quote/10).
- **Sell:** stock−=q, Gold+=floor(q*sellTenths/10), capped999.
- Buy maximum=min(999−stock,floor(10*Gold/buyTenths)).
- Sell maximum=min(stock,floor(10*(999−Gold)/sellTenths)).
- Successful buy/sell/hire consumes one order and5 Judgment. Browsing, cancellation and rejected transactions consume neither.

Thus a displayed6.4 costs7 Gold for one unit and71 for11; a displayed0.7 sale credits0/4/6 Gold for quantities1/7/9. A one-unit trade can legitimately have a zero sale credit. Quotes do not eliminate whole-gold rounding or action costs. Cross-merchant profit/loss also depends on different base values, not solely the10% seller adjustment.

### Availability and offers

Presence is a country-specific word per merchant;1 allows the merchant,0 rejects it. Global offer masks are booleans, not quantities: Sell is3×11 words at7A8C; Buy is3×17 at7ACE. A purchase does not deplete an offer. Every seasonal refresh guarantees at least one item in each merchant/mode mask, then tests remaining items against original chance tables (bank1AFB2/AFF4). There is no global claim that every item is naturally available at every merchant at once.

Presence refresh uses updated Economy and current Morale:

```
threshold = WORD[bank1:B1C6 + 54*m + (N==27 ? 2*remainingCount : 0)]
            + floor(Economy*Morale/20000)
presence = (rand(100) > threshold)
```

Here N is the scenario's country count and remainingCount runs N..1. It is not a zero-based country index. The divisor is20000, not the earlier transcription50000. The portable regression uses an input distinguishing those values. Exact generator order and chance tables are in the source appendix.

The live matrix completes84 transactions:17 buys and11 sales for each merchant. Every trial restores the same full machine state, then explicitly sets all offer/presence words to1 to isolate item/merchant behavior. It verifies Gold, stock or Arms, Judgment and orders, including Uighur's own heading. This controlled coverage is separate from naturally generated offer/presence evidence.

### Weapons and hired troops

All six weapons apply this original rule, using **China's base column regardless of purchasing merchant**:

```
ArmsGain = floor(9*ChinaBase[i]*(q+1)/P)
Arms = min(999,Arms+ArmsGain)
maxWeaponQ = min(999, floor(P*(999-Arms)/ChinaBase[i]),
                 floor(10*Gold/buyTenths))
```

The q+1 is literal, not a typo to repair. At P103/Arms110, a one-unit Uighur dagger purchase at5.0 costs5 Gold and raises Arms to113. Weapons cannot be sold through the11-column seller list.

Hire quantity is bounded by min(999−P,floor(10*Gold/hireTenths)). Add q Troops; undrafted reserve does not change. Weight Arms and Skill against newcomer value100, and Morale against50+rand11, using old **P**, not old Troops:

`newValue=min(999,floor((oldP*oldValue+q*newcomerValue)/(oldP+q)))`.

Then debit the rounded-up hire cost. Hiring8 in the archived positive case gives Troops61→69, Gold368→272, Arms110→109, Skill124→122 and Morale118→113. Zero Gold, full drafted capacity and insufficient Judgment are separate refusal controls.

## 3. Tax, gifts and command costs

### Ordinary Tax

Legal requested rate is1..100; zero means no completed change and101 is invalid. Identical requested/current rates do not complete a command. Changing the requested value can still complete a command even when rounding leaves the resulting tax unchanged.

For lowering oldTax→r: tax=r; add `(oldTax−r)*(2+rand3)` Morale, capped999.

For raising, let d=r−oldTax and signed margin=floor(Leadership/5)−r. Response percentage is100 if margin>50;40+rand21 if margin>0; otherwise10+rand11. Tax becomes min(100,oldTax+floor(response*d/100)); subtract d*(2+rand3) Morale, floored0. The full requested increase determines morale damage even when the actual tax increase is much smaller. No Gold is collected immediately.

Observed baseline requested1 gives tax1/Morale254; requested99 and100 each produced actualTax41/Morale0 in their respective controlled runs. The six-stat debit is Leadership5/Charm5 and one order.

### Extra Tax

With P>0 and the use word0:

`maxQ=min(Economy,1+floor(floor(Economy*(1+rand100)/100)*(Leadership+Morale+Charm)/3000))`.

Valid q1..maxQ credits q Gold and `floor(q*(1+rand100)/100)` Food, each capped999. The earlier unexplained9-Gold/Food+1 result is an explicit randomized Food credit. It sets the use word1, reduces Economy byq and updates Morale/workforce through:

```
f(E,x,q) = floor(x/2)
           + floor(floor((((E-q)*x*10) mod65536)/E)/20)
```

Both multiplications wrap at16bits. Set Morale=f(E,oldMorale,q). Target drafted total=f(E,oldP,q); if necessary repeatedly scale all five groups by floor(target*100/currentP), flooring each group independently until their sum≤target. Rounding can overshoot the intended loss. No removed workers are credited to undrafted reserve. If P reaches0, Arms and Skill clear.

At baseline E185/Morale118, q1 produces Morale64 and worker groups39/3/2/3/17 (Troops/Town/Masons/Artisans/Foodworkers). This is far more severe than an unlimited-precision smooth percentage would suggest. Extra Tax costs Leadership10/Charm10 and one order. Second use is denied. **The use word resets at Autumn harvest, not Spring.** Later consequences follow the changed Morale, Economy, workers and resources through the ordinary seasonal/revolt/theft rules; no invented delayed penalty is added.

The [observed levy-year ledger](GK1_OBSERVED_LEVY_YEAR_LEDGER_v1.07.csv) carries an actual Spring levy through Autumn reuse and the following Spring. Reserve34 does not absorb the lost workers; normal Spring reserve growth later gives36. The intervening10-Gold Ally transfer is separate. Spring ends Gold463/Food248/Troops28/use1, and another levy is refused without cost.

### Give

Gold gift value V=q. Food uses index10. Give Item uses the country's native specialty, not an arbitrary inventory choice. For Food/Item:

`avg=floor((sellQuoteChina+sellQuoteIslam+sellQuoteUighur)/3)`;
`V=min(999,floor(q*avg/10))+1`.

Debit the selected stock; add `floor(30*V/P)+1` Morale, capped999. A nonhuman country receives an additional10 Morale in the shared source rule. All ten native-specialty indices were exercised at quantity8, with additional quantity, Rate and population controls. Repeated Gold1 gifts from Morale998/Gold8 reach Morale999/Gold6, consume two orders and10 Planning, and retain those results into Summer. An accepted gift at already999 still consumes its stock and action cost. Positive player gifts cost Planning5 and one order. Cancel/zero/insufficient stock do not complete a command. The prior simple Food step-function fit is superseded: gift value depends on current Rate, all three merchant bases, and P.

## 4. Workforce, drafting and training

Assign Labor partitions Troops plus the selected job. Let pair=min(999,Troops+job); requested newJob is0..pair. Set job=newJob and Troops=pair−newJob. Other jobs and undrafted residents stay fixed. A preview uses a temporary record; cancellation rolls back, and unchanged confirmation consumes no cost. Successful confirmation costs Leadership5/Planning5 and one order. If finalTroops exceeds initialTroops and t=floor(100*Troops/P)>65, set Morale=floor(Morale*(100−2*(t−65))/100). Exactly65% has no penalty.

Draft maximum=min(floor(reserve*eligibility/100),999−P). A positive q adds q Troops and removes q from reserve. Arms/Skill/Morale are weighted against newcomer value1 using old P. Set eligibility byte to `max(0,floor(100*(oldEligible−q)/(oldReserve−q))−rand3)`. Rounded eligible headcount afterward can fall by more than q without people disappearing: the percentage is rounded. Baseline Draft8 observed Troops69/reserve26/Arms102/Skill115/Morale109/eligibility5; P+reserve stays137. Cost: Leadership20/Charm10/Battle10 and one order.

Train People changes only eligibility. With reserve>0, rawGain=floor((Leadership+Battle)/600)+rand2+2; factor=100−6*min(5,unsigned16(difficulty−1)); gain=floor(rawGain*factor/100), cap50. At eligibility50 it refuses. At reserve0 the original code returns a completed command despite no improvement: the live test confirmed one order and5 Charm charged. Baseline32→35 and49→50 were observed.

Train Troops raw=15+rand11+floor((Leadership+Charm+Battle)/40); sizeAdjusted=floor(999*raw/(P+999)); factor=100−4*min(5,unsigned16(difficulty−1)); Skill gain=floor(sizeAdjusted*factor/100), cap999. It costs Leadership5 and one order, including at cap999. Baseline124→143 was observed. Summer later multiplies Skill by93%, so the increase is persistent state but is not immune to seasonal decay.

[Complete inspected cost table](GK1_COMMAND_COST_MATRIX_v1.07.csv) includes all32 source rows and identifies the economic command subset. Unmapped non-economic row identities are not invented.

## 5. Seasonal engine and natural Rate

At season end, every country first accumulates Foodworkers into Food history and Artisans into commodity history, each capped999. Then:

```
Economy = min(999,Economy-floor(Economy*4/100)+Town)
Defense = min(999,Defense-floor(Defense*4/100)+Masons)
```

Presence words and global offers refresh; season advances, year increments when it becomes Spring; Rate changes; the new season's events/economics run. AI automatic Food purchasing follows the seasonal dispatcher. Per-country revolt/theft/warning checks and AI/player commands occur in the outer turn loop, so their effects must not be mislabeled production or tax.

### RNG and seasonal Rate

Native random state is big-endian48bits at84..89. A C289 draw computes `s'=-((s|1)*0xE0900E0187C5) mod2^48`, returns `(s'>>32)&0x7FFF`; rand(n) applies modulo n. Modulo0 returns0 without drawing. Console NMI also advances part of the seed when byte8A is0; therefore an isolated native-RNG replay is not a claim that elapsed frames never affect outcomes. Full-machine replay preserves timing; explicit event-seed tests set the original8A guard1 and are labelled controlled.

Spring Rate=50+rand(94+7*difficulty). The UI's levels1..5 therefore yield maxima150,157,164,171,178 respectively. Summer/Autumn/Winter draw p=1+rand9 and reduce Rate by floor(Rate*p/100), floored50. The source's direction draw is rand1, always0; an increase branch is unreachable without changing that operand. A mutation1→2 demonstrates the distinction. The shared display is an index, not an individual commodity price or a binary high/low label; at Rate100 a base value is doubled before the seller adjustment.

The natural Mongol sequence observed in this run is101→99→91→85→110 from Spring1175 to Spring1176. The prior101→93 Spring/Summer run remains valid under its different input/RNG history. Controlled Rate190 price evidence is not presented as a reachable difficulty1 initial Rate.

### Autumn production

Food+=FoodHistory and the country's specialty stock+=CommodityHistory, capped999. Clear both histories and the extra-tax-used word. Current workers are unchanged. Production is accumulated worker-seasons, not simply final staffing or a division-by-four of it. Stocks carry over until consumed, sold, moved, lost or capped. Four worker counts10/20/30/40 produce history100;0/0/0/40 produces40 despite equal final staffing. The initial Spring1175 state already holds histories27/6, so its first Autumn includes that earlier contribution.

### Spring upkeep, receipts and payroll

Use the five-group drafted P. If P≤2*Food, consume floor(P/2) Food. Otherwise repeatedly set p=floor(200*Food/P), floor each group's multiplication byp/100 and recompute P until supported; after shortage finish Food0. Independent rounding can destroy more allocated people than a single total-based calculation. Undrafted reserve is not in this upkeep P.

Food processing precedes payroll. With post-shortage troops:

```
a = floor(Economy*Tax/100)
f = 12+min(13,floor(Morale/9))
grossTax = floor(a*f/25)
available = oldGold+grossTax
newTroops = min(oldTroops,available)
newGold = min(999,available-newTroops)
```

Gold caps after receipts and payroll. An inability to pay one Gold per soldier reduces troops to available Gold. Population/ruler stats have no direct input to the isolated revenue formula; their indirect effects through Economy/Morale remain real.

Later Spring reduces Morale by floor(Morale*pct/100), where pct=2 for Tax<50, otherwise Tax−48. This follows that Spring's revenue calculation. Allocated population normally grows3%, capped999; below20, add1 Troop instead. Divide the growth equally among five groups, then distribute leftovers in order Troops, Masons, Town, Artisans. AI countries with leader status100 or a player-owned neighbor can receive an additional min(3,difficulty) growth percentage; humans do not.

Undrafted reserve separately gains floor(oldReserve*5/100)+1, capped999. Before that reserve growth, eligibility becomes20+rand11 when reserve<20; otherwise `min(50,floor(100*(floor(reserve*(oldEligibility+3)/100)+1)/(floor(reserve*105/100)+1)))`.

### Observed annual ledger and controlled staffing

| Country1 point | Rate | Gold | Food | Fur | Economy | Commodity/Food history |
|---|---:|---:|---:|---:|---:|---|
|Spring1175|101|439|208|73|185|6 / 27|
|Summer1175|99|439|208|73|183|12 / 54|
|Autumn harvest|91|439|289|91|181|0 / 0|
|Winter1175|85|449|289|91|179|6 / 27|
|Spring1176|110|449|238|91|177|12 / 54|

The10-Gold Autumn increase is now traced to an AI Ally proposal, not left unexplained: Country8 loses10 while Country1 gains10. The actual native write has computed449, destination6E0D, delta10 and saved caller return93BB in bank2. The proposal transfers the10 before acceptance/refusal handling. Spring then collects61 tax and pays61 troops, leaving449; consumes51 Food; Morale118→116; adds one each to Troops/Town/Masons, and reserve34→36. Every deterministic country1 annual delta is reconciled.

Four native matched Winter→Spring experiments restore the same checkpoint and preserve drafted P103. Compared with control, moving ten Foodworkers to Town yields Economy187 instead of177 and Gold453 instead of449; to Masons yields Defense125 instead of115; to Artisans yields commodity history22 instead of12. Food history is44 instead of54 in all three. The before/after snapshots and exact interventions are retained.

## 6. Seasonal losses, theft and unrest

Summer Skill=floor(Skill*93/100); Winter Arms=floor(Arms*97/100). These are not equivalent to subtracting floor7%/3% at every integer.

For ordinary event reduction, `reduce(x,p)=x-floor(x*p/100)`. D is rand6 plus floor(3*difficulty/2) for human countries,0 extra for AI; draws are separate where specified:

- Storm: Defense reduces25+D%, Economy by another25+D%; reserve and eligibility each retain independently drawn80..90%.
- Cold: Economy reduces20+D%; both production histories clear; reserve/eligibility retain80..90%.
- Sickness: Economy reduces10+D%, Morale20+D%, each drafted group15+D% using one common group percentage; reserve/eligibility retain80..90%.
- Major epidemic: separate45..55% reductions to Morale, all drafted groups and Economy; reserve/eligibility clear. Family/battle consequences are separate full-game requirements.

Storm selection is World-only with a60% global gate, region eligibility and5% per eligible country. Country1 is ineligible; World2/18/19/23/24/25/26/27 qualify. Cold uses60% global gate and5% eligible-country checks; Mongol14 bypasses its region restriction. Sickness uses80% global gate and5% per-country checks. Major-epidemic attempt is Spring, year divisible by12 except1176, then5%; it replaces ordinary sickness for that call. Source probabilities are exact code rules, not measured empirical frequencies from a few frames.

Popular revolt is triggered if Tax>50+rand(210−10*difficulty), or otherwise if Morale+rand101<25+difficulty+rand16; the right-side rand16 is drawn first. Equality does not trigger. Its eventual civil-war outcome depends on the separate battle system; no fabricated flat economic penalty replaces a battle.

Provision theft affects Food. If floor(Morale*eligibility/100)+rand201>16, a further Food/P comparison may exclude theft: floor(Food/90)<floor(P/90)+rand(176−5*difficulty). If still eligible, rand100 must be≤70+6*difficulty. Retain floor(Food*(55+rand21)/100), then subtract5+rand11 floored0. A low-Morale warning alone writes no economic resource. Person-driven rebellion predicates use personality and relative ruler attributes, separately documented in the AI/unrest appendix.

## 7. Cross-country logistics

Move requires the ruler role and more than one owned country. Source can be any owned country; destination must share ownership and be in the selected source's adjacency list. Lists are directed by source, even though the natural tested map is symmetric. Ownership is the byte at officer+1D. A same-country or friendly nonadjacent transfer is rejected. The source picker temporarily changes7B34 and restores it before final ability charging.

Gold, Food and any named Item transfer exactly q from source to destination, no percentage loss or RNG fee. Maximum=min(sourceStock,999−destinationStock). Item is not restricted to native specialty. Success costs5 each Leadership/Judgment/Planning and one order.

People transfers one of the five drafted occupations; maximum=min(sourceOccupation,999−destinationP). Undrafted reserve, eligibility and historical production stay in their original countries. Pairwise P and total residents are conserved. Cost is Leadership10/Charm10 and one order.

After moving people, source cleanup clears Arms/Skill if its drafted P became0. Destination then averages its old quality and the **current source quality** using old destinationP and movedq. Emptying the source therefore contributes already-cleared Arms/Skill0; this original order is preserved. Source Morale is not cleared. With source8 troops/Arms110/Skill124/Morale118 and destination10/150/80/100, moving all8 produces source0/0/118 and destination18troops/Arms83/Skill44/Morale108.

### Move Base and destination resale

Move Base swaps the source/destination officer pointers, costs20 Body and clears the current remaining orders. Resources stay in their countries. The original country scheduler continues; an owned destination whose slot has not yet run can give the relocated ruler fresh orders in that same season. The baseline's country2 slot is later than country1's.

The native test explicitly changes country2 ownership to the player and its officer role to delegated governor101, then uses normal controller commands throughout. One Jewel moves from country1 to country2: stocks26/29→25/30. Move Base then swaps pointers65BF/65DE, lowers Temujin Body170→150 and ends the current turn. The original scheduler reaches country2, granting3 orders. Selling that Jewel to Islam credits5 Gold (66→71), restores destination stock29, leaves source stock25, costs Judgment5 and one order. This is a controlled ownership fixture, not a claimed conquest. Temujin's abilities are read through his own pointer after relocation; the snapshot helper's fixed country1 abilities then belong to the replacement officer.

## 8. AI economics

AI rules have explicit assistance and should not be reproduced as player earnings. Main economic order is formation, tax, a separate political/espionage path, labor, conditional hire/gifts/weapons, then later relocation/diplomacy/training choices. Delegated policies select branches separately.

AI labor uses Town/Masons/Foodworkers/Artisans percentages20/5/30/5 below Economy300;18/7/30/5 at300..499;15/8/30/7 at500+. Troops receive the exact remainder. A special5/1/8/1 row requires a1/5 draw, Gold>150, P>150, non-Winter and the AI flag. This conserves P. Winter AI P>600 can set Tax50..55 without a Morale charge; normal tax35..50 is retained, otherwise adjusted toward38..43 using ordinary tax response logic.

AI hiring requires Gold>2*Troops and hires half the affordable/cap-limited maximum. Shared dilution and debit rules apply. AI gifts choose the largest commodity stack (first tie), can give it all, and receive the extra10 Morale. AI weapon purchasing uses a specific source quote/value combination rather than random symmetrical merchant choice. Free AI training can improve Skill and the lowest ruler attribute without Gold debit. Exact thresholds/rounding and original vectors are in the source appendix.

After seasonal processing, AI Food assistance can convert surplus Gold at2Food perGold, independently of Rate, targeting floor(1.5*Troops). Human countries are excluded. Seasonal AI Gold subsidies depend on human countries, human player count, scenario, season parameter and difficulty. Summer can add50..100 to one random commodity when the ten-stock total≤349. Summer AI drafting has a distinct eligibility update that omits multiplication by100 and commonly writes0; it is not silently replaced with the player formula. These rules have original-instruction controls for human exclusion and threshold boundaries.

## 9. Regional data and yearly policy comparison

[All41 country economic records](GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv) include14 Mongol and27 World countries, original spellings, native specialties, all eleven inventory words, Gold, workforce, reserve/eligibility and exact source offsets. Values are explicitly **Winter1174/1205 pre-setup templates**. Difficulty/player setup and activity before the first turn alter fields. The original loader and15 condition-labelled setup replays preserve that distinction; runtime Spring1175/1206 files are separately compared. Baghdad's specialty is Textiles, despite a larger initial Perfume stock; specialty is an explicit byte, not the largest-stock guess.

[The one-year ROI comparison](GK1_YEARLY_ROI_COMPARISON_v1.07.csv) executes a deterministic economic model checked against original6502 operations at50 checkpoints for10 policies. Baseline is the observed Spring1175→Spring1176 trajectory; price/offer schedule and no adverse events are stated conditions. Alternative policies are modeled, not mislabeled played campaigns. The originally residual10-Gold event is now identified as the treaty transfer and is held equal across these comparisons.

Reassigning ten Troops to Artisans yields20 extra harvested Fur plus20 pending commodity history, saves10 payroll, and leaves ten fewer troops. Selling that extra20 Fur at the observed Autumn China1.7 quote credits34 Gold, costs an additional order/Judgment5, and gives modeled final Gold493 versus baseline449. Reassigning ten to Town instead yields Economy215 versus177 and final Gold473. Hiring ten and assigning them preserves baseline troop count but incurs120 Gold initially, five more Food upkeep and lower tax through morale dilution; the Artisan case ends326 Gold before sale,360 after. These choices are not assigned an invented equivalent military utility.

Pending history is not already harvested inventory. Caps can erase marginal output: Food980 saturates at999 under both baseline and extra workers; Fur980 retains only one extra unit at the first harvest. Gold headroom limits how much can be sold. The exact ledgers, costs, changed military quantities and cap/shortage controls are in the ROI appendix.

## 10. Native evidence and independent verification

### Natural World year

A genuine World Conquest start used27 countries, Genghis Khan, automatic statistics and difficulty1. Its initial Spring1206 country1 state was Gold193, Food332, Troops116, drafted P181, reserve106, eligibility32, Economy234 and Morale118. The observed Rate sequence was114→113→108→102→118 through Spring1207.

Summer retained Gold193/Food332. Autumn produced Food475 and Fur109. After development and sickness, Troops were116→93, Economy231→201, Morale118→91 and reserve106→94. An AI treaty proposal subsequently transferred10 Gold, separately identified as income. Winter held Gold203/Food475. Next Spring's65 gross tax,93 payroll and73 Food upkeep gave Gold175/Food402 before the observed population increase. Source templates are not substituted for these post-setup observations.

### Actual later-Rate buy and resale

One Jewel was bought from China in Spring1175 at Rate101, quote6.4, for7 whole Gold: stock26→27 and Gold439→432. No RAM intervention enabled this purchase or its later sale. Summer had no Jewels sale offer, so the experiment continued to Autumn. At Rate91, China naturally offered to buy Jewels at5.4; selling the same acquired quantity credited5 Gold and restored stock26. The signed trading result is **−2 Gold**, plus one order and5 Judgment spent at each end. The intervening treaty's10 Gold is accounted separately; the resulting overall cash increase is not called trading profit.

### Controlled events and AI

Native controls reached original routines through normal Pass, then paused at bank-specific execution or bytecode-read breakpoints. Written country fields, RNG seeds and the original NMI RNG guard are listed per case. They never replace ROM instructions or jump the native CPU/VM into a manufactured result. Cold, sickness, storm, theft, starvation and harvest controls cover both triggering and nontriggering branches where applicable. Harvest history40 versus100 produces distinct output despite equal current staffing; stock saturation and extra-tax reset are also captured.

Ten bounded native AI observations independently match original-instruction replay in every measured economic word and final48-bit RNG state: labor at Economy299/300/500; Winter tax at P600/601; Food assistance with AI versus human ownership and insufficient surplus; and hire at Gold200/201 with100 troops. These routines were reached in real AI turns (countries3 and10 for commands, country1 for the controlled assistance loop). They prove their bounded economic rules, not an entire autonomous campaign.

The second natural Mongol year reaches Spring1177. Its Autumn Food346→214 loss is traced directly through the original theft entry and positive completion at nativeDE0C with **no RAM intervention**; Gold449 stays unchanged. That loss is not charged to farming, upkeep or an unexplained residual. Spring1177 then has Gold444 before another10-Gold treaty transfer, and454 afterward, Food161, Fur115 and63 Troops.

### Source and capture verification

The portable original-instruction/capture package passes262 unittest methods plus15 scenario setup cases; the additive Move Base package passes19 more methods. Root and independent reviewer reruns used clean copies and exact input hashes. The runners require a completed expected count, `OK`, and exit0; missing inputs, bad hashes, a premature exit0 and source mutation do not pass. These counts describe test methods, not281 played scenarios.

Native verification recomputes amounts from captured bytes rather than accepting the runtime scripts' `passed` flags. It checks8,192-byte SRAM hashes, prices, rounding, qualities, resource changes, abilities, orders, seasonal accounting, event controls and corruption rejection. Historical captures and fresh logged replays remain separately identified. [The completion and reproduction index](GK1_ECONOMIC_MAPPING_COMPLETION_AND_REPRODUCTION_v1.07_2026-11-04.md) points to exact receipts, manifests, scripts and the private evidence archive.

## 11. Corrections, failures and evidence boundaries

No failed attempt is erased or counted as an accepted mechanic:

- Earlier Uighur “dagger” navigation first reached Ceramics and then Food. The later correctly rendered dagger capture and complete84-transaction matrix supersede those labels.
- An early Tax100 probe entered1 because numeric digits were reversed. The corrected input helper and the fresh100 probe verify requested100→actual41/Morale0. The old capture remains historical.
- Early Extra Tax repeat probes accidentally opened Assign Labor. Later Tax-menu no-change captures prove counters only; they are not described as containing refusal text. The final message proof uses the actual refusal branch.
- Merchant presence's source divisor is20000. Earlier prose/model used50000; original CPU execution was correct. A discriminating regression corrects the model. Ability-actor index is word7B34, not7B66. A legacy trace field named `country` records7B66; use the new `active_country` field or decode the captured SRAM when interpreting actor identity.
- A one-hour emulator session expired during the first storm-negative attempt. It was restarted with a longer limit and full-machine checkpoints. The failed attempt and subsequent passing control are retained.
- Long-lived output file handles yielded incomplete persisted raw logs despite later screenshots/SRAM being present. Logging now opens, appends and closes each record, and affected checks were rerun in a separate directory. Historical evidence is not retroactively labelled fully logged.
- An original treaty prompt interrupted the levy-year and second-year progress checks. Those pending UI states were captured, answered and continued. An attempted message breakpoint initially targeted nativeEB58 as a data read; the correct bytecode stream beginsEB5D after its native prologue. Restore requests waiting behind an original message were acknowledged before retrying from the intended checkpoint.

An exact full-machine restore includes CPU/PPU/mapper state. It is a research replay mechanism; it does not certify battery-save corruption recovery or Windows save/load behavior. Synthetic ownership, full merchant offers and seeded adverse events are explicitly labelled controls. Ruler deaths, battles, diplomatic tribute, espionage, complete campaigns and native-port acceptance remain under their unchanged full-game requirements.

## 12. Reference map

| Detailed appendix | Subject |
|---|---|
|[01 Commodity identity](appendices/01_Commodity_identity.md)|Manual icon names and original item layout|
|[02 Trade, hire and weapons](appendices/02_Trade_hire_weapons.md)|Quotes, offer/presence generation, transaction arithmetic|
|[03 Domestic commands](appendices/03_Domestic_commands.md)|Tax, relief, labor, Draft and training|
|[04 Annual engine](appendices/04_Annual_engine.md)|Operation order, production, development, upkeep and growth|
|[05 AI and unrest](appendices/05_AI_and_unrest.md)|AI economic rules, assistance, revolt and theft predicates|
|[06 Logistics](appendices/06_Logistics.md)|Transfers, ownership, adjacency, caps and quality averaging|
|[07 Scenario data](appendices/07_Scenario_data.md)|All41 templates and original setup loader|
|[08 Economic ROI](appendices/08_Economic_ROI.md)|Ten explicitly modeled yearly policies|
|[09 Annual runtime audit](appendices/09_Annual_runtime_audit.md)|Independent original-instruction reconciliation of native annual states|
|[10 Base relocation](appendices/10_Base_relocation.md)|Officer swap, Body cost, scheduler and destination trade|

The [complete167-row proof register](GK1_COMPLETE_167_PROOF_MATRIX_v1.07_2026-11-04.csv) preserves every earlier requirement and evidence column. Its new economic-mapping status is distinct from closure of an original row that also asks for war, dynasty, whole-campaign or port proof. Earlier versions remain available unchanged; this edition controls the economic findings it corrects and completes.
