#!/usr/bin/env python3
"""EXEC0287: stage reviewed EXEC0286 campaign-bound C289 source into Win32.

Reconcile the complete inherited application, not a research-only fixture.
No original ROM, checkpoint, Mesen image, or licensed assets may be exported.
"""
from pathlib import Path
import hashlib
import json
import sys

HERE = Path(__file__).resolve().parent
PREV = HERE.parent / 'execution_286_c289_campaign_trace'
PATCH = PREV / 'GK1_EXEC0286_Source_And_Integration.patch'
MANIFEST = PREV / 'Source_Changed_SHA256.json'
CHANGED = ('gk282_stock_rng_trace.c', 'gk282_stock_rng_trace.h',
           'gk1_app_state.c', 'test_gk285_source_difficulty.c')
NEW_TEST = 'test_gk286_c289_scenario_binding.c'
TEST_NAME = 'GK1_286_C289HistoricalTraceMustMatchSelectedCampaign'
REGISTRATION = '''\n# EXEC0287 Win32 incorporation of independently tested EXEC0286 game source.
# An absent private ROM tests only fail-closed source admission, not parity.
add_executable(test_gk286_c289_scenario_binding test_gk286_c289_scenario_binding.c)
target_link_libraries(test_gk286_c289_scenario_binding PRIVATE gk1_app_state)
add_test(NAME GK1_286_C289HistoricalTraceMustMatchSelectedCampaign
 COMMAND test_gk286_c289_scenario_binding
 "${CMAKE_CURRENT_BINARY_DIR}/gk286_scenario_trace.tmp")
'''

def sha(b): return hashlib.sha256(b).hexdigest()

def hunks(patch_text):
    """Read reviewed unified diff; never substitute a guessed source transform."""
    blocks = {}
    current = None
    old = []
    new = []
    in_hunk = False
    def save():
        if in_hunk and current:
            blocks.setdefault(current, []).append(('\n'.join(old)+'\n', '\n'.join(new)+'\n'))
    for line in patch_text.splitlines():
        if line.startswith('diff --git '):
            save(); current = line.split(' b/execution_199/',1)[-1];old=[];new=[];in_hunk=False
        elif line.startswith('@@ '):
            save();old=[];new=[];in_hunk=True
        elif in_hunk and line and line[0] in ' +-':
            if line[0] in ' -': old.append(line[1:])
            if line[0] in ' +': new.append(line[1:])
        elif in_hunk and line == r'\ No newline at end of file':
            raise ValueError('Unexpected no-newline patch hunk')
    save()
    return blocks

def stage(app_dir):
    app = Path(app_dir)
    if not PATCH.is_file() or not MANIFEST.is_file():
        raise ValueError('Reviewed EXEC0286 public source or hash manifest missing')
    manifest = json.loads(MANIFEST.read_text(encoding='utf-8'))
    if sha(PATCH.read_bytes().replace(b'\r\n',b'\n')) != manifest['patch_sha256']:
        raise ValueError('EXEC0286 immutable reviewed patch SHA mismatch')
    entries = {Path(x['path']).name: x for x in manifest['changed']}
    if set(entries) != set((*CHANGED, NEW_TEST, 'CMakeLists.txt')):
        raise ValueError('Unexpected changed-source inventory')
    blocks = hunks(PATCH.read_text(encoding='utf-8'))
    if any(not blocks.get(name) for name in CHANGED):
        raise ValueError('Missing or duplicate reviewed source hunk')
    before = {}
    after = {}
    for name in CHANGED:
        b = (app/name).read_bytes().replace(b'\r\n',b'\n')
        if sha(b) != entries[name]['previous_sha256']:
            raise ValueError('Stale or corrupt inherited Win32 C11 source: '+name)
        s = b.decode('utf-8')
        for old,new in blocks[name]:
            if s.count(old)!=1 or not old or old==new:
                raise ValueError('Reviewed C11 patch hunk not uniquely applicable: '+name)
            s = s.replace(old,new)
        out = s.encode('utf-8')
        if sha(out)!=entries[name]['new_sha256']:
            raise ValueError('Reviewed source postimage SHA mismatch: '+name)
        before[name] = b
        after[name] = out
    newtest = (PREV/NEW_TEST).read_bytes().replace(b'\r\n',b'\n')
    if sha(newtest)!=entries[NEW_TEST]['new_sha256']:
        raise ValueError('New C11 test differs from reviewed GitHub source')
    if (app/NEW_TEST).exists():
        raise ValueError('Duplicate EXEC0286 native test file')
    cmake = app/'CMakeLists.txt'
    cm = cmake.read_bytes().replace(b'\r\n',b'\n').decode('utf-8')
    if (cm.count('GK1_285_SelectedDifficulty_OriginalC289_CausalAndAtomic') != 1 or
        cm.count('add_executable(test_gk285_source_difficulty') != 1 or
        cm.count('target_link_libraries(test_gk285_source_difficulty PRIVATE gk1_app_state') != 1 or
        TEST_NAME in cm or 'test_gk286_c289_scenario_binding' in cm):
        raise ValueError('Inherited CMake selected-difficulty gate absent/duplicate')
    gate = app/'packaging/test_gk265_current_win32_source.py'
    gate_text = gate.read_bytes().replace(b'\r\n',b'\n').decode('utf8')
    oldpin = "EXPECTED_APP_SHA = '"+entries['gk1_app_state.c']['previous_sha256']+"'"
    newpin = "EXPECTED_APP_SHA = '"+entries['gk1_app_state.c']['new_sha256']+"'"
    if gate_text.count(oldpin)!=1 or newpin in gate_text:
        raise ValueError('Legacy Win32 source-provenance hostile gate cannot be updated')
    # All prerequisites checked before any source mutation.
    for name in CHANGED: (app/name).write_bytes(after[name])
    (app/NEW_TEST).write_bytes(newtest)
    cmake.write_text(cm+REGISTRATION,encoding='utf-8',newline='\n')
    gate.write_text(gate_text.replace(oldpin,newpin),encoding='utf-8',newline='\n')
    for name in CHANGED:
        if sha((app/name).read_bytes())!=entries[name]['new_sha256']:
            raise ValueError('Post-write native source mismatch: '+name)
    print('EXEC0287 exact EXEC0286 native C11 patch, test and Win32 source gate staged; FIRST_SPRING_OPEN')

if __name__=='__main__':
    try:
        if len(sys.argv)!=2: raise ValueError('Usage: stage287.py <Win32 execution_199 directory>')
        stage(sys.argv[1])
    except (ValueError,OSError,UnicodeError) as exc:
        print('EXEC0287 FAIL CLOSED: '+str(exc),file=sys.stderr)
        sys.exit(2)