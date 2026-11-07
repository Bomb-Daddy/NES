# Enhanced Run019 — Q09 attacked-recipient battle attachment (partial)

**Direct immutable parent**: Original-KOEI Enhanced Run018, complete 535-entry ZIP SHA-256
`95a9e1133684d7b18841d275bf8d32d0b45b814b30521836cbf5922dd56e3ba5`.
Never merge with canonical NES Classic or DOS source. No changes to approved Enhanced
strategic economy, marketplace or Body Points.

When a Q09 escape has already entered an under-attack receiving province, original
SNES/English-NES unified battle evidence `hsovl2 D648/D69A` requires newest-HEAD
insertion and preservation of original battle-scale strength. Run018 already wrote
these data, but the next explicitly initiated receiving battle resorted officers
alphabetically and rebuilt hundredths from rounded strategic troops.

Run019 updates **the actual** `third_branch/officer_battle_adapter.py` initialiser:
source-known pending arrivals lead the defender ordering, retain their captured
`troops100` values and are dequeued only on construction of the recipient tactical
battle, without creating armies or soldiers twice. Corrupt pending/army mismatches
are rejected before acceptance. A specific event explicitly identifies this as
`q09_recipient_source_attached_port_partial`.

**Unfinished and not claimed**: this does not start a *simultaneous* second tactical
battle as soon as an escape arrives; the test explicitly starts a subsequent real
Campaign battle against the attacked province. Four-list topology, original turn
initiative, natural campaign source-byte hydration, actual original 30 NES event
replays and native Windows/physical Xbox acceptance all remain OPEN. This is one
observable-behavior parity correction, **TESTED_PORT / PARTIAL**, not a 1:1 release.

Actual Linux Python 3.13/Xvfb evidence:
- Parent RED: 3 failed, 2 passed (five non-Tk new regressions)
- Patch focused GREEN: 7 passed (includes actual Tk software-controller six-menu)
- Negative mutations: original-scale removal, reversed head order, skipped pending
  dequeue each caused an expected regression failure; source restored
- Broad selected game/army/economy/UI suite: 329 passed, 2 skipped
- Actual .ro2 persisted at end of battle, separately cold loaded in a new Python
  process, fingerprint identical; validates no duplicate officers
- Full unfiltered collection remains blocked by private `ROTK2_TEST_ROM` and a
  historical rollback import. Not equivalent to original NES emulator goldens.