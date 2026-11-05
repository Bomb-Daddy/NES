# ROTK II battle knowledge — findings v1.00

**Date:** 2026-11-05. **Author/executor:** Zhuge Liang / ChatGPT, with independent source and evidence review.  
**Scope:** original English NES observations and original SNES source analysis under the Director-authorized unified battle rules.  
**Disposition:** substantive gaps resolved; the Completion Register retains unexecuted edge cases and the pending-allied troop-scale candidate.

## 1. Current reference and authority

The newest complete battle guide located is the **Battle Answer Matrix v1.4, November 1**, [part 1](https://drive.google.com/file/d/1r4KOqPC8uN1VDmaNlfpjHa_k0J__GYZZ/view) and [part 2](https://drive.google.com/file/d/1uUNq4BWRie_b-DR3nh7h0jUqiAgw4Mtd/view). Its detailed source rules remain useful, but its blanket “every question closed” wording is too broad: Q09 explicitly left the in-battle destination writer open; Q14 did not establish the fallback field; Q19 named handlers whose game-level effects were incomplete; and the NES field observations required reconciliation.

The [November 2 trace addendum](https://docs.google.com/document/d/1uZFMG5I0uR5GxmTZ1uhkdbmxSD6hRyhg1dogZ0D_lJk/edit), including its **final correction**, is later evidence. The [November 3 capture branch](https://github.com/Bomb-Daddy/NES/tree/c4a6f5a8d3a6f545a72da097d998356a3b852bd8/ROTK2_NES_Battle_UI_Headless_Capture_20261103) adds actual Normal attack and capture UI. The [November 5 headless operations guide](../../../reference/MESEN_HEADLESS_CHATGPT_OPERATIONS_v1.00.md) is an operations reference, not a replacement mechanics guide. See [README](README.md) for the complete source index.

The Director's November 1 ruling makes the original decoded SNES rules authoritative for the shared NES/SNES battle system. This work preserves that ruling. It distinguishes platform-specific addresses and English NES messages without inventing a separate mechanics system.

The plan was published on the research branch **before** these experiments: commit `9d80c6482a11a1f774635f0466918e95f9b8eddf`. The work below implements it.

## 2. Original runtime and source identity

| Input | Identity | Evidence |
|---|---|---|
| English NES original | 524,304 bytes; SHA-256 `11e312691b516e5e29dade3c8ce5434df3561cc69e1aecad9c948acdfddc517a` | Hash checked before every original-ROM probe; unchanged after execution |
| Original SNES reference | 1,048,576 bytes; SHA-256 `815bfcf4fd6eb23a20c2e50dde023c210b273ffb6cd86a93909d803c3643ce46` | Regenerated source and bounded probes checked against this image |
| Historical Mesen executable | SHA-256 `f02685dafe559addf8dd8d769621a89777137d0c4592436aed65694a7e740676` | Original standalone native test runner; isolated portable profiles |
| Extracted Mesen core | SHA-256 `9d21269be65268c1f56543b83e2f10650f3a28ef2566a4acd861615ecc6ed189` | Exact runtime core matched |

Native headless operation succeeded with **no X server and no DISPLAY**, using `--testRunner --enableStdout --doNotSaveSettings`. A newly saved original checkpoint was cold-loaded in another process. Native load returned true; CPU registers, exposed state fields, 2 KB internal RAM, the 8 KB CPU window, all 32 KB SRAM and 1 KB mapper RAM matched. A decoder-ready PNG also matched byte for byte. An invalid-header checkpoint returned false and was rejected. These are original-emulator results, not port tests.

The recovered runtime and reproducible CLI are documented in [HEADLESS_RUNTIME_v1.01.md](HEADLESS_RUNTIME_v1.01.md). The harness preserves exact schedules, generated Lua, complete stdout/stderr, process exits, source-state hashes, screenshots, memory dumps and native checkpoints in private evidence.

The SNES analysis regenerated **29 modules** from the identified ROM. Independent listing verification compared **177,982 bytes**, with zero missing bytes, overlaps or mismatches, and detected one deliberate changed-byte control in each of the 29 modules. Symbol names and generated pseudocode were treated as clues; critical conclusions were checked against original instruction bytes. Static identity does not itself prove every inferred semantic label.

## 3. G01: current native battle fields and writers

**OBSERVED_RUNTIME / VERIFIED_STATIC.** The following fields are established in the fresh original NES runs while physical SRAM bank 1 holds the battle state. In this bank, CPU `$6000–$7FFF` maps to physical SRAM `$2000–$3FFF`.

| Meaning | Native NES CPU address in bank 1 | Width / observed interpretation |
|---|---|---|
| Defender gold | `$7C19` | 16-bit word |
| Attacker gold | `$7C1B` | 16-bit word |
| Defender rice | `$7C1D` | 32-bit little-endian; hundredths of displayed rice |
| Attacker rice | `$7C21` | 32-bit little-endian; hundredths of displayed rice |
| Weather | `$7C25` | Byte; value 3 correlates with the rain picture in the fresh HUD captures |
| Wind | `$7C26` | Byte; value 4 shows a downward arrow, value 6 shows “No Wind” |
| Result / reason | `$7D0B` / `$7BF4` | Running pair FF/FF; successful whole withdrawal observed as 0/6 |
| Battle day | `$7D1C` | Byte, observed 1 through 11 |
| Attack mode | `$7D0C` | Normal 1, Simultaneous 2, Fire 3, Charge 4 |
| Terrain / occupancy / region | `$7D58` / `$7DF4` / `$7E90` | Three 156-cell grids |

The main side-slot base is `$7BF5`, stride 9. A slot's province pointer is at +2 and deployed-list head at +4. Thus the defending province is `word[$7BF7]`, its fielded list is `word[$7BF9]`, and the attacking fielded list is `word[$7C0B]`.

Actual native writer observations:

| Field | Native CPU store | VM instruction pointer after operand | Mapped PRG offset | Supporting original-ROM operand |
|---|---|---|---|---|
| Day | `$E7C4` | `$8294` | `$10294` | `A9 1C 7D` at headered file `$102A1` |
| Weather | `$E7C4` | `$B5AF` | `$1D5AF` | `A9 25 7C` at headered file `$1D5BC` |
| Wind | `$E7C4` | `$BA9F` | `$1DA9F` | `A9 26 7C` at headered file `$1DAAC` |

The PRG offsets exclude the 16-byte iNES header. VM pointers are observations at the store callback, not a claim that the VM opcode begins at that exact pointer. Food DWORD writes also appeared at native CPU `$F20D`, VM pointer `$B7F2`, mapped PRG `$1D7F2`.

The uninterrupted day-2-to-11 sequence recorded wind writes on entry to **days 4, 7 and 10**, with values **4→6→2→4**. No intervening wind write appeared. Two weather writes appeared. **No same-value wind write was observed in this sample.** This sequence is consistent with the decoded schedule; it is not a statistical revalidation of every RNG probability or weather transition.

The old proposed day field `$7D25` stayed **3** across the same transitions, with zero writes. This establishes the live field in the new run; it does not identify the cause of the older extraction's nine-byte discrepancy.

### Container and bank cautions

The original v1.4 extraction described an older FCS blob and a block at offset 13,312. Those original blobs and the referenced `loop5_nes_run.py` / `loop9_nes_ram.py` were not recovered. The Nestopia reference documents a tag/length/flag wrapper of nine bytes, but the particular historical wrapper position is unverified. Current Mesen MSS uses a nine-byte **whole-payload** compression envelope and a **19-byte** header for its keyed `mapper.saveRam` record. Neither fact proves the old FCS cause.

A fresh Fire run also captured a transient switch of the CPU window to **SRAM bank 3** during a prompt. Interpreting that window as battle bank 1 would give false zero or unrelated values. The harness records conversion metadata and the full SRAM; use the bank-1 physical slice when evaluating the persistent battle state. Do not generalize a CPU-window snapshot across a mapper switch.

## 4. G02: food counts residents as well as fielded defenders

**OBSERVED_RUNTIME with a causal control, supported by VERIFIED_STATIC list traversal.**

The extra defender input is a dereferenced province list:

`defender_resident_head = word[word[$7BF7] + 2]`

In the SNES source the corresponding expression is `word[word[$FC2D] + 2]`. It is not `word[$FC2D+2]`. Native list summation follows each officer's next pointer at +0 and reads the troop word at +C. The defender-selection routine detaches fielded officers and leaves unselected residents in the province list.

In the new one-human, difficulty-1 Yuan Shu P19→Liu Biao P20 run:

| Component | Actual linked-list count | Troops in battle units |
|---|---:|---:|
| Fielded defenders | 10 | 19,000 |
| Resident defenders | 1 | 1,000 |
| Total counted defenders | 11 | 20,000 |
| Fielded attacker | 1 | 10,000 |

The HUD shows **190** defending soldiers because it reports fielded troops. The daily food calculation sees **200** soldiers: `floor(20000/30) = 666` hundredths of rice. The attacker loses `floor(10000/30) = 333` hundredths. The original input schedule sent 200 rice in this run; it must not be described as the later 300-rice reproduction.

The resident officer is at `$68B9`, with troop word `$68C5`. From an identical restored day-2 checkpoint:

| Branch | Explicit intervention | Defender rice before → after | Daily loss |
|---|---|---|---:|
| Unmodified baseline | None | 44,334 → 43,668 | 666 |
| Resident-zero control | Resident troop word 1,000→0 | 44,334 → 43,701 | 633 |
| Unrelated-officer control | Liu Bei's unrelated strategic troop word 100→0 | 44,334 → 43,668 | 666 |

The altered branches are explicitly labeled **controlled RAM interventions**. They are not ordinary campaign sequences. Independent review verified that the restored bank-1 memory differed only in the declared troop bytes. This establishes causation for the resident input.

The two-human difficulty-2 reconstruction selects the documented four defenders—Liu Biao, Kuai Liang, Han Song and Huang Zhu—and sends Yuan Shu, Yuan Yin and Ji Ling with 120 soldiers, zero gold and 300 rice. Its actual initial province has 200 soldiers, including Lu Gong before invasion. The selected fielded force is 130 and the remaining resident force is 70. The current replay therefore does **not** reproduce the old exact 130-fielded/60-resident population. The old raw checkpoint is needed to identify precisely which historical roster difference produced 190 counted soldiers. The extra-list mechanism is resolved; the identity of the old missing 60 is not silently invented. A complete Wait-only day in the new two-human replay changes defender rice 45,000→44,334 (−666) and attacker rice 30,000→29,600 (−400), with every recorded troop count and position unchanged. Its day field moves 1→2 at $7D1C, while $7D25 stays 3. The day-2 HUD shows 130/120 fielded soldiers and 443/296 rice. This independently confirms the current layout and resident contribution in the second-player setup; it does not isolate player count from every other setup difference.

### Correct order of the daily update

The regenerated original source calls food resolution at hex `$7BD2`, then the weather roll at `$7BD5–$7BE9`, then fire handling at `$7BEC`. The matrix's prose ordering of weather before food should be corrected. This matters when attributing changes within a day boundary.

## 5. G03 / G08: where an escaping officer actually goes

**VERIFIED_STATIC source closure, plus OBSERVED_RUNTIME for a successful single-officer whole-army withdrawal.**

The Q09 gap is resolved in the [source report](source_findings/q14_q09/STATIC_FINDINGS_v1.00.md) and [escape-score addendum](source_findings/q14_q09/Q09_ESCAPE_SCORE_ADDENDUM_v1.00.md):

1. Hsovl2 `$D905` constructs eligible adjacent friendly or unowned destinations. No candidates produces “Cannot flee.”
2. `$D78C` selects a one-based province number; FFFF means cancel.
3. `$D648` evaluates escape. Success detaches the officer from the battle list and grid, clears the battle position, divides troops by 100 for a peaceful destination, and calls root `$5154(destination, officer)` to insert the officer into the province and maintain its ownership/list links.
4. If the destination is already under attack, troops remain in battle scale. Failure instead places the officer in the capture path, without destination insertion.
5. Whole-army withdrawal loops through the officers; each officer's result must be examined. It is not sufficient to inspect the terminal message.

In the fresh original NES whole-withdrawal run, the popup lists **29 Open territory, 31 Open territory, 19 Last province**. The default first entry, Province 29, was accepted by the exact recorded input timing. No claim is made that a separate explicit “29” input was issued.

Before the change Yuan Shu is the sole attacking fielded officer at `$672A`, with 10,000 battle troop units and position C2. After the original transfer:

- The attacking fielded-list head becomes zero.
- P29's officer-list head becomes `$672A`; its owner becomes Yuan Shu's faction, 4.
- Yuan Shu's troop word becomes **100** at campaign scale; position becomes FF and mobility zero.
- The result/reason pair becomes **0/6**.
- The game displays **“Yuan Shu was crushed”** and **“Yuan Shu succumbed to a greater power.”**

Two fresh runs reached the same persistent transfer. A further native continuation returned to the campaign province-selection screen with Yuan Shu still in P29 at 100 soldiers. P29's pre-existing gold and rice remained 100 and 20 in this path. P20 receives gold 1,000 and rice 640, matching the combined remaining battle pools (44,334+19,667 hundredths, divided by 100). These observations agree with the source rule that the withdrawing officer takes soldiers while the battle pools settle into the contested province.

Thus these defeat messages are **not proof of capture or death**. They occur here with a verified successful escape. This result concerns the sole fielded attacker; it does not claim that a multi-officer whole withdrawal always escapes everyone. The historical undocumented withdrawal's exact fate cannot be recovered from its text alone.

### Individual Flee, friendly destination and cancellation

An additional ordinary-input branch uses the three-attacker, two-human battle. Yuan Shu first Waits, then noncommander Yuan Yin selects Flee and the visible **19 Last province** entry. The attempt fails: Yuan Yin ($673F) leaves the attacking list and becomes the defender's prisoner-list head at $7BFB, retains 1,000 battle troop units, and gets position FF. He is not inserted into P19. The battle remains running with FF/FF. Thus selecting a valid province does not guarantee escape.

From that same destination prompt, B cancellation preserves all three attacking officers and their troop counts, the friendly province list, and the 30,000-rice attack pool; Yuan Yin's command menu returns.

Ji Ling then attempts the same friendly destination in the continuing failure branch and succeeds. His record ($6754) is inserted into P19's serving-officer list, his troops convert **1,000→10**, and position becomes FF. The attack list retains Yuan Shu alone; Yuan Yin remains on the defender's prisoner list. These native branches distinguish successful individual transfer, failed escape, and cancellation using persistent lists rather than menu intent. Case name `individual_flee_home_001` records the intended destination; its actual outcome is failed escape, not a successful home transfer.


## 6. G04: Release fallback and record-type corrections

**VERIFIED_STATIC.** Sengo `$8436` first considers the supplied province, then performs its ordered adjacent search. If those searches find no destination, it returns **`word[ruler+2]`**, the head of that ruler's owned-province list. The pointer's meaning is established from original list/ownership writers, not guessed from an informal symbol name.

For an ordinary released officer, a nonzero destination leads to the normal insertion path, including battle scaling if that province is under attack. A zero result follows the separate clearing/free-officer path. A defeated ruler with a nonzero destination can be released or beheaded; when the destination helper returns zero, the source offers only beheading. An empty owned-province list alone is not the full predicate, because an eligible supplied or adjacent unowned province can be returned before that fallback. The [complete report](source_findings/q14_q09/STATIC_FINDINGS_v1.00.md) preserves special-flag handling and exact branches.

This closes the unknown return-field semantics under the unified source authority. A deliberately prepared original runtime case with no suitable adjacent province was not used to claim ordinary campaign coverage.

Two schema errors also require correction:

- SNES `$F204`, stride 34, is the **ruler table**, not the general officer table. Actual SNES officers begin at `$E04C`, stride 21; provinces at `$F424`, stride 25.
- The NES officer record at `$6031`, stride 21, uses **decimal +18 (hex +12) for battle position** and **decimal +19/+20 (hex +13/+14) for the name ID word**. The older single-byte “removal marker +19” label is incomplete. Successful withdrawal clears position to FF and preserves the name ID; deletion can instead set the name ID word to FFFF, as the prepared beheading fixture confirms. These are different state transitions.

## 7. G05 / G06: Q19 handlers and transfer semantics

**VERIFIED_STATIC.** The [complete Q19 analysis](source_findings/q19/Q19_STATIC_CLOSURE_v1.00.md) follows the five formerly incomplete handlers through their branch conditions, randomness, persistent effects and callers. Several inherited routine names imply combat even though their functions are strategic:

| Former label / location | Established role |
|---|---|
| `dispatch_officer_combat_cell`, csovl1 `$D131` | Preferred-partner diplomacy: marriage, gifts or alliance |
| `scan_officer_province_links`, csovl1 `$D004` | Joint agreement, hostile target, ally or covert-action selection |
| `rank_officers_B97A`, comroot `$B97A` | Foreign officer recruitment, with personal/horse/gold/general routes |
| `draw_combat_anim_cell`, csovl1 `$CF7C` | Covert action against a rival frontier province |
| `rank_officers_B8D1`, comroot `$B8D1` | Activation of an already planted spy |

A nonzero dispatch return stops the action queue; it does **not** by itself mean a diplomatic attempt succeeded. Result flags carry that distinction. The source report documents the bounded action budget, rotating slots and eligibility rules.

The disputed `$D409` handler is **friendly transfer / empty-province occupation**. Its destination selectors allow same-owner or unowned provinces, not a hostile occupied destination. `$98F4` moves officer links and resources, and updates ownership for empty territory. `$BBF6` resets the strategic officer queue. The actual hostile-launch path is separate at `$D92B/$D98A` and can fall back to a transfer. The old names should not be used as proof that `$D409` launches an invasion.

The original rice transfer uses 16-bit intermediate behavior:

`floor(u16(rice * selected_troops) / u16(total_troops + 1))`

For the explicitly synthetic arithmetic example rice=6,000, selected=200, total=600, the original low-word calculation transfers **33**, whereas widening the product would produce 1,996. This is an original-instruction arithmetic finding, not an observed ordinary campaign frequency claim. Preserve it in Classic behavior unless the Director explicitly authorizes a separate change.

Bounded execution of actual original instruction bytes against synthetic RAM passed **12 queue fixtures, 7 arithmetic fixtures and 3 negative controls**. The probe deliberately lacks full game startup, peripherals and RNG. Its evidence label is VERIFIED_STATIC; it is not OBSERVED_RUNTIME or a Windows test.

## 8. G07: additional NES attack and result evidence

The new captures extend the November 3 Normal-only observations. All ordinary attack branches below restore the same naturally reached day-5 checkpoint: Yuan Shu at C4 with 9,847 battle troop units, adjacent Deng Yi at B4 with 1,000. The movement path into the forest and all intervening turns are retained.

| Command | Verified mode | First completed original exchange, attacker / target | Scope |
|---|---:|---:|---|
| Normal | 1 | 9,608 / 692 | One adjacent target |
| Simultaneous | 2 | 9,608 / 692 | Only one enemy is adjacent in this fixture; multiple-target coverage is not claimed |
| Charge | 4 | 9,641 / 856 | One exchange observed; neither officer removed and attacker stayed C4 |

The figures above stop before later enemy-phase combat. For example, a later Normal-branch snapshot has further losses; it must not be substituted for the player's first exchange. The Charge matchup screen identifies Deng Yi and Yuan Shu with their displayed troop totals.

Fire was observed separately from the original unmoved position C2. On day 5, weather 1 and wind 6, Fire mode 3 toward C3 sets occupancy `$7E27` from 00 to 20. It displays **“It worked!”**. The same burning cell clears 20→00 during the end-of-day fire step with “No Wind”; day 6 follows. On the earlier day-2 branch the attempt creates no fire and displays **“Oops!”**. These are the actual English NES messages; the matrix's “Caught fire!” / “Didn't work” wording must not be copied into the NES UI as if observed there.

The successful Fire run establishes ignition and no-wind extinction. It does not claim a fresh runtime demonstration of rain clearing an existing fire, every terrain probability, simultaneous multi-target ordering, every Charge round count, or all capture/death branches. Shared source rules remain authoritative where already decoded.

### Filled white cursor

The [original NES marker audit](source_findings/white_marker/NES_WHITE_MARKER_FINDINGS_v1.00.md) identifies the fresh 16×16 filled white square as the blinking battle-cursor overlay. The turn loop derives its coordinates from the acting officer. Four $E0 sprites at palette3, sprite slot $20, blink in ten-frame on/off phases through native kernel $F8A5–$F8FA. The C2 occupancy remains $04 throughout; this UI effect is independent of occupancy bit $80. A clean 43-frame repeat recorded 20 expected shadow-OAM writes and no C2 occupancy writes. The direction arrow shares the blink facility but uses a different template and slot $24. The exact match to the unavailable historical S-02 screenshot remains inferred.

### Prepared capture and ruler menus

To isolate the unrecorded aftermath UI, an explicit cloned-state fixture swaps Liu Biao's and Deng Yi's field positions and sets Liu Biao's battle troop word to100. All three patches and every descendant checkpoint are declared; this setup is not presented as ordinary campaign play. The original Normal attack then captures Liu Biao, produces result/reason **1/1**, and reaches the prisoner decisions.

The ordinary Han Song menu offers **1.Recruit / 2.Release / 3.Behead**. Separate branches from the same menu verify recruitment into P20 with allegiance4; release into P29 with allegiance7; and deletion with name-ID word FFFF after beheading. The defeated-ruler menu offers **1.Release / 2.Behead**. The Release branch says “Liu Biao was set free” and inserts him at the head of P29's serving list. The Behead branch displays a death message; after acknowledgement it displays that Cai Mao is the new ruler. Those text displays precede the final record writes. The separately retained continuation `prepared_ruler_death_commit_001` verifies the committed result at frames50,100 and200: Liu Biao's name ID becomes FFFF, and both ruler7 and P29 serving-list heads become $6850, Cai Mao. Raw writes occur at frame15, before the scheduled A pulse at20; the evidence establishes the continuation and commit order, not that this later button press caused the writes.

These prepared captures supply the missing UI/branch observations, not evidence that the edited opening position was reached legally. The [prepared UI review v1.00](evidence/prepared_ui_review/REVIEW_v1.00.md) and its [v1.01 commit supplement](evidence/prepared_ui_review/REVIEW_v1.01.md) record exactly which persistent transitions were committed at each sample. The combined review covers eleven runs, 221 products, 103 PNGs, 35 snapshot frames and thirteen saved-state files.


### Battle summary versus officer View

The fresh main-menu B test opens the overall battle summary panel. The separate View command opens an officer-selection prompt, “View whom? (A)View (B)Quit.” The earlier capture matrix's claim that B is ignored at the battle root, and its conflation of View with this summary panel, should not be used as the NES input reference. The original `summary_b` and `view_day2_001` runs preserve both cases; Start and Select did not open the summary in their recorded tests.

## 9. G09 / Q12: one remaining troop-scale reachability question

**PARTIAL. VERIFIED_STATIC conditional branch; INFERRED campaign candidate; ordinary-play replay NOT RUN.**

The expanded [writer audit](source_findings/q14_q09/G09_WRITER_AUDIT_v1.00.md) reviewed 57 matching +C write sites: 47 officer-troop writes, eight province-population writes and two ruler-aggregate writes. Ordinary assignment and redistribution are capped at 100; a conservative special-scenario initialization bound is 500; an unsigned battle return divided by 100 is at most 655. A typed syntax inventory is not proof that every aliased or bulk write has been found.

The remaining candidate is a **list transition without a troop-word write**. Comroot `$9A45` scales allied reinforcements ×100 and leaves them pending. Deployment can leave them pending if all eligible entry cells are unavailable. Original map inspection verified five entry cells for each of 186 adjacent regions across 41 maps. At resumed battle setup, hsovl3 `$D900` can return pending allied officers to a peaceful province after the alliance has ended **without dividing their troop word by 100**.

The [next experiment](source_findings/q14_q09/G09_NEXT_EXPERIMENT_v1.00.md) specifies the exact slot, pointers, flags, splice, pending-list clear and negative controls. If a legitimately sent 100-soldier officer remains at 10,000 when returned to the strategic list, a later original ×100 operation would wrap to 16,960. That is a conditional source result. The missing proof is a legal campaign reaching all required conditions: pending auxiliary force through the month boundary, unchanged peaceful source, and ended alliance before resume.

A prepared RAM fixture could verify the conditional branch but would not close ordinary-play reachability. No cap, normalization fix or changed Classic rule is authorized on this candidate alone.

## 10. Review findings and limits

Independent review checked source semantics and raw original-state evidence. It found a real harness weakness: the first PNG gate accepted a header-only truncated image. The corrected gate checks PNG structure, chunk CRCs, IDAT/IEND, bounded decompression, exact scanline size and filter bytes. Failing tests were preserved before the fix. Existing reviewed captures decoded correctly.

It also found one retained executable copy in `weather_day3` truncated relative to the prelaunch hash in its receipt. The raw products and extracted core remained valid, but the cause and exact timing of that retained-file mismatch are unknown. The original directory was preserved. A fresh `weather_day3_recheck` restored the same state, reproduced the rain HUD and matching memory, and retained the complete canonical executable. The runner now verifies the copied executable **both before launch and after exit**, and includes post-run identity in its acceptance gate. This is an evidence-integrity correction, not a game-mechanics discovery.

The final published manifest records the actual successful and failed runs. No failures were deleted or relabeled as raw successes. The [Completion Register](COMPLETION_REGISTER.md) is the authority for coverage: source closure, selected original-runtime observations and ordinary campaign reachability are separate statuses. Windows launch, Xbox-only control, saves in the Windows port and full campaign acceptance were outside this research execution.
