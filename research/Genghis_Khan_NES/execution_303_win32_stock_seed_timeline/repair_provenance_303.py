#!/usr/bin/env python3
"""EXEC0303: advance stale EXEC0265 Win32 source provenance, without disabling it.

The old independent hostile test assumed EXEC0264's app SHA forever. The
complete C11 app has advanced, so we authenticate the original gate, the exact
post-EXEC0303 native source and test registry, then change only the one pinned
expected SHA. All original test cases and rejection branches stay intact.
"""
from pathlib import Path
import hashlib
import os
import sys
import tempfile

OLD_GATE_GIT_BLOB="3aee55fa2f5969815e4e1e63f3c844b6c8654962"
NEW_APP_SHA="1e18aa0bf086b762f93aef3720c083f4e427d9fb9e5cb212a4755a9cb56ad813"
NEW_HEADER_SHA="afbf999525593bec1857890d0b61905e16069b76cc0c58d1280656dae6302c97"
NEW_CMAKE_SHA="3d73c3a1d502bedbfbd62bb697175f8ea48bece325b6046dddbb6cda122c1191"
OLD_APP_SHA="a0edd0a5dc4974a17485320368c6613240785a2c813a941dcaef527fd632c05c"
OLD_TOKEN="EXPECTED_APP_SHA = '"+OLD_APP_SHA+"'"
NEW_TOKEN="EXPECTED_APP_SHA = '"+NEW_APP_SHA+"'"

def canonical(b):
    if b'\0' in b or b'\r' in b.replace(b'\r\n',b''):
        raise ValueError("unexpected original source encoding")
    return b.replace(b'\r\n',b'\n')
def sha(b):return hashlib.sha256(b).hexdigest()
def blob(b):return hashlib.sha1(b'blob '+str(len(b)).encode()+b'\0'+b).hexdigest()
def revised_gate(data):
    clean=canonical(data)
    if blob(clean)!=OLD_GATE_GIT_BLOB:
        raise ValueError("original 0265 hostile provenance test no longer matches published source")
    txt=clean.decode('utf-8')
    if txt.count(OLD_TOKEN)!=1 or NEW_TOKEN in txt:
        raise ValueError("unrecognized or already changed 0265 source gate")
    for required in ("EXPECTED_PREDECESSOR_SHA", "EXPECTED_TEST_SHA",
                     "for index,values in enumerate(mutations,1)", "def audit(",
                     "GK1_264_AtomicScenarioReinstallClearsSourceAndPortState"):
        if required not in txt:raise ValueError("old gate lost independent hostile control")
    updated=txt.replace(OLD_TOKEN,NEW_TOKEN,1).encode('utf-8')
    if updated.count(NEW_TOKEN.encode())!=1:
        raise ValueError("provenance expected-hash mutation failed")
    return updated
def run(appdir):
    root=Path(appdir)
    for n,expected in [('gk1_app_state.c',NEW_APP_SHA),
                       ('gk1_app_state.h',NEW_HEADER_SHA),
                       ('CMakeLists.txt',NEW_CMAKE_SHA)]:
        actual=sha(canonical((root/n).read_bytes()))
        if actual!=expected:raise ValueError("post-303 app provenance mismatch: "+n+" "+actual)
    src=root/'packaging'/'test_gk265_current_win32_source.py'
    before=src.read_bytes();updated=revised_gate(before)
    fd,tmp=tempfile.mkstemp(prefix='.gk303-265-',dir=src.parent)
    try:
        with os.fdopen(fd,'wb') as f:
            f.write(updated);f.flush();os.fsync(f.fileno())
        os.replace(tmp,src)
    finally:
        if os.path.exists(tmp):os.unlink(tmp)
    print("EXEC0303 native 0265 hostile source gate ADVANCED (not suppressed):",
          sha(updated),"; all ten old mutations retained")
    return sha(updated)
if __name__=='__main__':
    if len(sys.argv)!=2:sys.exit("Usage: repair_provenance_303.py path/to/complete/execution_199")
    try:run(sys.argv[1])
    except (OSError,ValueError,UnicodeError) as e:
        sys.exit("EXEC0303 OLD GATE RECONCILIATION REFUSED: "+str(e))
