# G09 / Q12 — pending allied return candidate

**Author:** Zhuge Liang research subagent, 2026-11-05.  
**Status:** PARTIAL; a source-verified scale transition requires a legitimate-play reachability replay.  
**Evidence:** VERIFIED_STATIC for the instructions and original map counts below; INFERRED for the proposed campaign sequence. No original emulator or full game execution was performed by this subagent. This is a research finding, not an authorized gameplay change.

## Finding

The inspected source does **not** support closing Q12 with a global strategic troop cap of 100, 500, or 655. Most positive troop writers are bounded, and the normal battle-return writers divide the troop word by 100. A separate resumed-battle branch, hsovl3 `$D900`, can instead splice pending allied reinforcements into a province's resident-officer list while preserving their existing troop words.

The pending list is initially filled by comroot `$9A45`, which multiplies each selected officer's troop word by 100. If an officer is still pending when `$D900` takes the no-alliance branch and returns that list to a peaceful source province, a troop word such as 10,000 would remain 10,000 in that strategic list. That is a conditional source result. The ordinary-play sequence reaching this conjunction remains to be captured.

## Exact source chain

| Stage | Source | Reviewed behavior |
|---|---|---|
| Send an allied force | comroot `$9A45–$9B39` | Record the allied ruler and source province in slot 1 or 3. For selected officers, mark action used, multiply troop word `officer+C` by 100, detach from the source province, and append to `FC5D` or `FC5F`. Set an arrival day from two record bytes, specified below. |
| Find deployment cells | hex native `$BEB9–$BF54`, with province-index helper `$BF55` | Scan all 13 × 12 cells. A cell qualifies when `FE90[index]` equals the source province number and `FDF4[index] & $2F == 0`. The return is the number of qualifying cells. |
| Leave unplaced officers pending | hsovl6 `$CDF6–$CE44`, `$CF94–$D015` | The deployment loop sets a candidate's position to `FF`, calls `$BEB9`, and stops if no cell is available. The commit loop advances the pending head only for actually placed officers, stopping at a position `FF`. It does not discard or normalize the remaining pending list. |
| Preserve pending state across continuation | hsovl1 `$D2A4`, `$D14D`; root `$2066`, `$23B1` | The 30-day continuation path sets `FC2A=FF`, sets auxiliary arrival days to 1, and serializes the battle blocks including `FC5D/FC5F` and `FC95`. It does not perform the final-result pending-force return. Root skips the terminal result overlay for `FC2A=FF`. The next main-loop iteration performs the month update and strategic commands before `world_turn_loop` resumes existing battles. |
| Resume an existing battle | root `$4820–$48BF`; hsovl3 `$DA43–$DAC6` | The world loop resumes provinces whose battle marker is present and continuation bit is set. The resumed setup loads the saved battle block through `$D53A` and calls `$D900`; it skips the new-battle defender scaling routine `$D693`. |
| Decide whether allies remain valid | hsovl3 `$D900–$D975` | For auxiliary slots 1 and 3, skip if the relevant `FC95` bit is already set (2 or 8), or there is no auxiliary ruler. Test the auxiliary ruler's alliance with the primary ruler through root `$4072`. A failed test sets another `FC95` bit (4 or 16), then reads the corresponding pending head. |
| Return pending nodes | hsovl3 `$D978–$D9DC` | If the source province has a resident head with the same liege as the pending officer, append the pending list at that list's tail. If the source is empty, install the pending head and update province ownership. Other cases try an adjacent destination, the ruler's owned-province head, or the contested province's free list. Clear the pending pointer. There is **no troop-word read, multiplication, division, or zeroing in this return routine**. |

The simplest test case should use a still-owned, peaceful source province with an existing same-liege resident. This takes the direct append branch `$D981–$D9BA` and avoids the fallback list and ownership cases.

The type of the alliance test is established independently: root `$4072` reads a ruler's alliance bitset at `ruler+A`, indexed by the other ruler's index in the 16-record ruler table at `$F204`. The `$F204` records are rulers, not the 21-byte officer records at `$E04C`.

## Original map constraint

