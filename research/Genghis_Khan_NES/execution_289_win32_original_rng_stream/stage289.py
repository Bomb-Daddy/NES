#!/usr/bin/env python3
"""Stage EXEC0288's reviewed real C11 original-stock RNG stream into the
inherited EXEC0287 Win32 game. Exact predecessor and postimage SHA checks.
No private ROM, Mesen states, RAM, licensed assets or user saves.
"""
import hashlib
import json
from pathlib import Path
import sys

HERE = Path(__file__).resolve().parent
UPSTREAM = HERE.parent / 'execution_288_original_rng_stream'
PATCH_FILE = UPSTREAM / 'GK1_EXEC0288_Implementation.patch'
MANIFEST_FILE = UPSTREAM / 'GK1_EXEC0288_Source_Changed_SHA256.json'
PINNED_PATCH_SHA = '6819b4c1c8741be9c611b0c35dfb1096f7117b5046eb7caa6b3ecb1a05e94d88'
PINNED_MANIFEST_SHA = 'c67f461f4f5898924dc1dd8c3b218637cc0290171070b61ae9d61c3008ee454e'
APP_PATCHED = ('gk1_app_state.c', 'gk1_app_state.h')
ADDED = ('gk288_original_rng_stream.c', 'gk288_original_rng_stream.h',
         'test_gk288_original_rng_stream.c')
CTEST = 'GK1_288_SourceOriginalC289MultiDrawNativeSetupAndRollback'
REGISTRATION = '''\n# EXEC0289 Win32: inherited EXEC0288 original C289 multi-step source regression.
# No private ROM input needed for fail-atomic and stage tests.
add_executable(test_gk288_original_rng_stream test_gk288_original_rng_stream.c)
target_link_libraries(test_gk288_original_rng_stream PRIVATE gk1_app_state)
add_test(NAME GK1_288_SourceOriginalC289MultiDrawNativeSetupAndRollback
 COMMAND test_gk288_original_rng_stream
 ${CMAKE_CURRENT_SOURCE_DIR}/../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
'''

def sha(data):
    return hashlib.sha256(data).hexdigest()

def lf(data):
    return data.replace(b'\r\n', b'\n')

def c11_hunks(text):
    """Parse the exact reviewed unified patch; do not guess a source edit."""
    current = None
    in_hunk = False
    old, new, result = [], [], {}
    def save():
        if in_hunk and current:
            result.setdefault(current, []).append(('\n'.join(old)+'\n', '\n'.join(new)+'\n'))
    for line in text.splitlines():
        if line.startswith('--- a/') or line.startswith('--- /dev/null'):
            save()
            current, old, new, in_hunk = None, [], [], False
        elif line.startswith('+++ b/execution_199/'):
            save()
            current = line[len('+++ b/execution_199/'):]
            old, new, in_hunk = [], [], False
        elif line.startswith('@@ '):
            save()
            old, new, in_hunk = [], [], True
        elif in_hunk and line.startswith((' ', '+', '-')):
            if line[0] in ' -': old.append(line[1:])
            if line[0] in ' +': new.append(line[1:])
        elif line == r'\ No newline at end of file':
            raise ValueError('Unreviewed no-newline patch refused')
    save()
    return result

