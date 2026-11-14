#!/usr/bin/env python3
"""EXEC0271 hostile static gate for the executed Win32 EXEC0270 integration.
No ROM, private RAM, screenshots or original checkpoint data are included.
"""
from pathlib import Path
import sys

REQUIRED = (
    'execution_270_classic_view_pass_navigation/GK1_EXEC0270_Integration_From_0269.patch',
    'execution_270_classic_view_pass_navigation/test_gk270_view_pass_navigation.c',
    'a0edd0a5dc4974a17485320368c6613240785a2c813a941dcaef527fd632c05c',
    'ee00ef0fc03676d09edfef9ca630acb9e605f128e5af5a66b1eafb2e86c2dc68',
    '490c9d476ffd272b5e0dcd507c19c241556df1b7d5aa4d417ae10ffb6e450f4c',
    'apply_gk271_win32_270.py',
    'python $integrator $app --self-test',
    'python $integrator $app',
    'GK1_270_ControllerViewNavigationDisarmsPendingPass',
    'add_executable(test_gk270_view_pass_navigation',
    'target_link_libraries(test_gk270_view_pass_navigation PRIVATE gk1_app_state)',
    'ctest --test-dir build-gk1-win32 -C Release',
    'NSIS',
    'test_gk257_win32_acceptance.ps1',
    'windows-2022',
)

def validate(source):
    missing = [x for x in REQUIRED if source.count(x) < 1]
    if missing:
        raise ValueError('missing/nonunique mandatory source gates: ' + ', '.join(missing))
    if source.index('EXEC0270 source-integrated into native Win32') > source.index('Compile actual current 1280x720'):
        raise ValueError('EXEC0270 source integration after native compilation')
    if source.index('EXEC0270 source-integrated into native Win32') < source.index('Reconcile actual Windows game with EXEC0264'):
        raise ValueError('EXEC0270 source integration before baseline source')
    return True

def hostile(source):
    assert validate(source)
    for n, marker in enumerate(REQUIRED):
        if marker == 'NSIS':
            continue
        mutant = source.replace(marker, 'MUTATED_REJECTED')
        try:
            validate(mutant)
        except (ValueError, AssertionError):
            continue
        raise AssertionError(f'hostile source mutation {n} survived')
    print('EXEC0271 mandatory Win32 source gates and hostile mutations PASS')

if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit('usage: test_gk271_win32_0270_provenance.py WORKFLOW.yml')
    hostile(Path(sys.argv[1]).read_text(encoding='utf-8'))
