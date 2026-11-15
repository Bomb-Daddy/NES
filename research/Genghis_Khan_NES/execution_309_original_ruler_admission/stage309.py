#!/usr/bin/env python3
"""EXEC0309: human-ruler admission in existing native Gk1App original Spring.
No ROM, RAM, captured original country bytes, images or source checkpoint in this patch.
Fail closed and atomically stage only against the exact inherited gk162 boundary.
"""
import hashlib
import os
import pathlib
import sys
import tempfile

BASE = pathlib.Path(__file__).resolve().parent
ORIGINAL_GK162_SHA = '8d29890d8eafbfd01dd3c045b3df16e7a504bf5d1c5a60cc3fc38926a6a0a0b3'
TEST_SHA = '1d8247a8186795085a2e1e14e299e2c8aa535f9475228912fcfb7bd16ddc2bde'
CMAKE_MARKER = 'GK1_309_StockHumanRulers_FailClosed14And27'

def digest(b): return hashlib.sha256(b).hexdigest()
def norm(b):
    if b'\r\r' in b or b'\r' in b.replace(b'\r\n',b''):
        raise ValueError('malformed CR or CRCRLF')
    return b.replace(b'\r\n',b'\n')
def once(s, old, new):
    if s.count(old) != 1: raise ValueError('source signature/unique anchor mismatch: '+old[:60])
    return s.replace(old,new,1)

ENTER = ''' unsigned w=profile==GK77_ARCHIVED_WORLD_SAVERAM?1u:0u;
 if(a->rules_profile>1u||!a->world_ready[w]||'''
ENTER_PATCH = ''' unsigned w=profile==GK77_ARCHIVED_WORLD_SAVERAM?1u:0u;
 /* EXEC0309: original-stock numeric country records are not evidence that
  * any country is a selectable HUMAN ruler. The NES allows only Temujin
  * for Mongol, four named national rulers for World. Check before source
  * loading; do not manufacture an original first Spring. */
 if(!gk53_setup_allowed_lord((uint8_t)w,player))
  return GK162_ENTRY_TEMPLATE;
 if(a->rules_profile>1u||!a->world_ready[w]||'''
COMMIT = ''' unsigned w=source->profile==GK77_ARCHIVED_WORLD_SAVERAM?1u:0u;
 if(a->rules_profile>1u||!a->world_ready[w]||'''
COMMIT_PATCH = ''' unsigned w=source->profile==GK77_ARCHIVED_WORLD_SAVERAM?1u:0u;
 /* Public copied-checkpoint path must not bypass human-ruler selection. */
 if(!gk53_setup_allowed_lord((uint8_t)w,source->player_index))
  return GK162_ENTRY_TEMPLATE;
 if(a->rules_profile>1u||!a->world_ready[w]||'''
CMAKE = '''
# EXEC0309 source-authenticated checkpoint HUMAN ruler admission only.
# NOT independent original-stock first-Spring initialization.
add_executable(test_gk309_original_player_gate test_gk309_original_player_gate.c)
target_link_libraries(test_gk309_original_player_gate PRIVATE gk1_app_state)
add_test(NAME GK1_309_StockHumanRulers_FailClosed14And27 COMMAND test_gk309_original_player_gate ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
'''

def prepare(app):
    app=pathlib.Path(app)
    source=norm((app/'gk162_original_entry.c').read_bytes())
    if digest(source)!=ORIGINAL_GK162_SHA:raise ValueError('unknown original gk162 source SHA')
    cmake=norm((app/'CMakeLists.txt').read_bytes())
    if CMAKE_MARKER in cmake.decode():raise ValueError('already staged')
    if 'add_test(NAME GK1_307_PhysicalXboxDiagnosticInputFSM_NoHardwareClaim' not in cmake.decode():
        raise ValueError('requires verified inherited EXEC0307+ Win32 application')
    test=norm((BASE/'test_gk309_original_player_gate.c').read_bytes())
    if digest(test)!=TEST_SHA:raise ValueError('published test altered')
    if (app/'test_gk309_original_player_gate.c').exists():raise ValueError('duplicate test source')
    amended=once(source.decode(),ENTER,ENTER_PATCH)
    amended=once(amended,COMMIT,COMMIT_PATCH)
    return {'gk162_original_entry.c':amended.encode(),
            'test_gk309_original_player_gate.c':test,
            'CMakeLists.txt':cmake+CMAKE.encode()}

def stage(app, fail_after=0):
    app=pathlib.Path(app)
    files=prepare(app)
    backup={name:(app/name).read_bytes() if (app/name).exists() else None for name in files}
    tmp=[]
    try:
        for name,data in files.items():
            fd,part=tempfile.mkstemp(prefix='.gk309-',dir=app)
            tmp.append(part)
            with os.fdopen(fd,'wb') as f:
                f.write(data);f.flush();os.fsync(f.fileno())
        for ix,(name,data) in enumerate(files.items(),1):
            os.replace(tmp.pop(0),app/name)
            if fail_after==ix:raise OSError('injected write failure')
        if any((app/name).read_bytes()!=data for name,data in files.items()):
            raise OSError('afterimage check failed')
    except BaseException:
        for name,data in backup.items():
            if data is None:(app/name).unlink(missing_ok=True)
            else:(app/name).write_bytes(data)
        raise
    finally:
        for part in tmp:
            try:os.unlink(part)
            except FileNotFoundError:pass
    return {name:digest(data) for name,data in files.items()}

if __name__=='__main__':
    if len(sys.argv)!=2:raise SystemExit('stage309.py LIVE_PREPARED_GK1_NATIVE_APP_PATH')
    try:
        for name,sha in stage(sys.argv[1]).items():print('EXEC0309 verified',name,sha)
    except (ValueError,OSError) as e:
        print('EXEC0309 fail closed: '+str(e),file=sys.stderr);sys.exit(1)
