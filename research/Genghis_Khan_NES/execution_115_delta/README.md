# EXEC0115 verified native source delta (NOT the complete native game)

The full inherited, tested, copyright-clean 296-file native C11 source ZIP, plus all 9,849 preserved predecessor archive entries, is in the verified project Collaboration Drive folder:
https://drive.google.com/file/d/1EftkiNxhLHhmKqvcCoocmGC9TfV0I6Wb/view

This GitHub directory contains a partial, separately navigable SOURCE DELTA, **not** a buildable full source release. The complete code has NOT been committed here. For the full version use the exact source ZIP above (16,269,422 bytes, SHA-256 fe63f3411a8b90b81caa450e5a6f8e9806da692046c9becc8f5f88eaef08739d). Prior complete GitHub execution_112 remains unchanged.

## Genuine EXEC0115 fix
The inherited `Gk1App` entry `gk1_app_trade()` could consume Gold, Judgment and an order from a synthetic world state after a caller forged world.phase=2, market.playable=1, merchant presence and an offer. EXEC0115's integrated `gk115_stock_market_gate.c/h` refuses that action unless a source-authenticated original merchant event exists. None exists, so all real app Trade currently refuses; do not fake merchant RNG or mark first Spring playable. Standalone source-derived `gk61_commit()` math remains tested on isolated test fixtures. Consult full source archive for linked CMake, Gk1App integration, updated gk62 regression and packaging.

## QA / publication
Linux Release 73/73, Debug 73/73, ASan+UBSan 73/73, clean extracted full source 73/73, native X11 1280×720 120 frames and Debian extracted executable 120 frames PASS. Debian: https://drive.google.com/file/d/1LswGHMr-T7JvXz2j7qS8JHBk0Jd5ySmM/view ; QA: https://drive.google.com/file/d/1I2YDHw1uUMpkc9k5U-JTniveyq5DHJzf/view ; master v2.94: https://docs.google.com/document/d/1kITd-zpShG4KFHkU07Ni6NK-Wd73btfsZO0jNj5u3qE/edit .
Prior original fresh and archived NES Mesen captures remain historical, not rerun. Win32/NSIS/physical Xbox/full campaigns NOT RUN. First playable authentic Spring with 3 orders, live Trade and remaining 12 commands, AI/diplomacy/dynasty/battle aftermath all OPEN.
No original NES ROM, RAM, SRAM, images, audio or copyrighted original assets were uploaded to public GitHub.