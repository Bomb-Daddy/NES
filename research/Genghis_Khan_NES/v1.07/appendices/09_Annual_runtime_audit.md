> This is the preserved source-batch explanation. Its original runtime targets and stage-specific status are resolved or explicitly bounded by the v1.07 master and final acceptance register; these source tests are not relabelled as Mesen playthroughs.

> v1.07 supersession: controlled native Autumn harvest cases now show extra-tax-used1→0 and both histories clear. Ally writer evidence is independently checked. See final acceptance and native verification for current results.

# Natural Mongol year: independent economic audit

Author: Codex seasonal-source auditor. Date: 2026-11-04 UTC. Source authority: private authorized English NES ROM SHA-256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`, the six original-game captures under `batches/runtime/live`, and the bank-aware source reports under `batches/seasonal_source`.

**All deterministic country1 annual economic values match the source formulas.** The only change outside those annual formulas is the 10 Gold received during the Fall treaty response. Country8 loses exactly 10 at the same time; the relevant original treaty routine makes exactly that transfer regardless of acceptance. The later `ally_gold_writer.json` native write trace now confirms that event attribution as **OBSERVED_RUNTIME**. The independent `check_ally_writer.py` validation ties the real store to the original treaty caller, target address and amount.

The audit independently read six 8192-byte SRAM captures, checked each against its recorded SHA-256, inspected the two Fall screenshots and original input log, and compared the full economic field set. It did not start Mesen. Captured state values are **OBSERVED_RUNTIME** evidence produced by the root runtime session; the new executions below are **VERIFIED_STATIC** source replays seeded from those captured SRAM values.

## Observed country1 ledger

The first five columns are in1175; the final column is Spring1176. Original frames are3615,6577,10574,11851,15228,18480. All numbers below come from the SRAM bytes, including the unaligned resident word and trained byte.

| Field | Baseline Spring | Summer | Fall ready | Fall player | Winter | Spring1176 |
|---|---:|---:|---:|---:|---:|---:|
| Gold |439|439|439|449|449|449|
| Food |208|208|289|289|289|238|
| Fur, specialty index0 |73|73|91|91|91|91|
| Economy |185|183|181|181|179|177|
| Defense |115|115|115|115|115|115|
| Arms |110|110|110|110|106|106|
| Skill |124|115|115|115|115|115|
| Troops |61|61|61|61|61|62|
| Town |5|5|5|5|5|6|
| Masons |4|4|4|4|4|5|
| Artisans |6|6|6|6|6|6|
| Foodworkers |27|27|27|27|27|27|
| Allocated total |103|103|103|103|103|106|
| Commodity history |6|12|0|0|6|12|
| Food history |27|54|0|0|27|54|
| Morale |118|118|118|118|118|116|
| Ordinary Tax |35|35|35|35|35|35|
| Extra Tax used |0|0|0|0|0|0|
| Resident reserve |34|34|34|34|34|36|
| Trained percentage byte |32|32|32|32|32|33|
| Derived eligible reserve |10|10|10|10|10|11|
| Merchant status words |1,1,1|1,1,0|1,1,0|1,1,0|1,1,0|1,1,1|
| Rate |101|99|91|91|85|110|

All nine non-specialty goods remain `[29,26,16,20,50,21,30,41,18]`. Lord pointer stays `$65BF`. Year/season and difficulty1 are consistent throughout. The snapshot JSON's even-address `country_words` view crosses the one-byte trained field and the unaligned resident word, so those particular displayed words must not be interpreted as standalone game quantities. This audit decodes the true widths from SRAM.

## Exact explanations and residuals

Common development occurs before the seasonal effect. Economy loses floor4% and gains Town5:185→183→181→179→177. Defense115 loses4 and gains Masons4, so it stays115. Summer Skill is floor(124×93/100)=115. Winter Arms is floor(110×97/100)=106.

The first observed Fall harvest adds **81 Food and18 Fur**. The baseline already contains history27/6; the transitions into Summer and Fall add another27/6 each, yielding81/18 immediately before harvest. Thus Food208+81=289 and Fur73+18=91; both histories reset0. This particular first-year capture does not represent four newly observed seasonal contributions. The incoming baseline history is actual captured state, not a fourth contribution invented by the model.

Winter→Spring adds the Winter workers to history, so Food history27→54 and commodity history6→12. Food upkeep uses the pre-growth allocated total103:289−floor(103/2)=238. Economy has already become177 when tax receipts are calculated:

```
gross = floor(floor(177*35/100) * (12+min(13,floor(118/9))) /25)
      =61
