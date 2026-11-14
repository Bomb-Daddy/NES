# GK1 EXEC0273 — exact EXEC0272 C11 source in Win32 build

Existing complete GK1 C11 game, not a separate prototype. The Win32 CI workflow reconstructs an older pinned source and applies 0264, 0266 and 0270 deltas; it was therefore missing the EXEC0272 original-template verification. This stage adds the **exact original C11 source** and **original linked executable test** from EXEC0272, checking exact full-file SHA-256 before and after. Both files run seven hostile corrupted-source controls.

* Input real application SHA: `ee00ef0fc03676d09edfef9ca630acb9e605f128e5af5a66b1eafb2e86c2dc68`
* Result real application SHA: `e7c7e02ed35760234bf7ddc8738f3a0ac87699fb2f1b46ca40f3071df7207372`
* Input linked GK214 test SHA: `bc92f911454cf3479698c21b88bbd6ea306a052fcac9cff0e9fc4e0617272b1b`
* Result linked GK214 test SHA: `ca52f9386bef4600e93fcecdb55aa59f706660599ebb4787a37089ee382898ad`

Run before any native Windows compilation. Never disable GK214 or inherited Win32 tests, and do not equate a successful source provenance gate to authentic first-Spring setup/RNG or physical Xbox acceptance. User-private original NES ROM and RAM are not included.
