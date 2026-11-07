# Enhanced Run033 — Q14 visible prisoner/fate UI — TESTED_PORT/PARTIAL

**Direct immutable parent:** Run032 SHA256 `014ae2dcdc189395ee17f6d52d4568d526952180a7d4a4311583033741bd0e95`; 550 ZIP members, independently Drive-recovered 18/18 parts, SHA/CRC verified. The separate canonical NES Classic SUP-0016 and DOS trees were not touched.

**New source:** SHA256 `76b9247f9d1f0e21d143352134e8eaf93fe21f3daf529b757460017f1684a95e`; 148,777,481 bytes; 551 CRC-clean members. Exactly 549 of 550 parent members byte-identical; only `third_branch/tactical_view.py` modified and one six-case test added. Two independently built candidates produced identical full ZIP bytes.

**Bug:** Run032's Q14 fate menu was transactionally bound to a named captured officer and decision, but its renderer overwrote the selected officer/fate status on every redraw with a generic 'PRISONER DECISIONS REQUIRED' message and did not draw the actual choice in the in-game battle canvas. Thus the player could press A without seeing whether Release or Behead was chosen.

**Correction:** During active post-battle prisoner modal, retain the exact selected officer/fate prompt and display the officer plus frozen eligible choices, with the currently selected fate visibly highlighted in the existing 1280×720 one-screen canvas. It uses the exact frozen identities already committed by Run029, does not add a battle mechanic, consume RNG, alter strategic economy/BP, or change invalid-cursor handling. **Exact NES UI pixel/text parity NOT VERIFIED**; the choice/actor visibility is verified port behavior.

**Tests executed:** exact pristine Run032 RED 4 fail / 2 pass; Run033 focused 6 pass; intentional hide-modal mutant 4 fail / 2 pass; broader real Tk/army/Q09/Q14/economy/BP Linux Xvfb working 70 pass; completely cold extracted Run033 ZIP selected 190 pass; true GUI/controller from View>Prisoner Fate through right/SELECT/A/B, 1280x720 actual Tk screenshot; real `.ro2` disk save and separate Python process cold-load fingerprint pass. Original 30 NES goldens, native Windows, physical Xbox, unrestricted full suite and Director acceptance NOT RUN/OPEN.

**Run script:** `PYTHONPATH=.:tests xvfb-run -a python -m pytest -q tests/test_run033_q14_prisoner_visible_choices.py` (requires Python/Tk/Pillow/pytest). Builder `build_run033.py` refuses any parent/source/test hash mismatch.

**Next executable:** first publish/read back all 18 source parts and QA from correct original-game Drive folder; then authenticate Q09 attacked-recipient automatic second tactical battle order from original source/emulator fixture, implement directly on exact Run033 source, compare red/green/mutants/controlled RNG, original event traces if obtainable, controller state, troop conservation and two-process cold save. Do not make up unknown original scheduling. Maintain Classic SUP-0016 independent.
