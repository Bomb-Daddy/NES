# GK1 EXEC0138 — Source PRNG reverse-trace / original Spring Rate alignment (research, not playable)

Inherits **complete** native Genghis C11 application from EXEC0137, unmodified scenario provenance, original-source cold-start guards and Classic/Enhanced isolation. Same 1280x720 controller UI. Adds gk138_rng_reverse.c/h and source-gated read-only app method. No original-ROM, SRAM, assets, saves, or copyrighted extracts included.

Verified arithmetic: the source-v1.07 C289 generator maps `s -> -(s|1)*0xe0900e0187c5 mod 2^48`. The multiplier has inverse `0xd8e495025f0d mod 2^48`. Every output is odd, and forcing input bit 0 means two preceding inputs (one even, one odd) are indistinguishable. For an exact observed output seed and known number of draws, backwards traversal gives this precise two-candidate input pair. A Spring Rate numerical match within a scan is only *compatible* with an unknown chronology, never proof of that chronology. An isolated source expression is *not* a real first Spring or a seed authentication protocol.

The user-authorized original NES game must still independently produce all 14/27 post-setup original country states with real ability/difficulty/RNG, three orders and live merchant/AI/diplomacy/battle. **No* staged snapshot has been promoted to genuine setup; no original ROM or private RAM is published. Previous genuine Mesen fresh Mongol1175 Rate121 and archived Mongol1175 Rate101/World1206 Rate114 have distinct RNG histories. This patch does not claim a new native Mesen run or Windows/physical Xbox QA.

QA commands (from execution_138):
`cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j4 && ctest --test-dir build --output-on-failure`

Linux `Xvfb` smoke and Debian tests are separate from Windows/physical-Xbox acceptance; publish only observed results and exact artifact SHA-256.
