# GK1 EXEC0045 — live NES join, two-step movement and Divide entry

**Date:** 2026-11-06. **Status:** original modified-NES native controller behavior captured; standalone C11 implementation contract built and tested on Linux. NOT a complete battle engine or a Windows player release.

[Complete copyright-clean source ZIP](GK1_EXEC_0045_Movement_Join_Path_Divide_Source_v1.00.zip) includes gk1_observed_turn.c/h, 45-assertion C11 tests, CMake, numeric observation vectors and native evidence auditor. Only exact recorded cases are accepted; unknown inputs fail closed. SHA-256 source ZIP: c3726cd163d07130022cfd85c2fcbaeffce6816d9992fb8394d27cb65771aa79 (8447 bytes).

**New native findings:** in a controlled friendly-occupied (8,7) tile, mover 40→34→74 while partner 40→0 and completion word0→2 (source CPU writes recorded). A two-step route went (7,7)40/Mob10 → (8,7)34/Mob6 → (9,7)34/Mob3 (second destination raw terrain2, explicitly controlled), then B completed/refilled Mob10 and word1. Original raw terrain7 rejected the attempted second step; one B dismissed error and the next finished the prior committed step. Divide menu at Mobility10–12 refused;13–16 and20 reached the numerical quantity prompt; actual troop splitting not yet executed.

**Linux QA:** Debug/Release/ASan+UBSan one CTest each PASS, 45 C assertions; clean extracted source Release 1/1 PASS. Original gameplay evidence auditor 32 checks PASS plus altered 74→75 negative rejected. No Windows executable, physical Xbox controller or campaign acceptance run.

**Director/implementer handoff:** https://drive.google.com/file/d/1NWj4BhAneMmvErcss1FNGlYQSTqw7HDD/view  
**Private raw Mesen proof archive** (ROM/save/screenshots must remain private): https://drive.google.com/file/d/1q0SEnEObpK4veyACALFtEkuYwFQ2lZ5I/view  
**Cumulative master guide v2.27:** https://docs.google.com/document/d/18iOc7jneTl-mlqLpmn-SnxfLL8qAHNa2VNHpwRgiubU/edit

Nobunaga: incorporate source-bounded acceptance vectors into existing C11 desktop Genghis Windows program, then complete original mechanics and real gameplay. Cao Cao: hostile QA. Zhuge Liang: investigate actual Divide quantity/split and remaining combat/AI/terminal outcomes without rerunning settled mechanics.
