# SUP-NES-COMBAT-20261102-031 — Q17 commander survival and Q18 reserve fail-closed

Authority read before edit: current `PROJECT_REFERENCE_04_QA.md` (1KBLG5jJzqLBjhCQSP_VKY6Pibn7l6CoQ), `PROJECT_REFERENCE_01_AUTHORITY.md` (1x57L8j4WQgPHwtww4y9Tt6_zzGdB51lH), live supersession (1qEVhDdiG2oH8LvPz0jan5DCtFFVxHhQWdWRUpduTllQ), and living Battle Answer Matrix v1.4 parts 1/2 (1r4KOqPC8uN1VDmaNlfpjHa_k0J__GYZZ, 1uUNq4BWRie_b-DR3nh7h0jUqiAgw4Mtd). Both canonical projects are separate. No release acceptance claimed.

## Executable changes

* Q17 both Classic and Enhanced: an active CPU list commander with fewer than 3,000 battle-scaled soldiers holds instead of making a normal tactical attack. Emergency fire evacuation remains higher priority. Original source Q17 permits retreat **or** hold at this threshold; exact retreat routing remains unimplemented. The 3,000 boundary is strict: at exactly 3,000, the commander remains attack-eligible. This is source-constrained PORT behavior, **not** a claim of exact original AI target/retreat parity.
* Q18 Classic: a pending Q09 tactical reinforcement now fails closed before any RNG draw or offscreen battle mutation, rather than silently including undeployed officers in the six-round CPU resolver. Enhanced already had the same guard. The original offscreen reinforcement handling remains an explicit open parity gate.

## Immutable source status

* Enhanced38 complete merged parent: 18/18 Drive parts downloaded, per-part SHA verified, full SHA256 `c2f2497f68a08bfc3c6fcef271f155008ee895348fb2a12f65329622ab6f60b3`, 506 entries, ZIP CRC passed. Enhanced39 is a candidate based on this verified parent.
* Classic37 complete parent **NOT** independently recoverable: Drive folder contains only 3 of approximately 5 split parts and an explicitly INVALID truncated ZIP. Classic36 complete parent SHA `a564a5d8fe92d96156fabf00d698a7367dd13d32dfc599f3259f24150c1aa962` was available locally. Applying published Classic36→37 Q03 diff and rebuilding produced 4,281,926 bytes and a byte-identical 3,145,728-byte prefix against the three Drive parts, but its full ZIP SHA `7b2fd9e011065881f22d971c250e1692bd429049413bde9bf3571254ce36d711` **does not match** the expected Classic37 SHA `679c344ed98e574952289088e58012c1d76495bc9b268dcb7e6b3f510623a56f`. Therefore Classic38 is a **reconstructed candidate**, not a verified canonical descendant; no canonical source-promotion claim is permitted. Reconcile the missing tail bytes before promotion.

## Tests

Parent RED: Classic Q17 below-3000 commander attacked; Classic Q18 pending reserve was accepted and mutated; Enhanced Q17 below-3000 commander attacked. After correction: four targeted Classic and four Enhanced tests passed, including exact-boundary negative controls. Classic available suite 74 passed, 1 skipped (ROM-gated test excluded from collection). Enhanced build-scoped suite 154 passed, 2 skipped with DISPLAY unset; live Linux Tk/Xvfb test separately passed. The original NES ROM was unavailable for runtime comparison in this environment. No native Windows binary, physical Xbox controller or original-emulator replay performed.

## Outstanding Director acceptance

Q01–Q22 remain PARTIAL. Q04 interactive placement/stacking, Q09 full four-slot arrival, Q14 succession, Q16 strategy codes, Q17 exact AI target/retreat ranking, Q18 six-round source-exact intermediate arithmetic, Q19 invasion AI, original hardware RNG, native Windows/controller and end-to-end save/quit/cold-load acceptance remain open. Do not run long campaigns until implementation is complete. Preserve prior parents for rollback.
