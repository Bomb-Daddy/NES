# Original-KOEI Enhanced Run029 Q14 prisoner modal identity

Exact immutable parent: Enhanced Run028 SHA256 95f5660ff01e3f92fcb449876a0779f388f6fe4c7240dba9c0d4f7de247db907.
New complete source SHA256 262ce504f0324cf28d580a9afdcc93fd7ddba5b96d69c3aa9f63f6298defd794, bytes 148766932, 547 CRC-clean entries, 545/546 existing members unchanged.

Player-visible defect: live queue reordered while prisoner fate modal open could switch the visible prisoner to the next officer, and live option reorder could change Release to Behead. The patch freezes displayed officer and decision identities and revalidates them at confirm; fails closed before journal, RNG or disk mutations. It does not claim precise NES rejection text or original emulator golden replay.

Evidence: immutable parent RED 3 failed/3 pass, new GREEN 6 pass, wrong-actor mutant 3 expected failed, wrong-decision mutant 1 expected failed; broad 107 passed, fresh-extracted broad 107 passed, true Linux Xvfb/Tk; separate-process .ro2 cold-load validated. No real Windows/Xbox or 30 original NES event comparisons.

All previous Run028 source unchanged. Distinct from canonical NES Classic SUP-0016 and DOS. Enh strategic economy/BP deliberately unchanged.
