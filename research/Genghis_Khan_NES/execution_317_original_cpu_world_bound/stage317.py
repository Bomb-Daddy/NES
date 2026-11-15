#!/usr/bin/env python3
"""EXEC0317: preserve original stock campaign provenance in current C11 app.
Apply ONLY after exact, SHA-verified EXEC0316 native source integration.
Never allow claimed CPU logs to grant first Spring / orders.
"""
import hashlib
import os
from pathlib import Path
import sys
import tempfile

HERE=Path(__file__).resolve().parent
PREV=HERE.parent/'execution_316_stock_cpu_trace_ingest'
if PREV.is_dir():sys.path.insert(0,str(PREV))
try:
 import stage316
except ImportError:
 # For a checkout running the source from the actual 317 GitHub directory.
 sys.path.insert(0,str(HERE.parent/'execution_316_stock_cpu_trace_ingest'))
 import stage316

ORIGINAL=stage316.WRAPPER
OLD=' return gk316_load_cpu_claims(private_trace,out);'
NEW=''' /* An authentic World transcript cannot be associated with Mongolia,
  * nor a Mongolia transcript with the World scenario. Until source ROM
  * correlation, both remain untrusted external CPU event CLAIMS. */
 if(!out) return GK316_ARGUMENT;
 Gk316Trace *candidate=malloc(sizeof *candidate);
 if(!candidate) return GK316_IO;
 Gk316Status result=gk316_load_cpu_claims(private_trace,candidate);
 if(result==GK316_OK && candidate->world!=app->active_world)
  result=GK316_STAGE;
 if(result==GK316_OK) *out=*candidate;
 free(candidate);
 return result;'''
if ORIGINAL.count(OLD)!=1:raise RuntimeError('EXEC0316 wrapper source unexpected')
CORRECTED=ORIGINAL.replace(OLD,NEW,1)
CMAKE_APPEND='''\n# EXEC0317: reject cross-campaign original stock CPU claim logs atomically.
add_executable(test_gk317_world_bound test_gk317_world_bound.c)
target_link_libraries(test_gk317_world_bound PRIVATE gk1_app_state)
add_test(NAME GK1_317_OriginalCpuClaimsAreWorldBoundWithoutOrders
 COMMAND test_gk317_world_bound
 ${CMAKE_CURRENT_SOURCE_DIR}/../execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv)
'''
SOURCE_SHA='2f751ea6c6367f57e6e6a34497cb05d37b9c37684cf88f29933ae93c621213e6'

def digest(x):return hashlib.sha256(x).hexdigest()
def norm(x):
 if b'\0' in x or b'\r\r\n' in x or b'\r' in x.replace(b'\r\n',b''):
  raise ValueError('invalid inherited source encoding')
 return x.replace(b'\r\n',b'\n')
def prepare(root):
 root=Path(root)
 app=norm((root/'gk1_app_state.c').read_bytes())
 cm=norm((root/'CMakeLists.txt').read_bytes())
 original=ORIGINAL.encode(); replacement=CORRECTED.encode()
 if app.count(original)!=1 or replacement in app:
  raise ValueError('exact EXEC0316 native wrapper missing or duplicate')
 if cm.count(stage316.CMAKE.strip().encode())!=1 or 'GK1_317_OriginalCpuClaims' in cm.decode():
  raise ValueError('exact EXEC0316 test registry missing or EXEC0317 duplicated')
 if (root/'test_gk317_world_bound.c').exists():raise ValueError('duplicate EXEC0317 C11 test')
 src=(HERE/'test_gk317_world_bound.c').read_bytes()
 if digest(src)!=SOURCE_SHA:raise ValueError('EXEC0317 reviewed C11 test source mismatch')
 return {'gk1_app_state.c':app.replace(original,replacement,1),
         'CMakeLists.txt':cm+CMAKE_APPEND.encode(),
         'test_gk317_world_bound.c':src}

def stage(root):
 root=Path(root); modified=prepare(root)
 originals={k:(root/k).read_bytes() if (root/k).exists() else None for k in modified}
 done=[]
 try:
  for name,content in modified.items():
   fd,temp=tempfile.mkstemp(prefix='.gk317-',dir=root)
   try:
    with os.fdopen(fd,'wb') as stream:
     stream.write(content);stream.flush();os.fsync(stream.fileno())
    os.replace(temp,root/name)
    done.append(name)
   finally:
    if os.path.exists(temp):os.unlink(temp)
 except BaseException:
  for name in reversed(done):
   if originals[name] is None:(root/name).unlink(missing_ok=True)
   else:(root/name).write_bytes(originals[name])
  raise
 return {k:digest(v) for k,v in modified.items()}
if __name__=='__main__':
 if len(sys.argv)!=2:sys.exit('Usage: stage317.py EXISTING_CURRENT_C11_GAME_DIR')
 try:
  for k,v in stage(sys.argv[1]).items():print('EXEC0317',k,v)
 except (ValueError,OSError) as e:sys.exit('EXEC0317 fail-closed: '+str(e))
