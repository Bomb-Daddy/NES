#!/usr/bin/env python3
"""EXEC0326: staged source-authentic 6502 writer address interpreter.

Integrates into the SAME native game library after EXEC0325, never an isolated
replacement or gameplay unlock. Strict content SHA and atomic rollback.
"""
from __future__ import annotations
import hashlib
import os
from pathlib import Path
import sys
import tempfile

SOURCE_NAMES = (
    "gk326_sha256.h", "gk326_sha256.c",
    "gk326_cpu_country_store.h", "gk326_cpu_country_store.c",
    "test_gk326_cpu_country_store.c",
)
TOKEN = b"add_library(gk1_app_state STATIC "
REGISTRATION = b"""
# EXEC0326 stock-ROM 6502 country-store effective-address interpreter.
# Only synthetic CPU-register assertions in public CI; NOT a real stock turn.
add_executable(test_gk326_cpu_country_store test_gk326_cpu_country_store.c)
target_link_libraries(test_gk326_cpu_country_store PRIVATE gk1_app_state)
add_test(NAME GK1_326_6502CountryStoreEffectiveAddressSourceBankAndRollback
         COMMAND test_gk326_cpu_country_store)
"""

class Refused(ValueError): pass

def canonical(raw: bytes) -> bytes:
    if not raw or b"\x00" in raw: raise Refused("empty or NUL source")
    out = raw.replace(b"\r\n", b"\n")
    if b"\r" in out: raise Refused("unsupported CR source")
    return out

def sha(raw: bytes) -> str: return hashlib.sha256(raw).hexdigest()

def pinned_sources(folder: Path) -> dict[str, bytes]:
    manifest = {}
    for line in canonical((folder / "SOURCE_SHA256.txt").read_bytes()).decode().splitlines():
        parts=line.split("  ")
        if len(parts)!=2 or parts[1] in manifest: raise Refused("bad/duplicate hash manifest")
        manifest[parts[1]]=parts[0]
    if set(manifest)!=set(SOURCE_NAMES): raise Refused("source list differs")
    result = {}
    for name in SOURCE_NAMES:
        raw=canonical((folder/name).read_bytes())
        if sha(raw)!=manifest[name]:raise Refused("source hash mismatch: "+name)
        result[name]=raw
    return result

def cmake_edit(original: bytes) -> bytes:
    c=canonical(original)
    if c.count(TOKEN)!=1 or b"gk325_opening_sequence.c" not in c or b"GK1_325_" not in c:
        raise Refused("not the current post-EXEC0325 complete game")
    if b"gk326_cpu_country_store.c" in c or b"GK1_326_" in c:
        raise Refused("already integrated EXEC0326")
    return c.replace(TOKEN,TOKEN+b"gk326_sha256.c gk326_cpu_country_store.c ",1)+REGISTRATION

def integrate(source: Path,game: Path,inject: str|None=None) -> int:
    source,game=Path(source),Path(game)
    files=pinned_sources(source)
    target=game/"CMakeLists.txt"
    old=target.read_bytes();new=cmake_edit(old)
    if any((game/n).exists() for n in files):raise Refused("source collision refused")
    changed=[]
    try:
        for path,raw in ([((game/n),v) for n,v in files.items()]+[(target,new)]):
            fd,tmp=tempfile.mkstemp(prefix=".gk326-",dir=game)
            try:
                with os.fdopen(fd,"wb") as f:
                    f.write(raw);f.flush();os.fsync(f.fileno())
                os.replace(tmp,path)
            finally:
                if os.path.exists(tmp):os.unlink(tmp)
            changed.append(path)
            if inject=="first" and len(changed)==1:raise OSError("injected source failure")
            if inject=="cmake" and path==target:raise OSError("injected commit failure")
        for name,raw in files.items():
            if (game/name).read_bytes()!=raw:raise Refused("postwrite source mismatch")
        if target.read_bytes()!=new:raise Refused("postwrite CMake mismatch")
    except Exception:
        for path in reversed(changed):
            if path==target:path.write_bytes(old)
            else:path.unlink(missing_ok=True)
        raise
    return len(files)

if __name__=="__main__":
    if len(sys.argv)!=2:raise SystemExit("usage: stage326.py <current-inherited-native-execution_199>")
    print("EXEC0326 staged into inherited app without ROM/first-Spring unlock,",integrate(Path(__file__).parent,Path(sys.argv[1])),"sources")
