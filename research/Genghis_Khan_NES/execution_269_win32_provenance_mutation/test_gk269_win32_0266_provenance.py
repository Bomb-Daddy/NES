#!/usr/bin/env python3
"""EXEC0269: fail-closed current Windows source and hostile mutation QA.
Supersedes the flawed first-occurrence mutation test of EXEC0267 without
altering or rewriting its historical source. Static checking is not Win32 QA.
"""
from pathlib import Path
import sys

PINS = {
    'gk1_pad_router.c': '32fb1ac5f23520eeaf4a42aa1c6a147d9d2df9587b11f2f224604612d010ccbd',
    'gk1_pad_router.h': '481b9f39103721369e24214d2d6510d10f01ce7d7dc2b4d2021a81e43f9136ba',
    'test_gk266_controller_chord.c': 'c628c476fe003d1fe5cf4564ad074499152eafa29559daaeda3d61a145e497e8',
}
REQUIRED = (
    'execution_266_controller_atomic_chords',
    'gk268_normalize_pinned_checkout.py',
    'test_gk268_normalize_pinned_checkout.py',
    'EXEC0267 source input checksum mismatch',
    'Copy-Item -LiteralPath $from -Destination $to -Force',
    'Get-FileHash -LiteralPath $to -Algorithm SHA256',
    'add_executable(test_gk266_controller_chord test_gk266_controller_chord.c)',
    'target_link_libraries(test_gk266_controller_chord PRIVATE gk1_pad_router gk99_xinput_bridge)',
    'add_test(NAME GK1_266_Controller_Atomic_Commands COMMAND test_gk266_controller_chord)',
    'test_gk269_win32_0266_provenance.py',
    'cmake --build build-gk1-win32 --config Release --parallel 4',
    'ctest --test-dir build-gk1-win32 -C Release --output-on-failure',
    'PHYSICAL_XBOX_NOT_RUN', 'FULL_CAMPAIGNS_NOT_RUN',
    *PINS.keys(), *PINS.values(),
)


def audit(body: str) -> bool:
    return (all(s in body for s in REQUIRED)
            and 'PHYSICAL_XBOX_TESTED' not in body
            and 'FULL_CAMPAIGNS_TESTED' not in body
            and body.index('EXEC0267 source input checksum mismatch')
                < body.index('cmake --build build-gk1-win32 --config Release --parallel 4'))


def main(workflow: Path) -> int:
    body = workflow.read_text(encoding='utf-8')
    if not audit(body):
        print('EXEC0269 FAIL: source provenance missing or false claim', file=sys.stderr)
        return 1
    mutants = [
        *(body.replace(sha, '0'*64, 1) for sha in PINS.values()),
        body.replace('Copy-Item -LiteralPath $from -Destination $to -Force',
                     'Write-Host skipped', 1),
        body.replace('add_test(NAME GK1_266_Controller_Atomic_Commands',
                     'add_test(NAME GK1_266_Skipped', 1),
        body.replace('gk99_xinput_bridge', 'gk99_fake_bridge', 1),
        body.replace('Get-FileHash -LiteralPath $to -Algorithm SHA256',
                     'Write-Host fake-hash', 1),
        body.replace('gk268_normalize_pinned_checkout.py', 'gk268_unverified.py'),
        body.replace('test_gk268_normalize_pinned_checkout.py', 'test_fake.py'),
        body.replace('PHYSICAL_XBOX_NOT_RUN', 'PHYSICAL_XBOX_TESTED'),
        body.replace('FULL_CAMPAIGNS_NOT_RUN', 'FULL_CAMPAIGNS_TESTED'),
        body.replace('ctest --test-dir build-gk1-win32 -C Release --output-on-failure',
                     'Write-Host skipped', 1),
    ]
    if any(m == body or audit(m) for m in mutants):
        print('EXEC0269 FAIL: corrupted workflow passed hostile QA', file=sys.stderr)
        return 1
    print(f'EXEC0269 PASS {1+len(mutants)}/{1+len(mutants)} source/mutation cases; Win32/Xbox status requires separate execution')
    return 0


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit('usage: test_gk269_win32_0266_provenance.py WORKFLOW')
    sys.exit(main(Path(sys.argv[1])))
