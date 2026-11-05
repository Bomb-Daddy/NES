# ROTK II — Q14/Q09 static investigation and G09 boundary findings

**Date:** 2026-11-05 UTC  
**Author:** Cao Cao research worker / Zhuge Liang investigation  
**Scope:** Published battle-gap plan Tasks 3–4 support; Q14 Release, Q09 Flee/Withdraw, and proposed G09 strategic troop reachability.  
**Evidence class:** VERIFIED_STATIC. No original game, original VM, Windows port, or controller test was executed by this worker.

## Disposition

| Item | Result | Remaining acceptance boundary |
|---|---|---|
| Q14 / G04 unknown `lord+2` return field | **Source question closed.** It is the head pointer of the ruler's linked list of owned provinces. | The deliberately constructed no-adjacent-match runtime case remains NOT RUN. |
| Q09 / G03 destination writer | **Source question closed.** The selector returns a province number; successful escape detaches the battle officer and calls the province officer-list insertion routine. | Native before/after capture and full port acceptance remain NOT RUN by this worker. |
| G08 historical crushed/succumbed withdrawal | **PARTIAL.** A failed-escape capture path is identified, but it does not establish what happened in the old run. | Reproduce and record the actual selection, escape branch, battle result, and persistent state. |
| Proposed G09 / Q12 strategic value above 655 before ×100 | **PARTIAL.** Player assignment and AI redistribution cap at 100; initialization has a conservative bound of 500; return through unsigned ÷100 yields at most 655. | Prove the complete reachable strategic-state invariant or find a legal sequence that violates it. No new 655 clamp is authorized. |
| Q03 supporting pointer type | **Source correction verified.** `word[$FC2D]` is the defending province pointer; its resident officer-list head is `word[word[$FC2D]+2]`. | Root's independent runtime census is required to identify the historical extra 60. |

These conclusions use the Director-approved unified NES/SNES mechanics authority. They do not create a NES-only rederivation gate or change approved game rules.

## 1. Source identity and reproducibility

The original, authorized SNES image has 1,048,576 bytes and SHA-256:

`815bfcf4fd6eb23a20c2e50dde023c210b273ffb6cd86a93909d803c3643ce46`

