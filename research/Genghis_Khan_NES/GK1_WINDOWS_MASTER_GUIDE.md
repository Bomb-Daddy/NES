# Genghis Khan I NES → Windows — Unified Knowledge and Completion Guide

**Record:** ZL-GK1-MASTER-0017 · **Version:** 2.01 · **Date:** November 5, 2026  
**Author:** Zhuge Liang · **Recipients:** Director; Nobunaga, Windows implementation; Cao Cao, independent QA  
**Project:** Genghis Khan I, English USA NES, and the Director-approved modified NES builds. Not ROTK II NES/DOS or the separate original KOEI game.

## 1. Read this document first

This is the single current guide to what is already known, which implementation material to reuse, and precisely what remains to finish the Windows game. It replaces the work ordering and stale backlog statements in the v1.00–v1.02 gap-closure plans. Their evidence and original files remain preserved. The sections below are cumulative, not competing addenda.

**Director instruction: reuse completed work; do not retest it.** Existing source findings and recorded native Mesen results are accepted for their recorded ROM, mode and scope. Do not restart economic mapping, revalidate the sound sequencer, repeat Rest/Demobilize/menu-setting captures, or repeat the already completed invasion–retreat–save–cold-load sequence to obtain another green result. Use those outputs directly. Investigate only unanswered questions. Testing a newly implemented Windows component against the existing reference is new integration work, not a reason to replay the old NES experiment.

**The v2.00 consolidation performed no gameplay retests.** It recovered existing data and reconciled old inventories. **Version 2.01 adds EXEC0018: new normal-melee research and executable arithmetic, not a replay of settled work.** Nine new controlled native attacks and a cancellation case used the existing in-battle checkpoint; 44 bounded source-instruction vectors and new-component checks support the decoded arithmetic. No Windows executable or complete tactical game is claimed.

### Current outcome

| Area | Accepted material to consume now | Work still required |
|---|---|---|
| Economics | All 11 scoped v1.07 batches, exact formulas, merchant tables, workforce, seasonal engine, logistics, economic AI and 41 country economic templates | Integrate in Windows; investigate only uncovered war/political/dynastic interactions |
| Command data | All 34 cost rows; **30 labels identified** after recovering Train Prince/Self from the older table | Identify callers of rows 16–19; finish uncovered actor/commit paths, not cost extraction |
| Approved ROM changes | Demobilize/Fonzarelli, shared BP/Rest, changed menus, bounded event handling, native save/load and affected HD screens | Port these exact features; finish only unvisited routes and full-world interactions |
| Audio sequencing | Independent working four-channel sequencer, token/loop/repeat/replacement rules and native parity evidence | APU-to-PCM renderer, exact scheduling integration, remaining event associations, audible Windows output |
| Audio event routing | Ten gameplay-associated entries after EXEC0018 cue41 normal-attack animation; cue0 is structural | **41 other entries need caller/event classification.** Cue41 option gate/additional uses remain open; do not redo the sequencer |
| Strategic war / tactical rules | Accepted invasion/retreat/save route plus EXEC0018 executable normal-melee strength, matchup, terrain, castle and casualty arithmetic | Legal movement/target selection, ranged/special commands, commander-zero/terminal handling, victory/defeat/capture, auto-resolve and combat AI |
| Graphics | 64 extracted portraits, 9 static tile sets, 19 attribute tables, five observed palettes, loader research and approved HD artwork | Bank 10 faces, 43 computed load sites, remaining layouts/fonts/sprites/animation and live overlays |
| Scenarios / campaigns | Economic data for both scenarios, startup observations, modified World initialization and separate original campaign records | Remaining character/map/setup rules, end conditions and integrated Windows campaigns |
| Windows product | Specifications, extracted data, reusable source and sound module | Integrated game, controller-only operation, packaging, native Windows execution and Director acceptance |

The old register's **61 PASS VERIFIED / 40 PARTIAL / 66 NOT RUN** is a November 4 historical snapshot of 167 mixed research/implementation obligations. It is **not** a current count of 106 entirely untouched mechanics. Section 11 reconciles every original ID to reuse or a specific remaining work package.

## 2. Sources, modes and authority

The exact build that produced an observation is its behavior reference. Modified NES evidence is real evidence of the modified game; it is not a second-class hint. The unmodified game does not need to contain Demobilize or Rest to prove how those added commands work.

| Reference | SHA-256 | Use |
|---|---|---|
| Stock USA NES, 262,160 bytes | `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3` | Original behavior and Classic compatibility |
| Demobilize + Fonzarelli | `61ad28688d503231d12537b2cbebb073ae5f91ec1391f825c1d05312436393be` | Its six-attribute modified behavior, naming and manpower command |
| Shared BP + Rest + Demobilize + Fonzarelli | `4764d46359581be1e7d12bcb85bd375c84cd4497d1084d9d306a6e884afbe7c5` | Combined approved modifications and EXEC0013–0015 runtime captures |

Keep profile-specific rules and save versions explicit. This separation preserves the Director's modifications; it is not permission to omit them or demand that they behave like stock. Do not import another KOEI game's BP, market, combat, or geography rules. Retain controller-only Xbox play, a 1280×720 Windows interface, dependable saves and the NES aesthetic. The faithful 256×240 scene can be integer-scaled without stretching it to fill 16:9; changed artwork remains explicitly selectable/profile-specific.

**Evidence ledger:** retain the original investigator, source version, ROM hash, actual method and limitations. Use VERIFIED_STATIC / OBSERVED_RUNTIME, with a MOD qualifier where appropriate; Windows results require TESTED_PORT. “Not rerun during consolidation” does not invalidate an accepted prior result. A source-only result is not silently promoted into a player-driven native route. A native NES result is not silently promoted into a Windows run.

**Roles:** Zhuge Liang maintains this guide, resolves source conflicts and closes unanswered mechanics. Nobunaga integrates directly into the isolated Genghis Windows target and supplies playable builds. Cao Cao challenges the new implementation and unresolved claims using existing evidence, rather than requalifying settled research. The Director controls design, scope and final acceptance. No routine approval waits.

## 3. Recovered data and source-to-code starting points

### 3.1 The command-cost table is available now

Headered ROM offset `0x1F1C8`, PRG bank 7 CPU `$B1B8`, 34 rows × 6 bytes. Columns below are Leadership, Judgment, Planning, Charm, Body, Battle. The six-component values come from the accepted Demobilize build; shared BP sums the six components and stores the charge in Body. The original economic table also identifies rows 6 and 7. They were mistakenly left unnamed in the later export: **6 = Train Prince; 7 = Train Self**. This is recovery of existing information, not a new runtime test.

| Row | Command | L | J | P | C | Body | Battle | Shared BP |
|---:|---|---:|---:|---:|---:|---:|---:|---:|
|0|Tax: Change rate|5|0|0|5|0|0|10|
|1|Tax: Extra tax|10|0|0|10|0|0|20|
|2|Assign Labor; reused by Demobilize|5|0|5|0|0|0|10|
|3|Assign Army|5|0|5|0|0|0|10|
|4|Give|0|0|5|0|0|0|5|
|5|Train Troops|5|0|0|0|0|0|5|
|6|Train Prince — recovered label|0|0|0|5|0|0|5|
|7|Train Self — recovered label|0|0|0|0|0|0|0|
|8|Train People|0|0|0|5|0|0|5|
|9|Trade: Buy / Sell / Hire|0|5|0|0|0|0|5|
|10|People: Switch|5|10|5|0|0|0|20|
|11|People: Select|5|10|5|0|0|0|20|
|12|People: Demote|5|10|5|0|0|0|20|
|13|People: Promote|5|10|5|0|0|0|20|
|14|People: Marry|0|10|10|0|0|0|20|
|15|People: Draft|20|0|0|10|0|10|40|
|16|Caller unresolved|0|0|0|5|10|0|15|
|17|Caller unresolved|0|0|5|5|10|0|20|
|18|Caller unresolved|0|0|0|5|10|5|20|
|19|Caller unresolved|0|0|0|5|10|0|15|
|20|Paid foreign State / Ruler View|0|5|0|0|0|0|5|
|21|Move Base|0|0|0|0|20|0|20|
|22|Move People|10|0|0|10|0|0|20|
|23|Move Goods|5|5|5|0|0|0|15|
|24|Policy|5|5|0|5|0|0|15|
|25|Treaty: Vassal|0|0|0|25|5|10|40|
|26|Treaty: Tribute|0|0|0|15|5|5|25|
|27|Treaty: Alliance|0|0|0|5|5|0|10|
|28|Spy: View report|0|0|5|0|0|0|5|
|29|Spy: Havoc|0|0|0|0|20|0|20|
|30|Spy: Kill|0|0|0|0|0|20|20|
|31|Spy: Search|5|0|5|0|0|0|10|
|32|War: Self|40|10|5|0|0|20|75|
|33|War: Prince|10|5|5|5|0|0|25|

