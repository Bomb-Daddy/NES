#!/usr/bin/env python3
"""EXEC0314: retain inherited hostile Windows source gate after one C11 fix.

Runs AFTER EXEC0313 source/gate and AFTER exact stage314 staging.
Never weakens or removes the inherited ten mutation controls.
"""
import hashlib
import os
from pathlib import Path
import sys
import tempfile
import stage314

PRE313_APP_SHA = 'a9772cda18258580dc46d47a3b77934d25a5cdc940d88130d42ee5694023abdf'
PRE313_CMAKE_SHA = '972b1d9f4877f51ef6859bacc19859100dc4c3f1f9e5af28bce2f895196d6312'
PRE313_GATE_SHA = '7586b22e7ac5ddec09207e3725ce0248b4dea54ebebd729e178613650e85e988'
BEFORE_313 = '2922886bf38ff441bdf7d629ae47455734363e213992d45ff4dcba68c5ebe71e'
AFTER_313 = PRE313_APP_SHA
OLD_LITERAL = "EXPECTED_APP_SHA = '" + BEFORE_313 + "'"
CURRENT_LITERAL = "EXPECTED_APP_SHA = '" + AFTER_313 + "'"
REQUIRED = (
    'EXPECTED_TEST_SHA', 'EXPECTED_PREDECESSOR_SHA',
    'GK1_264_AtomicScenarioReinstallClearsSourceAndPortState',
    'GK1_265_CurrentWin32BuildSourceMustMatchEXEC0264',
    'for index,values in enumerate(mutations,1)', 'def audit(',
    'GK1_EXEC0265_AtomicRestart_SourceOnly.patch',
)

def sha(raw): return hashlib.sha256(raw).hexdigest()

def normalized(raw):
    if b'\x00' in raw or b'\r\r\n' in raw or b'\r' in raw.replace(b'\r\n',b''):
        raise ValueError('forbidden source encoding/newline')
    return raw.replace(b'\r\n',b'\n')

def authenticate_patched_source(root):
    root=Path(root)
    raw=normalized((root/'gk1_app_state.c').read_bytes())
    before=stage314.SOURCE_ANCHOR.encode()
    after=stage314.SOURCE_PATCH.encode()
    if raw.count(after)!=1 or raw.count(before)!=0:
        raise ValueError('EXEC0314 code missing, duplicate or source drift')
    if sha(raw.replace(after,before,1))!=PRE313_APP_SHA:
        raise ValueError('C11 app predecessor SHA mismatch')
    c=normalized((root/'CMakeLists.txt').read_bytes())
    block=stage314.CMAKE_APPEND.encode()
    if not c.endswith(block) or sha(c[:-len(block)])!=PRE313_CMAKE_SHA:
        raise ValueError('CMake afterimage predecessor mismatch')
    test=(root/'test_gk314_replay_source_baseline.c').read_bytes()
    if test!=Path(stage314.__file__).with_name('test_gk314_replay_source_baseline.c').read_bytes():
        raise ValueError('C11 hostile regression test missing/changed')
    return sha(raw)

def update_gate(raw, next_source_sha, predecessor_gate_sha=PRE313_GATE_SHA):
    raw=normalized(raw)
    if len(next_source_sha)!=64 or any(x not in '0123456789abcdef' for x in next_source_sha):
        raise ValueError('invalid next C11 SHA')
    if raw.count(CURRENT_LITERAL.encode())!=1 or OLD_LITERAL.encode() in raw:
        raise ValueError('inherited EXEC0313 source gate literal missing/duplicated')
    reverted=raw.replace(CURRENT_LITERAL.encode(),OLD_LITERAL.encode(),1)
    if sha(reverted)!=predecessor_gate_sha:
        raise ValueError('inherited hostile gate predecessor SHA mismatch')
    text=raw.decode('utf-8')
    for token in REQUIRED:
        if token not in text: raise ValueError('lost inherited hostile check: '+token)
    new_literal="EXPECTED_APP_SHA = '"+next_source_sha+"'"
    if new_literal==CURRENT_LITERAL: raise ValueError('no actual native change')
    return raw.replace(CURRENT_LITERAL.encode(),new_literal.encode(),1)

def repair(root):
    root=Path(root)
    new_sha=authenticate_patched_source(root)
    gate=root/'packaging'/'test_gk265_current_win32_source.py'
    prior=gate.read_bytes()
    updated=update_gate(prior,new_sha)
    fd,tmp=tempfile.mkstemp(dir=str(gate.parent),prefix='.gk314-gate-')
    try:
        with os.fdopen(fd,'wb') as out:
            out.write(updated);out.flush();os.fsync(out.fileno())
        os.replace(tmp,gate)
        if gate.read_bytes()!=updated:raise OSError('hostile gate readback failure')
    except BaseException:
        gate.write_bytes(prior)
        raise
    finally:
        if os.path.exists(tmp): os.unlink(tmp)
    print('EXEC0314 afterimage authenticated; all old hostile controls retained',new_sha,sha(updated))

if __name__=='__main__':
    if len(sys.argv)!=2: sys.exit('Usage: repair314_provenance.py COMPLETE_STAGED_APP_DIR')
    try:repair(sys.argv[1])
    except (ValueError,OSError,UnicodeError) as e:sys.exit('EXEC0314 fail-closed '+str(e))
