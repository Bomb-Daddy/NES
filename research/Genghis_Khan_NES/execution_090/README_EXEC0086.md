# Genghis Khan I EXEC0086: exact source-bound Spring checkpoint persistence

In the inherited real 1280x720 C11 native game, `gk80_save_new` now writes V2
research checkpoints. The V2 save includes a 32-byte SHA-256 identity computed
over all 83 original NES source bytes in each of the 14 or 27 source countries,
not merely the 31 numeric fields projected into the port. An independently
SHA-gated private stock capture must still be supplied when loading. The full
SHA-256 of the owner source is stored; neither ROM nor copyrighted source RAM
is serialized into any public artifact.

V1 research saves remain readable, with a **distinct warning** that V1 never
recorded the unmodeled original source bytes. Loading a V1 save does not alter
the source itself or claim stronger fidelity; users can create a new V2 save in
an empty slot. V2 mismatch/corruption refuses atomically, and existing saves
are never overwritten. Synthetic 14/27-source tests check mismatched unmapped
bytes despite identical numeric projections, attacker-rehashed invalid tags,
checksum/version/length corruption, genuine prior V1 format, save collision,
and rollback. No new ROM gameplay, first Spring RNG, AI, merchant scheduling,
full campaign, Windows runtime or Xbox acceptance is claimed.
