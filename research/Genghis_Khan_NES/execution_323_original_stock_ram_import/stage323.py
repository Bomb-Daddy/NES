#!/usr/bin/env python3
"""EXEC0323 SHA-pinned atomic C11 stock-source-import stage into SAME native game.
The importer reads supplied authentic snapshots; it never generates RNG/orders.
"""
from pathlib import Path
import hashlib
import os
import sys
import tempfile

FILES = {
    'gk323_original_spring.c': 'd428b0c0b9e683076a6fd4e39f3bc7893c0919cc694d4c6315187a3305a034b1',
    'gk323_original_spring.h': 'fc2074ef04969e2f73cfb45421828844c7e440f0a7a5981aaf831fc04a05c657',
    'gk323_app_stock_bridge.c': '23b4616022c38b1604c31e5c768774ee099780be087bd7e531f5ed0f63c8fb7c',
    'gk323_app_stock_bridge.h': '3f3d5ed0477651e0030b7b81a4169ddb4b6bbecb64d19949b228a8409fa8d6ea',
    'test_gk323_original_spring.c': '256f556d2ca44f1e1d484e86e607e25a0800d8d842364c9202acd7b864cd944c',
    'test_gk323_app_bridge.c': '2a572727ade4e50e1a5e07c9cd67b5843329973e3cc17207d041f2d5072d97df',
}
TOKEN = b'add_library(gk1_app_state STATIC '
REGISTRATION = (b'\n# EXEC0323: original-stock Spring captured 83-byte country parser/hostile tests.\n'
 b'add_executable(test_gk323_original_spring test_gk323_original_spring.c)\n'
 b'target_link_libraries(test_gk323_original_spring PRIVATE gk1_app_state)\n'
 b'add_test(NAME GK1_323_OriginalSpring_AuthenticRecordDecode_BanksAndRollback COMMAND test_gk323_original_spring)\n'
 b'add_executable(test_gk323_app_bridge test_gk323_app_bridge.c)\n'
 b'target_link_libraries(test_gk323_app_bridge PRIVATE gk1_app_state)\n'
 b'add_test(NAME GK1_323_LiveGk1App_OriginalSpringReadOnlyBridge COMMAND test_gk323_app_bridge)\n')
class Refused(ValueError): pass

def normalize(raw):
 if not raw or b'\x00' in raw: raise Refused('empty or NUL source')
 data=raw.replace(b'\r\n',b'\n')
 if b'\r' in data: raise Refused('bad CRLF/CRCRLF')
 return data

def source_files(source):
 good={}
 for name,sha in FILES.items():
  raw=normalize((source/name).read_bytes())
  if hashlib.sha256(raw).hexdigest()!=sha:raise Refused('source SHA mismatch '+name)
  good[name]=raw
 return good

def changed_cmake(raw):
 t=normalize(raw)
 if t.count(TOKEN)!=1 or b'gk1_app_state.c' not in t or b'gk321_first_spring_phase.c' not in t:
  raise Refused('not inherited GK321 complete native game')
 if b'GK1_323_' in t or b'gk323_original_spring.c' in t:raise Refused('duplicate EXEC0323 stage')
 t=t.replace(TOKEN,TOKEN+b'gk323_original_spring.c gk323_app_stock_bridge.c ',1)
 return t+REGISTRATION

def install(source,app,inject=''):
 source,app=Path(source),Path(app)
 src=source_files(source)
 cmake=app/'CMakeLists.txt'
 old=cmake.read_bytes()
 new_cm=changed_cmake(old)
 for name in src:
  if (app/name).exists():raise Refused('existing target '+name)
 touched=[]
 try:
  for path,body in [(app/name,data) for name,data in src.items()]+[(cmake,new_cm)]:
   fd,p=tempfile.mkstemp(dir=app,prefix='.gk323-')
   try:
    with os.fdopen(fd,'wb') as f:f.write(body);f.flush();os.fsync(f.fileno())
    os.replace(p,path)
   finally:
    if os.path.exists(p):os.unlink(p)
   touched.append(path)
   if (inject=='after_first_copy' and len(touched)==1) or (inject=='after_cmake' and path==cmake):
    raise OSError('deliberate rollback injection')
  for name,data in src.items():
   if (app/name).read_bytes()!=data:raise Refused('post-copy mismatch '+name)
  if (cmake.read_bytes()!=new_cm):raise Refused('post-stage CMake mismatch')
 except Exception:
  for path in reversed(touched):
   if path==cmake:path.write_bytes(old)
   else:path.unlink(missing_ok=True)
  raise
 print('GK323 source+tests linked into inherited C11 Gk1App; read-only stock observations, no source-generated first Spring or playable orders')

if __name__=='__main__':
 if len(sys.argv)!=2:raise SystemExit('usage: stage323.py <inherited-native-app-directory>')
 install(Path(__file__).resolve().parent,sys.argv[1])