**Implementation cautions:** Demobilize is not People Demote. Rows 6/7 are retired from the shared-BP Train menu; their retained data does not make them reachable. Four unknown labels must stay unknown until caller analysis identifies them. Numeric cost is not proof of affordability timing, payer identity, refund semantics or runtime reachability; reuse known handlers and resolve only uncovered cases.

The recovered package contains the full paired table, all 80 patch locations, all 16 UI surfaces and its extraction receipt. The derived `command_costs_v1.01.csv` adds the two recovered labels without altering prices. Original files remain unchanged. The 80 guarded patch intervals account for the accepted 3,139 differing bytes between the two modded ROMs; this is a source map, not an instruction to recheck all patches.

### 3.2 Known engine anchors

| Area | Existing source anchor | How to use it |
|---|---|---|
| Cost/affordability/debit | Bank0 `$8B18`; bank2 `$9863/$989C`; bank3 `$8242/$827B` | Map actor and command variant; do not replace six-stat checks with summed BP in the six-stat profile |
| Acting country | `$7B34`, not `$7B66`; actor resolver references `DBA7` and `DDD6` | Distinguish payer, viewed subject, selected prince and base ruler |
| Strategic budget | `$7B36` is a word; source role100 branch gives three orders, other role branch one | Import the existing role-based budget finding; do not derive orders from BP |
| Full character | 29-byte full record; stats at `+0E..+18`, Body `+16` | Keep compact child structures distinct; follow known copy/compaction and ownership paths |
| AI training | Bank2 `$8A38`, segment `$8A5C..$8A97`, callers `$816D/$83E8/$83F7/$842E` | Reuse decoded routine; discover missing natural scheduling/decision conditions only |
| Successful native Load | Bank0 `$97AA` continuation | Reuse checksum-success path and mod migration; inspect remaining RNG/reset semantics |
| Graphics upload | Fixed `$C36A`, writes `$2007`; VM syscall `$0A` | Use existing loader mapping to resolve computed arguments and screen composition |
| Sound cues | Bank14 table `$8608`, 52 contiguous entries | Use the completed sequencer; identify missing callers rather than decode the grammar again |

These addresses are build-/bank-specific references, not unbanked universal addresses. Existing source maps distinguish inspected, bounded source-executed and naturally player-reached routes. Preserve that distinction when importing them.

### 3.3 The old UI inventory is not the latest verdict

The 16 surfaces are: setup; strategic HUD; ruler/prince/child/Outstanding Man details; Train; command cost variants; insufficient BP; Rest quote; Rest interaction/result; own/foreign View; Draft/Demobilize/Help; War confirmation; Battle Leader; events/AI results; save/load; geometry/numeric formatting; HD presentation.

Later NOB-0008 and EXEC0013–0015 results override older inventory labels for the cases they actually completed: native Rest, Train/Help, own-character displays, Self/Prince war quotes/cancellation, shared-BP save/load/continue, HD main/Rest/Train and the new invasion/retreat/native-save route are accepted. Do not turn stale UI07 “NOT RUN” or UI14 “NOT RUN” wording into a request to repeat those cases. Remaining routes include paid foreign View, multi-country Move/Policy, full Treaty, full child/Outstanding Man views, complete battle Leader/command screens, untouched option settings and their combinations. A battle transition capture alone does not close every battle information panel.

## 4. Accepted economic specification — import, do not investigate again

The v1.07 economic master and its numerical tables/source replayers are implementation inputs. The formulas below consolidate their decisive rules; the original detailed appendices remain provenance and edge-case support. There are no new balance choices here.

### 4.1 State model and integer behavior

Country record base is `$6DCD + 83*i`, zero-based country index. Offsets are hexadecimal. Ordinary numeric fields below are little-endian words unless stated otherwise.

| Offset | Field |
|---|---|
|14 / 16|Arms / Skill|
|18 / 1C / 20 / 3A / 3E|Troops / Town / Masons / Artisans / Foodworkers|
|1A / 1E|Economy / Defense|
|22 + 2*i, i=0..9|Ten goods in the order in §4.2|
|36|Food|
|38 / 3C|Commodity / Food histories|
|40 / 42 / 44 / 46|Gold / Morale / Tax / Extra-tax-used|
|48, byte|Draft eligibility percentage|
|49, unaligned word|Undrafted resident reserve|
|4B / 4D / 4F|China / Islam / Uighur presence|
|51|Officer pointer|

P = sum of the five allocated occupations; it excludes undrafted reserve. Eligibility-derived headcount is not another population pool. Baseline P103 + reserve34 = 137 residents. Baseline first-turn values include Troops61, Town5, Masons4, Artisans6, Foodworkers27, Economy185, Defense115, Arms110, Skill124, Gold439, Food208, Morale118 and Tax35. Different starts/versions have different initial values; never overwrite them with this fixture.

Globals: country count `$65B9` (14/27), year `$65BB`, season `$65BD` (Spring0 through Winter3), difficulty `$7A88`, Rate `$7A8A`, actor-country `$7B34`, order word `$7B36`. Full character abilities are six words in L/J/P/C/Body/Battle order.

Plain VM arithmetic wraps modulo65536. `$DA67` uses wide multiplication/division; `$DA8B(a,b)=floor(a*b/100)`; `$DA99` adds as16bits then caps999; `$DAAD` subtracts `min(request,current)`. Preserve the actual sequence of wrap, floor and cap operations. Do not algebraically simplify Extra Tax or population rounding into a different game.

### 4.2 Merchant bases, quotes and stock

| Item | China | Islam | Uighur |
|---|---:|---:|---:|
|Fur|10|4|8|
|Silk|16|37|25|
|Jewels|32|28|25|
|Metals|18|22|10|
|Medicines|14|8|15|
|Woodworks|11|14|7|
|Ceramics|13|15|18|
|Glass|8|5|20|
|Textiles|20|15|31|
|Perfume|18|29|38|
|Food|10|11|11|
|Daggers|20|15|25|
|Swords|80|75|82|
|Axes|100|110|90|
|Lances|40|40|35|
|Pikes|60|70|65|
|Bombs|150|200|250|

For base T, quotes in tenths of Gold are:

```
buy = floor(T*(Rate+100)/100)
sell = max(1,floor(buy*90/100))
hire = floor(60*(Rate+100)/100)
buyDebit = ceil(quantity*buy/10)
sellCredit = floor(quantity*sell/10)
maxBuy = min(999-stock, floor(10*Gold/buy))
maxSell = min(stock, floor(10*(999-Gold)/sell))
```

Positive transactions cost one order and five Judgment in the six-stat profile; shared BP uses the corresponding cost row. Browsing/cancel/refusal is free. A small positive sale can legitimately credit zero whole Gold. Sell inventory has 11 columns; weapon purchases change Arms, not a separately sellable weapon stock.

Merchant presence is per-country; offers are global boolean masks, not depleting quantities. Presence refresh uses the source chance table plus `floor(Economy*Morale/20000)` and the original comparison/draw order. The old divisor50000 transcription is superseded. Reuse the decoded presence/offer generators and their tables; do not infer availability from current quoted prices.

Weapon gain is `floor(9*ChinaBase[item]*(quantity+1)/P)`, capped at Arms999, even when bought from another merchant. Preserve the literal `q+1`. Quantity is bounded by affordability and the original Arms/headcount limit. Hire adds troops without subtracting reserve; quality averages use old P, not just troops, with newcomer Arms/Skill100 and Morale `50+rand(11)`.

### 4.3 Tax, relief, labor, draft and training

Tax requests are1..100; zero, same-rate and invalid cases follow their known no-op rules. A different requested increase can still complete an order when output rounding leaves actual Tax unchanged. Lowering to r adds `(oldTax-r)*(2+rand(3))` Morale, capped999. Increasing by d uses signed margin `floor(Leadership/5)-r`: response100% if margin>50; otherwise40+rand21 if positive; otherwise10+rand11. Actual increase is `floor(response*d/100)`, but Morale falls by the full requested `d*(2+rand3)`. There is no immediate Gold receipt.

