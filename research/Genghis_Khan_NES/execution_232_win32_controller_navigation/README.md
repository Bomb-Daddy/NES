# Genghis Khan I — EXEC0232 WinMain controller navigation acceptance

Extends the proven EXEC0231 source and Windows 1280x720 WinMain GUI test. This source overlay creates a **bounded opt-in** \`--smoke-gamepad-nav\` mode. It exercises the **same native WinMain gamepad decoder, pad router and \`pad_actions\` application dispatcher** using a deterministic sequence of raw XInput button bits: neutral, A, release, RIGHT, release, A, release, B, release, A. Test mode alone injects this sequence; ordinary interactive launches still poll physical XInput devices and use the regular window style.

The 10-frame test checks exact title → campaign → Classic profile → 14-country selection → back → reenter progression and fail-closes if either country's baseline data changes, either scenario becomes falsely PLAYABLE, or the real GUI fails to paint. An absent 41-country source CSV returns 7; a wrong controller route returns 8; malformed --smoke-gamepad... options return 4. This is not a simulated original Spring, not a physical Xbox hardware test, and not a full campaign.

To reproduce, hydrate the exact 922-member EXEC0228 source via EXEC0229, apply EXEC0229 portability and EXEC0230 source patch, then EXEC0231 overlay, then:

\`\`\`sh
python research/Genghis_Khan_NES/execution_232_win32_controller_navigation/test_gamepad_smoke.py
python research/Genghis_Khan_NES/execution_232_win32_controller_navigation/apply_gamepad_smoke.py gk1-integrated-source
# Windows: build same gk1_native_shell WinMain, run gk1_native_shell.exe --smoke-gamepad-nav
# Confirm process exits 0; verify native and NSIS-installed executables separately.
\`\`\`

Classifications: VERIFIED_STATIC source and negative test; TESTED_PORT_WIN32 only after successful live Actions execution; historical stock WorkRAM fresh and SaveRAM archived unchanged; TESTED_PORT_LINUX previous runs only; NOT RUN physical Xbox, full campaigns or original generated first Spring. Do not place original ROM or private Mesen data on public GitHub.