The symbol repository is [rot3k2-snes-decompiler at 7c2bab7](https://github.com/chrisaacson69/rot3k2-snes-decompiler/tree/7c2bab7d634e22a3f5870fe548d6a0556760be98). The current generator is [emu-harness at 9a436f4](https://github.com/chrisaacson69/emu-harness/tree/9a436f493bf20f760c5eac49014163c2570e77bd). The historical `koei-snes` repository is a retired forwarding wrapper. The generator was installed into an isolated Python virtual environment and invoked through its installed module entry point; the generation receipts identify Python 3.12.14.

All 29 code modules were regenerated from the pinned original image into a new scratch directory. Neither upstream source tree was edited. The two generation receipts preserve exact commands, Python version, repository commits, timestamps, stdout, stderr, exits, output lengths and hashes. Both generation commands exited 0 with empty stderr.

`verify_listing_bytes.py` independently parses the ROM archive directory at file offset `$7800`, locates each module, derives its load base from its header, then compares every raw listing byte against the ROM. **29 modules / 177,982 bytes matched, with zero omitted bytes, overlaps or mismatches.** An in-memory mutation of the first listed byte was detected for all 29 modules. This establishes byte provenance; it is not a claim that every generated symbol name or lifted C expression is correct.

`collect_static_evidence.py` separately checked **23 reviewed instruction anchors** and read the **14 scenario multiplier rows** directly from the ROM. All assertions passed. `key_static_evidence.json` records exact file offsets and inspected bytes for these anchors. The full source listings and commercial image remain private inputs; publish authored scripts, this report and evidence manifests only.

### Relevant module locations

| Module | ROM file offset | WRAM load base | Bytes |
|---|---:|---:|---:|
| root | `$08000` | `$2000` | 20,978 |
| settei | `$0D1F2` | `$7200` | 10,816 |
| comroot | `$22B76` | `$7200` | 20,072 |
| excsub2 | `$28376` | `$C300` | 1,876 |
| excsub4 | `$291E6` | `$C300` | 1,810 |
| hex | `$35D82` | `$7200` | 22,898 |
| hsovl2 | `$3C90C` | `$CC00` | 4,264 |
| hsovl3 | `$3D9B4` | `$CC00` | 4,054 |
| sengo | `$42006` | `$7200` | 12,078 |

For each listed routine, `ROM file offset = module file offset + WRAM address − module load base`. All addresses below are from the SNES reference and are not assertions about NES RAM placement.

### Correct record typing

Several old names are misleading. In particular, the symbol `officer_table` at `$F204` denotes **16 ruler records of 34 bytes**, initialized by settei `$932C`. Ruler `+0` points to the ruler's officer record; ruler `+2` is the owned-province list head. The scenario officer loader at settei `$93D6` starts officer records at `$E04C` and advances by **21 bytes**. Province records are 25 bytes starting at `$F424`.

Native root `$697C` reads an officer's liege index at `+7`; `$69B9` reads a province's owner index at `+E`. Both convert non-`FF` indexes to `$F204 + index×34`, or return zero for `FF`. These original instructions independently establish the ruler/province distinction. Do not build probes that treat `$F204` as a table of all officers with 34-byte stride. This corrects source schema, not battle mechanics.

## 2. Q14 — Release no-adjacent-match fallback

### Complete return decision

The sengo helper at `$8436` takes a ruler pointer and the relevant battle slot's province pointer.

| Order | Condition | Returned destination |
|---|---|---|
| 1 | The supplied slot province is unowned, or its owner equals the supplied ruler | That slot province |
| 2 | The two-pass adjacent scan finds a destination | That adjacent province |
| 3 | Neither previous branch returns a destination | `word[ruler+2]`, the first linked owned province, or zero if the list is empty |

The adjacent scan at sengo `$83A4` examines six neighbors of the global contested province (`$FC28`). Pass 0 accepts a matching-owner province whose `+F` field is `FF` (not under attack). Pass 1 accepts matching-owner provinces regardless of that flag, or an unowned province. It returns the first eligible province in its traversal, otherwise zero.

The formerly unknown fallback is exactly sengo `$8462–$8465`, ROM file `$43268`: `load arg1; add 2; load word; return` (`0C 72 B0 CF`). Its type is proved by writers, not by a guessed field name:

- root `$507C` writes a province owner at `province+E` and inserts that province into `ruler+2`, linking provinces through their `+0` word.
- root `$5091–$5098` initializes an empty ruler list to the supplied province and zeroes that province's next pointer.
- root `$50A5–$50BC` prepends the ruler's own province or appends an ordinary province to the existing owned-province list.
- root `$51BD–$51CA` moves a ruler's current province to the front when the ruler officer is inserted there.

The fallback should therefore be described as **the ruler's first linked owned province**. It is not a hard-coded adjacent province, an officer pointer, or an arbitrary zero/default province. A ruler's current province is deliberately promoted to the head by the writer, but the exact pointer returned remains the list head; avoid replacing this with a broader invented “capital selection” rule.

### Caller behavior at zero and nonzero return

For an ordinary released officer, sengo `$84A7` calls `$8436`. A nonzero destination reaches `$837D`, which multiplies strategic troops by 100 if that destination is still under attack, then calls root `$5154` to insert the officer into its province list.

If the helper returns zero, sengo `$84F6–$84FA` instead calls root `$4EB0` with the contested province and officer. That routine clears troop count, resets training to 100, and normally routes the officer through root `$5122` into the province's free-officer list (`province+4`) with liege `FF`. Preserve the special flagged-officer return branch at root `$4ED8–$4EE8`; flattening it into unconditional free-list insertion would change source behavior.

For a defeated ruler in the player-facing handling at sengo `$85FE`, the zero return at `$868D` branches to `$86CB`, where the menu offers only Behead. A nonzero return permits the Release/Behead choice. The AI handling at `$8593` also treats zero separately. Thus a missing owned-province head is a meaningful caller boundary, not a reason to manufacture a fallback province.

### Focused acceptance cases

The remaining original-runtime or port tests should exercise: valid supplied province; first-pass friendly neighbor; second-pass attacked-friendly or unowned neighbor; no adjacent hit with a nonzero owned head; and no adjacent hit with a zero head. In the fourth case, change only the owned-list head and verify that the returned destination follows it. For ordinary Release, check exact destination list membership and troop scale; for defeated-ruler handling, verify the corresponding menu branch. These cases are specified here, **not claimed executed**.

## 3. Q09 — Individual Flee and whole-army Withdraw

### Selection and eligibility

Player entry hsovl2 `$D905` calls hex `$7FED` to build the destination list. That builder walks the contested province's six neighbors, using a direction order shuffled by root `$28AB`, and stores eligible **province pointers** in a zero-terminated array at `$FC17`; `$FF5F` receives the count. It accepts an unowned province or a province whose officer-list head has the same liege as the fleeing officer. It does not apply an “under attack” exclusion in this builder.

An empty list produces “Cannot flee” and returns before a move. The destination helper hsovl2 `$D78C` is a selector despite its old `officer_move_exec` name: it displays Home/Open/Ruled labels and returns the selected province's **one-based province number**, calculated from the selected `$FC17` pointer. It returns `FFFF` on cancellation. The validator at `$D877` confirms that the number is present in that same list.

For an individual officer, `$D989` calls the selector and `$D9AA` calls the actual writer `$D648` after validation. Cancellation bypasses that writer and the action-completed write. For a primary commander, the entry first asks whether to withdraw the whole army. The loop at `$D89D` preserves each officer's next pointer, asks for a destination for each officer, validates it and calls `$D648` independently. Cancellation is allowed at the initial current-officer prompt; later invalid/cancel results repeat the prompt.

### Actual persistent writer

The destination writer is **hsovl2 `$D648`**, reached by both individual Flee and whole-army Withdraw.

| Branch | Original effects and anchors |
|---|---|
| Escape check succeeds | Construct destination as `$F424 + 25×(provinceNumber−1)` at `$D664`. |
| Successful escape from an active battle unit | Call hex `$7D70` at `$D679` to unlink the officer from its battle-slot list; call hex `$9395` at `$D67E` to remove its occupied hex and position. |
| Destination is not under attack (`province+F == FF`) | Unsigned divide officer troop word `+C` by 100 at `$D69A–$D6A2`. |
| Destination remains under attack | Preserve the battle-scaled troop word. |
| Successful destination insertion | Call **root `$5154(destination, officer)`** at `$D6A5`, ROM file `$3D3B1`. |
| Escape check fails during battle | `$D6AC–$D6C8` selects the opposing primary commander and invokes hex `$7E2D`, leading to unlink/capture-list insertion and removal from the hex. The province insertion call is bypassed. |
| Failed escape on the non-active-battle path | `$D6CF–$D6E1` selects the opponent and calls `$D62E`, which clears the officer's next pointer and calls hex `$92F7` to add it to the corresponding prisoner list. |

Root `$5154` is the common province officer-list writer. An ordinary officer is appended (or becomes the first officer of an empty list); the ruler officer is prepended. It clears/sets the appropriate next pointers. For a peaceful, previously empty province it invokes `$507C` to establish the province's owner and owned-province link. When applicable it promotes the ruler's province to the head of the ruler's owned-province list. This is the persistent list update that the prior matrix lacked.

Hex `$7D70` is mislabeled `setup_duel` in the old symbols. Its inspected instructions actually find the officer in the battle slot's linked list, update that list's head/previous link, maintain current/next action pointers, and set the appropriate terminal battle status when a primary commander/list empties. Never infer duel behavior from that name.

### Escape is a separate branch from destination choice

The check at hsovl2 `$D582` runs before relocation. Officer flag `+2 & $40` gives immediate success. Otherwise it refreshes the movement value and combines movement, War, training, battle troops divided by 100, a neighboring-unit count, ruler/special-officer bonuses, and random terms; the final random draw is compared against a score capped at 100. The source therefore permits a valid selected destination followed by capture. This explains a possible mechanism for the historical failed withdrawal, but **does not identify that historical run's cause**.

### Focused acceptance cases

Record the selected province number and pointer, officer identity, source battle-list link, destination province-list link, position, troops and action/result flags. On success the officer must occur exactly once in the destination and no longer in the source battle list/grid. A peaceful destination receives `floor(battleTroops/100)`; an attacked destination retains the battle-scale value. Test cancellation and no eligible destination as no-move controls, then an explicitly captured failed-escape branch. For whole-army Withdraw, preserve the per-officer choices and results instead of assuming a single shared destination or universal success. These original-runtime captures remain the parent's work.

## 4. G09 / Q12 — strategic values above 655

The known unchecked 16-bit multiplication by 100 is preserved. `655×100 = 65500`; `656×100 mod 65536 = 64`. Those arithmetic examples do not establish that 656 is reachable through ordinary strategic play.

This investigation narrowed the actual reachable-input problem:

| Inspected path | Source evidence | Bounded result |
|---|---|---|
| Player soldier assignment | csovl2 `$D39F`, with “Remaining soldiers” / “Assign to whom?”, calls comroot `$9BA1`. Both direct-fill and numeric entry use a maximum of 100, written at `$9C80`. | Assigned troop word is at most 100. |
| AI redistribution | excsub4 `$C3AA` pools/clears officer troops; `$C3F0` allocates `min(100, calculatedShare)` to each officer. | Each redistributed officer receives at most 100. |
| Ordinary scenario initialization | settei `$93D6`, especially `$94F8–$9527`, assigns 100, 50 or 10. New-ruler setup `$7370` assigns 100 and 10. | Base initialization at most 100. |
| Special scenario multiplication | settei `$7AF9` reads 14 four-byte rows at resource offset `$1FCE`; `$7AB6` multiplies the matched officer's troops and arms. | Actual multipliers are 2, 3 and 5, with no repeated officer ID within either scenario subset; conservative initialization bound is 500. |
| Inspected return from battle | For example, hsovl2 `$D69A` stores unsigned 16-bit troop word divided by 100. | This writer cannot output more than 655, regardless of its incoming word. |

The resource is **`sdata`**, as established by the resource string at root `$6C4B`, not the old `load_bdata_bgmap` label. Its file offset is `$FC32`; its size is 57,658 bytes and SHA-256 is `250a0902225cc316e694c13a71062cb67bf02c1905163b824d65f80abd53c9e9`. The 56-byte multiplier table is at ROM file `$11C00`. Scenario index 0 uses rows 0–9; index 1 uses rows 10–13; later scenarios skip this multiplier routine. The only inspected call at settei `$7E21` immediately follows complete scenario-record initialization, preventing repeated scenario selection from compounding the old troop values. The row values and table hash are preserved in `key_static_evidence.json`.

**Still open:** a complete audit of every legitimate writer/transition that can place an officer into a strategic province list, including reinforcements, under-attack destinations, battle finalization, recruitment/defection and restored battle state. The observed normal cap and initialization bound are strong bounds for those paths, but do not alone prove a universal campaign invariant. No runtime sequence above 655 was obtained in this worker's static investigation.

Add G09 to the plan and keep it PARTIAL. The decisive closure is either (a) a source-backed writer inventory proving every reachable strategic input is at most 655, with scaled/unscaled list transitions accounted for, or (b) a replayable legitimate sequence producing a value above 655 immediately before the original ×100 instruction. A fabricated 656 fixture can test wrap behavior but cannot prove strategic reachability.

## 5. Q03 supporting fact: defender resident-list pointer

Excsub2 `$C544–$C551` obtains the owner of the contested province into `$FC2B` and writes the contested province pointer into `$FC2D`. It separately writes the attacking ruler and source province to `$FC3D/$FC3F`. The same routine copies defending province gold/rice into the defender pools.

Hsovl3 `$D610–$D618` reads the province pointer at battle slot `+2`, then its officer-list head at province `+2`. It detaches selected defenders into the slot's fielded-officer list at `+4`; residents can remain in the province list. Therefore `word[word[$FC2D]+2]` is the relevant resident list. It is **not** `word[$FC2D+2]`, which reads another field of the battle-slot block. Root is independently tracing the food accumulator and exact 60-soldier membership; this static type result does not itself identify those historical soldiers.

## Artifacts and commands

Authored files in this folder:

- `run_source_probe.py`: guarded generator runner with pinned image check and execution receipt.
- `verify_listing_bytes.py`: independent archive/listing comparison and per-module mutation controls.
- `all_listing_byte_verification.json`: all 29 module hashes, locations and verification results.
- `collect_static_evidence.py`: bounded reviewed anchors and scenario multiplier inspection.
- `key_static_evidence.json`: 23 verified anchors and 14 exact table rows.
- `STATIC_FINDINGS_v1.00.md`: this report.

The full generation receipts are in the sibling `regenerated` folder. Reproduction uses explicit paths; the image is never embedded in a script:

```bash
python verify_listing_bytes.py --rom /authorized/game.sfc --listings /private/regenerated/src --out /private/listing-verification.json
python collect_static_evidence.py --rom /authorized/game.sfc --out /private/key-static-evidence.json
```

Both inspection commands were executed successfully against the identified image. No full-game completion, port parity, Windows launch, controller acceptance, or historical runtime reproduction is claimed here.