Extra Tax maximum is `min(Economy,1+floor(floor(Economy*(1+rand100)/100)*(Leadership+Morale+Charm)/3000))`. Valid q adds q Gold and `floor(q*(1+rand100)/100)` Food, capped999. It lowers Economy byq, marks use, and transforms Morale and target P using:

```
f(E,x,q) = floor(x/2) + floor(floor((((E-q)*x*10) mod65536)/E)/20)
```

Preserve both16-bit multiplications. Scale five occupation groups with original independent flooring until within target P. Removed workers do not enter reserve; P0 clears Arms/Skill. The extra-tax flag resets at **Autumn harvest**, not Spring. Earlier “Food+1 unexplained” and “Spring reset” tasks are closed.

Gold gift value isq. For Food or the country's native specialty Item, average the three sell quotes, then `V=min(999,floor(q*average/10))+1`. Morale gain is `floor(30*V/P)+1`, capped999; the shared source gives an additional10 to a nonhuman country. Give Item is not an arbitrary inventory picker. Positive gifts at the cap still cost resources/action. The earlier simple Food-step approximation is superseded.

Labor moves allocation between the five jobs and conserves P; use the established edit-buffer/commit, zero/max and quality rules. Draft moves eligible reserve into Troops and uses the established newcomer-quality dilution; it is not Hire. Train People raw gain is `2+rand2+floor((Leadership+Battle)/600)` with difficulty factor `100-6*min(5,u16(difficulty-1))`, cap50; reserve0 can still complete and charge the command. Train Troops raw gain is `15+rand11+floor((Leadership+Charm+Battle)/40)`, size adjustment `floor(999*raw/(P+999))`, difficulty factor `100-4*min(5,u16(difficulty-1))`, Skill cap999. Already-capped training can still cost the action. Preserve the already mapped branch-specific gates, not a general “no visible change means no charge” rule.

### 4.4 Season, economy, population and RNG

At season end, add Foodworkers/Artisans to their respective histories, cap999; update Economy and Defense as old minus `floor(old*4/100)` plus Town/Masons, cap999. Refresh offers/presence in the mapped order, advance season/year, update Rate, run the next-season dispatcher and then the outer AI/event loop. Do not label treaty transfers or theft as ordinary production.

Autumn adds the full accumulated histories to Food/native-specialty stock, caps999, then clears histories and extra-tax-used. There is **no division by four**. Spring Food upkeep uses allocated P: consume `floor(P/2)` if supported; otherwise apply the original repeated group-wise shortage scaling and finish Food0. Reserve is not in that upkeep P.

After shortage, Spring taxation/payroll is:

```
a = floor(Economy*Tax/100)
factor = 12 + min(13,floor(Morale/9))
grossTax = floor(a*factor/25)
available = oldGold + grossTax
newTroops = min(oldTroops,available)
newGold = min(999,available-newTroops)
```

Gold caps after payroll. Then the mapped Morale decay and allocated population growth run. Ordinary P growth is3%, cap999; below20 add one Troop; distribute growth using the documented group order. AI can receive its separately established conditional growth bonus. Reserve grows independently by `floor(oldReserve*5/100)+1`, cap999; eligibility has its own original formula. Summer Skill is `floor(Skill*93/100)`; Winter Arms is `floor(Arms*97/100)`, not subtraction of a separately rounded percentage.

Rate is one global index. Spring `50+rand(94+7*difficulty)`; later seasons subtract `floor(Rate*(1+rand9)/100)`, floor50. The direction draw `rand1` is always0 in the original operand path. Source RNG is48-bit big-endian at `$84..$89`: `s'=-((s|1)*0xE0900E0187C5) mod2^48`; return `(s'>>32)&0x7FFF`, then modulo n. Modulo0 returns0 without a draw. NMI can advance part of the seed when `$8A=0`; input/frame scheduling can therefore matter. Native cold-save RNG restoration is a different question and remains G10, not an excuse to redo the known RNG function.

Seasonal storm/cold/sickness/epidemic economics, loss percentages, theft, warnings, revolt predicates and difficulty differences are already decoded in v1.07. Import the existing source/table results. A revolt can enter civil war: its **battle outcome** remains G04. Do not replace it with a fabricated flat penalty.

### 4.5 Logistics, AI economics and scenario data

Owned-source/adjacent-owned-destination constraints, item/Gold/Food transfers, capacity999, zero/cancel/refusal and exact conservation are established. There is no general percentage transport tax. Move People transfers a selected allocated group; reserve, eligibility and production histories stay in their original countries. Emptying a source can clear its Arms/Skill before destination averaging; preserve that order. Move Base swaps the relevant ruler/governor role and order state; broader autonomous multi-country play still needs G07.

AI economic routines are not assumed identical to player Trade. Their source thresholds, labor allocation, support/subsidies, growth and automatic Food purchase have been mapped; reuse them. The complete autonomous political/war decision pipeline is not implied by those bounded routines.

All **41 economic country templates** (14 Mongol +27 World), specialties and inventories are published in `v1.07/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv`. They describe **loaded templates before setup and seasonal processing**, not universally identical first playable Spring values. Reuse all15 labeled startup cases. Non-economic roster/family/geography/difficulty coverage belongs to G02. Do not infer specialty from the largest stockpile or substitute DOS/SNES data.

## 5. Approved modified-game behavior — ready for Windows implementation

### Demobilize and naming

People → Troops → Demobilize transfers q from Troops to undrafted reserve, with `q <= min(Troops,999-reserve)`. It does not allocate civilian jobs or invent people. Original Draft remains. Success costs one order and L5/P5 in the six-stat build, corresponding to10 shared BP in the combined build. Cancel/refusal charges nothing. Preserve original empty-P quality cleanup and known re-draft dilution. Fonzarelli naming and native saved-name migration are implemented; unrelated custom names/World names are not globally replaced.

Recorded native example: Demobilize41 changed Troops61→20, reserve34→75. A subsequent battle displayed200 soldiers, and the recorded commander retreat returned40 soldiers as4 strategic Troops. This is accepted modified-ROM evidence, not a conjecture.

### Shared BP / Rest

Body is the canonical BP field; compatibility copies synchronize the other five words. Paid commands sum original components once, free commands stay free. Troop Skill, Arms, Morale, tactical Mobility and tactical Pass retain their meanings. The strategic menu replaces Pass with Rest and retires individual Self/Prince stat training.

```
gain = min(maxBP-currentBP, floor(maxBP*33*remainingOrders/100))
```

The selected Rest order is included; one/two/three orders restore33/66/99% before headroom. Yes consumes all remaining orders exactly once. B/No is free and preserves BP/orders/RNG. Held input cannot auto-confirm twice. The supplied build's max999 is an implementation default, not a claim that the Director separately selected a universal cap. At999, uncapped gains are329/659/989.

The mod explicitly recovers the player's named living princes on confirmed Rest; AI recovery replaces each actual original character-training opportunity with one33%-maximum tranche. Do not fabricate an AI three-order budget. Native routine fixtures establish bounded AI/event behavior; natural scheduler selection remains an unanswered integration question, not a rejection of those fixtures.

The mod treats exhausted zeroBP as recoverable and uses its separate death marker; do not silently import that marker into stock death rules. Duel/espionage/epidemic/victory effects are applied according to the modified handlers. Save migration uses saved Body without refilling it, retains naming migration and repairs the compiled cost copy. The existing source retains original generation draws before its synchronization.

**Accepted prior evidence:** 43 instruction checks, nine native Rest cases, twelve isolated native event/AI fixtures, World initialization, real Train/Help/quotes/cancellation, shared-BP native save/load/continue and HD affected screens. These stay accepted. Their precise limitations are preserved, not converted into blanket “NOT RUN.” The Windows port must implement them rather than commission another feasibility study.

## 6. Audio — completed sequencer, exact remaining integration

### 6.1 Consume the implemented module

`execution_014/sound_driver_model.py` is a working independent sequencer, not a full audio output backend. It reproduces four-channel note/duration/envelope/loop/replacement/repeat logic using the private matching sound bank. Its bank14 bytes matched the accepted Demobilize and shared-BP builds. It outputs ordered NES sound-register writes, not PCM samples or a finished Windows executable.

