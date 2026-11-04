> This is the preserved source-batch explanation. Its original runtime targets and stage-specific status are resolved or explicitly bounded by the v1.07 master and final acceptance register; these source tests are not relabelled as Mesen playthroughs.

> Integration errata: merchant presence divides by **20000** (original immediate$4E20), and ability actor is word**$7B34**. Earlier author transcription/tests used50000 and a pointer note named$7B66. Original CPU replay always executed the correct ROM. The portable package preserves originals and adds discriminating regressions.

# Genghis Khan NES: completed trade and hiring source mapping

Date: 2026-11-04 UTC. Owner: Zhuge Liang/Codex research, delegated source pass. Scope: original English NES Genghis Khan. Evidence: VERIFIED_STATIC (original instructions executed in synthetic memory). No emulator, Windows port, controller, or player-runtime claim is made here.

Private ROM SHA-256: ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3. The scripts read the Director-supplied ROM and existing v1.06 authored replay dependencies; no ROM or graphics are included. All switchable addresses below are qualified by physical 16-KiB PRG bank. Fixed addresses are bank15. File offset is 16 + bank*16384 + (CPU-8000 for banks0–14, CPU-C000 for bank15).

## Defined state

`C` is the country pointer. For baseline country1, C=$6DCD. All listed country fields are unsigned 16-bit words. `G=word[C+$40]`; `A=word[C+$14]` is Arms; `K=word[C+$16]` is Skill; `M=word[C+$42]` is Morale. Total manpower is

`T = word[C+$18] + word[C+$1C] + word[C+$20] + word[C+$3A] + word[C+$3E]`

The source sum is fixed routine $DECD (VM $DED2–$DEF2). The fields are troops, town workers, masons, artisans, and farmers. Baseline values 61+5+4+6+27=103. Troops alone are not the weighting population.

Commodity stock for index `i=0..10` is `word[C+$22+2*i]`, fixed pointer helper $DF05 / VM $DF0A–$DF10. Food is index10: C+$36=$6E03. Previously purchased index2 uses $6DF3 and index5 uses $6DF9. Index11–16 are weapon choices; their calculated commodity pointer is not used as a weapon inventory store.

## Quotes and accepted numeric domain

Prior v1.06 quote proof is retained: `B=floor(base[m,i]*(Rate+100)/100)` tenths, seller `S=max(1,floor(90*B/100))`, hiring `H=floor(60*(Rate+100)/100)`. The new work completes consumers and constraints. Conclusions below concern normal field bounds0..999, positive quote, valid indices, affordable selected quantities, and total manpower<=999. Internal arithmetic is not silently generalized to arbitrary65535 inputs.

Buy/sell/hire quantity widgets pass minimum1. Fixed $D3CC VM $D3D1–$D3D8 and bank3 $8493 VM $8498–$84A4 route to $D3B6 then $D369. Its original input gate $D3A1–$D3B5 rejects0 and values above max, accepts1..max, and passes cancellation sentinel$FFFF. Tests execute this gate directly without pretending to run UI input.

## Sale credit, limits and mutation order

Bank3 seller command native $8B96, price/maximum region VM $8BE6–$8C06, confirmed mutation VM $8C20–$8C33:

- Quote `S` is selected merchant's seller quote.
- `maxSale = min(playerStock, floor(10*(999-G)/S))`.
- `newStock = stock-q`.
- `credit = floor(q*S/10)`.
- `newGold = min(999,G+credit)` via fixed capped add $DA99 (VM $DA9E–$DAAC).
- Success result is9. Cancellation/unavailable/no-capacity returns$00FF.

The maximum is conservative at fractional boundaries: at Gold999 even a quote0.1 sale of1 that would credit0 is disallowed because maxSale=0. Source products stay below9990 for accepted transactions. Tests preserve the prior Fur0.7 examples: q1/7/9 credit0/4/6. The observed index2 sale432→437 at q1,S50 is reproduced by the source commit.

## Purchases and affordability

Bank3 purchase debit native $8644 / VM $8649–$865C:

`newGold = floor((10*G-q*B)/10) = G-ceil(q*B/10)`

Bank3 affordability helper native $865D / VM $8662–$8670:

`affordableCap = min(capacity, floor(10*G/B))`.

