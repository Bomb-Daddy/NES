# Move Base → later-country Trade

Author: Codex trade/logistics source subagent. Date: 2026-11-04 UTC.

**VERIFIED_STATIC:** original 6502/VM instructions and original callees executed in bounded synthetic states. No Mesen, full country scheduler, screen/controller, Windows, or naturally acquired second-country claim. The parent owns original-emulator validation. Private ROM and baseline hashes are checked by the test suite; neither private input is distributed here.

## Answer and runtime route

Move Base ends the current country's orders, but the baseline schedule permits Trade from the new base **later in the same season**. The baseline displayed country order is `5,12,13,1,3,9,10,4,6,7,2,8,11,14`. Country1 is at zero-based slot3; country2 is at slot10. The scheduler resolves the current officer when that later country is reached. Temujin at country2 therefore receives three fresh orders. If the destination was already processed, relocation does not rewind the schedule and the new base waits for a subsequent schedule.

For a controlled continuation from the parent’s baseline:

1. Establish the explicitly controlled friendly destination: byte `$65FB=0` (country2 officer owner), byte `$65FC=101` (delegated status). This is an intervention, not evidence of naturally acquired ownership. Country1→country2 adjacency is already present in the original data.
2. Use Move → Item to transfer the commodity, then Move → Base → country2. Base is submenu option1 and uses the current country directly; it does not present the separate source-country picker used by Item/Gold/Food.
3. Advance ordinary messages/other-country turns. Do not modify active-country or schedule fields. At country2's later scheduled turn, Trade → Sell uses country2's inventory and Gold.
4. Capture the destination stock, Gold, Temujin Judgment and order count before/after sale. One sale costs5 Judgment and one order; merchant offer/availability gates still apply.

Baseline country2 has all three merchant flags nonzero, at `$6E6B,$6E6D,$6E6F`; its commodity word i is `$6E42+2*i` (Jewels i2 at `$6E46`), Gold `$6E60`. Global sell offer word for merchant m/item i is `$7A8C+22*m+2*i`. These masks remain global and are unaffected by Move Base. Country2 may still encounter ordinary pre-turn random events; the bounded source replay does not predict those intervening events.

## Original source and persistent writes

| Function / region | Result |
|---|---|
| Bank3 native `$97FC`, VM `$9801–981A` | Move requires current ruler status100 and more than one owned country. |
| Bank3 native `$9662`, VM `$9667–9677` | Base checks action21 abilities, calls destination picker `$956E`, aborts on zero selection. |
| Bank3 `$956E`; predicate `$84CA` | Destination requires same owner and source-directed adjacency. Restores the original active-country field after picker use. |
| Bank3 `$967A–9689` | Charge action21, call fixed `$E00B`, explicitly set orders word `$7B36=0`. |
| Fixed `$E00B`, VM `$E010–E031` | Clear first words of the two 19-byte country records and swap officer-pointer words at `country+$51`. |
| Bank3 `$9834–983F` | Successful Base returns sentinel255 to parent Move handler, preventing a second ability/order deduction. |
| Bank0 `$8C95–8CA9` | General command accounting skips sentinel255. |

Action21 cost row at `$7E96..7E9B` is `[0,0,0,0,20,0]` in ability order Leadership, Judgment, Planning, Charm, Body, Battle. Minimum Body20 is allowed; Body19 is rejected. On baseline Temujin Body at `$65D5` changes170→150. Other abilities do not change from relocation alone.

Pointer swap: country1 `$6E1E`: `$65BF→$65DE`; country2 `$6E71`: `$65DE→$65BF`. Temujin's owner/status bytes remain0/100, now reached from country2. With the controlled destination role101, country1 now reaches the delegated officer. The swap does not rewrite role bytes or repair a duplicate-ruler setup: leaving the destination officer at role100 leaves two role100 officers. This is why the controlled setup explicitly uses101.

Fixed `$DCB7` maps country index i to record `$77D2+19*i`; relocation clears the first word of each record. For countries1/2 those are `$77D2/$77E5`. Their broader semantic label is not asserted here. Tests seed321/456 to prove the reset instead of relying on both baseline words already being0.

The exact persistent write set in the selected relocation commit is limited to Temujin Body, both officer pointers, both record words and orders. Gold, Food, every commodity, population, quality and ownership/status bytes remain in place. Active country `$7B34` and schedule position `$7B38` do not change during this commit.

## Why the later turn is playable

Bank0 native `$86CD` processes the country schedule. VM `$86DF–86F2` reads `schedule[$7B38]` from word array `$7B3A`, writes country index `$7B34`, and resolves its pointer through `$DBA7`. VM `$8735–8749` evaluates player ownership with fixed `$DC98` and delegated role101 with fixed `$DE0C`. A player country whose current officer is not101 enters command handler `$8BF8`; others enter policy/AI handler `$8262`.

The command handler `$8C02–8C17` gives3 orders to role100 and1 to another directly controlled role. Its branch through `$8CEA` writes the fresh count. There is no per-person “already acted this season” check in these selected dispatch/initialization regions. Thus the tested baseline slot10 recognizes relocated Temujin as role100 and writes3 orders. Old country1 now fails the direct-player dispatch because its officer is delegated101.

Role102 is a distinct directly controlled role: the dispatch test accepts it, initializes1 order, and ability helper `$DDD6` can route its costs through the same-owner ruler search. This report does not relabel role102 as ruler or delegated governor.

## Source tests and selected continuous-state chain

Run from workspace:

```sh
/usr/bin/python3 batches/base_relocation/test_base.py
/usr/bin/python3 batches/base_relocation/replay_base.py
```

Final result: **19 unittest methods, exit0, complete `Ran 19 tests` / `OK` footer**. Initial18-method pass is retained as `tests_initial.*`; final raw stdout/stderr and receipt are `tests_final.*` and `tests_final_receipt.json`. Dependencies are imported read-only from the trade/logistics batches and earlier v1.06 CPU helper. No source batch was modified.

`source_chain_replays.json` preserves a selected original-instruction chain using the same persistent synthetic state: transfer Jewels1, original scalar-command cost, original Base commit, original later-slot selection, ownership/status dispatch, fresh-order initialization, seller quote and sale commit, original Trade cost/order processing. UI, random pre-turn events and intervening country turns are explicit boundaries, not replaced by fake success values. The active-country field after relocation is written by the original scheduler code.

For q1 from the baseline: source Jewels26→25, destination29→30 after transfer, destination30→29 after sale; source Gold stays439; destination Gold66→71 with baseline China seller quote57 tenths. Temujin abilities finish `[84,67,107,61,150,129]`: transfer costs5 Leadership/5 Judgment/5 Planning, Base20 Body, sale5 Judgment. Orders3→2→0→3→2. Quantities3 and26 are also replayed in the test method. This is selected source execution, not evidence that those UI actions or intervening turns ran in Mesen.

Boundary/negative controls include Body19/20, zero/cancel selection without SRAM writes, all remaining order counts0..3, enemy/self destination rejection, delegated-role dispatch, an already-processed destination schedule, untouched resources, duplicate role100 preservation, and two source-data/instruction mutations: Body cost20→19 and order-clear A0→A1. Mutations change their asserted observations, demonstrating that the checks depend on the executed source path.

Unresolved within this narrow batch: final full-runtime progression through intermediate country events and sale screen (parent task), and the broader meaning of the reset country-record word. Neither prevents identification of the relocation and later-country Trade path.