```
driver = SoundDriver.from_rom(private_rom_bytes)
# or SoundDriver.from_sound_bank(private_16384_byte_bank)
driver.queue_cue(cue_id, repeat=True)
for address, value in driver.tick():
    apu.write_register(address, value)
state = driver.export_state()
driver.import_state(state)
driver.request_stop()
```

Call `tick` at the corresponding native driver update, preserve state across cue changes and preserve register order. Exact within-call cycle emission and full APU mixer/phase state are not generated by this high-level module. A sequencer-state snapshot is not a complete sample-continuous audio snapshot.

### 6.2 Known grammar and state

| Token | Meaning |
|---|---|
|00..7D|Note|
|7E|Extend accumulator and continue reading|
|7F|Rest/silence for current duration, retain stream|
|80..BF|Duration selector, low five bits|
|C0..CE / CF|Loop entry / loop end|
|Dn|Alternate ending according to iteration|
|E0/E1|Pulse instrument|
|E2|Triangle gate/instrument|
|E3|Noise tuple|
|FF|End voice|

Pointers/timers use original little-endian16-bit arithmetic; preserve8-/16-bit wrap. Driver resumes after last consumed voice and returns after the next consumed note; it is not “advance every voice every video frame.” Important state: repeat `$B2`; request `$B3:B4`; live pointers `$B5..BC`; start pointers `$BD..C4`; accumulators `$C5..CC`; durations `$CD..D4`; speed `$D5:D6`; load/repeat `$F5`; stop `$F6`; loop indexes `$F7/F9/FB/FD` into `$0731..0770`; last voice `$FF`.

A zero header pointer **preserves** that existing voice; a zero timing byte preserves speed/related melodic state. This allows effects to replace some channels while other music continues. Repeat restarts three melodic channels, not noise; noise does not block that repeat. Empty-header cue0 is not equivalent to explicit stop. Do not substitute isolated WAV-per-cue playback without preserving these semantics.

Accepted comparison: EXEC0014 matched33,419 native calls and55,814 ordered within-call writes plus state. EXEC0015 reused the same module against21,832 further calls and26,168 within-call writes. All52 entries have modeled complete playback;40 reached native end in the sweep,12 have native prefixes. Do not replay the twelve solely to improve counts; investigate a missing tail only if it introduces a new behavior or an actual output mismatch.

### 6.3 Event routing already known

| Cue | Event/use | Setting | Source |
|---:|---|---|---|
|1|Scenario selection|Music|EXEC0013/0014|
|17|Opening/title|Music|EXEC0013/0014|
|6|Strategic scene; resumes after tested retreat|Music|EXEC0013/0015|
|32|Train Troops result|Sound|EXEC0013|
|30|Give Food result|Sound|EXEC0014|
|20|First observed invasion-departure effect|Sound|EXEC0015|
|48|Second observed invasion-departure effect|Sound|EXEC0015|
|5|Battle overview/deployment/field background|Music, repeating|EXEC0015|
|15|Commander Flee/retreat-result transition|Music, nonrepeating|EXEC0015|
|41|Normal-attack animation; five requests at22-frame intervals in eight watched cases|Gate not yet identified; both settings On|EXEC0018|

The two invasion effects are sequence-specific labels, not invented sound names or claims of exclusive use. Music and Sound settings are independent and survived native saves. Do not repeat those toggle tests. The following matched modified-ROM routes are implementation fixtures:

- Train Troops:86 sound-register writes with Sound on,0 with Sound off; equal gameplay/RNG. Result sound precedes the final observed acknowledgement debit.
- Give Food1:157 writes on,0 off; Food208→207, Morale118→119, BP100→95, Orders3→2, identical RNG. Timed feedback can finish the command without another acknowledgement. Training's timing is not a universal commit rule.
- Invasion requests with both on:20,0,48,0,5; Sound off:5; Music off:20,48.
- Retreat with Music on, whether Sound on or off:0,15,6; Music off:no requests. Cue15 is Music-gated despite being an action outcome.
- In the recorded input schedule, Sound-off reached battle music144 bridge frames sooner. Do not require identical presentation time when sound is disabled, or generalize144 into every action.

**Remaining cue bookkeeping:**52 table entries are not52 identified songs. Ten have gameplay associations after EXEC0018. Cue0 is a known structural empty header. The41 remaining caller classifications are:
`2,3,4,7,8,9,10,11,12,13,14,16,18,19,21,22,23,24,25,26,27,28,29,31,33,34,35,36,37,38,39,40,42,43,44,45,46,47,49,50,51`.
Some may have shared uses or be unreachable in the selected profile. Resolve and record that instead of manufacturing52 song titles. Cue0 reclassification was bookkeeping; cue41 is a new native association. Its gate and additional uses are not resolved merely by naming the animation.

## 7. Battle and persistence — reuse the completed route

EXEC0015 ran actual controller navigation in the combined shared-BP ROM: Mongol Conquest, Spring1175, country1/Fonzarelli invading country5/Targutai.

| State | Gold | Food | Troops | Reserve | BP | Orders |
|---|---:|---:|---:|---:|---:|---:|
|Before War|423|208|61|34|100|3|
|After Self invasion accepted|403|198|41|34|25|3|
|After commander Flee and return|403|198|45|34|25|2|
|After native Save, shutdown, new-process Load|403|198|45|34|25|2|

The overview showed200 attacking soldiers versus350, tactical Food100 versus430, and90 days. Five40-soldier attacking units occupied slots0,1,2,4,7 and were controller-deployed legally. Early commander Flee returned its40 soldiers as4 strategic Troops; the rest of the expedition did not return. Reserve/labor were unchanged. B-cancel of the Self confirmation preserved resources, full compared country/ruler records, BP/orders/RNG.

**Commit timing:** BP75 and expedition resources were deducted before battle. The strategic order remained3 while in battle and fell to2 on return. Native displayed footer values could briefly lag RAM during animation. Do not deduce all commit timing from a screenshot alone. This is the captured early-retreat path, not a general proof of every outcome.

**The prior reconstructed-save gap is closed for this build/route.** This time natural battle output was followed by native “Data saved,” process exit and a different process loading the battery, without restoring a reconstruction. Country/ruler records, resources, troops/workers, Arms/Skill, BP, orders, date and both audio settings matched. Battery SHA-256:
`aff390ef554415fce338aa4918e03205157ab4928cd61c83546f00d20659c9dc`.

RNG differed: `d14c2f94e0e9` before Save versus `063206320000` after cold Load. Record the difference; do not claim native RNG continuation, and do not replay the entire invasion to investigate it. G10 isolates only the outstanding save/initialization rule.

EXEC0015 alone does not establish normal attack damage, victory, all retreat variants, all terrain or combat AI. EXEC0018 now supplies the normal-melee numeric contract below. The other branches remain open; none invalidates the successful invasion/retreat/save route.

### 7.1 Normal melee — implemented arithmetic, new EXEC0018 evidence

`execution_018/normal_melee.py` now supplies the independent normal-melee numeric kernel. Nine new controller-driven attacks from the existing in-battle checkpoint matched it; one target-selection cancellation preserved all20 unit records, strategic resources, BP/orders and RNG. Forty-four additional bounded source-instruction vectors cover arithmetic/attribute/word boundaries. These are new unanswered combat findings, not repeats of completed invasion/retreat/save or economy work.

**Scope and fixtures:** the combined modified ROM in §2 was used. Enemy unit11 was positioned adjacent to commander unit0 through a disclosed x/y write at`$60DB`; other cases change only named input type/Skill/Arms/terrain/count. Game code and computed losses were not patched. The castle logical-terrain case does not certify natural castle generation/graphics. The killed target was a noncommander; commander capture/victory remains G04.

| New native case | Attacker/defender remaining soldiers |
|---|---|
| Cavalry vs cavalry, forest defender |24 / 60 from40 /70|
| Infantry attacker |26 /57|
| Infantry defender |24 /61|
| Archer attacker |26 /57|
| Attacker Skill102→500 |25 /59|
| Attacker Arms124→500 |26 /58|
| Plain defender |23 /63|
| Castle defender |20 /67; original defending-country Defense91→83 first|
| One-soldier defender |39 /0 from40 /1|

**Records:**count pointer`$6060+11*i`,20 slots; relative count+0,x+2,y+3,type+4,mobility+5, preceding flag byte. Side descriptor`$6001+27*side` holds leader pointer+0,role+2,country+3; Skill at`$6037+2*side`, Arms`$603B+2*side`. Terrain`$615C+16*y+x` masked7F for logical16×10 map. Confirmed type IDs0 cavalry,1 infantry,2 archer; terrain2 plain,3 forest,6 castle. Do not invent other labels.

