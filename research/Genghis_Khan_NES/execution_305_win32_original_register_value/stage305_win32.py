#!/usr/bin/env python3
"""EXEC0305: SHA-pinned Win303 -> Win304 native C11 stage, preserving CMake and the old 0265 gate.

Windows CI must stage the exact existing complete inherited app, then compile and
run actual WinMain/NSIS. It is NOT a first-Spring gameplay implementation.
"""
from pathlib import Path
import hashlib
import os
import sys
import tempfile

PRE = {
    'gk1_app_state.c': '1e18aa0bf086b762f93aef3720c083f4e427d9fb9e5cb212a4755a9cb56ad813',
    'gk1_app_state.h': 'afbf999525593bec1857890d0b61905e16069b76cc0c58d1280656dae6302c97',
    'CMakeLists.txt': '3d73c3a1d502bedbfbd62bb697175f8ea48bece325b6046dddbb6cda122c1191',
}
SOURCES = {
    'gk304_stock_store_value.c': '315351065db80d9a4b466f4a1bd69ce452192f4988ecd5cf14d92a13ba1cf7e3',
    'gk304_stock_store_value.h': 'ee6603e159111e2376cad1b2a208d2dd43fae5b3e4bb59724ac1cc49bba15afa',
    'test_gk304_stock_store_value.c': 'c2ab30829b4e356138edf0dcfd4ffc24b5677279bc963544b8c43c76ff0af0df',
    'app304.c.inc': '17978a0a6367468f2352e533146a7e40b66a8b71240ec4d1516952d41d0de595',
    'app304.h.inc': '81f8c90d7bbb1220e08934659cb605d50855487208a7ecbabcd345cb892c4677',
}
OLD_GATE_SHA = PRE['gk1_app_state.c']
GATE_NAME = 'packaging/test_gk265_current_win32_source.py'
CMAKE_ADD = """
# EXEC0305 native Win32 integration of reviewed EXEC0304 source; public refusal test.
add_executable(test_gk304_stock_store_value test_gk304_stock_store_value.c)
target_link_libraries(test_gk304_stock_store_value PRIVATE gk1_app_state)
add_test(NAME GK1_304_PublicRegisterStoreRefusalNoROM
 COMMAND test_gk304_stock_store_value
 ${CMAKE_CURRENT_SOURCE_DIR}/../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
if(GK304_PRIVATE_ROM)
 add_test(NAME GK1_304_OriginalRomOpcodeRegisterAndAtomicRollback
 COMMAND test_gk304_stock_store_value
 ${CMAKE_CURRENT_SOURCE_DIR}/../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv
 ${GK304_PRIVATE_ROM})
endif()
"""

def sha(b): return hashlib.sha256(b).hexdigest()

def norm(data):
    if b'\0' in data or b'\r' in data.replace(b'\r\n', b''):
        raise ValueError('noncanonical source bytes')
    return data.replace(b'\r\n', b'\n')

def once(content, marker):
    if content.count(marker) != 1:
        raise ValueError('expected unique inherited anchor: '+marker[:96])

