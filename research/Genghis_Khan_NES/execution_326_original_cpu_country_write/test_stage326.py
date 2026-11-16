#!/usr/bin/env python3
"""Executable hostile staging and rollback checks; no copyrighted ROM data."""
import shutil
from pathlib import Path
import tempfile
from stage326 import integrate,pinned_sources,cmake_edit,Refused,SOURCE_NAMES

count=0
def verify(ok,msg):
    global count
    count+=1
    if not ok:raise AssertionError(f"{msg} ({count})")

ROOT=Path(__file__).resolve().parent
FILES=pinned_sources(ROOT)
with tempfile.TemporaryDirectory() as d:
    root=Path(d);src=root/"source";app=root/"app"
    src.mkdir();app.mkdir()
    for file in (*SOURCE_NAMES,"SOURCE_SHA256.txt","stage326.py"):
        shutil.copyfile(ROOT/file,src/file)
    parent=b"add_library(gk1_app_state STATIC gk325_opening_sequence.c gk325_app_opening_bridge.c gk323_original_spring.c)\nadd_test(NAME GK1_325_ActualGk1AppClassicWorldPreSetupRollback COMMAND foo)\n"
    (app/"CMakeLists.txt").write_bytes(parent)
    verify(integrate(src,app)==5,"exact source stage")
    verify(b"GK1_326_" in (app/"CMakeLists.txt").read_bytes(),"real native test registered")
    verify((app/"gk326_cpu_country_store.c").read_bytes()==FILES["gk326_cpu_country_store.c"],"readback")
    try:integrate(src,app)
    except Refused:pass
    else:verify(False,"duplicate must refuse")
    verify((app/"CMakeLists.txt").read_bytes()!=parent,"duplicate must retain first success")
    app2=root/"app2";app2.mkdir();(app2/"CMakeLists.txt").write_bytes(parent)
    try:integrate(src,app2,inject="first")
    except OSError:pass
    else:verify(False,"injected first failure must refuse")
    verify((app2/"CMakeLists.txt").read_bytes()==parent,"source failure original CMake retained")
    verify(not any((app2/n).exists() for n in SOURCE_NAMES),"source failure rollback")
    try:integrate(src,app2,inject="cmake")
    except OSError:pass
    else:verify(False,"injected final failure must refuse")
    verify((app2/"CMakeLists.txt").read_bytes()==parent,"CMake rollback")
    verify(not any((app2/n).exists() for n in SOURCE_NAMES),"all source rollback")
    (src/"gk326_sha256.c").write_bytes(FILES["gk326_sha256.c"]+b"\n// corrupted\n")
    try:integrate(src,app2)
    except Refused:pass
    else:verify(False,"altered C11 source must reject")
    verify(not any((app2/n).exists() for n in SOURCE_NAMES),"no mutation on source corruption")
    (src/"gk326_sha256.c").write_bytes(FILES["gk326_sha256.c"].replace(b"\n",b"\r\n"))
    verify(integrate(src,app2)==5,"Windows CRLF normalized exact trusted LF")
    verify((app2/"gk326_sha256.c").read_bytes()==FILES["gk326_sha256.c"],"normalized C11 byte-for-byte")
    app3=root/"app3";app3.mkdir();(app3/"CMakeLists.txt").write_bytes(parent.replace(b"GK1_325_",b"GK1_324_"))
    try:integrate(src,app3)
    except Refused:pass
    else:verify(False,"old native app refused")
    verify((app3/"CMakeLists.txt").read_bytes()==parent.replace(b"GK1_325_",b"GK1_324_"),"old app immutable")
    app4=root/"app4";app4.mkdir();(app4/"CMakeLists.txt").write_bytes(parent+b"gk326_cpu_country_store.c\n")
    try:integrate(src,app4)
    except Refused:pass
    else:verify(False,"partial duplicate refused")
    verify(not any((app4/n).exists() for n in SOURCE_NAMES),"duplicate rollback untouched")
    (src/"gk326_sha256.c").write_bytes(FILES["gk326_sha256.c"].replace(b"\n",b"\r\r\n"))
    try:integrate(src,app3)
    except Refused:pass
    else:verify(False,"malformed CRCRLF refused")
    verify(not any((app3/n).exists() for n in SOURCE_NAMES),"malformed no mutation")
print("PASS EXEC0326 hostile source-integrity/CRLF/rollback tests:",count)