**Source helpers:**bank5 strength`$8061`, defensive factor`$8024`, share`$80D4`, casualty`$811E`, normal numeric entry`$815F`, outer normal command`$81E9`. Ordinary word arithmetic wraps16bits. `wide(a,b,d)` is word inputs,32-bit product, low-word quotient; divisor0 givesFFFF. `pct(a,b)=wide(a,b,100)`.

```
sk = floor(Skill/10)+1
ar = floor(Arms/5)+1
trained = u16(pct(N,100+sk)+pct(N,100+ar)) >> 1
leaderFactor = 1090 if role==102 else u16(1000+Battle+(Leadership>>1)+(Body>>2)+(Judgment>>1))
strength = u16(trained+wide(N,leaderFactor,1000)) >> 1
roleBonus = 2 if role==100 else 1 if role==101 else 0
```

Matchup matrix (attacker type rows, defender type columns) is`[[3,5,5],[4,3,3],[4,4,3]]`; add attacker role bonus. Defender-type rows / terrain0..5 matrix is`[[1,1,3,2,1,5],[1,2,3,4,5,5],[1,2,3,4,5,5]]`; add defender bonus. For castle6 first subtract `5+rand(4)+floor(pct(attackerN,100+attackerArmsBucket)/30)`, saturating at0, from the **original defending side's country Defense**. Then use `3+rand(floor(updatedDefense/7)+1)+defenderBonus`. The side-country reference is not interchangeable with the current receiving unit's owner.

```
A = u16(attackerStrength*attackMultiplier)
D = u16(defenderStrength*defenseMultiplier)
shareA = 50 if A==0 and D==0 else clamp(wide(A,100,u16(A+D)),1,99)
shareD = 100-shareA
# Resolve attacker first, then defender, each with its own rand(24):
other = 100-ownShare
scaled = pct(N,87+rand(24))
ratio = wide(scaled,other*other,ownShare*ownShare+other*other)
loss = min(N,u16((ratio>>1)+floor(other/ownShare)+1))
remaining = N-loss
```

The square applies to the percentage shares, **not raw Skill squared**. The two sides use already computed shares, not recomputed strength after the first loss. The original calls zero-unit handling between casualty stages. The portable kernel returns zero flags but **does not implement terminal control-flow, leader capture or battle victory**; integrate those handlers without blindly committing both losses when a commander-zero branch can unwind the command.

**Direct use:**`normal_melee(attacker, defender, game_rng.rand, defending_country_defense=...)` returns strength, multipliers, shares, losses, counts, fortification changes and consumed random bounds/values. Import immutable `Fighter` records and preserve the selected ROM profile's leader attributes. `test_normal_melee.py`, nine portable native cases,44 source vectors and a raw-memory evidence verifier are supplied. New component checks reject a wrong coefficient; native-binding checks reject changed/rehashed troop data and a missing response.32 snapshots bind to456 complete request/response pairs. No Windows executable was run.

**Presentation harvested at the same time:**cue41 pointer`$96AC` appeared five times per watched normal attack at22-bridge-frame intervals, before casualties. Both audio settings were On; its exact gate remains G09. A separate exploratory move into forest changed40→34 troops and Mobility10→6; retain it as an observation, **not** a completed movement-loss formula.

**Completion:**normal-melee numerical rules and the bounded castle prelude are implementation inputs now. Nobunaga integrates them; Cao Cao compares the new Windows component to existing fixtures. Zhuge Liang continues only missing legal movement/ranged/special-command and terminal rules. The detailed report and private logs preserve all interventions, unsuccessful navigation and source-probe failures.

## 8. Graphics and emulator resources already available

### Graphics

The separate `genghis-khan-assets` branch contains64 bank11/12 portraits,9 static tile sets,19 attribute tables, five observed palette sets and the loader inventory. Full portraits use30 tiles in a5×6 layout,480 bytes each; bank12's first portrait matched its native1920-pixel reference. Preserve that completed extraction, not a second portrait project.

There is CHR-RAM, not CHR-ROM. The fixed uploader `$C36A` copies through `$2007`; zero-page `$52` is source bank, `$54/55` source pointer, `$56/57` PPU destination, `$58`16-byte-unit count. Portrait destination is `$1E20`, and the order-screen nametable uses column10,row6. Of75 discovered upload sites,32 literal sites yield28 unique loads;43 computed sites remain. Their missing arguments/layouts, bank10 faces, battle/event terrain, sprite OAM/priority/flip, fonts and animation belong to G08.

The approved four-season HD maps/Fonz portrait are real assets. The latest BP-compatible pack preserved26 files and changed the enclosing ROM-matching basename; that repackaging is not another art extraction. Native affected main/Rest/Train screens are accepted. Missing four-season/live-ownership/overlay combinations stay explicit. Do not confuse a static composite with live province ownership or a whole-screen HD success with every menu.

### Emulator access and capture reuse

The headless Mesen/AI environment, setup recovery, first-run-wizard fix, controller pulses, screenshots, CPU/RAM and checkpoint methods already exist. Do not report “no emulator” solely because a new temporary shell initially lacks the executable. Restore the saved exact runtime when a genuinely new capture is needed. Keep stock/mod save namespaces separate and apply the recorded SOROM mapping correction for the runtime that needs it. A pending asynchronous restore is not a completed restore; confirm the restored state before a new experiment. Never modify the Director's live campaign.

Existing sources: `reference/MESEN_HEADLESS_CHATGPT_OPERATIONS_v1.00.md`; `research/Genghis_Khan_NES/tools/headless_mesen_campaign_test.py`; v1.07 `runtime_tools`; private EXEC0015 scripts/bridge/checkpoints. These are enough to resume unknown routes. Recovery smoke checks required to launch a new environment are infrastructure checks, not renewed scientific qualification of every known mechanic.

## 9. Other old material recovered and correctly bounded

The latest separate original Temujin campaign ledger selects **Summer1178, 13 of80 seasonal transitions**, not a completed20-year campaign. It contains natural training, hires, labor, gift, commodity trades, seasonal transitions and an accepted proposal. Reuse those observations where relevant. A competing later Spring1179 branch was not verified as a continuation of the selected checkpoint; do not combine those state histories or overwrite the Director's selected save. Likewise, an older different run's Spring1186 checkpoint is not this campaign's continuation.

The selected Summer1178 native checkpoint is SHA-256 `994b677eb6f4fb3b86b14b583a83976b7838a656c9060dcc05f2d516e087ea66`, with Gold445, Food263, Troops91, Morale129, three orders and six abilities `[468,119,116,119,170,129]`. This establishes those observations, not all AI decisions, full diplomacy probabilities or a campaign ending. No need to finish an arbitrary20/30-year run merely to rediscover economics already settled.

Old military manuals/FAQ summaries provide command inventories and candidate leads, not exact damage formulas. Distinguish the off-screen resolution routine from live tactical combat. A folklore skill-squared claim or an unbanked address is not a substitute for the missing implementation rule. Use the existing source audit and call maps before taking a new capture.

## 10. Remaining implementation plan — only unanswered work

Each package below specifies the missing answer, existing material to start from, execution steps, receiving Windows component and finish condition. Start by reading the cited existing data. Do not make re-running all historical tests a prerequisite. For new discoveries, record enough precise evidence to support the new rule; for new Windows code, compare against the already accepted fixtures. Each package ends in data or working game behavior, not just another narrative report.

### G01 — Four unnamed cost callers and uncovered actor/command paths

**Reuse:**34-row table, recovered Prince/Self labels,80-patch index, actor/cost anchors,16 UI surfaces, existing charge/cancel traces. **Missing:**row16–19 identities/reachability; payer resolution and timing in still-unvisited variants; remaining original Self/Prince outcome/gating details not established by old source/campaign data.

**Do:** inspect existing source/VM dispatcher references to the four row selectors and follow actual callers; distinguish player, AI-only, retired and unreachable entries. Export the source name/row/actor/subject/commit/refund mapping. For unknown reachable branches only, use a checkpoint already positioned at the relevant menu or source call. Preserve known World/setup and original cost findings. Do not create a UI command merely because a table row exists.

**Deliver:**`CommandRegistry`, `ActorResolver`, `command_variants.json` with input, output, prerequisites, expense and commit stage. **Finish:**all34 rows are identified or explicitly classified with evidence, and each implemented command charges the right actor at the established stage. Owner Zhuge Liang → Nobunaga; Cao Cao reviews the new mappings. No dependence on sound/campaign completion.

