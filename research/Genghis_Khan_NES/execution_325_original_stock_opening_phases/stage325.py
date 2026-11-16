#!/usr/bin/env python3
"""SHA-verified, rollback-safe integration of EXEC0325 into inherited GK1 C11.
Post-EXEC0323 stage only. Never makes PRE_SETUP playable or copies ROM bytes.
"""
from pathlib import Path
import hashlib
import os
import sys
import tempfile

FILE_NAMES = (
    "gk325_opening_sequence.h", "gk325_opening_sequence.c",
    "gk325_app_opening_bridge.h", "gk325_app_opening_bridge.c",
    "test_gk325_opening_sequence.c", "test_gk325_app_opening_bridge.c",
)
REGISTRATION = b"""
# EXEC0325 authenticated original-stock opening subphase ordering (NOT game unlock).
add_executable(test_gk325_opening_sequence test_gk325_opening_sequence.c)
target_link_libraries(test_gk325_opening_sequence PRIVATE gk1_app_state)
add_test(NAME GK1_325_SourceStockSetupCalendarFinanceEventAndOrders COMMAND test_gk325_opening_sequence)
add_executable(test_gk325_app_opening_bridge test_gk325_app_opening_bridge.c)
target_link_libraries(test_gk325_app_opening_bridge PRIVATE gk1_app_state)
add_test(NAME GK1_325_ActualGk1AppClassicWorldPreSetupRollback COMMAND test_gk325_app_opening_bridge)
"""
TOKEN=b"add_library(gk1_app_state STATIC "
class Refused(ValueError):
    pass

def normalize(raw):
    if not raw or b"\x00" in raw: raise Refused("empty or NUL")
    canonical=raw.replace(b"\r\n",b"\n")
    if b"\r" in canonical: raise Refused("malformed line endings")
    return canonical

def sha(data):return hashlib.sha256(data).hexdigest()

def source_files(folder):
    folder=Path(folder)
    manifest={}
    for line in (folder/"SOURCE_SHA256.txt").read_text().splitlines():
        digest,name=line.split("  ",1)
        if name in manifest:raise Refused("duplicate manifest")
        manifest[name]=digest
    if set(manifest)!=set(FILE_NAMES):raise Refused("missing/excess sources")
    bodies={}
    for name in FILE_NAMES:
        raw=normalize((folder/name).read_bytes())
        if sha(raw)!=manifest[name]:raise Refused("source SHA mismatch: "+name)
        bodies[name]=raw
    return bodies

def update_cmake(raw):
    before=normalize(raw)
    if before.count(TOKEN)!=1 or b"gk323_app_stock_bridge.c" not in before or b"gk321_first_spring_phase.c" not in before:
        raise Refused("not the inherited post-EXEC0323 native application")
    if b"gk325_opening_sequence.c" in before or b"GK1_325_" in before:
        raise Refused("duplicate EXEC0325 stage")
    return before.replace(TOKEN,TOKEN+b"gk325_opening_sequence.c gk325_app_opening_bridge.c ",1)+REGISTRATION

def install(source,app,inject=None):
    source,app=Path(source),Path(app)
    files=source_files(source)
    cmake=app/"CMakeLists.txt"
    original=cmake.read_bytes()
    revised=update_cmake(original)
    for name in files:
        if (app/name).exists():raise Refused("overwrite forbidden: "+name)
    changed=[]
    try:
        for target,body in ([(app/name,data) for name,data in files.items()]+[(cmake,revised)]):
            fd,tmp=tempfile.mkstemp(prefix=".gk325-",dir=app)
            try:
                with os.fdopen(fd,"wb") as f:
                    f.write(body);f.flush();os.fsync(f.fileno())
                os.replace(tmp,target)
            finally:
                if os.path.exists(tmp):os.unlink(tmp)
            changed.append(target)
            if inject=="first" and len(changed)==1:raise OSError("injected rollback")
            if inject=="cmake" and target==cmake:raise OSError("injected rollback")
        for name,expected in files.items():
            if (app/name).read_bytes()!=expected:raise Refused("post-write mismatch")
        if cmake.read_bytes()!=revised:raise Refused("CMake post-write mismatch")
    except Exception:
        for target in reversed(changed):
            if target==cmake:target.write_bytes(original)
            else:target.unlink(missing_ok=True)
        raise
    return len(files)

if __name__=="__main__":
    if len(sys.argv)!=2:raise SystemExit("usage: stage325.py <existing-inherited-game-directory>")
    print("EXEC0325 source and two C11 native CTests staged; NOT a gameplay unlock; files:",install(Path(__file__).parent,sys.argv[1]))
