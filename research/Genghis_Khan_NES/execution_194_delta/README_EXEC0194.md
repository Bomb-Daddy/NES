# EXEC0194 — Controller OTHER → Classic GC19 after real source-bound orders

This modifies the **existing** 1280×720 X11/Win32-XInput C11 native port,
starting from complete EXEC0193 without deleting any original work.

The existing Classic ASSIGN screen can spend 1, 2 or 3 manual-derived labor
orders on an authenticated original Spring country snapshot. Previously, after
any of those actions the 14-command menu blocked OTHER entirely, and the
platform-specific `replay_key()` refused a save/load request originating from
the OTHER storage screen. Therefore a physical/controller-only player could
not perform the already-implemented GC19 cold-save there.

`gk194_classic_storage_allowed()` now accepts pristine original Spring and
**replay-attested** Classic action state, but rejects Enhanced, source tampering,
forged Classic actions, and stale/changing bank data. OTHER requires a second A
confirmation, unchanged from EXEC0193, and the native shell consumes the
one-shot before disk I/O. Save uses the existing exclusive-creation private
GC19 implementation with SHA and fail-closed cold-load; no licensed ROM or
original 83-byte country records are shipped.

This does NOT implement NES first-Spring setup/RNG, merchant scheduling,
authentic original command formula or the remaining original campaigns.
The original ASSIGN arithmetic remains manual-derived and its Y undo is a
port-only convenience. Windows executable, NSIS and Xbox hardware not tested
on Linux.
