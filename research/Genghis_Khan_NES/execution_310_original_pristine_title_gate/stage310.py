#!/usr/bin/env python3
"""EXEC0310 native original-stock fresh title source-compatibility fix.

Stage against EXACT complete inherited Gk1App AFTER EXEC0309; never an
independent replacement app. Reject any altered inherited source, malformed
line ending, duplicate stage, missing predecessor tests, or changed test.
The post-EXEC0277 game ALREADY contains the correct pristine-title guard.\nNever overwrite it; register an additional compiled regression in the inherited game.
"""
import hashlib
import os
from pathlib import Path
import sys
import tempfile

HERE = Path(__file__).resolve().parent
# The exact post-EXEC0309 Gk1App source receipt, independently published.
SOURCE_SHA = '04c5d36bb85462610f5728c8f8a22c82a007b123604c7dafcbfede5b6622803a'
TEST_SHA = 'e15f5e3ea94f253ec0fd17f93b0538c9ad4923e4ca5c5f1c978fe3c094f0a94d'
BEFORE = 'if(a->setup_stage[0]==GK53_SETUP_PLAYERS)return 1;'
AFTER = 'if(a->setup_stage[0]==GK53_SETUP_PLAYERS)\n  return a->active_world==0u && pristine_title(a,0u);'
MARKER = 'GK1_310_OriginalFreshTitleSetup_IsPristine'
TEST_NAME = 'test_gk310_original_pristine_title.c'
CMAKE_TAIL = '''\n# EXEC0310 original-stock source history: fresh title requires a PRISTINE wizard.
# Does not manufacture original NES startup RNG, three orders, or campaign.
add_executable(test_gk310_original_pristine_title test_gk310_original_pristine_title.c)
target_link_libraries(test_gk310_original_pristine_title PRIVATE gk1_app_state)
add_test(NAME GK1_310_OriginalFreshTitleSetup_IsPristine COMMAND test_gk310_original_pristine_title ${CMAKE_CURRENT_BINARY_DIR}/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
'''

def digest(b): return hashlib.sha256(b).hexdigest()

def normalize(raw):
    if b'\r\r\n' in raw or b'\r' in raw.replace(b'\r\n',b''):
        raise ValueError('malformed source newlines')
    return raw.replace(b'\r\n', b'\n')

def prepare(app):
    app = Path(app)
    c = normalize((app / 'gk162_original_entry.c').read_bytes())
    if digest(c) != SOURCE_SHA:
        raise ValueError('source is not exact EXEC0309 postimage')
    text = c.decode('utf-8')
    if text.count(AFTER) != 1 or text.count(BEFORE) != 0 or 'static int pristine_title(' not in text:
        raise ValueError('original title guard unique anchor absent')
    cmake = normalize((app/'CMakeLists.txt').read_bytes())
    if b'GK1_309_StockHumanRulers_FailClosed14And27' not in cmake:
        raise ValueError('requires integrated EXEC0309 native C11 game')
    if MARKER.encode() in cmake or (app/TEST_NAME).exists():
        raise ValueError('already applied or conflicting test')
    test = normalize((HERE/TEST_NAME).read_bytes())
    if digest(test) != TEST_SHA:
        raise ValueError('EXEC0310 exact published regression test changed')
    amended = text.encode()  # Post-EXEC0277/0309 code is already safe; NEVER re-patch it.
    return {'gk162_original_entry.c':amended,
            TEST_NAME:test,
            'CMakeLists.txt':cmake + CMAKE_TAIL.encode()}

def stage(app,fail_after=0):
    app=Path(app)
    payload=prepare(app)
    backup={name:(app/name).read_bytes() if (app/name).exists() else None for name in payload}
    staged=[]
    try:
        for name,content in payload.items():
            fd,path=tempfile.mkstemp(prefix='.gk310-',dir=app)
            staged.append(path)
            with os.fdopen(fd,'wb') as f:
                f.write(content); f.flush(); os.fsync(f.fileno())
        for i,name in enumerate(payload,1):
            os.replace(staged.pop(0),app/name)
            if fail_after==i: raise OSError('injected failure after file replacement')
        for name,content in payload.items():
            if (app/name).read_bytes()!=content:
                raise OSError('afterimage mismatch')
    except BaseException:
        for name,content in backup.items():
            if content is None:
                (app/name).unlink(missing_ok=True)
            else:
                (app/name).write_bytes(content)
        raise
    finally:
        for path in staged:
            try: os.unlink(path)
            except FileNotFoundError: pass
    return {name:digest(content) for name,content in payload.items()}

if __name__ == '__main__':
    if len(sys.argv)!=2: raise SystemExit('stage310.py /path/to/EXACT-EXEC0309-COMPLETE-GAME/execution_199')
    try:
        for name,sha in stage(sys.argv[1]).items():
            print('EXEC0310 staged',name,sha)
    except (OSError, ValueError) as e:
        print('EXEC0310 REFUSED:',e,file=sys.stderr)
        raise SystemExit(1)
