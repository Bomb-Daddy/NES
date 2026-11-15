# EXEC0314 — Windows CI native source integration (addendum)

After EXEC0313 successfully stages and authenticates the inherited complete C11 game, `stage314.py` applies the exact baseline preservation patch and adds native `GK1_314_ReplayPreservesSourceBaseline` CTest. `repair314_provenance.py` verifies the **unaltered** EXEC0313 afterimage by SHA (app, CMake, original ten-mutation source gate) through reverse comparison before advancing the existing exact SHA gate to the new source hash. No old hostile gate is skipped, removed or turned into an unverified acceptance claim.

`python test_stage314.py` (8 hostile tests) and `python test_repair314_provenance.py` (9 hostile tests) both passed on Linux; Windows-specific live CI is pending until its actual workflow completion. Test newly staged C11 with real native Windows compilation + NSIS smoke, not a surrogate Python test. The prior EXEC0313 Win32 CI227 remains independent; no physical Xbox campaign claim.

Current source implementation does not unlock authentic first Spring or create merchants/AI/campaign victory. The only change to native gameplay state is correcting preservation of both 14/27-country immutable source baselines when valid independent battle replay is restored. The original stock NES ROM remains private.