The resource archive identifies `hpoint` at ROM file `$31D12`, length 16,496, SHA-256 `a3df8f0c3f5b538c8b7958d756382edbc3e9b3580dbb4bdfaa8cb71000462ccb`. The entry-region map for province index `p` is the 78-byte nibble map at `hpoint+4+78*p`. Hsovl3 `$D362` reads and expands it; `$D31E` maps codes 1–6 through the province's six adjacency entries and code 7 to the contested province itself.

All 41 decoded maps have **five cells per valid adjacent-province entry region**, and 20 cells in the home region. Therefore a pending entry region can have no available cell when those five cells are occupied or otherwise excluded by mask `$2F`. This is a bounded, source-backed obstruction condition; it is not proof that a chosen scenario can put five units there before that particular auxiliary arrival day.

The source arrival day is `6-floor((byte[chosenOfficer+6]+byte[currentRuler+6])/50)`. The chosen officer is `FD8A` for auxiliary slot 1 and `F85C` for slot 3; the current ruler pointer is `F85A`. Officer `+6` is charm, while ruler `+6` is a different record field (the project's Trust candidate), so these must not be described as two officers' charisma values. Under normal 0–100 values the arrival day is 2–6. A practical reproduction must establish the blocker positions early enough, or demonstrate that some auxiliary officers remain pending after partial placement.

## Proposed decisive replay

1. Establish a normal campaign with three rulers: a primary attacker, an allied auxiliary sender, and a defender. Keep an ordinary officer in the auxiliary source province so the return can use the simple same-liege resident branch. Record the scenario, rulers, officer IDs, province ownership, alliances, troop words, and save hash.
2. Obtain an actual auxiliary commitment through the original command path. Record the selected officers before and after comroot `$9A45`, their strategic-to-battle multiplication, pending list membership, source province, and arrival day. Do not set their troop words to an out-of-range synthetic value.
3. Through legal battle actions, make the source province's entry tiles unavailable before deployment, retain at least one pending auxiliary officer, and continue the battle through its 30-day interval. Record all five region cells, occupancy bits, the pending list, and the serialized state. Keep enough food and living primary forces for genuine continuation.
4. Before the continued battle resumes, break the relevant alliance through a legal strategic action from another peaceful province, or capture another source-backed legal alliance change. The original attack consequence path excsub2 `$C6D5–$C6F6` calls comroot `$8522` to clear the mutual alliance when an ally is attacked. AI comroot `$A221` can also call `$8522`; avoid depending on a probabilistic AI event if a controlled player action is available.
5. Resume the original battle. Capture hsovl3 `$D900`'s inputs and selected branch. Verify the officer appears exactly once in the peaceful source province list, disappears from the pending list, and retains a troop word above 655. Then use an ordinary subsequent attack or reinforcement command and capture the exact input and wrapped output of its original ×100 instruction.

**Acceptance:** A replayable unmodified-game sequence proves legitimate reachability if it reaches the strategic input above 655 and then the original multiplication. If the program prevents a required step, record the actual guard and revise the candidate. A constructed RAM fixture can isolate `$D900`'s behavior but cannot satisfy the legitimate-play reachability acceptance criterion.

**Negative controls:** Keep the alliance intact; leave an entry tile available so the officer deploys; complete the battle normally instead of resuming it; and compare the ordinary return path that divides by 100. Preserve exact list membership and troop words for each case. No new cap or automatic normalization is authorized merely because this candidate exists.

## Provenance and remaining limits

All cited modules were regenerated from the authorized 1 MiB SNES image with SHA-256 `815bfcf4fd6eb23a20c2e50dde023c210b273ffb6cd86a93909d803c3643ce46`, using the pinned generator and symbols recorded in the generation receipts. `all_listing_byte_verification.json` independently compares all 177,982 listed bytes across 29 code modules against that image, with zero mismatches and one detected mutation negative control per module.

Full source listings and the ROM are private working inputs. The authored writer inventory and bounded map metadata belong in the research evidence package. Static coverage of all listing bytes establishes identity, not semantic completeness of every native routine. This candidate must remain separate from the source-closed Q09 and Q14 questions.
