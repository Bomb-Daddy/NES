#!/usr/bin/env python3
"""Hostile, executable static gate for the version-pinned *current* Win32 CI.

This verifies that the Windows workflow doesn't silently build the old
EXEC0228 application, skip the WinMain GUI, disable archived-World zlib,
substitute unverified source, or describe software XInput as physical Xbox.
Does not claim a native Windows run from Linux.
"""
import pathlib
import re
import sys

ARCHIVE_SHA = '97421aa2ef45288684bf22d868ad110b850396c316bce9253b0fc8a05166b619'


def audit(workflow: str, script: str):
    required = [
        'execution_257_current_win32_gate/GK1_EXEC0256_Lean_Inherited_Build_Source.zip',
        ARCHIVE_SHA,
        'git -C current-gk1 apply --check',
        'git -C current-gk1 apply "../$patch"',
        'cmake -S current-gk1/execution_199',
        'GK1_REQUIRE_ARCHIVED_WORLD_IMPORT=ON',
        'zlib:x64-windows-static',
        'ctest --test-dir build-gk1-win32 -C Release --output-on-failure --timeout 90 -j 4',
        'test_gk257_win32_acceptance.ps1',
        'cpack --config build-gk1-win32/CPackConfig.cmake -G NSIS',
        "Join-Path $target 'gk1_native_shell.exe'",
        'PHYSICAL_XBOX_NOT_RUN',
    ]
    if any(item not in workflow for item in required):
        return False
    if 'cmake -S research/Genghis_Khan_NES/execution_228_win32_build_gate/' in workflow:
        return False
    if re.search(r'ctest[^\n]*\s-R\s', workflow):
        return False  # no narrow historical subset masquerading as full acceptance
    scripted = ['--smoke-frames=15', '--smoke-gamepad-nav',
                '--smoke-frames=121', '--smoke-frames=4294967297',
                'WM_PAINT completed', 'Invoke-NativeExit', 'hardware Xbox NOT RUN']
    if any(item not in script for item in scripted):
        return False
    if script.count('Invoke-NativeExit') < 5:
        return False
    return True


def main(argv):
    if len(argv) != 3:
        print('Usage: test_gk257_windows_ci_gate.py WORKFLOW PS1', file=sys.stderr)
        return 2
    w = pathlib.Path(argv[1]).read_text(encoding='utf-8')
    s = pathlib.Path(argv[2]).read_text(encoding='utf-8')
    assert audit(w, s), 'current-source Win32 acceptance gate invalid'
    count = 1
    for bad in (
        w.replace(ARCHIVE_SHA, '0' * 64),
        w.replace('current-gk1/execution_199', 'execution_228_win32_build_gate/execution_199'),
        w.replace('-DGK1_REQUIRE_ARCHIVED_WORLD_IMPORT=ON', '-DGK1_REQUIRE_ARCHIVED_WORLD_IMPORT=OFF'),
        w.replace('zlib:x64-windows-static', 'zlib:x64-windows'),
        w.replace('ctest --test-dir build-gk1-win32 -C Release --output-on-failure --timeout 90 -j 4',
                  'ctest --test-dir build-gk1-win32 -C Release --output-on-failure -R GK1_0228'),
        w.replace('cpack --config build-gk1-win32/CPackConfig.cmake -G NSIS', 'echo dummy-package'),
        w.replace('PHYSICAL_XBOX_NOT_RUN', 'PHYSICAL_XBOX_TESTED'),
    ):
        assert not audit(bad, s), 'workflow corruption passed hostile gate'
        count += 1
    for bad in (
        s.replace('--smoke-gamepad-nav', '--version'),
        s.replace('WM_PAINT completed', 'NO_RENDER'),
        s.replace('hardware Xbox NOT RUN', 'hardware Xbox TESTED'),
    ):
        assert not audit(w, bad), 'acceptance script corruption passed hostile gate'
        count += 1
    print(f'GK1_EXEC0257 Windows source/CI gate PASS {count}/11 positive+negative cases; '
          'Windows executable / NSIS / physical Xbox NOT RUN by this test')
    return 0


if __name__ == '__main__':
    raise SystemExit(main(sys.argv))
