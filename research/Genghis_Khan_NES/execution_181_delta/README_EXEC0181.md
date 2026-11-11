# Genghis Khan I — EXEC0181 conditional stock source audit

Based on the full inherited EXEC0180 C11 application, without changing the
Classic/Enhanced gameplay contract, 1280x720 UI, source tables, or saves.

New original-stock-authenticated `--source-bank-seed-writers PRIVATE_ROM BANK`
(0..14) enumerates only opcode-aligned direct unindexed writers to zero-page
RNG seed bytes $84..$89 under an **explicit hypothetical** MMC1 low bank.
The same source graph covers fixed bank 15. It refuses unsupported/indirect
control flow and stops on possible bank-register stores. Indexed stores remain
unresolved, not mislabeled direct writes. Original ROM SHA256 is checked
before source-derived output. The Gk1App source research gate remains closed
outside validated pre-setup intent. No input state is modified on failure.

This is VERIFIED_STATIC, not stock Mesen execution: possible bank assignments
cannot establish actual bank at runtime, actual seed initialization order,
difficulty/ruler transforms, 14/27-country first Spring, or three Classic
executable orders. Original Trade/AI/dynasty/battle aftermath and final Windows
installer/physical Xbox campaign work remain OPEN. Never publish original
ROM, save states, screenshots, raw licensed source bytes or memory captures.
