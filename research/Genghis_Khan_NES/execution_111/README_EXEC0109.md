# EXEC0109 — complete documented NMOS 6502 memory RMW source family

Continues the full native game from EXEC0108 without replacing any scenario,
original evidence, Classic/Enhanced profile, controller or save interface.
Implements all 24 documented 2A03 memory-RMW opcodes (ASL/LSR/ROL/ROR/
INC/DEC, zero-page, zero-page X, absolute, absolute X) in the original-ROM
bounded source interpreter, preserving byte/page wrapping and the inherited
EXEC0108 old/new consecutive-cycle CPU bus hook.

This is **VERIFIED_STATIC / TESTED_PORT_LINUX** CPU source behavior only, NOT
an authentic stock-Mesen runtime event, first Spring, generated RNG, complete
trade, AI, battle ending or campaign. Any uncertain original hardware state
remains blocked. The original ROM is a private optional test input only and
must never be included in public game source or installer.

The source tests exhaust all 256 opcodes, 24 accepted modes, 3072 RMW flag
cases and wrong-input refusal, plus inherited integration tests. Native Win32,
NSIS and physical Xbox remain NOT RUN unless independently tested.
