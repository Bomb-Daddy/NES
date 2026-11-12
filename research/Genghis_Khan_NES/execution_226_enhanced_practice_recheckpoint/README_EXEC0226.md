# GK1 EXEC0226 — Source-gated Enhanced practice save after cold resume

This is a copyright-clean modification to the existing C11 `execution_199` native game, inherited from EXEC0225. After a real title-menu controller Y reload of the Enhanced research practice, X now validates both the 80-byte existing receipt and the new state and atomically updates the **same** save slot. The earlier original `gk224_save_new` exclusive-create behavior blocked this use case. Classic campaign save semantics and stock research evidence remain unchanged.

## Integration

Extract the complete EXEC0225 source archive and apply `GK1_EXEC0226_CopyrightClean_Integration.patch` with `git apply` at the archive root. The source patch covers exactly seven files. Release/Debug/clean source builds all passed 196/196 tests; focused native ASan/UBSan passed, plus four separate Linux Xvfb graphical processes verified 1→2→3→3 actions via controller software injection and 80-byte SHA checksum. The current .deb contains exact Release binary; Win32, NSIS, physical Xbox and complete campaigns **not run**.

## Privacy and claim boundary

This patch contains no original game ROM/CHR/SRAM, emulator recordings, screenshots, audio or copyrighted source material. Prior original NES Spring observations are historic (fresh WorkRAM and archived-restored SaveRAM separately); **no new original Mesen run**. Enhanced 3-order practice is not original NES first Spring, which remains a top priority with source-authentic RNG and seasonal setup for all 14/27 countries.
