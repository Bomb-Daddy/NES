<!-- Public readable mirror of the Director-approved native Google Doc, not executable Python. -->
<!-- Source of truth: https://docs.google.com/document/d/1Tn4xeiCFIUCl6eDgu4DLZKuby65mT_AdMRlV8IQDeko/edit -->
<!-- Mesen runner: ../../tools/headless_mesen_campaign_test.py ; Headless guide: ../../../reference/MESEN_HEADLESS_CHATGPT_OPERATIONS_v1.00.md -->

# TEMUJIN — UNIFIED CAMPAIGN AI OPERATING SCRIPT v2.00

Original English NES Genghis Khan I • Mongol Conquest • Province 1 • Difficulty 1 • 5 November 2026

Director-approved strategic rules consolidated; this document is an AI operator's decision script, NOT a tested unattended Python/Mesen controller.

STATUS: DOCUMENT ONLY. No game commands executed, saved states modified, or autonomous simulation initiated by this revision.


## 1. AUTHORITY, SCOPE AND SUPERSESSION

Run the original, unmodified NES ROM in Mesen using controller inputs. Preserve game outcomes without memory cheats or outcome rewinds. The long-term campaign remains Spring 1175 through Spring 1205 (120 actual seasonal transitions), or an authentic game-ending event.

Latest available baseline: Spring 1186 START, 3 orders; Gold 832, Food 443, Troops 146, Civilians 154, Morale 129, Rate 55, Troop Skill 601; Leadership 137, Judgment 102, Planning 125, Charm 119, Body 170, Battle 129. Re-read the actual state on loading; do not replace it with these reference figures.

New Director decisions supersede conflicting portions of strategy v1.04: maintain LEADERSHIP above 450; defer the BATTLE above-450 goal until immediately before an expansionist phase; schedule +10 actual soldiers per year from an initial 60 until a cap of 250; fund the ability to buy at least 10 troops in each winter using a Rate-140 reserve; permit flexible hiring to create treasury capacity for lucrative commodity sales. The old rigid 200-soldier limit and exclusive 20-person, below-Rate-90 purchase size no longer govern conflicts.

All unaffected earlier rules remain: keep the other ruler abilities above 100; never Draft; use the 40/30/10/20 civilian allocation; native Fur morale gifts; price-based long-term speculation; safe five-sword purchasing; accept all incoming alliances; preserve legitimate savestates and turn logs. No expansionist date or authorization has yet been set.


## 2. OPERATIONAL PRIORITY ORDER

FIRST: Respect authentic game events, ruler survival, food sufficiency, essential obligations, and the rule that no ruler ability may finish an ordinary action below 101.

SECOND: Advance Leadership toward 451 and, once reached, keep Leadership at or above 451 after known command debits. During a Winter with a feasible mandatory ten-person hire, reserve the hire, necessary labor orders and morale remediation before spending optional training orders.

THIRD: Meet the annual winter recruitment floor and the cumulative 10-soldier-per-year army target when feasible. Maintain payroll, food, and Fur reserves.

FOURTH: Maximize profitable, multi-season commodity speculation and use appropriately-sized hires to free cash capacity for eligible sales; do not buy or sell beyond protective floors or source-game limits.

FIFTH: Execute labor allocations, restore province Morale, and use any remaining safe orders on Train Troops. Leadership self-training takes precedence over spare Train Troops while Leadership is short of 451.

PRE-EXPANSION ONLY: Before entering an expansionist phase selected by the Director, prioritize raising Battle to at least 451. Battle training is not an ordinary economic-phase priority so long as Battle is above 100.

If command scarcity prevents the full sequence, record exactly which objectives were deferred; do not invent transactions or commit an order that makes an already protected essential action impossible.


## 3. RULER SKILLS — CURRENT DEVELOPMENT PHASE

Open every player turn by viewing Temujin's six ruler attributes and the actual displayed health/condition. Include Leadership, Judgment, Planning, Charm, Body and Battle. Train any ability at or below 100 before ordinary commands. Keep enough attribute buffer to pay each planned command without ending at 100 or less.

LEADERSHIP: Priority development target >450 (minimum 451). While below 451, allocate available Train Self commands to Leadership ahead of discretionary operations, subject to mandatory winter hiring and survival needs. After reaching 451, proactively train if any planned Leadership debit would make the post-action value fall to 450 or below; e.g., a five-point Leadership cost requires at least 456 before that action.

