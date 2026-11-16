#!/usr/bin/env python3
import tempfile
import shutil
from pathlib import Path
from stage325 import install,Refused,source_files,update_cmake,FILE_NAMES
ROOT=Path(__file__).resolve().parent
checks=0

def check(p,msg):
    global checks
    checks+=1
    assert p,msg

def fixture(home):
    p=home/"native";p.mkdir()
    contents=b"cmake_minimum_required(VERSION 3.16)\nadd_library(gk1_app_state STATIC gk1_app_state.c gk321_first_spring_phase.c gk323_app_stock_bridge.c)\n"
    (p/"CMakeLists.txt").write_bytes(contents)
    return p,contents

check(len(source_files(ROOT))==6,"source pins")
with tempfile.TemporaryDirectory() as d:
    base=Path(d);app,old=fixture(base)
    check(install(ROOT,app)==6,"stage source")
    new=(app/"CMakeLists.txt").read_bytes()
    check(new.count(b"GK1_325_SourceStockSetupCalendarFinanceEventAndOrders")==1,"test wired")
    check(b"add_library(gk1_app_state STATIC gk325_opening_sequence.c gk325_app_opening_bridge.c " in new,"library linked")
    check(all((app/n).read_bytes()==(ROOT/n).read_bytes() for n in FILE_NAMES),"copied byte exact")
    try:install(ROOT,app)
    except Refused:check(True,"duplicate refused")
    else:check(False,"duplicate accepted")
with tempfile.TemporaryDirectory() as d:
    app,old=fixture(Path(d))
    for inj in ("first","cmake"):
        try:install(ROOT,app,inj)
        except OSError:check(True,"injection raised")
        else:check(False,"injection accepted")
        check((app/"CMakeLists.txt").read_bytes()==old,"CMake rollback")
        check(not any((app/x).exists() for x in FILE_NAMES),"file rollback")
with tempfile.TemporaryDirectory() as d:
    app,old=fixture(Path(d))
    (app/"CMakeLists.txt").write_bytes(b"broken")
    try:install(ROOT,app)
    except Refused:check(True,"wrong inherited app refused")
    else:check(False,"wrong inherited app accepted")
with tempfile.TemporaryDirectory() as d:
    base=Path(d);app,old=fixture(base)
    copy=base/"src";shutil.copytree(ROOT,copy)
    bad=copy/FILE_NAMES[0];bad.write_bytes(bad.read_bytes()+b"changed")
    try:install(copy,app)
    except Refused:check(True,"hash pin refused")
    else:check(False,"tampering accepted")
    check((app/"CMakeLists.txt").read_bytes()==old,"tampering no change")
for broken in [b"",b"a\r\rb",b"a\x00b"]:
    try:update_cmake(broken)
    except Refused:check(True,"malformed input refused")
    else:check(False,"malformed accepted")
print("EXEC0325 hostile source integration PASS",checks)
