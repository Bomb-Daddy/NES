# GK1 EXEC0293 — Correct original NES cartridge PRG-RAM address, eliminate a false WorkRAM green

**Date:** 2026-11-14 (America/Detroit). **Role:** Source implementation and hostile QA, NOT game release.
**Predecessor:** verified Collaboration and GitHub EXEC0292; legacy EXEC0076 remains an original-ROM authority, not the current complete native source. The existing native C11 game remains unchanged by this correction.

## Confirmed error and correction

The prior EXEC0292 recorder used `emu.convertAddress(0x0dcd, emu.memType.nesMemory)` to decide whether original country records were in `nesWorkRam`. That is **wrong**: the address 0x0DCD is a physical offset of NES **cartridge PRG RAM**, accessed on CPU bus as `$6DCD`. CPU `$0DCD` is a mirrored address of internal NES system RAM, not the cartridge's country data. A prior synthetic Lua mock returned a fabricated `nesWorkRam` mapping for the incorrect address, masking the defect.

The new `GK1_EXEC0293_Private_Mesen_PowerOn_Recorder_V3.lua` queries CPU `$6DCD`, checks the mapped physical offset `$0DCD`, and explicitly refuses a mock/emulator that maps CPU `$0DCD` to the same cartridge bank. V3 logs both mapped CPU and physical addresses; the V3 Python verifier requires them and refuses the older V2 schema. This change is an incremental fix to the existing V2 event/cycle/source-seed recorder, not a separate source model.

## Authenticated original historical evidence (not a newly run emulator)

Recovered original 262160-byte ROM SHA256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3` checked directly. The private 34-member EXEC0076 historical ZIP SHA256 `8b757e11b12aeb3efe93e36c10628599ca5f12e91b15c02354cd3ebbd11bf52a` checked directly. Fresh original stock stage 09 had year1174, Gold425, Food258, Troops60, Rate0, orders0; stage 14 had year1175, Gold430, Food208, Troops61, Rate121, orders3, 14 real 83-byte records. At both stages, CPU `nesMemory` agreed with physical `nesWorkRam` for seven checked fields and `nesSaveRam` held zeros. This validates the historical address-bank distinction without implying uniform starting Gold/Rate across RNG histories.

## Actual executable QA

- V3 Python validator: 4/4 tests PASS; 43 deliberately invalid transcript variants rejected.
- Lua 5.4 **mocked Mesen API** execution: 4/4 tests PASS; old erroneous CPU `$0DCD` green mocked mapping fails closed and its incomplete transcript is rejected.
- Historical privately authenticated evidence validator: 2/2 tests PASS; 28 altered CPU/WorkRAM/SaveRAM field comparisons rejected. Original stock ROM and private archive hashes match.
- Unmodified complete inherited EXEC0288 C11 build configured; `test_gk288_original_rng_stream` compiled and native targeted Linux CTest `GK1_288_...` PASS 1/1. This does not constitute a new full Linux suite.
- No new authentic Mesen V3 execution; no source-authentic full first Spring; no Win32/XInput physical campaign test, no NSIS rebuild.

## Next executable priority

On the original authenticated Mesen executable/core, run V3 with the true original stock ROM and fresh controller setup; retain all logs, screenshots and original records privately. Audit CPU `$6DCD` mapping against the earlier 14-country fresh stock record; trace all six RNG seed writers, source PC/bank, NMI timing and C289 calls, compare **fresh and archived sources separately**. Only then connect deterministic initial seed, ruler/difficulty/character and seasonal updates into the latest inherited native C11 app to produce 14/27 source-authentic countries and three actual controller orders. No screenshot constants or checkpoint substitutions.

All stock ROM, licensed assets, Mesen states, raw RAM and private trace **stay in private Genghis research Drive** folder `1mJ72ezJb9kzIGUHHXPTh3lXtiVxWjEL7`. Public GitHub and Collaboration source archive contain copyright-clean code, checksums and textual QA only. Publishing does not prove implementer acknowledgment. Preserve the hourly development task until Director stop or verified accepted release.
