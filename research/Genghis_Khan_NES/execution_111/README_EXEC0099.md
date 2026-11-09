# EXEC0099 — Native Win32/XInput controller-only transport hardening

This is an incremental change to the complete inherited **EXEC0098** 1280×720
C11 Linux X11 / Windows Win32 game. **Not a released, playable NES port.**

- New `gk99_xinput_bridge.c/h` maps all documented XInput face, D-pad, menu,
  view, and shoulder buttons to the existing `Gk1PadSample`, converts the left
  stick Y axis with safe INT16_MIN saturation, and filters out unsupported bits.
- Native Win32 poller calls the same portable mapping (no duplicated Win32
  button switch) and uses a sticky four-controller slot policy. Disconnecting
  or switching controllers resets the pad router before a new input source is
  seeded, preventing phantom A/START commands after hotplug.
- New `test_gk99_xinput_bridge.c` exhaustively exercises all 65536 raw button
  patterns, all 65536 signed axes, all device-mask/previous-slot pairs,
  invalid pointers/arguments, and START/A rising-edge and reconnect behavior.
  These tests run under Linux but execute precisely the Win32 poller's mapping
  and selection logic. The Win32 operating-system calls, linking, XInput DLL
  loading, and physical Xbox hardware are **NOT RUN**.
- Source-authentic NES 6502, 14/27 Spring capture viewer, read-only original
  Trade quote reference, Classic/Enhanced profile separation, historic saves,
  genuine captured NES evidence and all earlier source files remain intact.
  No private cartridge, Mesen RAM, screenshots or licensed assets are included.
- Native Windows executable / NSIS package and physical Xbox testing remain
  explicit release gates. Original autonomous first Spring, RNG, 12 commands,
  merchant schedule, AI, diplomacy, dynasty, full battles and campaigns remain
  incomplete. No public source publication means someone else has run it.
