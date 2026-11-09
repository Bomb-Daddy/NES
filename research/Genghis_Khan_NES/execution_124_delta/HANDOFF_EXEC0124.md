# EXEC0124 — implementer / hostile-QA handoff — 2026-11-09

Authority: Preserve cumulative v3.01 and complete EXEC0123 inheritance; current directory `execution_124` is the whole game source, not a standalone CPU micro-project. An earlier EXEC0124+ must be checked on Drive/GitHub before merging this increment.

Code change: gk124_cpu_compare.c/.h, gk83_reset_trace.c, gk100_cpu_cycles.c, CMakeLists.txt, native version/installer assets and test_gk124_cpu_compare.c. All 14 official CMP/CPX/CPY forms use authenticated memory-read gates, including page-cross penalties and zero-page pointer wrap. Test rejects unknown opcodes, false-ROM source attachment, and state mutation on a failed source gate.

Reproduce tests with README_EXEC0124.md commands. Release, Debug, ASan+UBSan (ASAN_OPTIONS=detect_leaks=0), independent clean ZIP build, original-ROM source-gated CTest, real native Xvfb 120-frame smoke and extracted Debian execution must each have separate receipts. Never equate these with original Mesen, Win32 or physical Xbox.

Original stock history: fresh Spring1175 country1 Gold430 Food208 Troops61 Rate121 3 orders; archived Spring1175 Gold439 Food208 Troops61 Rate101 3 orders; archived World Spring1206 Gold193 Food332 Troops116 Rate114 3 orders. These DIFFER because setup/RNG histories differ. Source reset still unknown $00F5, explicitly MesenCE AllZeros progresses to unmodeled PPUSTATUS $2002; no fabricated first-Spring unlock. Private original ROM, SRAM, RAM, screenshots, patched ROM and copyrighted art/audio must remain PRIVATE in Genghis research folder only.

Next priorities remain original stock initialization/RNG/first Spring and real three orders, followed by authentic merchant scheduling, AI, campaign mechanics, Win32/NSIS and physical Xbox acceptance. Preserve all earlier game source, saves, existing Enhanced profile features and research evidence. Publication is NOT execution acknowledgement.
