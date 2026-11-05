# ROTK II original NES isolated runtime — 2026-11-05

## Proven launch route

The original English NES ROM ran in the exact historical Mesen executable with
no X server and no `DISPLAY` environment variable. The launcher uses a new
portable directory for each process, with `settings.json` adjacent to its
private executable. `HOME` and existing emulator profiles are not changed.

Use `run_mesen.py --mesen PATH --rom PATH --script PATH --output NEW_DIRECTORY
--emulator-timeout SECONDS`. The output directory must not exist. The runner
checks the original ROM and executable hashes before boot, keeps complete
stdout/stderr and a JSON receipt, then verifies the extracted core and unchanged
original ROM. The supplied Lua script controls its bounded experiment.

The native invocation is `Mesen --testRunner --enableStdout --doNotSaveSettings
--timeout=SECONDS SCRIPT ROM`. Environment changes are `LC_ALL=C`,
`DOTNET_SYSTEM_GLOBALIZATION_INVARIANT=1`, `SDL_AUDIODRIVER=dummy`, and removal of
`DISPLAY`.

| Identity | SHA-256 |
|---|---|
| Original NES ROM, 524,304 bytes | `11e312691b516e5e29dade3c8ce5434df3561cc69e1aecad9c948acdfddc517a` |
| Historical Mesen executable | `f02685dafe559addf8dd8d769621a89777137d0c4592436aed65694a7e740676` |
| Its embedded/extracted MesenCore.so | `9d21269be65268c1f56543b83e2f10650f3a28ef2566a4acd861615ecc6ed189` |
| Tested settings.json | `d8c5c019637436a1e6e54752ab4dc4f9202849b36d5169e397899bb834eff525` |

The historical executable was recovered from the existing local modified
launcher (`e777af28bc6cc49d2e61ca0c1a82fbccf13811355332a53ea00749fc37a92761`)
by restoring exactly two embedded ZIP entry names, at byte offsets 41921875 and
47434338, from `MesenCore.xx` to `MesenCore.so`. The resulting complete binary
matches the historical expected hash. The Genghis Khan SOROM-patched external
core is not used by these runs.

The full tested profile is in `sessions/smoke_direct_001/portable/settings.json`
and the `SETTINGS` constant of `run_mesen.py`. It sets Version 2.2.1, ConfigUpgrade
1, EnableTestMode true; script IO/network true with ScriptTimeout 60; standard NES
controllers on both ports; RAM `AllZeros`; mapper/alignment randomization false.
The full combination is runtime verified. Individual fields' minimality has not
been tested.

## Native Lua interfaces inspected and executed

Read memory without CPU side effects:

```lua
local byte = emu.read(cpu_address, emu.memType.nesDebug)
local size = emu.getMemorySize(emu.memType.nesSaveRam)
local physical_byte = emu.read(physical_offset, emu.memType.nesSaveRam)
```

Map a CPU address to the currently selected physical memory:

```lua
local m = emu.convertAddress(0x7D25, emu.memType.nesMemory, emu.cpuType.nes)
-- nil if unmapped; otherwise { address = physical_offset, memType = enum_value }
local st = emu.getState()
local bank_register = st["mapper.prgBanks0"]
local prg_mode = st["mapper.prgMode"]
local native_frame = st["frameCount"]
```

`getState` uses flat string keys containing dots. The PRG bank register keys are
`mapper.prgBanks0` through `mapper.prgBanks4`. Use `nesMemory` for
`convertAddress`; `nesDebug` is a special read mode with a side-effect-disable
flag, not an accepted address-conversion enum.

`emu.createSavestate()` returns a native binary MSS string and
`emu.loadSavestate(bytes)` returns a boolean. Both must be called inside a
main-CPU **exec memory callback**. These probes use the general `$8000–$FFFF`
execution range; no Genghis Khan address is used. A false load return is failure,
regardless of any surrounding printed message. Schedule a save before the stop
frame, so the pending operation reaches an allowed exec callback.

`emu.takeScreenshot()` returns encoded PNG bytes. After a cold restore, its first
frame callback returned an empty string; the second produced a valid 256×240
image. Require a valid PNG and bounded decoder readiness. `emu.stop(0)` can be
followed by additional frame callbacks before process shutdown, so guard final
outputs with a stopped flag. Input belongs in `emu.eventType.inputPolled`; pass
`{}` to release all buttons.

## Full SRAM and current bank evidence

The runtime identifies this ROM as MMC5, NES-EWROM, with 32 KB SRAM.
`emu.memType.nesSaveRam` is enum 50 in this pinned runtime, and its size is
32,768 bytes. `nesMapperRam` contains 1,024 bytes. Use symbolic names in scripts.