JUDGMENT / PLANNING / CHARM / BODY: Maintain strictly >100; do not target 450. Proactively repair a 102 Judgment before a Trade or Hire action that deducts five points (pre-action minimum 106).

BATTLE: Maintain >100 now. The >450 Battle target remains secondary and activates ONLY ahead of a Director-specified expansionist phase. Do not start that phase until Battle is at least 451 and other binding readiness requirements are met. No speculative timetable is inferred.

MECHANICAL BASIS: Original-game mapping explicitly includes Leadership in Train Troops raw gains (15+rand11+floor((Leadership+Charm+Battle)/40)); Leadership also enters other mapped calculations. The economic benefits of pursuing Leadership 451 are a strategy hypothesis, not a verified general-purpose multiplier for every action.

Training gains are not fixed. Read the value after every Train Self; do not assume 20, 30 or any constant gain. Train Self costs one turn order even when it has no mapped direct six-stat debit.


## 4. ARMY TARGETS AND POPULATION POLICY

Base standing army at campaign opening: 60 soldiers. Target ten additional ACTUAL soldiers in each campaign year, beginning in Year 1, until the standing army reaches 250. Year = (current calendar year − 1175 + 1). Cumulative target = min(250, 60 + 10×Year).

Checkpoints: Winter 1175 target 70; Winter 1176 target 80; Winter 1177 target 90; Spring/Winter 1186 (Year 12) target 180; Year 19 / 1193 target 250. Keep 250 as the permanent army cap thereafter. Record shortfalls and catch them up from later hires or natural soldier growth; do not silently declare them met.

Retain newly acquired soldiers only up to the current standing-army target; allocate all newly acquired personnel above that target to civilian work. Do not conscript from the undrafted pool. Do not remove existing civilian workers solely because an annual army target increased: meet increases using recruitment or actual growth.

Recruitment above the army target can still be beneficial as a treasury-capacity and civilian-economic investment; transfer the excess to civilian jobs, never keep it as soldiers over 250.

Whole civilian workforce C = assigned population P minus the soldiers actually retained. Split C at 40% farmers, 30% town, 10% masons and 20% artisans. Compute integer jobs by flooring each ideal percentage and awarding leftover people to the largest fractional remainders; ties go farmers, town, masons, artisans. Every actual labor confirmation must conserve P and leave undrafted reserve unchanged.

Labor command mechanics: a changed Assign Labor confirmation costs one order, five Leadership and five Planning. The command moves workers between Soldiers and the selected civilian job; multiple changes may require multiple orders. Preview, commit, and re-read the totals; do not assume a multi-job reallocation is a single free command.


## 5. WINTER RECRUITMENT — FUNDED EVERY YEAR

In every Winter, plan to HIRE AT LEAST 10 people when a live Hire offer, command budget, population capacity and operating reserves permit. This is an annual acquisition floor, even when the standing army is already at its year target: assign recruits above the military target to civilians.

Maintain earmarked Gold equivalent to buying 10 troops at Rate 140: hireQuoteTenths = floor(60×(140+100)/100)=144, so 10 troops cost ceil(10×144/10)=144 Gold. Set aside 144 in the year's spending plan, IN ADDITION to food, payroll and known obligations. Use the actual winter quote if it is higher; do not describe 144 as a guaranteed all-rate purchase price.

At actual Rate R: hireQuoteTenths=floor(60×(R+100)/100); hireCost(q)=ceil(q×hireQuoteTenths/10). Require capacity q ≤ 999−P and a real Hire offer. Every Hire costs an order and five Judgment; each follow-up labor adjustment can also cost an order and attributes.

At Rate <90, a 20-person purchase remains an attractive preferred bulk option if it is affordable and compatible with the new objectives. It is NOT an exclusive lot-size restriction. Winter ten-person minimums and treasury-headroom hires may require different quantities; do not reject such legal purchases because they are not exactly twenty.

Prefer to complete the year's winter recruitment before discretionary troop training and new commodity purchases. If the seasonal orders or merchant availability do not permit ten, hire the maximum compliant feasible quantity, record the actual count and remaining shortfall, and preserve the next available recruitment opportunity. Never invent a successful winter buy.

Buyers are not drafted residents: Hire increases assigned P and soldiers initially while undrafted reserve stays unchanged. Newcomers dilute Arms, Troop Skill and Morale by the original weighted formulas; allocate and recompute gifts AFTER hiring.


