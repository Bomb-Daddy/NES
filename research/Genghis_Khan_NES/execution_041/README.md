# EXEC0041: Native standalone Genghis Windows shell (first real-window vertical slice)

This is a copyright-clean **standalone native desktop prototype** at 1280x720.
It is NOT the complete Windows Genghis Khan I game, has no authored NES assets,
no audio, no campaign engine, no installer, and is not Windows/physical-controller verified.
It does not include ROM, SRAM, copyrighted game graphics, or checkpoints.

## Code and source-boundary

- `gk1_app_state.c/h`: all UI navigation from six controller commands, two campaign entries **explicitly locked** as unfinished, and six fixed Native modified-ROM experimental battle cases.
- `gk1_native_shell.c`: X11 window on Linux and Win32/GDI window plus XInput on Windows (untested Windows platform code). Both target a 1280×720 client area.
- Existing `execution_039` battle replay + `execution_034` native sourced bounded movement rules are linked unchanged; no guessed general movement.
- Prototype only: green checkerboard and simplified troop markers are PLACEHOLDER artwork, not NES fidelity. Linux joystick maps axes 0/1 and buttons 0/1; Win32 XInput maps D-pad and A/B. Keyboard is a QA fallback, not required navigation.

## Executed Linux build commands

```sh
cmake -S execution_041 -B build041 -DCMAKE_BUILD_TYPE=Debug
cmake --build build041
ctest --test-dir build041 --output-on-failure
./build041/test_gk1_app_state
Xvfb :95 -screen 0 1280x720x24 &
DISPLAY=:95 ./build041/gk1_native_shell --smoke-frames 4
```

Windows target (NOT executed):

```sh
cmake -S execution_041 -B build-win -G "Visual Studio 17 2022" -A x64
cmake --build build-win --config Release
```

## Evidence levels

1. Strict C11 model tests verify navigation and exact bounded replay fixtures only.
2. X11 smoke run, if executed, verifies that a real desktop window can draw frames in a virtual display. No human player, video/art parity, audio, or controller acceptance is implied.
3. Win32 and physical XInput need compilation, real device tests, and UI QA before acceptance.
4. A native full-machine Mesen trace remains necessary for the friendly-unit join source and Entire/Divide. Do not treat this UI shell as proof of source behavior.