At the verified opening-screen checkpoint, register `$5113`/`prgBanks0` equals 1:

| CPU address | Physical memory | Physical offset |
|---|---|---|
| `$6000` | NES SaveRAM | `$2000` |
| `$7C1D` | NES SaveRAM | `$3C1D` |
| `$7C26` | NES SaveRAM | `$3C26` |
| `$7D1C` | NES SaveRAM | `$3D1C` |
| `$7D25` | NES SaveRAM | `$3D25` |
| `$7FFF` | NES SaveRAM | `$3FFF` |

The native 8 KB CPU-window dump exactly equals full SRAM bytes `$2000–$3FFF`.
The MMC5 source maps `$6000–$7FFF` in 8 KB banks through `$5113`. These observations
do not identify any battle variable: this checkpoint is still at the opening
screen. Record actual bank conversion again at each battle snapshot.

## Native MSS extraction and the historical nine-byte question

`inspect_mss.py` follows the native format from `SaveStateManager.cpp` and
`Utilities/Serializer.cpp` in MesenCE commit
`4b8a196e299b3de297b204fd1208d2981562c89b`, then validates actual saved bytes.

Current MSS format 4 has a **nine-byte compression envelope for the whole
serialized state**: one compression flag plus two little-endian uint32 lengths.
After decompression, each record consists of a NUL-terminated ASCII key, a
little-endian uint32 value length, and exactly that many bytes. The
`mapper.saveRam` record therefore has **19 bytes before its data**, not nine.

In the verified checkpoint, the serialized payload is 53,091 bytes; the
`mapper.saveRam` record starts at payload offset 8,610 and its data starts at
8,629. All 32,768 extracted bytes exactly match the simultaneous native
`nesSaveRam` dump, SHA-256
`969f9d8292b9e7ac39e1d8147cbef19dea1b28077390292c6c5165b302266173`.

The v1.4 battle matrix instead describes an older **FCS blob**, taking an 8 KB
block at blob offset 13,312, and names `verify/loop5_nes_run.py` and
`verify/loop9_nes_ram.py`. Those original blobs/scripts have not been recovered
in the inspected local/GitHub material. Current MSS framing alone does not prove
the cause of that older address discrepancy. No RAM relocation or extraction
correction is asserted here without those bytes.