Commodity capacity is999-stock, computed at bank3 $8C82–$8C8A, then limited to999 and affordability at $8CC5–$8CD4. Commit $8CFB–$8D2D adds quantity through $DA99 and calls debit. The source reproduces baseline index2 purchase q11,B64: stock26→37, Gold439→368. The merchant offer masks are unchanged by commits.

## All six weapons

Bank3 weapon path: prerequisite $8C8D–$8CA7, capacity $8CA8–$8CD4, Arms commit $8D06–$8D24, same debit $8D25–$8D2B.

Prerequisites: total manpower must be nonzero (helper $83B1); Arms999 yields capacity0. The *Arms effectiveness* reads `byte[$7DE5+i]`, without merchant row offset. Thus it uses row0's base value even when another merchant supplies the quote. The source values for indices11..16 are20,80,100,40,60,150. Merchant-specific quote pricing remains separate.

For weapon base `W` and selected quantityq:

`maxWeapon = min(999, floor(T*(999-A)/W), floor(10*G/B))`

`newArms = min(999, A + floor(9*W*(q+1)/T))`

The `q+1` is an actual original increment at $8D0D; the9 is loaded at $8D15. Mutation controls replacing the increment with decrement change results. At Arms110,T103,index11,W20: q1 yields113; q2 yields115. A weapon purchase does not fill a separate weapon-stock word. The cap expression omits the9 that occurs in effect calculation; the capped Arms add handles overshoot. Preserve this original asymmetry. Root owns independent live q1/q2 confirmation and names/icons of the six weapons.

## Hiring, all three dilution paths, full commit

Bank3 hiring native $8D2E. Cap at $8D3F–$8D52:

`maxHire=min(999-T, floor(10*G/H))`.

Dilution native $8683 / VM $8688–$86CA uses fixed weighted average $DB48 / VM $DB4D–$DB92:

`weighted(old,T,incoming,q)=min(999,floor((old*T+incoming*q)/(T+q)))`.

When both weights are0 the helper returns0. Its products/additions/division use original32-bit native arithmetic helpers, not a16-bit substitute.

- Arms incoming value100, argument at bank3 $8693.
- Skill incoming value100, argument at $86A5.
- Morale incoming value `50 + rand(11)`, call $86B8 and addition $86BC. Range50..60; this is not a fixed50 rule.

Then $8D8A–$8D92 addsq to troops only, and $8D93–$8D99 debits gold. Full commit $8D84–$8D9A is replayed with no stubs. Starting Arms110,Skill124,Morale118, manpower[61,5,4,6,27], q8,G368,H120, deterministic seed used in the tests gives Arms109,Skill122,Morale113,Troops69,Gold272, matching the earlier runtime state. Matching this arithmetic result does not recover the earlier runtime RNG seed. Moving the103 preexisting manpower into any one of the five source fields produces the same dilution, establishing the total weighting denominator.

## Merchant identities, availability, and offers

Static bank3 pointer table $9BC4 has row0 China, row1 Islam, row2 Uighur. This completes source identity; runtime row2 identity remains a separate capture task.

Availability words are `C+$4B+2*m`: baseline addresses$6E18,$6E1A,$6E1C. Nonzero is available. Bank3 menu checks any available merchant at $8DAF–$8DCB and selected merchant at $8DF5–$8DFC. Both branches are source-replayed, including all-unavailable and each single available merchant.

Seasonal update bank1 native $9022 recalculates all three flags. Relevant original region $907A–$90CA:

`bonus = floor(word[C+$1A] * word[C+$42] / 20000)`

`available[m] = (rand(100) > baseThreshold + bonus)`

The comparison is strictly `>`, not `>=` or `<`; tests execute the original comparison. The economy field used here is its value after the preceding seasonal update in the same routine. Do not replace this unintuitive direction with a design assumption.

`baseThreshold = word[bank1 CPU $B1C6 + 54*m + 2*j]`, with j=0 when word[$65B9]!=27. In that scenario the three cutoffs are25,15,40. When word[$65B9]==27, j is the seasonal routine's remaining-country loop counter held at frame-2. The exact loop starts with that count and decrements after the body; this source pass retains literal indexing, including j27, and does not silently relabel it as a zero-based country index. A runtime crosswalk from these World-scenario iterations to displayed country numbers is still needed if documentation requires those names.

Offer generation is bank1 native $832C / VM $8331–$83C4, called by seasonal updater $9022 at $90D6. It writes global masks, shared across countries:

