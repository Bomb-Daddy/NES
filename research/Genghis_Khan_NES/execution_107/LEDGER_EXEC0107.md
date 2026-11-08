# GK1 EXEC0107 — completion ledger v1.00

- Inherited game: complete EXEC0106 C11 source maintained, version 1.07.0-dev1.
- New executable capability: mirrored write-only PPU open-bus reads with unknown
  and analog-age fail-closed semantics, wired to stock reset interpreter.
- New test: C11 test_gk107_ppu_ports and original ROM deterministic trace.
- Release Linux/Debug: 66/66 each PASS. ASan+UBSan: 2/2 targeted stock-ROM
  tests PASS, reduced suite 60/60 PASS, full 66/66 NOT CERTIFIED.
- Linux 1280x720 X11 graphical: 120/120 frames passed. Installed .deb
  executable reports 1.07.0-dev1 and runs 120 frames.
- Original-stock source-hypothesis read-only PPU-port counters 0 across three
  PPU reset origins; no claimed RNG/first Spring advancement.
- First independently generated Spring, setup/RNG, actual Trade, 12 locked
  strategic commands, autonomous AI, diplomacy, dynasty, battle aftermath,
  two complete campaigns, Windows installer and physical Xbox NOT COMPLETE.
- Source privacy: original copyrighted ROM and raw state in PRIVATE only.
- Next source investigation is actual CPU/PPU timing and setup-to-Spring entry,
  not manual re-research or hardcoded observed screenshots.
