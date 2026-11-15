# Genghis Khan I — EXEC0310 original-stock fresh title provenance correction

**2026-11-15 · Predecessor: verified EXEC0309 complete inherited native C11 game.**

## Reproducible defect (not a new source-independent first Spring)

The existing `gk162_original_setup_compatible()` rejects modified/partial wizard state before attaching **archived** stock Spring checkpoints, but the **fresh** profile had this shortcut:

```c
if(a->setup_stage[0]==GK53_SETUP_PLAYERS)return 1;
```

That accepted an unrelated, non-pristine pending wizard, e.g. `setup_candidate[0]=0` or `setup_difficulty[0]=3` while still at PLAYERS, as though it were an untouched stock reset. This contradicts EXEC0276's own explicit fresh-title-vs-known-chooser distinction and can misattribute an owner-private stock capture to an incompatible GUI setup history. The exact C11 public predicate from EXEC0276 was compiled: original rejected the hostile regression (exit 2), fixed predicate passed (exit 0), both optimized and under ASan/UBSan.

EXEC0310 makes **one real native application source change**:

```diff
- if(a->setup_stage[0]==GK53_SETUP_PLAYERS)return 1;
+ if(a->setup_stage[0]==GK53_SETUP_PLAYERS)return pristine_title(a,0u);
```

The existing `pristine_title()` already verifies unset rulers, selected candidate, base difficulty, player count, ability method, bonus points and setup cursor. This preserves the exact observed **Temujin/one player/difficulty 1/automatic** fresh-chooser continuation and archived-restored pristine-title handling. Both `gk162_enter_original_checkpoint` and `gk162_commit_verified_checkpoint` call this gate *before* touching `Gk1App` or processing the private source.

It **does not** interpret edited source bytes as the original 6502 startup, invent Gold/Rate values, derive original abilities, create three usable original orders, implement original Trade scheduler, or open a new original Spring. This corrects false admission and protects the existing game while the actual generator remains open.

## Exact production C11 integration

1. Expand the immutable **complete current inherited native source**, apply the existing production stages through **EXEC0309** to `execution_199`, not old EXEC0062 or an isolated test app.
2. Run `python3 execution_310_original_pristine_title_gate/test_stage310.py` for source-staging QA; run `python3 execution_310_original_pristine_title_gate/stage310.py execution_199`.
3. `stage310` pins the exact post-EXEC0309 `gk162_original_entry.c` SHA-256 `04c5d36bb85462610f5728c8f8a22c82a007b123604c7dafcbfede5b6622803a`, requires EXEC0309's actual `CMakeLists.txt` test registration, installs a **new native C11 CTest** `GK1_310_OriginalFreshTitleSetup_IsPristine`, checks immutable test SHA, handles real CRLF, and rolls back all affected source files on failure. No silent fallback to earlier source.
4. `cmake -S execution_199 -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build && ctest --test-dir build --output-on-failure -R GK1_310_OriginalFreshTitleSetup_IsPristine` (Linux). On Windows use full current WinMain source workflow. No original ROM/private 83-byte record required for the negative controls.
5. `python3 execution_310_original_pristine_title_gate/test_compiled_gate.py execution_199/gk162_original_entry.c` is an additional **Linux/clang-or-GCC** hostile regression (old source is reconstructed for negative control). Do not run the GCC sanitizer command under Windows MSVC.

## Execution evidence and acceptance, before Windows CI

- `VERIFIED_STATIC`: source SHA pin, direct exact public C11 predicate, atomic staging logic and test identities, latest EXEC0309 source authority; no ROM on GitHub.
- `TESTED_HOST_LINUX_PYTHON`: 11/11 hostile staging tests PASS, including 3 injected write-failure rollback positions, duplicate/tamper/malformed CRLF and exact source refusal.
- `TESTED_PORT_LINUX_C11_COMPONENT`: executable exact public EXEC0276 source predicate, original regression FAILED as intended (exit2), corrected predicate PASSED (exit0), optimized and ASan/UBSan. **Not** a full current native app test.
- `TESTED_PORT_LINUX_FULL`: NOT RUN in this local work until newest complete source is available.
- `TESTED_PORT_WIN32_CI`: NOT RUN for EXEC0310 until the new pinned GitHub workflow completes. Prior EXEC0309 225/225 must not be reattributed.
- `OBSERVED_RUNTIME_STOCK_FRESH` and `OBSERVED_RUNTIME_STOCK_ARCHIVED`: prior authentic Mesen captures unchanged and not rerun. Differences in Gold/Rate remain tied to original RNG history.
- `NOT RUN`: physical Xbox controller, original 14/27-country full campaigns, original independent first Spring.

## Privacy and next milestone

The private exact original USA ROM, original licensed art/audio, 83-byte stock RAM records, and checkpoints are **not** in this source. No original ROM hash content except the already-published identity metadata is necessary. This is actual native source integration, *not* a complete-game release or third-party agent read acknowledgment. Next: implement source-driven original setup/character/difficulty/RNG and three complete usable original player orders, rather than resurveying the 103-rule manual or mistaking one captured Spring for a fixed constant. Continue the existing recurring task pending Director stop/release acceptance.