### G02 — Non-economic scenario, roster and map completion

**Reuse:**all41 economic templates,15 startup cases, known country names/specialties, native scenario starts and mod initialization/record layouts. **Missing:**remaining family/prince/child/non-economic records, full selectable-ruler/difficulty initialization, adjacency/coordinates/ownership data and world/end-state transitions.

**Do:** extract already located scenario/character tables and original setup transforms rather than treating raw templates as first-turn state. Resolve full vs compact records and field identity from existing mod source. Build complete scenario factories and graph data, with canceled/manual/automatic setup contracts. Investigate only unrecorded setup branches and end-state conditions. Keep profile-specific generation and death rules distinct.

**Deliver:**`ScenarioFactory`, immutable scenario/roster/topology datasets and `CampaignEndState`. **Finish:**every supported start can initialize correctly, ownership/routes can change, and all source-defined win/loss/extinction/succession endings have an implementable transition. Owners Zhuge Liang for extraction, Nobunaga for factories; dependencies G01 only for actor-role integration.

### G03 — Tactical movement, attacks and battlefield commands

**Reuse:**actual battle checkpoint, known90-day case, five-unit structure, sound/music hooks, military command inventory and **EXEC0018 normal-melee numeric kernel and nine native fixtures**. **Missing:**legal movement Entire/Divide and target selection; duels, arrows, ambush, Pass, Demand, SOS, Raid/Hunt, alternative Flee; remaining terrain/range/weather rules; full command orchestration and commander-zero/terminal branches. The normal-melee strength/share/casualty math is no longer a discovery task.

**Do:** start from the accepted in-battle checkpoint, not another repeated invasion. Import the new normal-melee kernel directly. Locate missing movement, ranged and special-command handlers, their legal targets, unit records, RNG and result stores. Resolve source operations first and capture only an unanswered branch. Connect zero-count handling to G04 rather than assuming the numeric kernel decides victory. Map path/range legality, splitting and presentation together; harvest new audio/graphics calls during these routes.

**Deliver:**`TacticalRules`, `LegalActions`, `Damage`, `BattleCommandStateMachine`, terrain/map data and command-to-presentation events. **Finish:**all listed tactical commands have usable rules and the Windows player can conduct a real offensive/defensive battle. A new attack equation must explain its boundary/rounding cases; one retreat capture never substitutes for damage. Owners Zhuge Liang/Nobunaga; independent implementation QA Cao Cao. Critical dependency for G04/G12.

### G04 — Battle outcomes, logistics, auto-resolve and combat AI

**Reuse:**EXEC0015 entry/return ledger and early-Flee timing; EXEC0018 normal-melee zero flags and captured noncommander elimination; known manpower/economy formulas and original AI source leads. **Missing:**victory/defeat/surrender/starvation/time-limit outcomes; prisoner/ruler/prince handling; annexation/governor changes; captured goods; reinforcements; remaining food/day accounting; Prince/off-screen resolution; AI attack/defense decisions and rebellion war outcomes.

**Do:** follow the outcome dispatch and persistent writes from source. Reuse common end-of-battle cleanup once identified; inspect only distinct terminal branches. Separate live tactical AI from the delegated/off-screen engine. Enter a missing terminal state via a disposable explicit fixture when natural play would add no new knowledge, labeling it accurately; do not present fixture injection as a full natural campaign. Capture resulting ownership/prisoner/stock effects and subsequent seasonal consumption.

**Deliver:**`BattleOutcome`, `BattleEnterExit`, `AutoResolve`, `CombatAI`, and postwar ledger. **Finish:**normal and alternate endings genuinely change the campaign with no duplicated/lost state, and modified/original profile differences are explicit. Owners Zhuge Liang/Nobunaga; dependencies G02/G03, no repeat early-Flee/save requirement.

### G05 — Diplomacy, tribute, spying and information access

**Reuse:**cost rows20,25–31, existing incoming-alliance observations, source cost/actor maps, bounded modified event routines. **Missing:**target eligibility, result probabilities/conditions, tribute timing, alliance duration/expiry/renewal, vassal relations, spy fees/detection/retaliation and foreign-information permissions.

**Do:** trace Treaty and Spy success/refusal branches, relation storage and season-expiry reader. Recover fee constants from source instead of adopting the old unverified10/50/100 guide values. Map each command's effect to AI response and View availability. Use existing accepted incoming offers for already known transfer timing; capture only missing player branches, expiry or detected/undetected outcomes. Follow the logged event calls for any new audio associations.

**Deliver:**`Diplomacy`, `SpySystem`, `IntelligencePermissions`, relation/spy save fields and message/event IDs. **Finish:**agreements can be made/refused/expired, spies cause exactly documented effects, and the UI never leaks inaccessible information. Owners Zhuge Liang/Nobunaga; actor dependency G01 and topology G02.

### G06 — Dynasty, people management, death and succession

**Reuse:**29-byte/full and compact-record research, known stat readers/writers, mod generation/death/normalization source, natural original training/campaign observations. **Missing:**Select/Demote/Promote/Marry/Switch transitions, parent/child/heir eligibility, birth/aging and actual succession/extinction, remaining training branches, multi-country role changes.

**Do:** map record creation, removal, compaction and ownership inheritance. Preserve original six-stat predicates separately from modified zero-BP survival/age-marker behavior; do not rediscover the already implemented modification. Resolve branch conditions and copied fields from saved original slices/source maps first, then capture genuinely unrecorded transition or message behavior. Include death during espionage/battle versus ordinary aging where handlers differ.

**Deliver:**`Dynasty`, `PeopleCommands`, `CharacterLifecycle`, complete identity/owner links and succession save transitions. **Finish:**no stale references after death/promotion, an eligible heir actually continues play, and no-heir defeat follows the correct profile. Owners Zhuge Liang/Nobunaga; dependencies G01/G02, G04/G05 for trigger integration.

### G07 — Full strategic AI and delegated policy

**Reuse:**closed economic AI rules, original weakest-stat/training call path, modified per-opportunity recovery, existing seasonal/campaign logs and Policy source anchors. **Missing:**the full action order, priorities and target choice; natural scheduling of some known routines; domestic/foreign Policy controls, vassal activation and cross-system decisions.

**Do:** assemble the source scheduler/call graph and state predicates before running turns. Feed known economic decisions into it instead of inventing “balanced” AI. Document role/difficulty exceptions and policy state. Observe only missing natural scheduler/decision branches, not already proven isolated routine arithmetic. Use a disposable multi-country position, preserving the Director's separate campaign and avoiding an arbitrary years-played quota.

**Deliver:**`StrategicAI`, `GovernorPolicy`, traceable action priorities and next-actor/event scheduler. **Finish:**AI takes real autonomous economic, political and military actions through the same persistent world; no fixture-only idle AI and no invented three-order budget. Owners Zhuge Liang/Nobunaga; consume G04–G06 as available, do not hold the economic slice pending full AI closure.

### G08 — Remaining screen, sprite, map, text and option behavior

**Reuse:**all extracted assets, mapped static upload sites,16 UI surfaces with later passes, original palette/portrait matches, approved HD images and known scene geometry. **Missing:**bank10 faces;43 computed upload arguments; remaining nametables/OAM/fonts/text tables/animation sequences; paid/foreign/child/Policy/Treaty/battle views; Wait/Display/Animation/Battle option semantics and numeric/long-name edge layouts.

**Do:** resolve computed loader arguments from known call sites and read existing captured PPU state before capturing anew. Produce screen IDs, asset IDs, layer/flip/priority/coordinates, text width and event triggers. Extract dynamic ownership/flag layers rather than baking them into backgrounds. New captures should target missing distinct scenes and option effects only. Compose the Windows scene with matching glyphs, timing and control focus; no scrolling PC webpage substituted for a game screen.

**Deliver:**`SceneManifest`, `NESAssetAtlas`, `MessageCatalog`, `MenuStateMachine`, `Options` and separate approved HD routing. **Finish:**every reachable distinct scene has assets/data and a defined transition; controller-only navigation covers it; four seasons and ownership changes remain live. Owners Zhuge Liang/Nobunaga; can proceed parallel to combat/diplomacy.

### G09 — Remaining cue associations, not sequencer redevelopment

