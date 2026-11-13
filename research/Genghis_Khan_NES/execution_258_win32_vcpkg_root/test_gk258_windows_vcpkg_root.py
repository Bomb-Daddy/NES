#!/usr/bin/env python3
"""EXEC0258: source-specific hostile validation of Windows vcpkg discovery.

A static test only, never a substitute for actual Win32 runner acceptance.
"""
from pathlib import Path
import sys


def audit(s):
    required=(
        '$env:VCPKG_INSTALLATION_ROOT',
        "'C:\\vcpkg'".replace('\\\\', '\\'),
        "Join-Path $candidate 'vcpkg.exe'",
        "Join-Path $candidate 'scripts/buildsystems/vcpkg.cmake'",
        "if (-not $root) { throw 'No verified vcpkg installation",
        "& (Join-Path $root 'vcpkg.exe') install zlib:x64-windows-static",
        '"VCPKG_ROOT=$root" >> $env:GITHUB_ENV',
        '-DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"',
        'git -C current-gk1 apply --check',
        'ctest --test-dir build-gk1-win32 -C Release --output-on-failure --timeout 90 -j 4',
        'test_gk257_win32_acceptance.ps1',
        'cpack --config build-gk1-win32/CPackConfig.cmake -G NSIS',
        'PHYSICAL_XBOX_NOT_RUN',
    )
    return all(x in s for x in required) and "if (-not $env:VCPKG_ROOT) { throw 'Windows runner did not supply VCPKG_ROOT' }" not in s


def main():
    if len(sys.argv)!=2: return 2
    s=Path(sys.argv[1]).read_text()
    assert audit(s)
    bads=[
       s.replace('$env:VCPKG_INSTALLATION_ROOT',''),
       s.replace('"VCPKG_ROOT=$root" >> $env:GITHUB_ENV',''),
       s.replace("Join-Path $candidate 'scripts/buildsystems/vcpkg.cmake'", "Join-Path $candidate 'nothing'") ,
       s.replace('zlib:x64-windows-static','zlib:x64-windows'),
       s.replace('ctest --test-dir build-gk1-win32 -C Release --output-on-failure --timeout 90 -j 4','echo fakePASS'),
       s.replace('PHYSICAL_XBOX_NOT_RUN','PHYSICAL_XBOX_TESTED'),
    ]
    assert all(not audit(b) for b in bads), 'a corrupted source passed acceptance'
    print('EXEC0258 7/7 positive+negative source controls PASSED; Windows runtime requires actual runner')
    return 0
if __name__=='__main__': raise SystemExit(main())
