# GK1 EXEC0308 — final-neutral physical XInput gate

2026-11-15. Production-bound correction to EXEC0307 in the existing inherited native C11 WinMain game.

EXEC0307 completed its eight-key / analog diagnostic immediately on the final right-stick movement while the stick was still held; closing its window then yielded exit 0. The EXEC0308 source makes the eighth matched input pending, requiring a subsequent all-buttons-up, both-axes-centered XInputGetState sample from the *same controller slot* before complete=1 and successful exit. Wrong keys, held axis, slot change and disconnect do not silently complete. No normal-game world or save mutation; ROM and licensed assets not distributed.

This commit includes the actual C11 FSM and executable test and updates the exact SHA256 pins in the production stage307.py, preserving fail-closed Win32 source integration. Local GCC -std=c11 -Wall -Wextra -Werror -pedantic source regression: 75 assertions PASS. On Nov 15 16:29Z predecessor EXEC0307 Win32 CI 224/224 passed: https://github.com/Bomb-Daddy/NES/actions/runs/43735474327 . EXEC0308's new Windows CI acceptance is NOT implied by the predecessor and must be verified separately. Physical Xbox device and full campaigns remain NOT RUN. Original first playable stock Spring, real character/difficulty RNG, 14/27-country initialized game with three usable orders remain incomplete. Keep current recurring work enabled; no full release.

Source SHA256:
- gk307_physical_pad_qa.c: e700b54895a13422e9c5ceb3726f4163b4174abf81f6254c20e0e92e30c394bc
- gk307_physical_pad_qa.h: 76b2a1b64f6dfc11771de6059e16af303ef1085c854638be711d01f8babd7b02
- test_gk307_physical_pad_qa.c: a0b2e773e55fbd89db06c5d31b3b3a271f20f776287f416fec7e2797b6704e7d

Correct Drive Collaboration parent: 1d0TA1lgqsVA3AWQQ9o0FZgmvao90oCeT; private Genghis research: 1mJ72ezJb9kzIGUHHXPTh3lXtiVxWjEL7. User's original-stock ROM and Mesen captures must not go to public GitHub.
