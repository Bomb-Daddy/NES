# EXEC0119: Source-authenticated Enhanced shared three-order Pass and durable cold-load

Inherit the exact COMPLETE EXEC0118 C11 source tree, not obsolete 0062/0115.
Original source-controlled 14/27-country country arrays and recovered stock NES ROM stay PRIVATE.

**Implemented:** In prior native EXEC0118, Enhanced Assign had its own three-order budget,
while controller Pass spent the separate original-stock checkpoint budget. A player could
spend up to three labor orders and *then* Pass three further stock orders. In EXEC0119,
controller Pass routes to the sole effective Enhanced budget when an authenticated
Enhanced session exists. Source checkpoint and source 83-byte records never change.
Pass consumes the Enhanced remainder but does NOT advance Spring; unimplemented AI/
season logic still locked. Classic Pass without Enhanced retains exact old behavior. A discovered second defect has also been corrected: X/Y on the stock command, country or quote screens now persist an active Enhanced session through GA16, never accidentally write a GK80-only stock checkpoint that drops its actions and Pass.

`gk116_pass_remaining()` extends the existing exact source-checked action ledger.
The 96-byte GA16 receipt is backward compatible: reserved byte14 now records Pass=1;
old receipts continue using 0, unchanged. Cold load replays exact labor actions
**then Pass**, independently verifies all source records and checks the SHA-bound receipt.
Corruption, extra bytes, invalid order, missing original source, or wrong profile refuse
without modifying state. Source `Gk79Checkpoint` never mutates during Enhanced Pass.
Controller command menu, stock viewer, quote viewer and Enhanced Assign panel display the
same effective order count; PASS always uses two A presses and B cancellation.

**NOT CLAIMED:** independent original first Spring/RNG; real Classic Assign;
merchant offers, twelve other source-locked commands, AI, battle aftermath,
full campaign save or finished Windows/physical Xbox game.

`VERIFIED_STATIC`: source audit and SHA of historical original ROM references.
`OBSERVED_RUNTIME_STOCK`: historical 3 original Mesen profiles (fresh, archived Mongol,
archived World) only; **no new Mesen execution** in this increment.
`TESTED_PORT_LINUX`: see accompanying QA logs.
`NOT RUN`: Win32, NSIS install, physical Xbox controller.

Prior EXEC0118 tests and source archive members must remain unchanged; no private
ROM/raw RAM/source gameplay bytes should enter public artifacts.