The subsequently recovered Nestopia format reference (Drive file
`10NryEw5W768Ow5DEKgra5VUNfXgBZM6o`) and independently inspected upstream
[`NstState.cpp`](https://raw.githubusercontent.com/0ldsk00l/nestopia/master/source/core/NstState.cpp)
establish a different, plausible source of nine bytes: an NST memory chunk has
an eight-byte tag/length header, then a one-byte compression flag. For an
**uncompressed** WRM chunk, its raw bytes begin nine bytes after the chunk header.
The inspected source is preserved as `reference/NstState_upstream.cpp`, SHA-256
`fafa50e320c81c6cb29f5d54cfdeb689f46447bbb9183786eec7f4a40081a194`.

**INFERRED / conditional only:** if the historical WRM chunk header were at
offset 5,120, physical bank 1 would begin at `5120 + 9 + 8192 = 13321`, nine
bytes later than the matrix's claimed 13,312. Such an extraction would make
actual CPU `$7D1C` appear at a mislabeled `$7D25`. Neither the historical chunk
offset nor its original FCS/container bytes have been observed. This is a
source-grounded explanation to test, not an established correction to that
old extraction. It is also distinct from MMC5 bank selection.

## Verification record

`evidence/checkpoint_memory_002/verification.json` contains the comparison receipt.

- **OBSERVED_RUNTIME / PASS:** cold original-ROM smoke, exit 0, actual 256×240
  opening screen, read-only CPU RAM and CPU-window dumps.
- **OBSERVED_RUNTIME / PASS:** native save then cold process restore; load returned
  true. All 2 KB CPU RAM, 8 KB CPU window, 32 KB SRAM, 1 KB mapper RAM, CPU registers,
  address mappings and exposed state fields matched exactly.
- **OBSERVED_RUNTIME / PASS:** decoder-ready cold-load PNG matched the source PNG
  byte for byte (830 bytes; SHA-256
  `e2c384270cde73063da102c5024a77941909d41474f366a242a2411b31d0d7a5`).
- **OBSERVED_RUNTIME / expected rejection:** invalid MSS header returned false,
  CPU registers remained unchanged, native exit was 2 and wrapper exit was 1.
- **VERIFIED_STATIC + OBSERVED_RUNTIME / PASS:** keyed MSS SRAM extraction matched
  every native SRAM byte from the same callback.

Source checkpoint `evidence/checkpoint_memory_002/original_frame300.mss` is 5,003
bytes, SHA-256 `94ab8e58aed153e595b53936533066827c7129f9349d41cd91fb45b147b7c686`.
It was not overwritten. Raw process logs and exact commands are under
`sessions/smoke_direct_001`, `save_memory_001`, `save_memory_002`,
`load_memory_002`, `load_memory_003`, and `load_invalid_001`.

Preserved unsuccessful attempts: the initial apt update failed on host
setgroups/seteuid restrictions; the package mirror returned a site-blocked
response. Native direct mode subsequently made Xvfb unnecessary. The first save
fixture allowed a second terminal frame capture; its original log is retained.
The first cold-load capture wrote an empty PNG; its original output and log are
retained. The corrected probes use a terminal guard and a bounded nonempty-PNG
check. No running Genghis Khan process or profile was used.

The copied generic bridge source is reference material only. These successful
runs use standalone Lua and `run_mesen.py`, not its legacy launcher or any
campaign-specific checkpoint extension. This verifies original-emulator
infrastructure; it is not Windows-port or physical-controller acceptance.


## Reproducing the battle probes

The current authored wrapper is `tools/battle_probe.py`, with `tools/run_mesen.py`
and `tools/inspect_mss.py` alongside it. The private evidence archive preserves
all experiment schedules, generated Lua and raw native products. Supply your
own authorized original ROM; a different ROM hash is rejected before launch.

```sh
python tools/battle_probe.py \
  --rom /path/to/authorized-original.nes \
  --mesen /path/to/verified/Mesen \
  --schedule /path/to/evidence/runtime/experiments/wait_001/schedule.json \
  --state /path/to/evidence/runtime/experiments/cold_yuanshu_001/state_021800.b64 \
  --output /path/to/new-run-directory --timeout 60
```

Omit `--state` for a cold boot. Every output directory is new; existing evidence
is never reused as an output target. A schedule records frame-based button
pulses, screenshots, native saves, memory snapshots, watches and any explicit
RAM interventions. Frames are local to the probe, not the emulator's lifetime.
The exact Lua for each historical run remains authoritative even if the wrapper
was subsequently improved.

`input_port` is 0 for NES controller 1 and 1 for controller 2. The two-human
reproduction required port 1 when Liu Biao was the second player; port-0 input
had no effect at that player's selection. The other port is explicitly cleared.
A two-human Wait can advance quickly to the next human officer: an extra A pulse
that was harmless during the one-human AI phase can open the next officer's
Move submenu. Inspect the next menu before repeating an input sequence.

The full SRAM is the safest observation input. The battle state uses physical
bank 1 in the verified fixtures, but the live CPU window temporarily selects
bank 3 during some prompts. `tools/census_battle.py` deliberately reads physical
SRAM bytes 0x2000–0x3FFF and checks the linked lists and original-ROM name table.
It does not assume that the current CPU window is bank 1.

Native save/load callbacks are armed only for their pending operation and
removed afterward. This avoids an all-PRG callback during the whole run. A saved
state is requested before the stop frame, and the native callback must complete
it. `loadSavestate` must return true; a surrounding printed marker alone is not
accepted.

## Strengthened evidence checks and preserved failures

The current PNG gate validates structure, CRCs, IDAT and IEND, bounded complete
zlib decompression, exact 256×240 scanline size, and filter bytes. The earlier
header-only check had a demonstrated false-PASS case; failing and passing tests
are in `evidence/review/`. The current runner verifies the copied executable
before launch and after exit, the extracted core, unchanged original ROM,
expected native completion and output hashes.

One original retained executable copy in `weather_day3` was truncated despite a
correct recorded prelaunch hash. Its cause and exact timing are unknown. The
source artifacts remain preserved. `weather_day3_recheck` reproduced the HUD and
state with a complete retained canonical executable. The later post-run hash
gate prevents silently accepting that retained-artifact mismatch in a new run.

The immediately post-restore screenshot can be empty; schedule decoder-ready
screenshots from frame 2 or later and retain the validity check. A timeout or
missing final checkpoint is incomplete evidence, even when earlier screenshots
are useful. Preserve those partial products and rerun a smaller bounded step
from an untouched prior checkpoint.

Completed portable executable/core duplicates may be stored as verified
hardlinks to conserve workspace resources. This is recorded separately and
preserves each byte and path. Newly launched processes always receive independent
copies, so a writable runtime cannot alter another run or the canonical source.
The private archive stores one verified canonical executable/core and the known
anomalous retained copy, rather than hundreds of identical copies; its manifest
states the exact exclusions.

For integrity checking, run the harness tests and the runtime manifest builder
against the private archive's extracted artifacts. Tests of the wrapper are not
original-game tests. The source-only Q19 byte executor is not a complete console
emulator. These distinctions also appear in each result's evidence label.