def stage(app_dir):
    app = Path(app_dir)
    if not app.is_dir(): raise ValueError('Inherited Win32 game directory missing')
    patch_raw = lf(PATCH_FILE.read_bytes())
    manifest_raw = lf(MANIFEST_FILE.read_bytes())
    if sha(patch_raw) != PINNED_PATCH_SHA or sha(manifest_raw) != PINNED_MANIFEST_SHA:
        raise ValueError('Reviewed EXEC0288 source evidence SHA mismatch')
    manifest = json.loads(manifest_raw)
    if manifest.get('execution') != 'EXEC0288':
        raise ValueError('Wrong source execution')
    entries = {Path(e['path']).name: e for e in manifest['source_changes']}
    if len(entries) != 6 or set(entries) != set((*APP_PATCHED,*ADDED,'CMakeLists.txt')):
        raise ValueError('Reviewed change inventory differs')
    patch_hunks = c11_hunks(patch_raw.decode('utf-8'))
    pending = {}
    for name in APP_PATCHED:
        data = lf((app/name).read_bytes())
        if sha(data) != entries[name]['old_sha256']:
            raise ValueError('Win32 C11 predecessor does not match reviewed source: '+name)
        text = data.decode('utf-8')
        hunks = patch_hunks.get(name, [])
        if not hunks or len(hunks) != (2 if name.endswith('.h') else 1):
            raise ValueError('Reviewed C11 hunk count refused: '+name)
        for old, new in hunks:
            if not old.strip() or old == new or text.count(old) != 1:
                raise ValueError('Nonunique or corrupt reviewed C11 hunk: '+name)
            text = text.replace(old,new,1)
        written = text.encode('utf-8')
        if sha(written) != entries[name]['new_sha256']:
            raise ValueError('Exact reviewed C11 postimage hash mismatch: '+name)
        pending[name] = written
    for name in ADDED:
        if (app/name).exists():
            raise ValueError('Duplicate executable original RNG source: '+name)
        data = lf((UPSTREAM/name).read_bytes())
        if entries[name]['old_sha256'] is not None or sha(data) != entries[name]['new_sha256']:
            raise ValueError('Wrong original-source file bytes: '+name)
        pending[name] = data
    cm_path = app/'CMakeLists.txt'
    cm = lf(cm_path.read_bytes()).decode('utf-8')
    oldlib = 'gk280_original_rate_bridge.c gk282_stock_rng_trace.c'
    newlib = 'gk280_original_rate_bridge.c gk288_original_rng_stream.c gk282_stock_rng_trace.c'
    if cm.count(oldlib) != 1 or newlib in cm or cm.count(CTEST) != 0:
        raise ValueError('Original CMake source library is stale or duplicated')
    if (cm.count('GK1_286_C289HistoricalTraceMustMatchSelectedCampaign') != 1 or
        cm.count('target_link_libraries(test_gk286_c289_scenario_binding PRIVATE gk1_app_state)') != 1):
        raise ValueError('Required complete EXEC0287 Win32 predecessor test missing')
    pending['CMakeLists.txt'] = (cm.replace(oldlib,newlib,1)+REGISTRATION).encode('utf-8')
    gate_path = app/'packaging/test_gk265_current_win32_source.py'
    gate = lf(gate_path.read_bytes()).decode('utf-8')
    prior = "EXPECTED_APP_SHA = '"+entries['gk1_app_state.c']['old_sha256']+"'"
    after = "EXPECTED_APP_SHA = '"+entries['gk1_app_state.c']['new_sha256']+"'"
    if gate.count(prior)!=1 or after in gate:
        raise ValueError('Inherited hostile source gate missing or unrecognized')
    pending['packaging/test_gk265_current_win32_source.py'] = gate.replace(prior,after,1).encode('utf-8')
    # All inputs and full changed-source postimages validated before mutation.
    for name, payload in pending.items():
        (app/name).write_bytes(payload)
    for name in (*APP_PATCHED,*ADDED):
        if sha((app/name).read_bytes()) != entries[name]['new_sha256']:
            raise ValueError('Written source diverged from reviewed bytes: '+name)
    print('EXEC0289 VERIFIED_STATIC: reviewed EXEC0288 C11 stream integrated in inherited Win32 app; native CTest registered; FIRST_SPRING_OPEN')

if __name__ == '__main__':
    try:
        if len(sys.argv)!=2: raise ValueError('Usage: stage289.py <inherited Win32 execution_199>')
        stage(sys.argv[1])
    except (OSError, UnicodeError, KeyError, ValueError) as err:
        print('EXEC0289 FAIL CLOSED: '+str(err),file=sys.stderr)
        sys.exit(2)
