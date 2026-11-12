# GK1 EXEC0208 — Native C11 archived-stock RNG provenance and hostile QA

**Status** UNCERTIFIED candidate; inherits exact complete EXEC0207 native C11 source; no release/version promotion (still 1.99.0-dev1). 2026-11-11 America/Detroit.

## Completed original-source change
Existing game `gk163_mesen_import` now offers a backwards-compatible proof-returning extractor; new `gk208_archived_rng.c/h`, wired to the actual native shell, verifies original USA NES ROM SHA-256 and the **complete exact 14-country archived Mongol Spring1175 and 27-country archived World Spring1206 Mesen .mss source fingerprints** *before* exposing their private CPU 48-bit RNG state. This closes an evidentiary gap in which a country array could be genuine while CPU seed bytes were forged. CPU PC and cycles are preserved in the private QA receipt, without publishing licensed raw data.

A cross-implementation C11/6502 source arithmetic check computes **one hypothetical subsequent RNG state and draw** for each separately authenticated archived snapshot. Both sources match. This does **not** reconstruct power-on initialization or prove this random draw actually occurred. It does not convert archived screenshot Gold/Rate to constants. Existing stock and Enhanced behaviors remain distinct.

## Exact tests completed this execution
- Linux Release CTest **171/171** PASS, Linux Debug **171/171** PASS, independent clean replay CTest **171/171** PASS.
- New native C test **41 assertions PASS with two authenticated private archived Mesen checkpoints**, independent clean replay PASS, ASan+UBSan same 41 PASS. Hostile negatives: NULL/malformed args, nonexistent ROM, wrong archived profile, untrusted altered screenshot with genuine country records, and atomic rejection preserving output.
- Existing 1280x720 120-frame Xvfb native shell PASS; Debian-extracted launcher (using explicit `GK1_RESEARCH_BINARY_OVERRIDE` to point at extracted `/opt` binary, as documented in package wrapper) **120-frame graphical and original archive audit PASS**.
- Source manifest **719/719** exact SHA-256 identity after independent patch application from original complete EXEC0207 source; patch changed/added **8 files**; package CRC verified. Original stock ROM locally authenticated as **262160-byte SHA-256 ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3**.

## Observations and limitations
**VERIFIED_STATIC** original ROM and two complete original savestate SHA identities, C11/6502 bounded-source RNG parity, source replay, tests. **OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED** previous independent Mesen captures have now been extracted; Mongol1175 bank=`nesSaveRam` 14 countries, World1206 bank=`nesSaveRam` 27, both at PC=$EFC9 and two different archived CPU seed states. **OBSERVED_RUNTIME_STOCK_FRESH_RESET** prior WorkRAM 1175 capture retained, no new fresh run here. **SOURCE_BOUNDED_PORT_CPU** predecessor boot 70/142-instruction limits unchanged, not original Mesen. **TESTED_PORT_LINUX** above. **NOT RUN** new Mesen simulation, Win32 executable, NSIS, physical Xbox, full campaigns or true first playable Spring initialization.

**Highest priority remains original power-on/setup/character/difficulty seed and actual draw-sequence trace, then independently generate first playable Spring for all 14/27 countries and real 3 orders.** Merchant scheduling, AI, diplomacy, dynasty, war aftermath, AV and full save/cold-load campaigns incomplete. No original ROM, raw Mesen state, screenshots, original game art/music, or CPU seed bytes in public GitHub/Collaboration artifacts. Archive observations in PRIVATE Genghis research folder. Publication is not agent acknowledgement or Director acceptance. Keep recurring task running.