## 6. COMMODITY PRICE RULES — HOLD BETWEEN SEASONS

Use the Director's existing 11-commodity BUY / SELL price guide. Prices are Gold per unit; BUY at or below the cap and SELL at or above the target after confirming that the whole-lot sale is profitable. No same-turn arbitrage loop is requested.

Fur          BUY ≤1.2   SELL ≥1.3

Silk         BUY ≤4.8   SELL ≥4.9

Jewels       BUY ≤4.2   SELL ≥4.3

Metals       BUY ≤2.8   SELL ≥2.9

Medicines    BUY ≤1.8   SELL ≥1.9

Woodworks    BUY ≤1.7   SELL ≥1.8

Ceramics     BUY ≤2.3   SELL ≥2.4

Glass        BUY ≤2.6   SELL ≥2.7

Textiles     BUY ≤4.0   SELL ≥4.1

Perfume      BUY ≤5.0   SELL ≥5.1

Food         BUY ≤1.7   SELL ≥2.0  [optional speculation on SURPLUS only]

The non-Food sell figures are conservative best-merchant Rate-50 floors for inventory purchased at the listed caps, not a guaranteed future offer or an automatic profit for a 1-unit lot. Food's 1.7/2.0 rule is a separate chosen strategy threshold rather than the Rate-50 guaranteed floor.

Check all present merchants' real buy/sell offers, actual listed quotes, and stock when making decisions. Use the cheapest available qualifying buy quote and highest qualifying sell quote. If no qualifying quote or offer exists, hold. Preserve cost basis per purchased lot so an apparent price difference cannot conceal a whole-Gold loss.

Original-game integer arithmetic: buyTenths=floor(merchantBase×(Rate+100)/100); sellTenths=max(1,floor(buyTenths×90/100)); buyCost(q)=ceil(q×buyTenths/10); saleCredit(q)=floor(q×sellTenths/10). Individual price labels are tenths; cash is whole Gold, inventory and treasury max 999.

Food: preserve the full protected stock for ordinary upkeep, next spring consumption and known military commitments. Only the surplus is speculative. If Food is needed for survival, purchase it regardless of speculative price threshold when possible. Fur: reserve enough for the next required province morale gift before any sale.

Purchases must leave cash for protected requirements and Winter's 144-Gold troop reserve; do not assume hypothetical future sales as already funded. Do not spend an order on an unprofitable sale merely to rotate inventory.


## 7. SELLING AT THE 999-GOLD LIMIT — HIRE TO CREATE ROOM

Trigger: an available qualifying SELL quote exists, and eligible inventory is too large to sell in full because the treasury would hit 999 Gold. This rule also applies to surplus Food, but NEVER to the protected Food reserve or the Fur needed for morale.

Step A — Inventory eligible for sale: for an ordinary commodity use actual stock minus any protected stock and cost-basis limits. For Food use max(0, Food−protectedFood). Only count the quantity whose whole-lot sale is profitable at the currently offered quote. Use that maximum eligible quantity as saleTarget.

Step B — Current sale capacity: without hiring, the NES seller limit for sell quote S in integer tenths and current Gold G is saleCap0 = min(saleTarget, floor(10×(999−G)/S)). This formula is intentionally conservative like the original game; independently check whole-Gold rounding.

Step C — Calculate the hiring quantity: if saleCap0<saleTarget, evaluate every permitted hire quantity h from 1 to min(999−P, affordable recruits) using the actual Hire quote H. After hiring Gold G_h=G−ceil(h×H/10); new sale capacity saleCap(h)=min(saleTarget,floor(10×(999−G_h)/S)). Choose the SMALLEST h that permits selling all saleTarget. If none does, choose h that maximizes profitable saleCap(h), tie-breaking toward the smaller hire. Do not hire when it does not increase a sale you can genuinely execute.

Step D — Preconditions BEFORE executing Hire: a Hire offer exists; the relevant merchants/items remain offered; protect essential food and payroll; ensure the 144-Gold winter commitment if it is not yet fulfilled; allow for the hire cost, higher payroll, post-hire food need, morale dilution and Fur gift; ensure enough actual orders and attribute buffers for Hire, sale, required labor changes and gift (when needed). If the transaction cannot complete in the available order budget, do not issue a stranded hire.

Step E — Execute normal-game Hire, verify amount/Gold/new P, then recalculate the live seller quote and saleCap. Sell the maximum justified stock without breaching 999 Gold or food/Fur reserves. Re-evaluate eligibility after every trade; a lower live offer can invalidate the plan.

