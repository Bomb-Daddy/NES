# GK1 EXEC0043 — Xbox/controller-only native desktop input, v1.00 (2026-11-06)

## Provenance / boundary

Additive native C11 desktop integration on the same standalone Genghis Khan I
Windows-port research shell as EXEC0042. Source-only port progress, **not**
new NES emulator gameplay, genuine battle source tracing, an authenticated
campaign save, a finished Windows release, or a physical-controller pass.
Existing original-game evidence, ROM mods, user campaigns, save/checkpoints,
KOEI economics and all previous EXEC source are unchanged.

## Implemented this increment

* `gk1_pad_router.c/.h`: platform-independent Xbox-style A/B/X/Y and D-pad
  routing, plus left-stick axis and optional Linux joystick hat (axes 6/7).
  16,384 engage/11,500 release hysteresis; 360ms initial/120ms subsequent
  directional repeat; face buttons fire only on press edges. Holding A/B/X/Y
  cannot cause accidental repeat commands. Simultaneous conflicting directions,
  A+B, or X+Y are deliberately suppressed.
* `gk1_native_shell.c`: controller-only GUI interaction (no game keyboard or
  keyboard save/load shortcuts). Windows XInput 1.4 fallback 9.1.0 reads slot0,
  D-pad and left thumbstick, handles disconnect/reconnect without phantom
  inputs. Linux `/dev/input/js0` joystick plus `GK1_JOYSTICK_DEV` override,
  hotplug open/reconnect, axes 0/1 + hat 6/7 and A/B/X/Y 0/1/2/3; optional
  button-style D-pad 11–14 (driver mappings may vary). X/Y retain research
  replay saving/loading; B back/finish, A confirm/move.
* Native 1280x720 shell and both still-unimplemented campaign menu entries
  remain intact. Keyboard is not a gameplay input route; OS window close
  remains possible. Linux debug CLI `--cold-export`/`--cold-import` unchanged.
* `test_gk1_pad_router.c`: always-on, Release-safe adverse test assertions for
  face-button edges, conflicts, bounded action-buffer, repeat timing, analogue
  threshold/hysteresis, hat/Dpad precedence, disconnect and anti-phantom states.
* `test_gk1_controller_pipeline.c`: end-to-end software-sampled controller
  input through actual native app state: title selection, A hold, forest
  movement+finish, low-Mobility rejection, controller B exit and disconnect.

## Verified in the Linux executor

`cmake -S execution_043 -B build043 -DCMAKE_BUILD_TYPE=Debug`
`cmake --build build043 -j4 && ctest --test-dir build043 --output-on-failure`

Debug 4/4, Release 4/4, AddressSanitizer+UBSan 4/4 CTest pass.
Linux native X11/Xvfb six-frame 1280x720 render smoke passed.
Separate Linux processes exported/imported original archived friendly-join
*fixture* case3 events1 (74 soldiers, word2) via real 48-byte replay file.
That is a test of existing 0042 code path, not new native NES proof.

Physical Xbox controller not attached/used. Windows native branch authored but
not compiled or tested in this executor. Do not equate source/CI unit tests
with a physical Xbox acceptance or installed Windows game.

## Required next work / hostile QA open

Recover EXACT SHA-pinned matching MesenCE developer executable/core and
approved combined-ROM full-machine checkpoint before claiming native trace:
Mesen SHA256 e777af28bc6cc49d2e61ca0c1a82fbccf13811355332a53ea00749fc37a92761;
MesenCore.so 0668f6c09bc89a1c8199df6cbf15aeebba243169b30f8116df97127dbfd3d7af;
checkpoint SHA256 e1b6266c963a48d736627812d024910b6d600c2ad8571813eb3b0999e0f2938b.
A generic core may quietly zero SRAM and is not equivalent. No matching binaries
were present locally in EXEC0043. Reproduction must prove pending→loaded and
unit40 at (7,7), Mobility10, then native controller-driven $F1C8/$F1CD writer,
active bank/VM pointer $06:$07 / $613E:$613F, Entire/Divide and terminal logic.

Full windows game, true 2 campaigns, battles, AI, dynasty, audio renderer,
HD graphics, packaged installer and actual Windows/Xbox player tests remain OPEN.
