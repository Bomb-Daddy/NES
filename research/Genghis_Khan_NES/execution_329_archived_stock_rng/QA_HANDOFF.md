# GK1 EXEC0329 — Implementer & Hostile QA Handoff (2026-11-16)

**Predecessor:** EXEC0328 verified Windows CI240, native WinMain, simulated
controller, NSIS, 240/240 PASS. EXEC0329 is NEW Linux-tested C11 source,
not a new Windows release. Preserve every earlier source and checkpoint.

**Implementation:** `gk329_archived_seed_replay.c/h` authenticates PRIVATE
original-Mesen archived snapshot identity+bank+PC/cycles, takes the internal
NES RAM $84..$89 seed exactly from each original checkpoint, calls the
EXEC0327 fixed-bank C289 original NES ROM source CPU interpreter, and verifies
the next draw against independent EXEC0137 source arithmetic.
`gk329_app_bridge.c/h` adds a read-only entry that checks actual inherited
Gk1App source-verified session and unchanged raw 14/27-country projection,
refusing wrong profiles rather than granting a season or any orders. One
fresh original reset comparison is a **negative control only** because this
component is explicitly restricted to archived post-Spring SaveRAM histories.

**Private runtime acceptance:** exact original ROM and two original restored
named-field state blocks, independent archives from Mongol Spring 1175 and
World Spring 1206, execute 32/32 positive and hostile assertions under
ASan+UBSan. Two source-authentic next draws were observed *after* the
restored checkpoints (Mongol 12551; World 7846). Neither is a claimed
initial-Spring RNG draw or formula. Zero raw data or private replay logs were
published. Original fresh reset provenance is segregated, not conflated.

**Public hostile acceptance:** 13/13 Python stage rollback/tamper/partial
integration checks PASS, 2/2 new native CTests PASS linked into actual Gk1App.
Full inherited Linux application CMake target built, main full CTest suite
not declared PASS because an unrelated installed/X11 test did not complete.
No new Mesen session, new Win32 CI, or physical Xbox run this execution.

**Implementation follow-up** must trace *causal* original CPU initialization
and seed writes before source Spring processing; then implement source-authentic
initial ruler/difficulty/characters, all 14/27 countries, season event chain,
and three real controller orders. Do not treat post-checkpoint state as RNG
origin. Then market presence, AI, diplomacy, battle aftermath, dynasty,
production graphics/audio, save/quit/cold-load, full campaigns and Xbox.
No user save files or original copyrighted data were changed.

**Publication:** see GitHub execution_329 directory and exact source ZIP in
canonical Collaboration Drive, separately verify public Git blob SHA and
Drive ZIP SHA256 before marking VERIFIED_STATIC PASS. Publication does not
imply recipient read/execute acknowledgement.
