# G09 / Q12 — strategic troop writer and scale-transition audit

**Author/date:** Cao Cao research worker / Zhuge Liang investigation, 2026-11-05 UTC.  
**Disposition:** PARTIAL. Initialization, recruitment, redistribution, and the ordinary return paths are bounded. A specific pending-allied return branch prevents a universal strategic-value bound from being claimed.  
**Evidence class:** VERIFIED_STATIC for the inspected original bytes and record types; INFERRED for the proposed ordinary-play reachability sequence. No original emulator or Windows port was executed by this worker.

## Outcome

Do not add a 655 clamp. The original troop multiplication remains a 16-bit operation: for example, `656*100 mod 65536 = 64`. Whether ordinary play can supply a strategic input above 655 is a separate reachability question.

The expanded audit found **57 matching write sites in 29 regenerated modules**. Manual record typing identifies **47 officer-troop writes, 8 province-population writes, and 2 ruler-aggregate writes**. The numeric match count is a search-coverage result, not a proof of complete program semantics. The attached JSON enumerates every match with its module, routine entry and private listing location.

Most identified troop writers preserve a useful bound. The exception needing a decisive original-game replay is a **list transition with no troop-word write**: hsovl3 `$D900` can return unplaced pending allies to a province's resident list without division by 100. This is why counting arithmetic writes alone cannot close the question.

## 1. Positive strategic writers

| Writer/path | Reviewed source | Result and limit |
|---|---|---|
| Scenario base values | settei `$93D6`; new-ruler `$7370` | Troops initialized to 100, 50 or 10; new-ruler setup uses 100 and 10. |
| Special scenario values | settei `$7AB6`, called by `$7AF9` | Original 14-row table uses multipliers 2, 3 and 5. No repeated officer ID occurs in either scenario's subset. The caller `$7E21` follows fresh record initialization. Conservative initialization bound: 500. |
| Manual assignment | comroot `$9BA1–$9C85`, called by csovl2 `$D39F` | Both fill and numeric-entry paths cap the assigned value at 100. |
| Single-officer recruiting | csovl2 `$D5BA`, especially `$D617–$D628` | Calls root `$6703` on `officer+C` with maximum 100. Multi-officer outcomes use the assignment flow or `$D584`, which writes 100. |
| AI redistribution | excsub4 `$C3AA`, `$C3F0` | First pools/clears troops, then assigns `min(100, calculated share)` to each officer. |
| AI dismissal/spy preparation helper | excsub5 `$C30D` | Clears the selected officer's troops, then uses `min(100-current, remaining)` additions to that same selected record. Its output cannot exceed 100. Do not silently rewrite the actual target as a different officer merely because that would better fit the old function name. Subsequent free-officer insertion clears troops again. |

The 500 initialization result is conservative, not a claim that a particular scenario officer actually starts at 500. It also does not cap the whole campaign. The original table and its hash were already captured in `key_static_evidence.json`.

## 2. Scale entry and exit

| Transition | Reviewed source | Scale behavior |
|---|---|---|
| Primary attack or strategic reinforcement | excsub2 `$C580` | Selected officer troop word multiplied by 100. |
| Allied auxiliary commitment | comroot `$9A45` | Multiply by 100, remove from source province, place in pending auxiliary list. |
| Special officer seeking allied aid | excsub3 `$C6C3` / `$C794` | Multiply by 100, detach the officer, use `FC93` while the aid route is pending. |
| New battle's fielded and reserve defenders | hsovl3 `$D693`, called from `$D6AD` | Multiply both relevant lists by 100 during new-battle setup. Resumed setup takes `$D53A` / `$D900`, not this scaling branch. |
| Thirty-day continuation | hsovl1 `$D2A4`, `$D14D`; root `$2066` | Preserve pending heads and flags in the serialized battle block. Set `FC2A=FF`, so terminal-result pending-force cleanup is skipped. The root `$23B1` loop runs the next month update and strategic commands before resumed battles. |
| Automatic battle setup | excsub6 `$CB4B` | Multiply defending resident troops by 100. |
| Individual Flee / whole Withdraw | hsovl2 `$D648` | Divide by 100 for a peaceful destination; preserve battle scale for a destination already in battle. Then call root `$5154`. |
| Other inspected retreat writer | hsovl4 `$CE27` | Same conditional peaceful-destination division before root `$5154`. |
| Automatic result return | excsub6 `$C5E2` | Divide when returning to an unowned or peaceful province; retain battle scale when returning to an attacked province. |
| Main battle result lists | sengo `$721A`, `$735C` | Return/list joining normalizes troops with unsigned division by 100 when entering peaceful strategic storage. `$721A` retains scale for an attacked destination. |
| Aid-request officer at result | sengo `$82A4`; comroot `$AF28` | Explicit normalization on the inspected completed/returned paths. `$AF28` is conditional on diplomatic mode 5 and a terminal result code at least 5. |
| Prisoner handling | sengo `$85FE`, `$8749` | Normalize before ordinary strategic prisoner disposition. The same-liege return to another attacked province rescales by 100. |
| Release into an attacked province | sengo `$837D` | The already normalized officer is multiplied by 100 before province-list insertion. |
| Special diplomatic reentry | comroot `$B522` | Its terminal special-target branch multiplies by 100 if the officer is being returned into the attacked destination. This is a scale transition, not an independent strategic recruiting writer. |
| Pending ally returned on resumed battle | hsovl3 `$D900` | **No troop normalization.** The branch can splice the pending list directly into a source/fallback province list. Legitimate-play reachability remains the concrete unresolved case. |

