# GK1 cumulative master v4.42 — EXEC0293 source correction

Preserve v4.41 EXEC0292 and all earlier numbered original NES and native port source. The recovered original stock ROM is AUTHENTICATED and PRIVATE, not absent. EXEC0293 corrects false V2 CPU/PRG-RAM mapping: `$6DCD` maps to cartridge physical `$0DCD`, while CPU `$0DCD` mirrors internal RAM and is not a country-record address. Executable Lua V3 recorder + Python V3 verifier are published with hostile tests and a private historical EXEC0076 source comparison. The V2 transcript cannot establish a positive WorkRAM bank provenance claim; reject legacy V2 for that purpose.

Acceptance: VERIFIED_STATIC PASS; TESTED_HOST_LINUX_PYTHON_LUA_MOCK 8/8 PASS with 43 transcript and 28 private-capture adversarial controls; TESTED_PORT_LINUX targeted inherited GK1_288 C11 1/1 PASS; OBSERVED_RUNTIME_STOCK fresh and archived historical only; NOT RUN new real Mesen Lua, native Win32 for EXEC0293, physical Xbox or either complete campaign. Original native first-Spring generation still OPEN.

Next code: obtain live original source seed-writer timing, implement actual first Spring startup/ability/season RNG into current complete native C11 game, maintain Classic/Enhanced separation, do not hardcode fresh or archived numeric outputs.
