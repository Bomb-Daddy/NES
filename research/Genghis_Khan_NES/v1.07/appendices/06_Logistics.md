> This is the preserved source-batch explanation. Its original runtime targets and stage-specific status are resolved or explicitly bounded by the v1.07 master and final acceptance register; these source tests are not relabelled as Mesen playthroughs.

# Genghis Khan NES: cross-country economic logistics source findings

Date2026-11-04 UTC. Author Zhuge Liang/Codex source research. Scope: Move People/Gold/Food/Item, ownership/adjacency, conservation, quantity and ability/order costs. This closes the scoped source-dependency work for TD15, MV02–MV06, AS03 and CF03; it does not by itself close their multi-year, runtime or player-acceptance obligations. No Mesen session or public publication was performed by this batch.

Evidence class: VERIFIED_STATIC, original ROM instructions executed in a bounded synthetic6502 harness. Private ROM SHA-256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`. All CPU addresses below are bank3 unless another physical16-KiB PRG bank is given. Fixed routines are bank15.

## Corrections established by hostile tests

The earlier trade-source note incorrectly named `$7B66` as the ability actor country index. The real opcode operand at bank3 `$8235` is `$7B34`. Bank0 ability helper VM `$8B0B` likewise reads `$7B34`. A new disagreement test intentionally set these globals differently and failed against the earlier assumption; its raw failure is `tests_population.stderr`. The corrected test executes both helpers and proves `$7B34` controls the pointer. No prior trade arithmetic result changes; prior country1 tests initialized both effective selections to0.

For this task, `N` means drafted workforce: five occupation words. It is not total residents. Eligibility is a byte at country+$48; civilians are an unaligned word at+$49, independently mapped by `batches/domestic_source/DOMESTIC_SOURCE_FINDINGS.md`. Reading a word at+$48 incorrectly combines eligibility and the low civilian byte. This batch now tests their correct separate widths.

## Fields and original entry points

`C(i)=$6DCD+83*i`, zero-based country index. Country1 source is$6DCD and country2 destination$6E20.

| Field | Country-relative | Country1 | Country2 |
|---|---:|---:|---:|
|Arms|+$14|$6DE1|$6E34|
|Skill|+$16|$6DE3|$6E36|
|Troops|+$18|$6DE5|$6E38|
|Town workers|+$1C|$6DE9|$6E3C|
|Masons|+$20|$6DED|$6E40|
|Item i,0..10|+$22+2*i|$6DEF+2*i|$6E42+2*i|
|Food,item10|+$36|$6E03|$6E56|
|Artisan production history|+$38|$6E05|$6E58|
|Artisans|+$3A|$6E07|$6E5A|
|Food production history|+$3C|$6E09|$6E5C|
|Food workers|+$3E|$6E0B|$6E5E|
|Gold|+$40|$6E0D|$6E60|
|Morale|+$42|$6E0F|$6E62|
|Eligibility byte|+$48|$6E15|$6E68|
|Civilians word|+$49|$6E16|$6E69|
|Officer pointer word|+$51|$6E1E|$6E71|

Fixed native helper `$DECD`, VM `$DED2–$DEF2`, sums Troops+Town+Masons+Artisans+FoodWorkers. Fixed `$DF05`, VM `$DF0A–$DF10`, returns commodity field pointer C+$22+2*i.

Move main native `$97FC`; People `$9696`; scalar Gold/Food/Item `$974B`; scalar capacity/commit `$95F2`; destination quality `$95A5`; source picker `$955A`; destination picker `$956E`.

Original switch dispatch is executed in tests, including cancellation:

- Move menu option1 is base relocation, excluded from these MV02–MV06 findings; option2 People; option3 Gold; option4 Food; option5 Item.
- People submenu options1..5 map Town, Masons, Food workers, Artisans, Troops. Dispatch `$96C3` reaches `$96D4/$971B/$9725/$972F/$9739` respectively.
- Scalar dispatch `$976C` routes3/4/5 to `$977D/$97B1/$97C6`. Food sets item index10. Item selection `$9635` requires positive source stock and uses the generic item pointer; no local-specialty identity restriction is present in this path.

## Preconditions, ownership and direction

Main Move `$9801–$980E` requires active country's officer role100 (ruler) via `$82F2`/fixed `$DDFE`. Native `$97E2` counts countries with the same owner via `$85B2` and rejects a count equal to1 (“only a base country”). In a valid player state the current country is owned, so this is the normal multiple-country prerequisite. Do not infer behavior of an invalid zero-owned-country state from that message.

Ownership helper fixed `$DF83` compares owner bytes obtained via `$DBBF`: `byte[word[C+$51]+$1D]`. It does not compare officer pointers, role codes, map colors or the number printed on screen. The source picker callback `$84B6`, VM `$84BB–$84C9`, allows any country sharing the active owner's byte. It is not limited to neighbors of the active country.

Destination picker `$956E` first requires at least one owned neighbor (`$8417`), temporarily assigns the selected source index to word$7B34 at `$9584`, and invokes callback `$84CA`. Callback VM `$84CF–$84EE` requires both same ownership and adjacency from the selected source. It restores the original index at `$9596–$9599` before returning. A source replay verifies that restoration.

Adjacency is fixed `$DE9A`, VM `$DE9F–$DECC`. The list is at `$7693+12*sourceIndex`; up to seven bytes contain zero-based neighbor indices and$FF terminates. The check searches the **source** list for destination index. The source therefore uses directed adjacency; natural lists in the baseline are symmetric, but an asymmetric controlled mutation changes only one direction. Same-country movement is rejected because the source is absent from its own neighbor list. A friendly nonadjacent target and an adjacent enemy both fail. Tests cover the complete14×14 baseline adjacency matrix with all owner bytes explicitly intervened to the same owner, plus enemy, self, early-sentinel, seventh-slot and one-way controls.

Source country1 neighbors are `[1,4,5,7,FF,FF,FF]`, meaning displayed countries2,5,6,8. Country2's list starts$769F with `[0,2,5,6,FF,FF,FF]`.

## Gold, Food and Item transfer

Scalar helper native `$95F2` accepts `(sourceFieldPointer,destinationFieldPointer,destinationUsedCapacity)`.

Capacity VM `$95FC–$960A`:

`maximum = min(sourceField, 999-destinationUsedCapacity)`.

For Gold/Food/Item, used capacity is the destination's matching resource stock. For People it is the entire destination drafted workforceN, not just the chosen category.

Numeric prompt passes minimum1 and maximum above via fixed `$D3CC/$D369`. Zero and above-maximum values are rejected, cancellation$FFFF is passed back. If capacity0 the helper returns0 without transfer. Confirmed commit VM `$961C–$962B`:

`sourceField -= q; destinationField += q; return q`.

There is no percentage, RNG draw, gold fee, loss adjustment, rounding, or saturation in this commit. Accepted bounds make the plain addition safe. For every valid transfer, pairwise stock is exactly conserved. Tests cover all11 item indices (including Food), Gold, boundary values0/1/255/256/998/999, reverse transfers and exact SRAM write addresses. The Item path has no extra specialization test; only valid selected index, nonzero source stock and the common cap/gates apply.

Mutation control replaces the original destination addition at$9628 with subtraction; a10-gold test then produces429/56 instead of429/76, demonstrating the conservation check detects the wrong operation. Cancellation after the quantity prompt leaves all SRAM unchanged in the replay.

## Move People: five drafted categories, quality, and empty-source edge

For a selected occupation, transfer q is bounded by

`min(sourceOccupation,999-destinationN)`.

The generic scalar helper moves exactlyq in that occupation. No other occupation changes. Neither source nor destination eligibility byte, civilian word, or cumulative production-history words move or reset. Thus pairwise draftedN and pairwise total residents `(N+civilians)` are conserved; civilian counts stay in their original country. Changing occupation with Assign or changing drafted/civilian balance with Draft is distinct from this transfer.

After quantity movement, People command calls source cleanup `$DF6A` at `$96F7–$96FB`. If sourceN is now0, that helper sets source Arms and Skill to0; it does not clear source Morale. Then destination quality native `$95A5`, VM `$95AA–$95F1`, uses `oldDestinationN = currentDestinationN-q` and fixed32-bit weighted helper `$DB48`:

`newDestinationQuality = min(999, floor((oldDestinationQuality*oldDestinationN + currentSourceQuality*q)/(oldDestinationN+q)))`.

This runs independently for Arms, Skill and Morale. Source quality is unchanged when any drafted occupation remains. If the source has just been emptied, **the incoming Arms/Skill values are already0** because cleanup precedes averaging. This is an actual original instruction order and is retained rather than repaired.

Example source counts `[8,0,0,0,0]`, source quality110/124/118, destination counts `[10,0,0,0,0]`, quality150/80/100, q8 troops:

- Source becomes counts0, quality0/0/118.
- Destination becomes18 troops, quality83/44/108.

Keeping one town worker in the source changes incoming Arms/Skill to110/124, so destination quality becomes132/99/108. Tests independently check both cases. A completely empty destination receives source quality when source still has other drafted people, but receives0 Arms/Skill when the entire last source occupation was moved and cleanup zeroed those values. There is no random transport loss or random quality term.

The replay composes two original instruction regions: scalar commit$961C–$962B, then resumes the actual People caller at$96F7–$9703 after the intervening UI-only$E123 call. It explicitly installs the caller's source/destination/quantity locals. It does not claim to execute the skipped UI, source/destination picking, or an uninterrupted whole command.

## Ability and order costs

Success codes: People22, Gold/Food/Item23. Native gate `$827B` checks the command row at `$7E18+6*code`. Source stat order is leadership, judgment, planning, charm, body, battle.

| Command | Leadership | Judgment | Planning | Charm | Body | Battle | Orders |
|---|---:|---:|---:|---:|---:|---:|---:|
|Move People|10|0|0|10|0|0|1|
|Move Gold/Food/Item|5|5|5|0|0|0|1|

Costs are fixed per completed command, independent of quantity. The original parent loop in **bank0**, VM `$8C95–$8CA9`, processes returned code: if code!=$00FF, call original ability debit `$8B18`, then decrement word$7B36 by1. Cancellation/failure code$00FF skips both. Replay executes this complete parent result-processing segment with nested original pointer resolution/debit.

The actor is selected by **word$7B34**. Ability gate occurs before transport source selection. Destination picking temporarily changes this index and restores it; parent success debit therefore uses the restored active ruler context. Fixed `$DDD6` can redirect role102 to its ruler before resolving the six abilities. Tests include exact-cost sufficiency, each required attribute one point short, successful charges, and failed/cancelled result with no charges.

## Controlled second-country runtime setup supplied to root

Use a full-machine baseline restore for each independent trial. All following changes are explicit interventions, not evidence of natural ownership acquisition:

1. Baseline country2 officer pointer is$65DE. Set owner byte`$65FB` from1 to0, matching country1 owner byte$65DC=0. This is sufficient for the scoped same-owner predicates and owned-count preflight. Retain a record of the intervention and original bytes; do not call it a naturally conquered country. Role at$65FC is100 in the baseline; the active country1 role at$65DD must remain100 for main Move.
2. Country2 is already adjacent to country1; no adjacency edit is required.
3. Baseline Gold country1/country2 is439/66. Move10 predicts429/76.
4. Baseline Food is208/90. Move8 predicts200/98.
5. Baseline source drafted counts are61/5/4/6/27; destination63/5/2/5/30. Move8 troops predicts53/71 troops, other occupations unchanged. Source quality stays110/124/118; destination106/122/76→106/122/78.
6. Baseline ruler abilities are at$65CD,$65CF,$65D1,$65D3,$65D5,$65D7. Record both ability and order deltas, source and target ledger, eligibility byte, civilians word, and production-history words.
7. For the empty-source boundary, explicitly set source drafted counts to8/0/0/0/0 and destination10/0/0/0/0 with the example qualities above, then move all8. Preserve the intervention log and do not mix this with a natural campaign claim.

## Reproduction, retained failures, and status

Run `python batches/logistics_source/test_logistics.py` from the workspace root. Dependencies are the read-only `batches/trade_source/replay_trade.py` and its v1.06 authored CPU dependencies, the private ROM, and hashed baseline SRAM. Test logs and file hashes accompany this note.

- Initial run:34 methods, one error because the existing CPU harness lacked official6502 opcode$F1 (SBC indirect,Y), reached by the original switch dispatcher. Raw `tests_initial.*` retained. A small local subclass implements that official instruction; no game opcode, branch, or computed result is substituted.
- Subsequent34-method run passed.
- Extended36-method run failed the actor-pointer assumption (`$7B66` versus actual `$7B34`), retained in `tests_population.*`. This was a useful negative result; the correction is explicit above and communicated to root.
- Final run: **37 methods, OK, exit0**, independently captured by a synchronous subprocess wrapper in `test_run_receipt.json`; complete raw output is `tests_final_complete.stdout/.stderr`. The earlier `tests_verified.stderr` file ended mid-test without a unittest summary; it is retained as incomplete and is not used for success evidence. No failed or incomplete log was overwritten or relabeled.

No scope arithmetic or local predicate dependencies remain unresolved. Original-game transfer execution, source→destination→resale histories, physical-controller input, cold-load persistence and multi-year economic acceptance remain separate proof layers. This batch does not declare those accepted from source tests.
