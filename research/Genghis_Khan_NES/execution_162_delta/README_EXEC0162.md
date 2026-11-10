# GK1 EXEC0162 — Original-stock checkpoint controller entry, fail-closed source replacement

A new `gk162_enter_original_checkpoint` source-authenticated attachment boundary is
connected to the **existing** native controller `A` action in the Spring Archive.
It requires canonical owner-supplied CSV and a SHA-verified 14×83 or 27×83
actual ORIGINAL stock RAM export, preserves both PRE_SETUP template worlds,
opens the correct original-season country menu with 3 **available** orders, and
invalidates all prior Enhanced-derived labor stages **including Year2**.
Source errors and incorrect WorkRAM/SaveRAM profile selections are transactional
and leave the entire app unchanged. The player's explicit Classic vs Enhanced
preference survives attachment; Classic commands VIEW and PASS are controller
accessible, including B-cancel and a source-bound PASS receipt with cold-load.

This implementation is **not** a source-independent NES first-Spring
initialization. It does not implement all three original NES actions or generate
NES original character, difficulty or merchant RNG, run AI, or transition the
original season. No RAM/ROM/checkpoint bytes are in public code or installer.

`test_gk162_original_entry <source-csv> [<fresh-original-14x83> <restored-original-14x83> <restored-world-27x83>]`

`VERIFIED_STATIC`: code, source gates and original hash checks.
`OBSERVED_RUNTIME_STOCK`: inherited 2026-11-07 fresh WorkRAM and archived
SaveRAM observations, no newly executed Mesen original NES.
`TESTED_PORT_LINUX`: actual build, tests, native graphics and Debian QA when
independently executed and recorded.
`NOT RUN`: Windows Win32, NSIS installer, physical Xbox, whole campaigns.
