# Q09 — escape check before the destination writer

**Date/evidence:** 2026-11-05; VERIFIED_STATIC from the pinned original SNES image and byte-matched regenerated instructions. No escape frequency or original-runtime result was measured by this worker.

This addendum supports a controlled failed-escape negative test. It does not change the source-closed destination-writer result in `STATIC_FINDINGS_v1.00.md`.

## Source result

Hsovl2 `$D582–$D62D` runs before `$D648` relocates an officer. If officer flag `+2 & $40` is set, it returns success immediately. Otherwise it calls native hex `$BFBB` and counts opposing occupied neighbors through hex `$80D1`.

For ordinary valid movement values, the effective movement byte is raised to at least:

`floor(max(training-1, 0)/20) + 2`

Native `$BFBB` does not unconditionally replace a larger movement value. Its final comparison is literally an 8-bit subtraction followed by `BMI`: replace when the subtraction's N flag is set. The familiar maximum interpretation applies to the ordinary small movement values. Preserve the instruction behavior if a low-level fixture supplies arbitrary bytes.

Let `N` be the number of occupied hexes among the six neighbors whose army is on the opposing side. Native hex `$C00A` uses army-slot bit 1 (`slotIndex & 2`) to distinguish the two sides, so allied slots on the same side do not count as opponents.

After computing `N`, the routine draws `r10 = rand(10)`, then forms:

`score = floor((10*movement + War + training + floor(troops/100))/(N+2)) + rulerBonus + specialBonus + r10`

| Term | Source meaning |
|---|---|
| `movement` | Officer byte `+11`, after native `$BFBB`. |
| `War` | Officer byte `+5`. |
| `training` | Officer byte `+10`. |
| `troops` | Officer word `+C`, currently in battle scale. |
| `rulerBonus` | 20 if root `$4101(officer)` reports the ruler officer, otherwise 0. |
| `specialBonus` | 20 if root `$2569(officer, 1) == $A3`, otherwise 0. The record ID is stated exactly; no character name is inferred here. |

Finally it draws `r100 = rand(100)` and returns success exactly when:

`r100 < min(score, 100)`

The RNG order is `rand(10)` first, then `rand(100)`. Source anchors are hsovl2 `$D5C0–$D5C4` and `$D61D–$D62C`. The arithmetic and branch range is `$D5E9–$D62D`; native movement is hex `$BFBB–$C009`; opposing-neighbor count is hex `$80D1`, with native side comparison `$C00A`.

## Use in the destination experiment

A valid destination and a successful escape are separate conditions. Capture the actual return from `$D582`, selected province number, and before/after list membership. On failed escape the writer bypasses root `$5154` and enters capture handling. On success it performs the destination insertion and the conditional troop division already documented.

The messages “crushed” or “succumbed to a greater power” are insufficient evidence of failed escape. The battle-list unlinker can terminate a primary army after successful departure. The historical run's selected branch must be established from persistent officer location and battle/prisoner lists, not the message alone. Parent-owned runtime evidence may close that case independently.

For a controlled failure case, retain a non-ruler officer without the immediate-success flag, record the six neighboring occupancy entries and both RNG outcomes, and verify `r100 >= min(score,100)`. Do not claim a failed escape merely by reproducing the terminal message. A synthetic fixed-RNG fixture can verify the comparison; only an original-game capture can establish that the same fixture's inputs occurred there.
