#!/usr/bin/env python3
"""Hostile static gate: Windows CI must compile EXEC0266 controller source.
This proves source selection only; it is not hardware or Windows test evidence.
"""
import pathlib
import sys

PINS = {
    'gk1_pad_router.c': '32fb1ac5f23520eeaf4a42aa1c6a147d9d2df9587b11f2f224604612d010ccbd',
    'gk1_pad_router.h': '481b9f39103721369e24214d2d6510d10f01ce7d7dc2b4d2021a81e43f9136ba',
    'test_gk266_controller_chord.c': 'c628c476fe003d1fe5cf4564ad074499152eafa29559daaeda3d61a145e497e8',
}

def audit(workflow):
    required = [
        'execution_266_controller_atomic_chords',
        'EXEC0267 source input checksum mismatch',
        'Copy-Item -LiteralPath $from -Destination $to -Force',
        'Get-FileHash -LiteralPath $to -Algorithm SHA256',
        'test_gk266_controller_chord PRIVATE gk1_pad_router gk99_xinput_bridge',
        'GK1_266_Controller_Atomic_Commands',
        'test_gk267_win32_0266_provenance.py',
        'cmake --build build-gk1-win32 --config Release --parallel 4',
        'ctest --test-dir build-gk1-win32 -C Release --output-on-failure',
        'PHYSICAL_XBOX_NOT_RUN',
        'FULL_CAMPAIGNS_NOT_RUN',
    ]
    required += list(PINS) + list(PINS.values())
    if not all(t in workflow for t in required):
        return False
    if workflow.index('EXEC0267 source input checksum mismatch') > workflow.index('cmake --build build-gk1-win32'):
        return False
    return True

def main(argv):
    if len(argv) != 2:
        print('usage: test_gk267_win32_0266_provenance.py WORKFLOW', file=sys.stderr)
        return 2
    body = pathlib.Path(argv[1]).read_text(encoding='utf-8')
    if not audit(body):
        print('EXEC0267 FAIL: Win32 workflow is not source-verified for EXEC0266', file=sys.stderr)
        return 1
    count = 1
    for bad in (
        *(body.replace(sha, '0'*64, 1) for sha in PINS.values()),
        body.replace('Copy-Item -LiteralPath $from -Destination $to -Force', 'Write-Host skipped', 1),
        body.replace('GK1_266_Controller_Atomic_Commands', 'GK1_266_Skipped', 1),
        body.replace('gk99_xinput_bridge', 'gk99_fake_bridge', 1),
        body.replace('Get-FileHash -LiteralPath $to -Algorithm SHA256', 'Write-Host fake-hash', 1),
        body.replace('PHYSICAL_XBOX_NOT_RUN', 'PHYSICAL_XBOX_TESTED', 1),
        body.replace('FULL_CAMPAIGNS_NOT_RUN', 'FULL_CAMPAIGNS_TESTED', 1),
    ):
        if audit(bad):
            print('EXEC0267 FAIL: corrupted Windows source workflow passed', file=sys.stderr)
            return 1
        count += 1
    print(f'EXEC0267 PASS {count}/{count} Windows source/static mutation controls; Windows and physical Xbox not run here')
    return 0

if __name__ == '__main__':
    raise SystemExit(main(sys.argv))
