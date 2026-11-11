# GK1 EXEC0199 — Classic source-attested TAX Undo (port-only)

Based on the exact complete EXEC0198 C11 application, not a standalone demo.

Controller Y on the live Classic TAX screen now reverses the latest
UNPASSED TAX action, even if it was the third order. It replays every earlier
TAX/ASSIGN action against the original source checkpoint and atomically
restores the current tax rate and one remaining order. If the last action was
ASSIGN, pressing Y while TAX is visible refuses without undoing unrelated
work. If the source, action sequence or raw record is corrupt, nothing is
changed. The inherited original 83-byte records are never mutated.

A/DPAD retain manual-derived tax changes, B exits, X now saves GC19 while
in TAX; title Y still cold-loads. This is a controller usability enhancement,
NOT original NES TAX/Undo parity or an independently generated Spring.

Reproduce from complete source:
`cmake -S execution_199 -B build199 -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build199 -j8 && ctest --test-dir build199 --output-on-failure -j8`

Optional private original-stock checks: `build199/test_gk199_classic_tax_undo <41-country CSV> <fresh JSON> <archived Mongol MSS> <archived World MSS>` and `xvfb-run -a build199/gk1_native_shell --smoke-classic-tax-original <private source> 120`.

Evidence: source/port testing only. No fresh NES emulation, original character
stat RNG, merchant availability, full strategic AI, Win32 native testing,
NSIS installation, physical Xbox or complete campaigns claimed.
