#!/usr/bin/env python3
"""Hostile source-stage contract including exact rollback under injected error."""
from pathlib import Path
import tempfile
from stage321 import install,Refused,source_files,TEST_NAME,SOURCE_NAME

N=0
def check(x):
    global N
    N+=1
    if not x: raise AssertionError(f'check {N} failed')

source=Path(__file__).resolve().parent
base=b'cmake_minimum_required(VERSION 3.18)\nadd_library(gk1_app_state STATIC gk75_spring_economy.c gk1_app_state.c)\nenable_testing()\n'
with tempfile.TemporaryDirectory(prefix='gk321test') as tmp:
    app=Path(tmp)/'app';app.mkdir()
    cmake=app/'CMakeLists.txt'
    cmake.write_bytes(base)
    check(len(source_files(source))==3)
    for injected in ('after_first_copy','after_cmake'):
        try:install(source,app,injected)
        except OSError as ex:check('injected' in str(ex))
        else:raise AssertionError('injected staging failure was not raised')
        check(cmake.read_bytes()==base)
        check(not any(app.glob('*321*')))
    # Normalize legitimate Windows checkout CRLF, then compile-facing LF.
    cmake.write_bytes(base.replace(b'\n',b'\r\n'))
    install(source,app)
    check(cmake.read_bytes().count(b"add_library(gk1_app_state STATIC gk321_first_spring_phase.c ")==1)
    check(cmake.read_bytes().count(TEST_NAME)==1)
    check(all((app/x).read_bytes()==v for x,v in source_files(source).items()))
    try:install(source,app)
    except Refused:check(True)
    else:raise AssertionError('duplicate stage accepted')
    # Fail-closed before any file mutation on malicious CMake preimage.
    cmake.write_bytes(base+b'\x00')
    try:install(source,app)
    except Refused:check(True)
    else:raise AssertionError('NUL stage accepted')
    # Negative test for wrong repository source and no mutation.
    cmake.write_bytes(base)
    for name in source_files(source): (app/name).unlink()
    original=cmake.read_bytes()
    source_altered=Path(tmp)/'tampered';source_altered.mkdir()
    for name,body in source_files(source).items(): (source_altered/name).write_bytes(body)
    test=source_altered/'gk321_first_spring_phase.c'
    test.write_bytes(test.read_bytes()+b'\n/* bogus source drift */\n')
    try:install(source_altered,app)
    except Refused:check(True)
    else:raise AssertionError('tampered source accepted')
    check(cmake.read_bytes()==original)
    check(not any(app.glob('*321*')))
    cmake.write_bytes(base.replace(b'\n',b'\r\r\n'))
    try:install(source,app)
    except Refused:check(True)
    else:raise AssertionError('CRCRLF accepted')
    check(cmake.read_bytes()==base.replace(b'\n',b'\r\r\n'))
print(f'GK321 hostile source stage PASS {N} assertions, rollback exact, Windows CRLF, fake-source refusal')
