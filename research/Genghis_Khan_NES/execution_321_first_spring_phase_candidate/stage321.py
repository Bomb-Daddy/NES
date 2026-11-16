#!/usr/bin/env python3
"""Fail-closed, SHA-pinned source stage into the inherited native GK1 game.
Only adds a bounded finance-phase candidate and a CTest; never changes setup
phase, authentic records, saves, or order budget.
"""
import hashlib
import os
from pathlib import Path
import sys
import tempfile

FILES = {
    'gk321_first_spring_phase.c': 'fed5092af3ba8c7087a2ea840131d17e226fa0bd4231cecff3270a1190c1bfe8',
    'gk321_first_spring_phase.h': 'c656fea886920e2f62a02fb55a29fea0d6a3b0c1583994db8bdc40c4ede4ed9c',
    'test_gk321_first_spring_phase.c': 'f9529da5ee9f284332ea6cea63d5d578956ceeb9927910704b6401a2bdf2cb34',
}
TOKEN = b'add_library(gk1_app_state STATIC '
SOURCE_NAME = b'gk321_first_spring_phase.c'
TEST_NAME = b'GK1_321_BoundedSpringPhaseArithmetic_RejectsUniversalSnapshot'
REGISTRATION = (b'\n# EXEC0321 bounded source-math phase model; NO first-Spring or order unlock.\n'
                b'add_executable(test_gk321_first_spring_phase test_gk321_first_spring_phase.c)\n'
                b'target_link_libraries(test_gk321_first_spring_phase PRIVATE gk1_app_state)\n'
                b'add_test(NAME GK1_321_BoundedSpringPhaseArithmetic_RejectsUniversalSnapshot\n'
                b' COMMAND test_gk321_first_spring_phase)\n')

class Refused(ValueError): pass


def normalize(raw):
    if not raw or b'\x00' in raw:
        raise Refused('zero, NUL or nonexistent source')
    t=raw.replace(b'\r\n',b'\n')
    if b'\r' in t:
        raise Refused('lone CR or CRCRLF')
    return t


def source_files(source):
    good={}
    for name,sha in FILES.items():
        b=normalize((source/name).read_bytes())
        if hashlib.sha256(b).hexdigest()!=sha:
            raise Refused('source SHA256 drift: '+name)
        good[name]=b
    return good


def changed_cmake(raw):
    # The CMake original source may arrive as CRLF; canonical LF is accepted
    # only where every CR belongs to one CRLF.
    t=normalize(raw)
    if t.count(TOKEN)!=1 or t.count(b'gk1_app_state.c')<1:
        raise Refused('missing exact existing native app library')
    if SOURCE_NAME in t or TEST_NAME in t:
        raise Refused('duplicate source/test; no partial re-staging')
    t=t.replace(TOKEN,TOKEN+SOURCE_NAME+b' ',1)
    return t+REGISTRATION


def install(src,app,inject=''):
    src,app=Path(src),Path(app)
    good=source_files(src)
    cmake=app/'CMakeLists.txt'
    originals={cmake:cmake.read_bytes()}
    update=changed_cmake(originals[cmake])
    for name in good:
        if (app/name).exists():
            raise Refused('refuse existing target '+name)
    new={app/name:body for name,body in good.items()}
    new[cmake]=update
    # Source copied first, CMake last. Reverting edits on failure is mandatory.
    touched=[]
    try:
        for path,body in new.items():
            fd,tmp=tempfile.mkstemp(dir=str(app),prefix='.gk321-')
            try:
                with os.fdopen(fd,'wb') as h:
                    h.write(body)
                    h.flush()
                    os.fsync(h.fileno())
                os.replace(tmp,path)
            finally:
                if os.path.exists(tmp): os.unlink(tmp)
            touched.append(path)
            if inject=='after_first_copy' and len(touched)==1:
                raise OSError('injected failure')
            if inject=='after_cmake' and path==cmake:
                raise OSError('injected failure')
        if not all((app/k).read_bytes()==v for k,v in good.items()):
            raise Refused('post-stage source changed')
        if TEST_NAME not in cmake.read_bytes() or SOURCE_NAME not in cmake.read_bytes():
            raise Refused('post-stage native CMake missing')
    except Exception:
        for path in reversed(touched):
            if path in originals:
                path.write_bytes(originals[path])
            else:
                path.unlink(missing_ok=True)
        raise
    print('GK321 PASS SHA-pinned three C11 files linked into SAME gk1_app_state native game + CTest; original first Spring unimplemented')


if __name__=='__main__':
    if len(sys.argv)!=2:
        raise SystemExit('usage: stage321.py <inherited-native-execution_199-directory>')
    install(Path(__file__).resolve().parent,sys.argv[1])
