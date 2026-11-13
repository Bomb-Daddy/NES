#!/usr/bin/env python3
"""EXEC0244: exact-predecessor, atomic Win32 /MT CRT source repair.
Usage: python3 apply244.py ROOT    # ROOT contains execution_199/
No private source or licensed assets are handled by this patch.
"""
from pathlib import Path
import hashlib,sys
PRE='2bda9246ca69ba5b5ff36ad5a3f8f4091625b7482d91404bdbae1c4310ce2ff2'
POST='b288693a8e83056d2e35295792bf9dd487c6e6d0286308b697013b029bc6bdd2'
OLD=b'cmake_minimum_required(VERSION 3.15)\nproject(GK1NativeShell199 C)\nfind_package(ZLIB QUIET)\n'
NEW=b'''cmake_minimum_required(VERSION 3.15)
# Keep native MSVC CRT matching the approved static vcpkg zlib triplet.
# Set CMP0091 before project(), then set target defaults before any add_*.
if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif()
project(GK1NativeShell199 C)
if(MSVC AND GK1_REQUIRE_ARCHIVED_WORLD_IMPORT AND
   VCPKG_TARGET_TRIPLET STREQUAL "x64-windows-static")
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
find_package(ZLIB QUIET)
'''
def sha(b):return hashlib.sha256(b).hexdigest()
def apply(root):
    p=Path(root)/'execution_199'/'CMakeLists.txt'
    raw=p.read_bytes()
    if sha(raw)!=PRE: raise ValueError('wrong predecessor/duplicate application: refused before write; actual sha='+sha(raw))
    if raw.count(OLD)!=1:raise ValueError('CMake source context mismatch')
    after=raw.replace(OLD,NEW,1)
    if sha(after)!=POST:raise ValueError('unexpected postimage')
    p.write_bytes(after)
    if sha(p.read_bytes())!=POST:raise ValueError('write verification failed')
    return p
if __name__=='__main__':
    if len(sys.argv)!=2:sys.exit('Usage: apply244.py ROOT')
    try:print('EXEC0244 APPLIED:',apply(sys.argv[1]))
    except (ValueError,OSError) as e:print('EXEC0244 REFUSED:',e,file=sys.stderr);sys.exit(1)