Step F — Allocate all recruits: fill the year's soldier target using acquired soldiers, then turn all excess into the 40/30/10/20 civilian workforce. Inspect new Morale and restore it to the target with protected Fur if necessary.

Step G — If the full sale is impossible because offers/orders/capacity/ability/reserves are missing, sell only the profitable quantity that actually fits and carry the remaining inventory forward. Record what was not sold and why. Recruitment must not be used to manufacture an invented gain or destroy protected stock.

Illustration of the cap arithmetic, not a required purchase: at G=960 and a qualifying sale of 100 items quoted at 1.9, a sale would credit 190 Gold but only 20 units fit under the pre-hire game limit floor(10×39/19)=20. At Rate 55, an actual hire cost is 9.3 Gold/person; hiring 17 costs 159, lowers G to 801, and raises sale capacity to floor(10×198/19)=104, enough for the full 100. Actual population/reserve/order constraints must ALSO pass.


## 8. MORALE, SEASONAL FOOD AND ARMS

Keep province Morale at least 129 (10% above the mapped 117 tax-multiplier maximum threshold). Before a predicted spring loss under current Tax 35, target 131 to retain 129 afterward. Recompute following hiring, labor, events and taxes. Give ONLY the province's native Fur item, in the minimum quantity producing the required new Morale; never give Gold or Food.

Fur-gift calculator: avg=floor((ChinaFurSellTenths+IslamFurSellTenths+UighurFurSellTenths)/3); V(q)=min(999,floor(q×avg/10))+1; gain(q)=floor(30×V(q)/P)+1. Enumerate q from 1 upward and choose the smallest sufficient legitimate gift. Reserve Fur stock BEFORE speculation.

Hold food for immediate and next known seasonal costs. Conservative next-spring baseline is ceil(projected P/2), plus known additional military commitments and an allowance for winter recruits; review the real balance before selling Food.

Preserve the existing sword-buying policy only as an OPTIONAL purchase subordinate to leadership, winter manpower, cash protection and morale: buy in batches of five when an actual cheapest qualifying quote is ≤14.5 Gold per sword (ideal 11.2). Do not substitute other weapons. Weapon purchases cannot be resold as commodity speculation.

Spend remaining safe orders on Train Troops only after higher priorities are met. The original game reduces Troop Skill in Summer (roughly 93% of prior value); record changes and do not mistake temporary Skill loss for a failed order.


## 9. EXECUTABLE TURN-BY-TURN OPERATOR ALGORITHM

FOR EACH PLAYER SEASON, WHILE GAME CONTINUES AND YEAR<1205:

  1. COLD-LOAD current authoritative native save; verify ROM hash, year, season, active ruler, province and order count. SAVE the unaltered start-of-turn checkpoint.

  2. View Temujin: six abilities, health, Leadership/Battle progress. Inspect country state, troop count, P/reserve, worker jobs, Gold, Food, Fur, Morale, Rate, Arms/Skill, every present merchant and offer.

  3. Calculate Year target=min(250,60+10×(Year−1175+1)); soldier shortfall; winter 10-hire obligation; whole-civilian percentages; protected food, payroll and winter hire reserve; essential Fur gift; planned order and ability costs.

  4. Heal immediate attribute violations. If Leadership<451, train Leadership with available orders ahead of OPTIONAL speculation and troop training, but protect a feasible Winter hire and essential later actions. If at/above451, train ahead of any known debits that would end ≤450. Other four stats and current Battle remain ≥101.

  5. For each commodity, evaluate qualifying delayed buys and delayed profitable sales using the price guide. Compute actual whole-Gold profit, stock and treasury caps and food/Fur protections. If a good sale is cap-blocked, search permitted Hire quantities as in section7 BEFORE sale.

  6. Purchase manpower when Winter's 10-hire requirement is due, when a low Rate offers advantageous affordable bulk recruitment, or when it creates room for profitable eligible sale. Respect capacity and commands. Record cumulative hired THIS winter separately from total army, and actual soldiers toward annual target.

  7. Execute selected trade actions, rechecking offers, quotes, costs and cap after each. Do not waste commands on nonqualifying stock. Never use Draft or Train People.

  8. Commit only those labor adjustments that preserve P, make soldier counts consistent with current annual target, and allocate all remaining civilians 40/30/10/20, using largest remainders. Re-read and record actual job counts.

  9. Restore Morale to ≥129 or 131 where the planned spring buffer is needed by the minimum Fur gift, if resources and orders permit. Maintain the actual remaining protected Food.

 10. If entering an authorized pre-expansion phase, divert available training to Battle until ≥451 before expansion. Otherwise use remaining safe orders first toward Leadership threshold/maintenance, then Train Troops.

 11. Handle AI rotation and incidental diplomacy; accept EVERY incoming alliance offer. Do not skip terminal events, attacks, disease, ruler events or battle decisions. Do not claim a season is finished until the original game confirms the next season or end-of-turn checkpoint.

 12. SAVE a native .mss at each actual season, with before/after screenshot, state hash, orders, all six abilities, worker split, rate, offers, trades, costs, hiring, cash headroom, stock, Food/Fur reserve, Morale, army target and execution errors. Publish to campaign Drive folder; verify folder, bytes and journal link.

