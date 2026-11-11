# EXEC0186 — authenticated original-stock RNG callsite static decoder

Adds distinct JSR `$C289` callsite root reachability for all 15 assumed MMC1
lower-bank configurations within the SAME complete C11 game. The stock ROM
must pass original SHA256 authentication; no bytes of private ROM are bundled.
Test executable and opt-in `gk1_native_shell --audit-original-rng-calls <private_rom>`
show PCs, mapper hypotheses and RESET/NMI/IRQ bitmasks. An ordinary bytestring
`20 89 c2` in data is not recorded unless the existing bounded source decoder
reaches it. Calls behind unknown mapper changes remain unproven.

**Does NOT generate authentic first Spring**, reconstruct actual RNG seed or
character/difficulty setup, perform original Mesen gameplay, authorize any
original strategic action or claim Windows/Xbox validation. Follow-up
requires real Mesen RESET/setup call chronology and country-write traces.
