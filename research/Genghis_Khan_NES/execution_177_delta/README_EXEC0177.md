# EXEC0177 — Authenticated Enhanced continuation replay save and cold resume

This is a **real incremental save/cold-load improvement to the SAME 1280x720
native C11 game**, based on all files in EXEC0176. It does **not** implement
KOEI/NES first-Spring setup, AI, three Classic commands, original trade RNG,
dynasty, diplomacy, battle aftermath, a full campaign, or Win32 device QA.

Controller X while the Enhanced continuation is active creates an exclusive,
owner-private `.gk77` receipt. Y loads it without changing live state on
failure. From TITLE, X loads the exact private original `.gk17` source and
then the newest continuation `.gk77`, checking the complete immutable original
checkpoint and replaying all 5 inherited and up to 20 continuation seasons.
A corrupted newer receipt refuses rather than silently rolling back to an
older stage. No source ROM bytes, SRAM, images or audio go into `.gk77`.

The fixed 448-byte GC77/v1 format records the source SHA identity and ordered
from/to/amount action receipts, Pass flags and current season; trailing SHA256
validates file integrity but is **not** authentication. The actual authenticity
boundary is the SHA-validated private original source, complete upstream
stage replay, strict canonical re-encode and atomic commit. Classic mode
cannot import Enhanced receipts. Existing `.gk16`–`.gk21` receipts are retained.

Linux I/O: exclusive 0600 creation, file fsync, parent-directory fsync,
O_NOFOLLOW load, fstat regular/owner-only/single-link/exact-size checks.
Windows `_commit` branch authored but **NOT RUN** and Windows ACL equivalence
not certified. Only the verified `.gk77` ownership path is allowed to consume
this format, never the original NES source slot.

Build: `cmake -S execution_177 -B build177 -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build177 -j8 && ctest --test-dir build177 --output-on-failure`.
Private original runtime regression (do not publish fixtures):
`build177/test_gk177_enhanced_receipt build177/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv PRIVATE_FRESH_14x83.bin 1`;
use profile 2 for archived Mongol SaveRAM, profile 3 for archived World SaveRAM.

Labels: VERIFIED_STATIC (source/replay/format); OBSERVED_RUNTIME_STOCK (prior
three original Spring samples only); TESTED_PORT_LINUX (native integration
and controller UI). NOT RUN: new NES/Mesen playback, Windows installer,
physical Xbox, full campaigns. Publication of this source is not a Director
acceptance or other agent execution acknowledgement.
