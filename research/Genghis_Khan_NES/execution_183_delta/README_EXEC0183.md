# GK1 EXEC0183 — checkpoint lineage and private save isolation

Same inherited 1280x720 C11 game from EXEC0182, NOT a replacement prototype.
Fixes a concrete source-switch defect discovered in `gk162_commit_verified_checkpoint`:
the earlier commit cleared Enhanced stages through Year2 but did not clear
`enhanced_continuation`, the approved 20-season derived chain added in EXEC0176.
It also retained a one-shot port storage request from the preceding checkpoint.
A completed authenticated original stock checkpoint re-entry now invalidates
all six Enhanced derived stages plus pending private save/load UI state.

`gk167_save_private_new` also now blocks direct callers with a live Enhanced
continuation. The existing controller storage path already had this guard;
this closes the direct API discrepancy and prevents silently persisting a
Classic-only checkpoint while dropping active Enhanced history. Tests include
all six active stage isolation, invalid checkpoint rollback, a synthetic stale
receipt, original country/ROM witness immutability, and optional owner-provided
positive authentic Mesen fixture. Do not publish that fixture.

No original Spring is independently generated; Classic still supports VIEW/PASS
only and no original Trade/RNG/AI/diplomacy/war-end is unlocked. This is
VERIFY_STATIC and TESTED_PORT_LINUX only when local tests are run; new original
NES/Mesen execution, native Windows/NSIS/physical Xbox and full campaigns NOT RUN.
