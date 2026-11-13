#!/usr/bin/env python3
"""EXEC0255: compiled CTest X11 resource/Win32 registration portability gate.

This checks the actual generated CTest registry, not merely text comments.
Do not interpret it as a Win32 native test, physical gamepad test or ROM run.
"""
import json
import pathlib
import re
import subprocess
import sys

GUI = (
    'GK1_202_LinuxX11_SmokeFlagsRemainReachable',
    'GK1_217_Linux1280x720_ClassicControllerFinanceActualRendered',
    'GK1_219_Linux1280x720_ControllerSourceVersusLaborDraftOverlayRendered',
    'GK1_220_Linux1280x720_ControllerLaborUndoActualX11',
    'GK1_222_Linux1280x720_ControllerEnhancedSpringThreeOrders',
    'GK1_224_Linux1280x720_ControllerXSaveYLoadAndExclusiveFile',
    'GK1_225_TwoProcessTitleControllerColdLoad',
    'GK1_226_FourSeparateX11_ControllerTitleYSafeUpdate',
)


def validate(source, registry):
    # CMake's intended Win32 guard must enclose both Python/Xvfb scripts.
    for name in GUI[-2:]:
        guard = r'if\(UNIX AND NOT APPLE AND Python3_Interpreter_FOUND AND GK224_XVFB_RUN\)'
        pattern = guard + r'\s*add_test\(NAME ' + re.escape(name) + r'\b'
        if not re.search(pattern, source):
            return False, f'unguarded X11 test on Win32: {name}'
    found = {t['name']: t for t in registry['tests']}
    for name in GUI:
        if name not in found:
            return False, f'missing Linux X11 acceptance test {name}'
        props = {p['name']: p['value'] for p in found[name].get('properties', [])}
        if props.get('RESOURCE_LOCK') != ['GK1_EXCLUSIVE_XVFB_GUI']:
            return False, f'Xvfb shared resource not locked: {name}'
        if props.get('RUN_SERIAL') is not True or props.get('TIMEOUT') != 30.0:
            return False, f'unbounded or concurrently scheduled GUI test: {name}'
    if 'GK1_226_Enhanced_CrashSafeRecheckpoint_SourceRollback_14_27' not in found:
        return False, 'existing linked C11 source regression removed'
    return True, f'{len(GUI)} locked Xvfb tests; Win32-only registration avoided'


def main():
    if len(sys.argv) != 4:
        return 2
    cmakefile = pathlib.Path(sys.argv[1])
    cmake = cmakefile.read_text(encoding='utf-8')
    run = subprocess.run([sys.argv[2], '--test-dir', sys.argv[3],
                          '--show-only=json-v1'], text=True,
                         stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                         timeout=20, check=True)
    registry = json.loads(run.stdout)
    ok, reason = validate(cmake, registry)
    if not ok:
        print('FAIL', reason)
        return 1
    # Hostile QA: deliberately remove a resource lock, serial property and
    # one Win32 conditional; every mutation must be independently rejected.
    for name, mutated in (
        ('unguarded-windows', cmake.replace('if(UNIX AND NOT APPLE AND Python3_Interpreter_FOUND AND GK224_XVFB_RUN)\n add_test(NAME GK1_225', 'add_test(NAME GK1_225', 1)),
    ):
        rejected, _ = validate(mutated, registry)
        if rejected:
            print('FAIL hostile mutation survived:', name)
            return 1
    modified = json.loads(json.dumps(registry))
    next(t for t in modified['tests'] if t['name'] == GUI[-1])['properties'] = []
    rejected, _ = validate(cmake, modified)
    if rejected:
        print('FAIL corrupted generated CTest registration survived')
        return 1
    modified = json.loads(json.dumps(registry))
    t = next(t for t in modified['tests'] if t['name'] == GUI[-1])
    next(p for p in t['properties'] if p['name'] == 'RUN_SERIAL')['value'] = False
    rejected, _ = validate(cmake, modified)
    if rejected:
        print('FAIL concurrent X11 registration survived')
        return 1
    print('PASS EXEC0255', reason, '; 3 deliberate hostile mutations rejected')
    return 0

if __name__ == '__main__':
    raise SystemExit(main())
