# EXEC0228 — Win32 native compiler and installer qualification harness

This increment adds a compiled console smoke program linked to the **same**
`gk1_app_state` and the exact `gk99_xinput_bridge` used by the WinMain game.
It validates the full 41-country source identity and two source states,
negative market-phase protection, atomic refusal, controller mapping, and
Windows profile save destination. Its separate Win32 API calls distinguish a
native Windows execution from Linux tests. The hostile Python test proves
source tamper, truncation, and absence fail closed.

The new GitHub Actions workflow compiles the actual Win32 GUI game and the
smoke/test executables on an `windows-2022` image, invokes the NSIS packager,
and validates installer contents without a physical controller. The pipeline
must **run successfully** before calling Win32 or NSIS tested; merely committing
this YAML or passing Linux CTest is not Windows certification. No game ROM,
original screenshots, savestates, audio assets or user campaigns may enter the
public repository or artifact. Artifact retention is deliberately disabled
unless the Director explicitly enables a release publication gate.

Original source-derived first Spring, authentic RNG, AI, complete campaigns,
physical Xbox and customer acceptance are still open. This smoke harness is
NOT a substitute for source-authentic game initialization.
