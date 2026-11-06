# EXEC0029 — Native VM phase parity guard

**Status:** ARCHIVED MODIFIED-ROM ZERO-PAGE RECHECK / WINDOWS NOT TESTED (2026-11-06).

This directory adds portable, copyright-clean, source/test code for comparing recorded
tactical movement snapshots at **compatible interpreter contexts**, rather than
mistaking a constant elapsed frame count for command completion.

## New evidence

The previously captured 13 controlled modified NES movements all begin with VM
source pointer `$CB89` in zero page `$06:$07`. All are sampled 1,185 frames
after action setup, but post-snapshot VM source pointers differ:

- `$CC2B` — 6 cases (forest, infantry, archer, three troop-count variants)
- `$CBC0` — 3 cases (plain, castle, terrain 5)
- `$91C9` — 2 cases (terrain 1, terrain 4)
- `$8FF1` — 1 case (terrain 0)
- `$E6C5` — 1 case (forest unit reduced to zero soldiers)

These are interpreted **VM script addresses, not native 6502 CPU PCs**. The
mapped PRG banks and terminal command states remain unproven. A pass from
`same_captured_phase` never proves command completion; it only prevents
comparing known different captured VM contexts.

## Reproduction

```sh
cd research/Genghis_Khan_NES/execution_029
python -m unittest -v test_phase_contract
```

The seven source-only test methods passed in the authoring runtime; native
Mesen/Windows game acceptance was **not** performed on this code. The complete
private-archive auditor is in a separately generated source ZIP, SHA256
`f9825aa858970dc26e1aba254740ad2f99be153d1edd6d99bd7bd35fe02f6126`
(not published as a Drive ZIP because binary egress was blocked).

## Versioned handoff

- [Master guide v2.12](https://docs.google.com/document/d/1yzlAYz9Em5A0UuWp2DR2iCPgZjb9SerzfUs_a_vU_40/edit)
- [Numbered handoff](https://docs.google.com/document/d/1Q-hiy4sNsuZJ07Y7EcbecgXIizPOHPLbWYAlpgIOEDw/edit)
- [G03/G04 incremental ledger](https://docs.google.com/document/d/11tkCT868akNWdj5BO9sdrxM5X0EJE4qieGx0wrbWA0c/edit)

Private ROM, SRAM, savestates and screenshots must not be committed here.
Repair headless Mesen and instrument native `$613E:$613F` writes (CPU
`$F1C8/$F1CD`) with mapped bank/VM source pointer to resolve terrain0,
legal movement, Entire/Divide and zero-unit cleanup.
