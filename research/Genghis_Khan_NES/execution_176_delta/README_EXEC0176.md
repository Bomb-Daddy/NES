# GK1 EXEC0176 — Enhanced-only source-attested 20-season labor continuation

This is an **incomplete research game**, never original-NES campaign parity.
Directly inherits the full EXEC0175 native C11 app. Once a separately verified,
original-stock checkpoint has been attached in the user's private workspace,
and the five earlier Enhanced labor-only seasons have been completed, the
controller can progress through up to 20 further Enhanced-only seasons.
Each season enforces the existing three-action budget, labor transfers and Pass.
Replay-based attestation reconstructs the entire chain from the immutable
original 83-byte country records, rejecting corruption and mutated ancestors.
Source, stock Classic mode, and 14/27 original templates are unchanged.
No original CPU reset/RNG, AI, food, rates, taxes, dynasties, battles or merchants
are simulated by this feature. Continuation save/cold-load is NOT implemented.
Original Classic mode remains VIEW/PASS; no physical Xbox or Windows test.

Build: `cmake -S execution_176 -B build176 -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build176 -j8 && ctest --test-dir build176 --output-on-failure -j8`.
Private verifier (no redistribution): `build176/test_gk176_enhanced_cycle CSV PRIVATE_FRESH_14x83_RECORDS.bin`.
