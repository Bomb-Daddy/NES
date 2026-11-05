# G09 — next executable original-runtime experiment

**ID:** G09-PENDING-01 v1.00  
**Owner:** Original-runtime investigator; producer accepts evidence, not an invented troop cap.  
**Status:** NOT RUN. Source branch VERIFIED_STATIC; ordinary-play reachability UNKNOWN.  
**Reference:** Authorized SNES image SHA-256 `815bfcf4fd6eb23a20c2e50dde023c210b273ffb6cd86a93909d803c3643ce46`. Addresses below belong to that reference. An NES capture must use independently established address mappings; the accepted mechanics are shared.

## Question

Can a legitimately sent auxiliary officer remain pending through battle continuation, lose the alliance before resume, and return to a peaceful province with the battle-scale troop word intact—then supply that value to the original strategic ×100 writer?

## Smallest decisive return state

Use **auxiliary slot 1** and its unchanged source province, with an existing ordinary resident of the same liege. This avoids adjacent/fallback and province-ownership branches.

| Input immediately before hsovl3 `$D900` | Required value |
|---|---|
| Resumed battle | Province battle marker present and continuation bit set; setup has loaded the saved state through `$D53A`. |
| Auxiliary ruler | `word[$FC34] = R`, a valid ruler pointer. |
| Auxiliary source province | `word[$FC36] = P`. |
| Pending auxiliary head | `word[$FC5D] = O`, a valid officer pointer. One pending officer is sufficient; `word[O]=0` simplifies the test. |
| Auxiliary skip flag | `byte[$FC95] & $02 == 0`. |
| Alliance | Root `$4072(R, word[$FC2B])` returns 0. Equivalently, the corresponding primary-ruler bit in `word[R+$A]` is clear. |
| Source ownership and scale | `P` still belongs to `R`; `byte[P+$F] == $FF` (peaceful). |
| Source resident list | `word[P+2]=H`, nonzero; `byte[H+7] == byte[O+7]`. Record the current tail `T`. |
| Officer troop word | `word[O+$C]` still equals the value produced by the earlier legitimate send. Example: strategic 100 was multiplied to battle 10,000. Do not manufacture the value to prove legitimate reachability. |

## Capture and expected branch result

Capture before/after memory and instruction events at hsovl3 `$D946` (alliance call), `$D9B3` (resident tail lookup), `$D9BA` (append pending head), and `$D9DC` (clear pending pointer).

For the state above, the source predicts:

- `word[T]` changes from 0 to `O`; `O` occurs exactly once in P's resident list.
- `word[$FC5D]` becomes 0; `byte[$FC95]` gains bit `$04`.
- `word[O+$C]` remains **10,000**, with no division by 100.
- P remains peaceful. The officer has entered a strategic resident list carrying the battle-scale word.

Then issue a legal subsequent send/attack from P using O and capture its original multiplier. Comroot `$9A94–$9A9C` is the allied-send multiply; excsub2 `$C580` contains the primary attack/reinforcement writer. A strategic input of 10,000 predicts `10,000*100 mod 65,536 = 16,960`. Record the exact instruction input/output rather than relying on rounded UI text.

## Missing ordinary-play condition

The source proves the conditional return, but the following conjunction has not been reproduced: **an actual allied commitment leaves at least one officer pending through the 30-day interval, while the source province remains peaceful and owned, then the relevant alliance ends before the battle resumes.** The pending officer must come from the normal send path, not a RAM edit.

Entry obstruction is concrete: native hex `$BEB9` rejects a source entry tile when `FDF4[tile] & $2F !=0`; each valid adjacent entry region has five tiles. Hsovl6 stops when none is available and retains unplaced officers. Hsovl1 `$D2A4/$D14D` serializes pending heads at continuation. A legal attack against an ally clears the alliance through excsub2 `$C6F3` → comroot `$8522`; root's month loop permits strategic commands before continued battles resume. The timing, legal tile positions, surviving armies, food, and ownership must still be demonstrated together in an unmodified-game replay.

## Controls and completion criterion

Keep the alliance intact as the first negative control: `$D94A` must skip the splice and preserve the pending list. Use a normal completed-battle return as the scale control: pending-force cleanup sengo `$7D36` → `$721A` divides by 100 for a peaceful destination. Preserve list membership, troop words, ROM/core/save hashes, exact inputs and raw traces.

A constructed branch fixture may be labeled OBSERVED_RUNTIME for that prepared state, but G09 remains PARTIAL until a legitimate-play replay supplies the preconditions and reaches the later strategic multiplication. If a required step is blocked, record the actual source/runtime guard. No 655 clamp or automatic rescaling is authorized by this experiment.
