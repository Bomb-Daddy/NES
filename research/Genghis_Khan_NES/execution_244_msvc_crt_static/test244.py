#!/usr/bin/env python3
"""Self-contained exact source, rollback, Windows/static-zlib CRT contract test."""
from pathlib import Path
from zipfile import ZipFile
from tempfile import TemporaryDirectory
import hashlib, subprocess, sys
from apply244 import apply, PRE, POST

def check(x,msg):
    if not x:raise AssertionError(msg)

def main(zip_path):
 with ZipFile(zip_path) as z:original=z.read('source/execution_199/CMakeLists.txt')
 check(hashlib.sha256(original).hexdigest()==PRE,'EXEC0243 exact predecessor')
 with TemporaryDirectory() as d:
  root=Path(d);p=root/'execution_199'/'CMakeLists.txt';p.parent.mkdir()
  p.write_bytes(original)
  apply(root);got=p.read_bytes()
  check(hashlib.sha256(got).hexdigest()==POST,'positive exact postimage')
  check(b'cmake_policy(SET CMP0091 NEW)' in got.split(b'project(GK1NativeShell199 C)',1)[0], 'policy ordering')
  check(b'if(MSVC AND GK1_REQUIRE_ARCHIVED_WORLD_IMPORT AND\n   VCPKG_TARGET_TRIPLET STREQUAL "x64-windows-static")' in got, 'scoped /MT')
  check(b'set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")' in got,'right CRT')
  for label,mutant in [('different',original+b'!'),('wrong_context',original.replace(b'project(',b'PROJECT('))]:
   p.write_bytes(mutant)
   try:apply(root)
   except ValueError:pass
   else:raise AssertionError('bad '+label+' accepted')
   check(p.read_bytes()==mutant,label+' rollback')
  p.write_bytes(got)
  try:apply(root)
  except ValueError:pass
  else:raise AssertionError('duplicate accepted')
  check(p.read_bytes()==got,'duplicate rollback')
  # Windows CRLF must be accepted only with byte-identical LF-normalized
  # source; the exact physical CRLF representation must be retained on write.
  crlf=original.replace(b'\n',b'\r\n')
  p.write_bytes(crlf)
  apply(root)
  expected=got.replace(b'\n',b'\r\n')
  check(p.read_bytes()==expected,'complete CRLF postimage')
  mixed=original.replace(b'\n',b'\r\n',1)
  p.write_bytes(mixed)
  try:apply(root)
  except ValueError:pass
  else:raise AssertionError('mixed newline accepted')
  check(p.read_bytes()==mixed,'mixed newline rollback')
  # Evaluate same conditional with CMake itself, not a Python reimplementation.
  start=got.index(b'if(MSVC AND GK1_REQUIRE_ARCHIVED_WORLD_IMPORT AND')
  end=got.index(b'find_package(ZLIB QUIET)',start)
  block=got[start:end].decode()
  for ms,req,trip,expect in [
      ('TRUE','ON','x64-windows-static',True),
      ('TRUE','OFF','x64-windows-static',False),
      ('TRUE','ON','x64-windows',False),
      ('FALSE','ON','x64-windows-static',False)]:
   f=root/'crt.cmake'
   f.write_text('set(MSVC '+ms+')\nset(GK1_REQUIRE_ARCHIVED_WORLD_IMPORT '+req+')\n'+
     'set(VCPKG_TARGET_TRIPLET '+trip+')\n'+block+'\n'+
     'if(DEFINED CMAKE_MSVC_RUNTIME_LIBRARY)\n  message(STATUS "CRT=${CMAKE_MSVC_RUNTIME_LIBRARY}")\nelse()\n  message(STATUS "CRT=UNSET")\nendif()\n')
   result=subprocess.run(['cmake','-P',str(f)],capture_output=True,text=True,timeout=15)
   check(result.returncode==0,'CMake script: '+result.stderr)
   actual=result.stdout+result.stderr
   check(('CRT=MultiThreaded$<$<CONFIG:Debug>:Debug>' in actual)==expect,('CRT conditional',ms,req,trip,actual))
 print('EXEC0244 16/16 source/rollback/conditional acceptance groups PASS')
if __name__=='__main__':main(sys.argv[1])
