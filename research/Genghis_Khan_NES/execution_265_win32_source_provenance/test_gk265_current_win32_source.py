#!/usr/bin/env python3
"""Fail-closed executable source provenance gate for native Win32, EXEC0265.

Windows CI must test the same inherited campaign-restart C11 code and
linked regression as EXEC0264. Not a hardware Xbox or ROM-parity test.
"""
import hashlib
from pathlib import Path
import sys

EXPECTED_APP_SHA = 'a0edd0a5dc4974a17485320368c6613240785a2c813a941dcaef527fd632c05c'
EXPECTED_TEST_SHA = '6e62d9a2ba4317f0e6d3a23942203612861f501b64a7b90f18198c384cbd0d59'
EXPECTED_PREDECESSOR_SHA = '193a5c2f5d5c54524b8b70ad0d40992e8170a7a5d1d8a6de083bfa675fbb0926'

def sha(data: bytes):
    return hashlib.sha256(data).hexdigest()

def audit(app: bytes, test: bytes, cmake: str, workflow: str):
    if sha(app) != EXPECTED_APP_SHA or sha(test) != EXPECTED_TEST_SHA:
        return False
    if sha(app) == EXPECTED_PREDECESSOR_SHA:
        return False
    for token in (
        'add_executable(test_gk264_scenario_restart test_gk264_scenario_restart.c)',
        'target_link_libraries(test_gk264_scenario_restart PRIVATE gk1_app_state)',
        'GK1_264_AtomicScenarioReinstallClearsSourceAndPortState',
        'GK1_265_CurrentWin32BuildSourceMustMatchEXEC0264',
    ):
        if cmake.count(token) != 1:
            return False
    for token in (
        'GK1_EXEC0265_AtomicRestart_SourceOnly.patch',
        'git apply --directory=current-gk1 --check',
        'GK1_EXEC0265 current-source SHA mismatch',
        'GK1_264_AtomicScenarioReinstallClearsSourceAndPortState',
        'test_gk265_current_win32_source.py',
    ):
        if token not in workflow:
            return False
    if 'Skip the current source gate' in workflow:
        return False
    return True

def main(argv):
    if len(argv) != 5:
        print('Usage: test_gk265_current_win32_source.py app.c test264.c CMakeLists.txt workflow.yml',file=sys.stderr)
        return 2
    a,t,c,w = (Path(x) for x in argv[1:])
    app,test,cmake,workflow=a.read_bytes(),t.read_bytes(),c.read_text(encoding='utf-8'),w.read_text(encoding='utf-8')
    if not audit(app,test,cmake,workflow):
        print('EXEC0265 current Win32 inherited game SOURCE PROVENANCE FAIL',file=sys.stderr)
        return 1
    mutations=[
        (app.replace(b'fresh.rules_profile=s->rules_profile',b'fresh.rules_profile=1'),test,cmake,workflow),
        (app+b'\n',test,cmake,workflow),
        (app,test.replace(b'CK(s->screen==GK1_SCREEN_TITLE)',b'CK(1)'),cmake,workflow),
        (app,test+b'\n',cmake,workflow),
        (app,test,cmake.replace('GK1_264_AtomicScenarioReinstallClearsSourceAndPortState','GK1_264_SKIPPED'),workflow),
        (app,test,cmake.replace('target_link_libraries(test_gk264_scenario_restart PRIVATE gk1_app_state)',''),workflow),
        (app,test,cmake.replace('GK1_265_CurrentWin32BuildSourceMustMatchEXEC0264','GK1_265_SKIPPED'),workflow),
        (app,test,cmake,workflow.replace('GK1_EXEC0265_AtomicRestart_SourceOnly.patch','SKIP_SOURCE_PATCH')),
        (app,test,cmake,workflow.replace('GK1_EXEC0265 current-source SHA mismatch','ignore-mismatch')),
        (app,test,cmake,workflow.replace('test_gk265_current_win32_source.py','ignore-test')),
    ]
    for index,values in enumerate(mutations,1):
        assert not audit(*values),f'Mutation {index} evaded fail-closed source gate'
    print(f'EXEC0265 current Win32 EXACT EXEC0264 C11 source and regression provenance PASS 1+{len(mutations)} cases; HOSTILE_SHA and registry guarded; physical Xbox NOT RUN')
    return 0

if __name__=='__main__':raise SystemExit(main(sys.argv))