STOP only upon Spring1205 target, actual original-game terminal outcome, explicit Director stop, or genuinely insurmountable access/safety barrier. Recover from routine controller timing and session timeouts using verified checkpoints, not silent strategy changes.


## 10. PROOF GATES AND EDGE CASES

Check 1: Year 1 target 70, Year 12 (1186) target 180, Year 19 (1193) target 250, every later year 250. The old 200 cap is obsolete for this script.

Check 2: Rate140 Hire price 14.4 Gold/person; ten cost exactly144 Gold. At Rate55 the quote is9.3 Gold/person and ten cost93; both are conditional on actual offers and capacity.

Check 3: Leadership451 before a five-point cost is NOT sufficient to maintain >450; require456. Judgment102 cannot pay a five-point trade debit while staying >100; require106.

Check 4: Food kept for upkeep is unavailable for selling, even when the quoted Food price is ≥2.0. A full inventory sale blocked by 999 Gold does not justify spending protected payroll, food or the annual winter hire reserve.

Check 5: Hiring to unlock a sale consumes an actual order. Sale consumes another; changing civilian assignments and Fur gift consume further orders. If three orders cannot complete the safe sequence, defer or split over seasons without falsely reporting an immediate full-sale result.

Check 6: Do not count natural spring growth as Winter hiring; keep separate counters for actual Winter hires, resulting retained soldiers and assigned civilians. Recorded army count can decline from game events; carry shortfalls forward.

Check 7: Max Food 999, Gold 999, assigned P≤999. No Draft. Buying five swords does not create merchant inventory stock. Prior Save/Restore for testing must never be used to replay for a better economic result.

Check 8: When qualifying merchant offers disappear, no winter hire exists, or you cannot train Leadership above450 due actual mechanics, report the precise observed blocker; do not reduce the Director's rule silently.


## 11. EVIDENCE AND SOURCE LINKS

Prior full AI script and controller source v1.00 (historical strategy; source preserved): https://docs.google.com/document/d/1a9doDD78SPCGZrlJnJqIYTmSzVDU4WQ5-i66sGZZxj0/edit

Speculation Strategy Reference v1.04 (old policy and source equations): https://drive.google.com/file/d/1M0SxApaPgDkiNHagt2H6Mlh7r18STG35/view

Merchant Buy/Sell Guide v1.02: https://docs.google.com/document/d/1edcLC580d-eo9iv2nJpo4SDDdAjXR2LkZRTSdnowIBY/edit

Economic Mapping v1.07 (original NES ROM source and observed emulator mechanics): https://drive.google.com/file/d/1AC7h5seTDE4-KdAK7rP3Nv_g-bMJO5xk/view

Most recent saved Spring1186 campaign checkpoint: https://drive.google.com/file/d/1LgKX5HschZ1uLHYDgS0kEesgpiZuGgUw/view

Native seasonal save-state journal: https://docs.google.com/document/d/1TPKEQ_6E4zDBrdWkY47HBqNqcfgOeg98M_tAqWXw_OU/edit

Evidence classification: DIRECTOR_POLICY = intended future strategy; VERIFIED_STATIC / OBSERVED_RUNTIME = specific already-mapped mechanics or stored gameplay; INFERRED = predicted benefits and untested strategy interactions. Script composition is not gameplay execution.

IMPLEMENTATION STATUS: Consolidated operator policy only; new rules are NOT yet installed into the separate Python controller or run in Mesen. Do not mark Leadership 451, winter acquisitions, 250-soldier cap or sale-unlocking hires as achieved until tested in game.