**Reuse:**working sequencer,52-entry structure, ten event associations, cue0 structural semantics, archived APU/request traces. **Missing:**caller/profile classification of the41 IDs listed in §6.3, cue41 option gate, and additional uses of known entries.

**Do:** first search existing raw cue/request logs and source call sites, including the mod's relocated routines. Reuse an already captured event whenever available. Identify which branches request each ID and whether a value is shared, structural or unreachable. Only drive a new game route for an otherwise unanswered trigger/gate/timing question. Collect missing battle/diplomacy/succession sounds while executing G03–G07. Do not request every cue in isolation again: that playback work is finished.

**Deliver:**`AudioEventMap` rows with cue, scene/condition, Music/Sound gate, repeat/priority, interruption behavior and receiving Windows event. **Finish:**all52 structural entries are categorized, with genuine event uses resolved or explicit evidence-backed non-use; no invented52-song requirement. Owner Zhuge Liang; Nobunaga integrates progressively.

### G10 — Audible output and remaining persistence/reset semantics

**Reuse:**sequencer API/ordered writes, known native Save/cold-Load fixture, saved settings and recorded differing RNG bytes. **Missing:**faithful APU/PCM output and mixing; within-update scheduling where perceptible; full APU versus sequencer save state; native RNG/init/reset rule; untouched save permissions/options/corruption behavior.

**Do:** integrate a suitable NES APU renderer with the completed sequencer. Preserve update cadence, channel overlap, output order and mixer/filter assumptions. Use a supported native/frontend audio-capture path or reviewed runtime extension for missing waveform evidence; the old Lua dynamic-library WAV attempt failed, so do not retry it unchanged. Inspect saved-byte ranges and successful Load/init code to explain the RNG difference, without repeating War/Flee. Reuse old saved batteries and only capture a new missing branch if source/range inspection cannot resolve it. Separate native fidelity from explicit Windows safety improvements.

**Deliver:**`AudioRenderer`, audible reference/output comparisons, `SaveGameCodec`, versioned profiles, atomic-write/recovery behavior and a documented RNG/audio resume policy. **Finish:**Windows sounds correct and saves reliably; no claim of native RNG continuation unless actually established. Owners Nobunaga for backend/save implementation, Zhuge Liang for remaining source questions, Cao Cao for new-port comparison. Can run parallel to G09.

### G11 — Integrate the Windows game now

**Reuse:**the entire accepted specification and code/data above. **Missing:**an independently executed integrated Genghis Windows game; not another research report.

**Do:** identify the dedicated Genghis source tree and source version; do not mix it into ROTK II or another game. Implement the original/profile state model, scenario factory, original art/menu shell, cost/actor/command registry, existing economy, season progression and new Windows save service. Import the shared-BP/Demobilize behavior according to the selected approved profile. Wire the sound sequencer to the audio event interface immediately even if PCM output is temporarily pending. Add actual combat/diplomacy/AI/dynasty modules as their missing contracts close.

**Deliver:**successive runnable Windows builds, with source/build/version identity visible and controller-only title/new/load/menu/numeric input/quit routes. **Finish of first slice:**a real order changes persistent state, a season advances, save/quit/cold-load continues it. That is an integration milestone, not complete release. Owner Nobunaga; Cao Cao checks the new executable against existing known references. No fresh blanket NES retest gate.

### G12 — Complete campaigns, packaging and acceptance

**Reuse:**all established original/mod behavior and exact fixtures; no reset of prior evidence. **Missing:**integrated scenario completion, player-visible coherence and real Windows/controller packaging acceptance.

**Do:** connect G02–G11 into both supported scenarios and all source-supported choices. Complete win/loss/succession outcomes in the Windows program, not just fixtures. Check that graphics, menu text, audio and saved state agree as the world changes. Validate installation, launch, version visibility, controller reconnect/focus, save recovery and continuing a real campaign on Windows. Preserve failed observations and fix the implementation; do not reopen a settled ROM rule unless a concrete contradiction appears.

**Deliver:**release candidate, reproducible player paths and exact remaining-defect list. **Finish:**no gameplay-critical unknown or broken route is concealed; Director can play, save, quit, load and finish the game with the agreed controls and fidelity. Native hardware/Windows checks are new product acceptance, not repeats of old emulator experiments. Owner Nobunaga with Cao Cao QA; Director final acceptance.

## 11. Complete coverage of the original 167 IDs

Ranges below are inclusive. Every original ID is accounted for exactly once. “Reuse” means accepted prior knowledge, not that a Windows module has been tested. The remaining-work packages refine only unanswered portions; later modified-build results are not misreported as historical stock-matrix passes. Keep the original CSV unchanged as evidence history.

| Original group / IDs | Reuse and current disposition | Remaining work |
|---|---|---|
|SC01–SC11 (11)|SC07 economic specialty/inventory closed; SC03/06 economic templates and existing setup observations reused; known campaign seasons retained without claiming full campaign|G02 non-economic starts/maps/endings; G07 autonomous progression; G12 complete Windows campaigns; SC11 profile/source management, not renewed experiments|
|TX01–TX07 (7)|All scoped tax rules and costs accepted, including severe16-bit Extra Tax, Food credit and Autumn reset|G11 integration only; G04/G07 handle other systems' consequences without reopening tax|
|AS01–AS06 (6)|AS01/02/06 labor/costs accepted; AS03 peaceful conservation accepted; existing deployment evidence retained|G03 army composition/rounding and command rules; G04 war losses; G11 integrate|
|GV01–GV05 (5)|All gift calculations/gates accepted; new GiveFood timing and cue30 already captured|G11 integration, G09 only unknown different event uses|
|TR01–TR06 (6)|TR01/02/06 accepted; row6 Prince and row7 Self labels recovered; existing natural training and mod-retired-menu behavior retained|G01/G06 only unanswered Self/Prince output/eligibility branches; G11 integrate|
|TD01–TD16 (16)|All scoped Trade/rate/merchants/items/weapons/hire/rounding/logistics/ROI work accepted|G11 integration; no economic replay campaign|
|PE01–PE09 (9)|PE01/02 Draft accepted; all known cost rows, modified record/event/normalization and Demobilize facts retained|G06 remaining people/family/succession; G01 actor/gates; G11 integrate|
|VW01–VW06 (6)|Known own views and modified UI accepted; source layouts/costs reused|G05 information permission; G08 unvisited screens; G06 kinship data|
|MV01–MV07 (7)|MV02–07 established transfers/refusals accepted; MoveBase source behavior retained|G07 broader multi-country activation and G08 UI; G11 integrate|
|PL01–PL06 (6)|No-vassal precondition and bounded economic AI known; not a complete policy system|G07 all unanswered delegation/policy execution; G08 UI|
|DT01–DT07 (7)|Existing proposal/transfer observations and costs reused|G05 target/result/tribute/vassal/duration/expiry/interactions|
|SP01–SP06 (6)|Known source costs and modified isolated event effects reused|G05 full spy results/fees/detection/permissions; G06 death integration|
|WA01–WA08 (8)|Self invasion/cancel, quoted Prince cost, early Flee debit/return and uninterrupted save accepted for tested mod route|G03/G04 remaining attack/outcome/auto-resolve/AI/logistics; do not replay known early retreat|
|BT01–BT12 (12)|Five-unit deployment, early Flee,90-day case, normal-melee numeric rules/nine new native cases and cue41 association accepted within scope|G03 legal movement/ranged/special commands and presentation; G04 commander-zero/terminal accounting/AI; G09 cue41 gate|
|SE01–SE09 (9)|SE02/03/04/06/07/08 accepted; original Pass observations and modified Rest accepted; economic growth/events retained|G06 family aging; G07 full scheduler/event interleave; G04 battle/civil-war effects|
|OT01–OT09 (9)|Native Save/cold-Load and Music/Sound options/persistence accepted in captured profile; RNG difference explicitly retained|G08 untouched Wait/Display/Animation/Battle options; G10 remaining save/reset rules; G11 Windows service; G12 product acceptance|
|CF01–CF14 (14)|CF01/02/04/05 accepted; known cost/ability/AI/graphics/audio findings reused; CF14 evidence separation corrected, not a research restart|G01 remaining actor/cost branches; G03/04 combat; G05–07 world interactions; G08 visuals; G09/10 remaining audio; G02 topology|
|RE01–RE14 (14)|RE01/02/04/06/08/10/12/14 accepted in source scope. RE09 historical incomplete logs remain disclosed; do not recreate history. RE13 SNES comparison is conditional, **not a required gate** for this NES game|RE03 G01 call coverage; RE05 only unknown formulas in G03–07; RE07 G08 missing scenes; RE11 G10 unresolved save details. No repeat suite requirement|
|IM01–IM09 (9)|IM01 target is resolved by Director: standalone Windows game. IM08 source/profile separation is specified. Existing reference functions and artifacts are inputs, not a runnable product|G11 implements IM02–06; G12 completes IM07/09; no remaining design-approval stall for settled scope|