Any writer that actually performs unsigned `u16/100` outputs at most 655, even if the incoming battle word is 65,535. That arithmetic fact does not prove that every battle-to-strategic transition reaches one of those writers.

## 3. Ordinary strategic moves and other list changes

The outer province scheduler comroot `$7487` examines each ruler's owned-province list. At `$752E–$753F`, it skips a province whose battle marker `province+F` is not `FF`. Only after that gate does it dispatch player commands (`$734B`) or the AI overlay (overlay 0 at `$7609–$7611`). Thus the ordinary strategic transfer source is peaceful even though the transfer worker itself does not repeat the check.

The transfer worker comroot `$98F4` moves officers through root `$51E2` / `$5154` without changing their troop word. Player destination selection csovl2 `$CE1C` excludes any attacked province before ownership checks. AI selectors `$D220` and `$D394` exclude attacked same-owner destinations; their unowned branches return before that flag check, so the unowned/peaceful invariant remains an explicit prerequisite. Do not claim that `$D409` or `$98F4` contains a source gate of its own.

Other reviewed insertion paths preserve strategic values or clear them: the free-officer and hidden-spy writer root `$5122` zeroes troops; root `$4EB0` also zeroes before its flagged-spy return; csovl5 `$D6C3` recalls from the free-officer list; newly appearing officers are initialized before kisetsu `$9453` inserts them. The monthly governor-rebellion path skips attacked source provinces at kisetsu `$7C59–$7C6B`, so its followers' moves at `$77E2` are not themselves a battle-scale exit.

This is a reviewed path inventory. It does not claim formal alias analysis of every possible indirect memory write, full native-code coverage, corrupted-save behavior, or the absence of another unusual transition.

## 4. Reduction, zeroing, and false positive matches

The remaining direct officer-troop matches are zeroing and battle/event reductions: root `$4EB0/$5122`; comroot `$8C11`; hex `$7585/$75EC/$7E2D/$B75C/$B7D5`; excsub6 `$C30C`; hsovl5 `$D367`; and kisetsu `$7E73/$81B4/$8C39`. The event percentages are reductions under the normal attribute ranges. The `$81B4` event path also explicitly skips provinces already under attack. These observations do not authorize changing arithmetic overflow or signedness.

The ten non-officer matches are deliberately retained in the JSON rather than discarded without explanation. Province population occupies its own `+C` word, and ruler records have a troop aggregate at `+C`. For example, comroot `$B836` deducts recruited population; csovl3 `$CD67` returns dismissed troops to population; kisetsu `$8F31` grows population with a 30,000 cap. They are not evidence for an officer troop cap of 30,000. Settei `$932C` initializes ruler records, while `$93D6` initializes actual officer records.

Native root `$6967` and hex `$C14B` read officer troop words for aggregate sums. The inspected native `$000C` references in hex `$C1EF/$C69B` instead use direct-page workspace. None of these read/workspace sites is an additional positive officer-troop writer.

## 5. Remaining decisive experiment

`G09_PENDING_ALLY_CANDIDATE_v1.00.md` gives the detailed branch and map data; `G09_NEXT_EXPERIMENT_v1.00.md` is the smallest concrete experiment specification. Its necessary conjunction is:

1. A legitimately sent allied officer is still in `FC5D` or `FC5F` with its already multiplied troop word.
2. Entry placement never consumes that officer before battle continuation. Native hex `$BEB9` excludes occupied/fire cells; the original 41 maps contain five cells in every valid neighboring entry region.
3. The relevant alliance is absent when the continued battle reaches hsovl3 `$D900`, and the appropriate `FC95` skip bit is clear.
4. `$D900` appends the pending officer to a peaceful, same-liege source province. The troop word is unchanged.
5. A subsequent legal send/attack reaches the original ×100 instruction with that strategic value above 655.

The map inspection verified **186 valid neighboring entry regions across 41 maps**, each with five cells; each home region has 20. Source-verified entry obstruction and the return branch make this a precise candidate. They do not by themselves prove that one chosen campaign can satisfy all timing and ownership conditions.

**Close as reachable** only with a legitimate-play replay, or an equivalently complete source proof of that sequence. **Close as unreachable** only after identifying an actual guard that blocks this conjunction and completing the other relevant transitions. A synthetic 656 word proves only multiplication behavior. A synthetic pending-list fixture proves only conditional branch behavior. Neither is evidence of ordinary-play reachability.

## 6. Reproduction and provenance

`audit_troop_writers.py` was executed against the authorized original image and all 29 regenerated modules. It exited 0 with empty stderr. It classified all 57 search matches, independently compared **17 reviewed transition byte ranges** with the ROM, verified **9 short original-byte anchors**, and decoded the entry-region counts. Exact command, Python 3.12.14 environment, stdout/stderr, exit status and hashes are in `g09_audit_attempt02_receipt.json`; the full derived results are in `g09_writer_and_map_audit.json`. The first successful receipt is retained; the second adds four continuation/scheduling ranges without changing the writer classifications.

```bash
python audit_troop_writers.py --rom /authorized/game.sfc --listings /private/regenerated/src --out /private/g09_writer_and_map_audit.json
```

The script imports the sibling `collect_static_evidence.py`. Both require the exact ROM SHA-256 `815bfcf4fd6eb23a20c2e50dde023c210b273ffb6cd86a93909d803c3643ce46`. The earlier all-listing comparison established 177,982 matching bytes over 29 modules and detected all 29 mutation controls. This audit adds bounded semantic review; it does not elevate static metadata to observed game behavior.

No gameplay code was changed, no new cap was applied, and no source-closed battle formula was reopened.
