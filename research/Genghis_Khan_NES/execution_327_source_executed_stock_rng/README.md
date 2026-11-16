# GK1 EXEC0327 — Original-USA fixed-bank RNG source-executing C11 component

## Scope and precedence

Successor to existing complete native Genghis Khan I `EXEC0326` (Win32 CI238).
No historical game source is replaced and no unrelated project is touched.
No NES ROM, RAM snapshot, cartridge code, image, music, state, or private Mesen log is contained in this publication.

## Implemented, observable new behavior

`gk327_fixed6502_rng.c` interprets (rather than merely fingerprints or mathematically approximates) a bounded NMOS 6502 instruction stream from a **transient caller-supplied** exact original-USA image. For the authentic route it checks its 262160-byte exact SHA256, starts in fixed PRG bank15 at CPU `$C289`, executes opcode-by-opcode, and requires final RTS at `$C2EB`. Only CPU zero-page access is permitted. Any wrong opcode, bad memory write, overflow of the step budget, wrong input, or bad ROM must leave the output receipt byte-for-byte unchanged. Supports a separate entirely synthetic `fixture` route, which is NEVER labelled original-game evidence. No original ROM bytes are in this source.

`gk327_app_rng_bridge.c` is linked into the **existing** `Gk1App` C11 static library. It refuses Enhanced, wrong campaign, wrong active countries, non-PRE_SETUP, unchosen ability, source stage not completed and invalid ruler index. It only returns a *candidate* source RNG receipt — it **does not grant turns/orders**, change Gk1App or initialize Spring. The source exact original initial seed is not yet traced, and no authentic source call site or 14/27 country setup generation is established.

`stage327.py` SHA-pins six new files, converts only valid Git Windows CRLF checkouts to canonical LF before verifying the exact hashes, stages all six into the inherited source and appends CMake linking and two actual native CTests. Prevalidates before mutation and refuses duplicate stages or overwritten game sources. The full current Windows workflow must stage this after `EXEC0326` (which supplies SHA256 module) and before native `cmake` invocation.

## Actual executions on Linux

- Strict `-std=c11 -Wall -Wextra -Werror -pedantic` isolated authored-fixture native test: **39/39 assertions PASS**.
- Actual inherited `Gk1App` header bridge test: **20/20 PASS**.
- Same both with ASan/UBSan: **39/39 and 20/20 PASS**.
- Exact 5.47MB EXEC0278 COMPLETE native C11 source inherited, adding the new C modules into its `gk1_app_state` library: `cmake` configuration and *complete source build* PASS; 2 registered GK1_327 CTests **2/2 PASS**.
- Existing full older EXEC0278 suite attempted (229 registered): host command hit its timeout after 228 tests produced statuses; one historical Windows-workflow-source metadata test failed because inherited 0278 archive did not match the latest live 0326 workflow. **Not** a full-suite pass; previous real Win32 CI238 retains its own accepted result. No physical Xbox or complete campaigns were tested.
- Hostile SHA-pinned source staging: **17/17 assertions PASS**, including CRLF canonicalization, injected source alteration and duplicate refusal.
- Private-only real original-USA fixed-bank test (source and data NOT in published bundle): 2,500 consecutive fixed-seed interpreted RNG calls matched independently developed legacy native `gk137_stock_rng` at all six seed bytes and both result bytes **2500/2500 PASS**, negative controls for wrong ROM hash and size PASS. This is source instruction execution with **caller-supplied seeds**. This is NOT observed Mesen runtime, genuine NES timer/choice seed, or original Spring generation.

## Run new checks

```sh
python3 test_stage327.py
cc -std=c11 -Wall -Wextra -Werror -pedantic -I /path/to/execution_199 -I /path/to/execution_049 test_gk327_app_rng_bridge.c gk327_app_rng_bridge.c gk327_fixed6502_rng.c gk326_sha256.c -o /tmp/gk327-app-test
/tmp/gk327-app-test
cc -std=c11 -Wall -Wextra -Werror -pedantic test_gk327_cpu_rng.c gk327_fixed6502_rng.c gk326_sha256.c -o /tmp/gk327-cpu-test
/tmp/gk327-cpu-test
python3 stage327.py /path/to/current-inherited-app/execution_199
# Then configure/build CMake and run ctest -R GK1_327_ on the staged full game.
```

## Acceptance categories

VERIFIED_STATIC: exact local file SHA256, private unchanged.
TESTED_HOST_LINUX_C11: 39 + 20 assertions, sanitized, full inherited app compile and two CTests PASS.
TESTED_HOST_LINUX_PYTHON: 17 hostile staging assertions PASS.
OBSERVED_RUNTIME_STOCK FRESH/ARCHIVED: previous captured original Mesen evidence preserved; NO NEW Mesen execution in EXEC0327.
TESTED_PORT_WIN32_CI EXEC0327: NOT RUN / pending workflow integration and CI.
PHYSICAL_XBOX: NOT RUN.
FULL CAMPAIGNS: NOT RUN.
RELEASE_ACCEPTED: NO.

Next: source-trace actual setup call sites and RNG seed origins from original stock; reconstruct all country transforms/finance/character and **genuine 3 controller orders**; then autonomous AI, merchant availability, war aftermath, diplomacy/dynasty and both campaign acceptance. Do not infer runtime/agent read acknowledgment from publication.