Count:11+7+6+5+6+16+9+6+7+6+7+6+8+12+9+9+14+14+9 = **167**. This table is coverage accounting, not a fabricated new pass percentage. The next ledger update should record each genuinely closed subquestion and its Windows consumer; do not keep using “106 untouched” as the progress figure.

## 12. Execution order and completion control

1. **Completed by this consolidation:**recover accepted records; restore the command labels6/7; reconcile old UI statuses; recognize finished sound sequencing and natural battle/save; separate cue0 from the42 unknown event entries; publish one guide and recovered data. No NES gameplay replay.
2. **Nobunaga starts G11 immediately** with economy, known state/commands/menu/assets, approved modifications and new Windows saves. G01/G02 fill specific missing identity/setup fields without holding every known module.
3. **Zhuge Liang continues G03/G04:**normal-melee math is delivered in EXEC0018; next resolve legal movement/ranged/special handlers and commander-zero/other terminal outcomes, then missing AI branches. Harvest audio/visual events alongside them. G05/G06/G07 proceed from existing source and cases, not duplicated entry tests.
4. **G08/G09/G10 run in parallel where independent:**remaining graphics, cue callers and actual PCM backend. Do not wait for all cue names to integrate the working sequencer, or for full combat to finish the economic slice.
5. **G12 finishes the product:**real complete gameplay, Windows controller/save/audio/graphics coherence and Director acceptance. Do not call a data export or native ROM capture the Windows release.

Each active ledger entry must contain: requirement/subquestion; exact original or mod profile; accepted prior answer/source; remaining unknown; owner; receiving Windows function/data/asset; next executable action; status; actual evidence; blocker. Allowed statuses include NOT RUN, IN PROGRESS, PARTIAL, PASS VERIFIED, BLOCKED, NEEDS DIRECTOR and SUPERSEDED. Keep three independent axes: original research, modified-ROM behavior, Windows integration.

**Stop researching a settled rule.** Once existing or newly obtained information defines the required implementation, move to the consuming Windows code. Reopen only a specific contradiction, changed build or still-uncovered material branch. No generic “more tests” batch, no new report merely restating this guide, and no repeated economy/audio-decoder discovery loop.

No background execution or agent acknowledgment is implied by publication. This document authorizes and directs the work; execution status advances only on actual work. The completion of this consolidation does not close the still-unanswered combat/political/visual/audio questions or certify Windows.

## 13. Exact source and artifact locations

The guide contains the current decisions, essential data and all remaining instructions. These links are supporting implementation files/evidence, not additional plans that must be reconciled by the reader.

| Material | Location |
|---|---|
|Current guide|https://github.com/Bomb-Daddy/NES/blob/main/research/Genghis_Khan_NES/GK1_WINDOWS_MASTER_GUIDE.md|
|Recovered command table with30 names|`execution_017/command_costs_v1.01.csv` beside this guide's research directory|
|Original complete EXEC0012 data (five files)|https://drive.google.com/file/d/1kRiBYBfg8MIcq5_CzSUcvqBCf6sEhrxT/view|
|Existing economic specification and all appendices|https://github.com/Bomb-Daddy/NES/tree/main/research/Genghis_Khan_NES/v1.07|
|41-country economic dataset|https://github.com/Bomb-Daddy/NES/blob/main/research/Genghis_Khan_NES/v1.07/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv|
|Original32-row cost source recovering6/7|https://github.com/Bomb-Daddy/NES/blob/main/research/Genghis_Khan_NES/v1.07/GK1_COMMAND_COST_MATRIX_v1.07.csv|
|Historical167-row register|https://github.com/Bomb-Daddy/NES/blob/main/research/Genghis_Khan_NES/v1.07/GK1_COMPLETE_167_PROOF_MATRIX_v1.07_2026-11-04.csv|
|Demobilize/Fonzarelli source/build/evidence|https://drive.google.com/file/d/1iwsfnYa2ph-d26CMZ4KFIB-F6pXgElGE/view|
|Shared-BP source/build/evidence|https://drive.google.com/file/d/16NysbtEJGeCaVbLzSuEPRWOt8kUvijq9/view|
|Shared-BP final delivery/limitations|https://drive.google.com/file/d/19Z7HbseZUEDYB37_NTxM0j8eZ5sSdkmA/view|
|Native audio options/training report|https://github.com/Bomb-Daddy/NES/tree/main/research/Genghis_Khan_NES/execution_002|
|Sequencer, API, contract and verifier|https://github.com/Bomb-Daddy/NES/tree/main/research/Genghis_Khan_NES/execution_014|
|Invasion/retreat/native-save contract|https://github.com/Bomb-Daddy/NES/tree/main/research/Genghis_Khan_NES/execution_015|
|Normal-melee module, cases, formulas and verifier|https://github.com/Bomb-Daddy/NES/tree/main/research/Genghis_Khan_NES/execution_018|
|EXEC0018 private native evidence|https://drive.google.com/file/d/1The0pqj5hnTLySktLZRW2riDIip8qzC5/view|
|EXEC0018 source package and handoff|https://drive.google.com/file/d/1Jl1tETGdL4Oqe0fwTqYsbpKram5nJTu0/view ; https://drive.google.com/file/d/1VXLhOYT1m16FNw-4oL7nAT77lLejBtOy/view|
|Verified publication/evidence index|https://github.com/Bomb-Daddy/NES/blob/main/research/Genghis_Khan_NES/AUDIO_EXECUTION_PUBLICATION_2026-11-05.md|
|Original graphics extraction branch|https://github.com/Bomb-Daddy/NES/tree/genghis-khan-assets/research/Genghis_Khan_NES/Assets|
|HD/mod handoff|https://drive.google.com/file/d/1YJSCcMTl-HZS6eYoOxkwQfvJg_LHv6Sf/view|
|Headless operations/recovery|https://github.com/Bomb-Daddy/NES/blob/main/reference/MESEN_HEADLESS_CHATGPT_OPERATIONS_v1.00.md|
|Saved Mesen/bridge setup ZIP|https://drive.google.com/file/d/10a4CW22GYVbOOwAYx4MWPZ1ED496HzjD/view|
|Separate original campaign ledger|https://docs.google.com/document/d/1y2BcAHAfadc_KrxnzFXXuA5W2E0Nt2LKU53jWJ2z0SE/edit|
|Authority register|https://docs.google.com/document/d/1qEVhDdiG2oH8LvPz0jan5DCtFFVxHhQWdWRUpduTllQ/edit|
|Shared agent delivery folder|https://drive.google.com/drive/folders/1d0TA1lgqsVA3AWQQ9o0FZgmvao90oCeT|
|Private raw research evidence|https://drive.google.com/drive/folders/1mJ72ezJb9kzIGUHHXPTh3lXtiVxWjEL7|

GitHub holds authored source, documentation and permitted numeric findings. Private ROMs, game-image/audio payloads, SRAM, batteries and full machine checkpoints stay in the authorized Drive archives. This is deliberate storage separation, not missing research. Do not publicly upload those inputs to make the repository appear self-contained.

### Supersession and limitations

This v2.01 guide is the current work-order and consolidated knowledge entry point. It incorporates new EXEC0018 findings into v2.00 without restarting accepted work. Preserve v1.00/v1.01/v1.02 plans, HQA0010/0011, v1.07 source evidence and EXEC0012–0015 unchanged as history. EXEC0016's verified publication receipt overrides their original historical NOT DELIVERED statements. Older pending UI and reconstructed-save statements are overridden only for the later completed cases described here.

The v2.00 recovered facts remain source recovery. EXEC0018 adds explicitly controlled native modified-ROM attacks and source-executed numerical proof; no old original/mod pass is revoked and no Windows execution is claimed. All remaining material work retains its owner, method and receiving implementation in G01–G12; add new unknowns there rather than spawning a competing plan.

**End of unified guide. The deliverable is a complete, enjoyable Windows game; this document exists to stop rediscovery and get the remaining work implemented.**