payroll=61
Gold=449+61-61=449
```

The unchanged Gold therefore corroborates equal receipts and payroll in this state; it does not establish that neither operation occurred. Tax Morale reduction is later:118−floor(118×2/100)=116. Population growth adds floor(103×103/100)−103=3; the remainder order gives one each to Troops, Masons, Town. Artisans and Foodworkers receive none. Reserve34→36 independently; trained percentage becomes floor(100×(floor(34×35/100)+1)/(floor(34×105/100)+1))=33. Eligible reserve floor(36×33/100)=11 remains distinct from allocated total106.

No deterministic country1 economic residual remains in the four season-changing intervals. The script records every comparison, including unchanged goods, histories, reserve, Tax, and lord pointer.

The following are deliberately **not** promoted to fully traced outcomes:

- Rate101→99 is compatible only with a2% draw;99→91 with9%;91→85 with7%. Spring110 requires modulo101 result60. These outputs fit the source range and rounding, but the incoming native RNG state at each draw was not captured by these six SRAM files. This is compatibility, not exact RNG-sequence replay.
- The changes in merchant-status words and the global offer masks are random-refresh outcomes. Their exact draw histories remain unresolved. Country1 status changes are listed explicitly above; no deterministic economic discrepancy is hidden by them.
- The13 AI countries perform autonomous orders, transfers and training between broad snapshots. Their complete economic field deltas are preserved in five `*_all_country_deltas.json` files, but the audit does not claim to assign each change to its exact AI branch. Apart from the short Fall treaty interval, those AI trajectories remain **UNKNOWN at per-action attribution level**.
- Extra Tax used is0 throughout, so this natural sequence does not demonstrate its reset from1. The Autumn reset remains source evidence until a controlled runtime flag case is added.

## Fall treaty transfer

`fall1175_ready.png` explicitly shows a messenger from Toktogho of country8 saying “Let's ally,” with Ally/Refuse choices. The next recorded actions are Down with60-frame pause, A with600, then A with600. The later snapshot is the country1 command menu.

The complete economic change over that interval is:

| Country | Gold before | Gold after | Delta |
|---|---:|---:|---:|
|1, player|439|449|+10|
|8, Toktogho|19|9|−10|
|Every other country|unchanged|unchanged|0|

All other decoded economic fields of **all14 countries** remain identical in that interval. The source is physical bank2 native `$938C`. Human-target response is obtained through `$8E75` with selector2. Immediately afterward, VM `$93AA-$93BB` performs:

```
subtract_saturated(proposingCountry.Gold,10)
add_capped999(receivingCountry.Gold,10)
```

The response value is not tested until `$93CF`, after both transfers. Acceptance then sets a treaty byte to5 and selects the “signed a peace treaty” text at `$B22E`; rejection selects “refused to sign a peace treaty” at `$B247`. The surrounding caller's eligibility gates still matter; the transfer formula does not imply a player can invoke it arbitrarily.

Two new bounded original-instruction replays load the actual `fall1175_ready.sram`, set source/recipient to country8/country1, supply response0 and1 in separate cases, and execute `$93AA` through the stop before `$93BC`. Both produce country1 Gold449 and country8 Gold9, and match every captured economic field of all14 countries in `fall1175_player`. Native 6502 instructions and the actual subtraction/add-cap helpers execute; no transfer formula is substituted.

This exact source-state match was initially classified INFERRED. The subsequent native writer trace supersedes that limitation: physical bank2 is verified from its unique16-byte header; the live VM frame has saved return`$93BB`, target argument`$6E0D`, amount10 and computed result449. The following section records the check. The treaty table remains unchanged across the response captures; no treaty-signing claim is made.

The root restored the Fall state and captured a real write breakpoint on`$6E0D-$6E0E`. The event reports write address`$6E0E`, value1, native PC`$F41D`; VM PC is`$DAAC`, immediately following the helper’s store opcode at`$DAAB`. Source bytes show the completed high-byte store at`$F41B` and the next instruction`DEY` at`$F41D`. This distinction matters: old439 (`$01B7`) and new449 (`$01C1`) share highbyte1, so the paused memory still reads439 until the subsequent low-byte store at`$F420`. At that pause country8 already has9 Gold. After resuming, the hash-verified snapshot has country1=449 and country8=9.

The recorded VM frame starts at`$0553`; its saved return word at+6 is`$93BB`, the final byte of bank2 call`$93B8` to add-cap helper`$DA99`. Arguments at+11/+13 are`$6E0D` and10; the popped destination at frame−2 is also`$6E0D`. `check_ally_writer.py` asserts these offsets, exact original call/store bytes, unique physical bank, breakpoint event, input/output SRAM hashes and temporal ordering. It exits0. This closes the real treaty Gold-writer attribution without treating a screenshot alone as a native execution trace.

## Exact matched staffing interventions before Winter Pass

Restore `winter1175_menu.mss` separately for every case. Preserve all other state, including RNG and timing. Apply the specified **16-bit little-endian RAM writes** before the same Pass input sequence. Each treatment preserves allocated total103 and Troops61, so Food upkeep, payroll and annual growth amount remain matched. The patches change the worker allocations; they do not spend an order through the Assign UI.

All four predictions have been checked using original instruction slices loaded from `winter_step2.sram`: common labor, country1 Food, country1 tax/payroll, then the complete14-country annual Morale, trained-percentage and population loops. Routines between these slices, such as sickness and display events, were not silently emulated; the intervention predictions are conditioned on no intervening adverse event, command or diplomatic transfer.

| Case | Exact patch before Pass | Spring Economy | Defense | Gold | Food | Commodity history | Food history | Spring groups: Troops,Town,Masons,Artisans,Foodworkers |
|---|---|---:|---:|---:|---:|---:|---:|---|
|Control|None|177|115|449|238|12|54|62,6,5,6,27|
|Town +10|`$6DE9=15`, `$6E0B=17`|187|115|453|238|12|44|62,16,5,6,17|
|Masons +10|`$6DED=14`, `$6E0B=17`|177|125|449|238|12|44|62,6,15,6,17|
|Artisans +10|`$6E07=16`, `$6E0B=17`|177|115|449|238|22|44|62,6,5,16,17|

Across all four, Arms106, Skill115, Morale116, Tax35, resident reserve36, trained33, eligible11, Fur91 and the other goods remain the same. The Town treatment produces tax65 versus control61; payroll remains61, revealing a4-Gold difference despite control's zero net Gold change. Masons and Artisans are negative controls for Gold and Economy. Histories still measure pre-growth workers: the new Spring Town/Masons units do not retroactively contribute to the completed Winter.

Rate and merchant flags should be logged, but the table intentionally does not assert a particular random output from the source-slice harness's synthetic seed. If the native matched runs follow the same draw history, these unrelated random fields should agree; differences require tracing, not adjusting the expected deterministic arithmetic after observing results.

## Reproducibility and outputs

Command from workspace root:

```
PYTHONDONTWRITEBYTECODE=1 python batches/annual_runtime_audit/audit_runtime.py
```

The original audit and the later native-writer checker both exited0; stdout/stderr are retained, including the earlier audit pass. The private ROM path can be overridden with `GK_ROM`. The main audit also invokes `check_ally_writer.verify()` and records the confirmed native attribution. The script imports the already frozen `batches/seasonal_source/source_cpu.py` and `inspect_source.py`; it writes only inside this audit directory. It asserts input hashes, all deterministic country1 predictions, both treaty responses, all14-country economic equality after the bounded treaty transfer, and all four staffing predictions.

Result: six verified captures, five intervals, four staffing cases,26 source slices,1,302,740 original CPU instructions. These are audit/source execution counts, not26 new Mesen gameplay experiments. `annual_events_raw_excerpt.jsonl` is explicitly an excerpt of the first1026 raw original events, ending after the first-year capture checkpoint; the live source log may continue to grow during other runtime tests.