def prepare(root, source, expected=None):
    root,source=Path(root),Path(source)
    if not root.is_dir():raise ValueError('complete inherited native app missing')
    pin=PRE if expected is None else expected
    pre={}
    for name,expected_sha in pin.items():
        data=norm((root/name).read_bytes())
        if sha(data)!=expected_sha:
            raise ValueError('wrong Win303 inherited source / already integrated: '+name+' '+sha(data))
        pre[name]=data.decode('utf-8')
    additions={}
    for name,expected_sha in SOURCES.items():
        data=norm((source/name).read_bytes())
        if sha(data)!=expected_sha:raise ValueError('reviewed EXEC0304 source changed: '+name)
        if name.startswith('gk304_') or name.startswith('test_gk304_'):
            if (root/name).exists():raise ValueError('already integrated: '+name)
        additions[name]=data.decode('utf-8')
    c=pre['gk1_app_state.c']; h=pre['gk1_app_state.h']; cm=pre['CMakeLists.txt']
    # Exact PRE SHA-256 already authenticates this inherited C translation unit.
    # EXEC0303 reorganized the replay call; requiring one literal return line
    # wrongly rejects the actual native Win32 source despite a matching hash.
    once(h,'#include "gk302_stock_seed_timeline.h"\n')
    once(h,'Gk302Status gk1_app_original_seed_timeline(')
    once(cm,'gk302_stock_seed_timeline.c gk282_stock_rng_trace.c')
    once(cm,'GK1_302_PublicTimelineRefusalNoROM')
    once(cm,'GK1_257_CurrentWin32Workflow_RejectsStaleSourceAndFalsePass')
    if 'gk304_stock_store_value' in c+h+cm:raise ValueError('already integrated')
    if not c.endswith('\n'):raise ValueError('C app not newline terminated')
    c += additions['app304.c.inc']
    h=h.replace('#include "gk302_stock_seed_timeline.h"\n',
        '#include "gk302_stock_seed_timeline.h"\n#include "gk304_stock_store_value.h"\n',1)
    h=h.replace('Gk302Status gk1_app_original_seed_timeline(',
        additions['app304.h.inc']+'Gk302Status gk1_app_original_seed_timeline(',1)
    cm=cm.replace('gk302_stock_seed_timeline.c gk282_stock_rng_trace.c',
        'gk302_stock_seed_timeline.c gk304_stock_store_value.c gk282_stock_rng_trace.c',1)
    cm+=CMAKE_ADD
    new={**{'gk1_app_state.c':c,'gk1_app_state.h':h,'CMakeLists.txt':cm},
         **{key:additions[key] for key in list(SOURCES)[:3]}}
    gate=norm((root/GATE_NAME).read_bytes()).decode('utf-8')
    old_token="EXPECTED_APP_SHA = '"+OLD_GATE_SHA+"'"
    once(gate,old_token)
    for token in ('EXPECTED_PREDECESSOR_SHA','EXPECTED_TEST_SHA',
                  'def audit(', 'for index,values in enumerate(mutations,1)',
                  'GK1_264_AtomicScenarioReinstallClearsSourceAndPortState'):
        if token not in gate:raise ValueError('old Win32 provenance hostile gate disabled: '+token)
    new_sha=sha(c.encode('utf-8'))
    gate=gate.replace(old_token,"EXPECTED_APP_SHA = '"+new_sha+"'",1)
    new[GATE_NAME]=gate
    return {name:value.encode('utf-8') for name,value in new.items()}

def commit(root, changes):
    root=Path(root)
    old={name:((root/name).read_bytes() if (root/name).exists() else None) for name in changes}
    temps={}
    try:
        for name,content in changes.items():
            dest=root/name
            fd,filename=tempfile.mkstemp(prefix='.gk305-',dir=dest.parent)
            temps[name]=filename
            with os.fdopen(fd,'wb') as f:
                f.write(content);f.flush();os.fsync(f.fileno())
        for name,filename in temps.items():os.replace(filename,root/name)
    except BaseException:
        for name,content in old.items():
            if content is None:(root/name).unlink(missing_ok=True)
            else:(root/name).write_bytes(content)
        raise
    finally:
        for filename in temps.values():
            if os.path.exists(filename):os.unlink(filename)

def main(argv):
    if len(argv)!=2:
        print('Usage: stage305_win32.py full-native-execution_199-dir',file=sys.stderr)
        return 2
    try:
        source=Path(__file__).resolve().parent.parent/'execution_304_stock_store_register'
        if not source.is_dir():source=Path(__file__).resolve().parent
        changes=prepare(Path(argv[1]),source)
        commit(Path(argv[1]),changes)
        for name,data in changes.items():print('EXEC0305 STAGED',name,sha(data))
        return 0
    except (ValueError,OSError,UnicodeError) as e:
        print('EXEC0305 SOURCE REFUSED: '+str(e),file=sys.stderr)
        return 3

if __name__=='__main__':raise SystemExit(main(sys.argv))
