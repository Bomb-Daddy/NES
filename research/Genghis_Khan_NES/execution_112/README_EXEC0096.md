# Genghis Khan I EXEC0096 — source-derived NES NMI boundary

Inherited **entire EXEC0095** native C11 Windows/Linux game. This increment does not claim the first Spring is implemented. It adds `gk96_ppu_warmup.c/.h` to the exact-owner-ROM SHA-gated reset path: two bounded counterfactual PPUSTATUS input sequences demonstrate that the real 6502 startup code stalls at a source-derived $C460 waiting state when the interpreter lacks NES NMI. **These are not Mesen observations and do not establish real NTSC vblank timing.** The launcher START source probe reports the missing NMI capability rather than presenting the first PPU read as the final obstacle.

`test_gk96_ppu_warmup` validates fail-closed absent/incorrect ROM and deterministic original-source traces with `GK1_ORIGINAL_STOCK_ROM` set to private authorized owner ROM. Private ROM, RAM/screenshot and checkpoints are excluded from all public releases.

**Next actual blocker**: implement correct hardware NMI and PPU timing in the source interpreter or use exact headless Mesen runtime; only then source-trace original setup/character/difficulty RNG and implement first independently generated spring. Unmodified Classic original rules remain separate from Enhanced. Full campaigns, merchant presence/transaction, Win32 runtime and physical Xbox are NOT tested.