| Direction from player perspective | Mask word address | Choices | Threshold table in bank1 |
|---|---|---:|---|
| Sell to merchant | $7A8C +22*m+2*i |11|CPU$AFB2 +22*m+2*i|
| Buy from merchant | $7ACE +34*m+2*i |17|CPU$AFF4 +34*m+2*i|

For each merchant, the routine first picks `forced=rand(choiceCount)` and writes1 for that item. Each other item is enabled iff `rand(100)<threshold[m,i]`. The forced item skips that per-item RNG call. Each row therefore always offers at least one choice. Masks contain0/1, are checked before item selection, and are never decremented by trade. They are offer flags, not finite merchant quantities. Numeric quantity limits arise from player inventory, total manpower, Arms capacity, and affordability.

The threshold arrays are recorded as numeric facts in test_trade.py. Tests independently simulate the RNG sequence and forced-item skip rule, compare complete masks and final seed, and mutate all thresholds to0 or100: exactly one item versus every item remains enabled. No claim is made that modulo-reduced RNG values have perfectly uniform mathematical probability.

## Judgment and orders

All three completed trade commands return action code9. Bank3 $827B requires each of six ability words to meet its command-table cost; bank3 $8242 deducts those costs with saturation at0. Command row9 is bytes at$7E4E..$7E53: `[0,5,0,0,0,0]`, verified against hashed baseline SRAM.

The source string pointer table at bank3 $9BEC labels the slots leadership, judgment, planning, charm, body, battle. Therefore each completed Buy/Sell/Hire costs5 judgment and0 of the other five abilities. Judgment4 is rejected;5 is allowed. The actor is resolved using active country index word[$7B34] through country pointer helper $DBA7 and officer-ability helper $DDD6; governor-role102 can route to the ruler's country through $DF11. Do not assume every action always charges a locally displayed governor.

Baseline current country ruler pointer `word[C+$51]` = $65BF. Ability words are$65CD,$65CF,$65D1,$65D3,$65D5,$65D7; judgment is$65CF=77. The source outer Trade loop $8E38–$8E4D calls ability debit only for a result other than$00FF, then decrements word[$7B36] by1. Cancel/failure sentinel skips both. Source tests execute the threshold, saturating debit, order decrement, and cancellation branch. Actual failed-command UI and physical-controller behavior remain runtime work.

## RNG dependency resolved in source

`rand(n)` fixed native $DB93 / VM $DB98–$DBA6 returns0 without advancing the seed for n0; otherwise it calls syscall17 through $F988, original native RNG $C289, then uses unsigned remaindern. The new CPU extension executes PHP, software-interrupt dispatch, original48-bit multiply/negation, and RTI; it does not stub or replace RNG.

Treat seed bytes$84..$89 as a big-endian48-bit integerz. The original instruction result equals:

`z' = (-(z|1) * 0xE0900E0187C5) mod 2^48`

`r = (z' >> 32) & 0x7FFF`

`rand(n) = r % n`.

The independent model is checked against source execution for multiple seeds and bounds. This gives reproducible synthetic state, not a claim about console frame-driven seed scheduling.

## Verification and limitations

Run from workspace root:

`python batches/trade_source/test_trade.py > batches/trade_source/test_final_verified.stdout 2> batches/trade_source/test_final_verified.stderr`

The final log is the authority for test count and exit status; verification.json records the command, environment, count and hashes. First replay and both earlier test runs are retained, not rewritten. The tests use original CPU instructions and independent formulas, actual v1.06 baseline hash, mutation controls, capacities, accepted/rejected quantity boundaries, all six weapon indices, all five manpower fields, deterministic RNG, all offer masks, and cancellation branches.

The CPU harness is a deliberately bounded extension of the existing v1.06 source harness, not an NES emulator. The inherited diagnostic instruction counter is periodically reset to avoid its artificial100000-instruction ceiling; a separate monotonically increasing total and an external two-million-step budget are used. No game-state registers are reset by this bookkeeping. UI routines are excluded by explicit start/stop boundaries. The complete seasonal tick and external interrupts are not replayed by this batch.

Scoped source formulas and dependencies requested from this batch are resolved. Remaining genuine dependencies are independent live confirmation of source-derived weapon/dilution/cap/error behavior, the weapon name/icon crosswalk, and any World merchant threshold *displayed-country* naming crosswalk. These are not missing arithmetic or unknown local callees. Source mappings do not close the broader economic runtime or full-year obligations by themselves.
