#!/usr/bin/env python3
"""Stage new, copyright-clean C11 original-stock merchant offer RNG into current
Gk1App native source. Requires inherited COMPLETE app; no overwrite/conflict,
no private ROM, zero changes if any source or marker is already incomplete.
"""
from __future__ import annotations
import hashlib
from pathlib import Path
import os
import sys

HERE = Path(__file__).resolve().parent
SOURCES = (
 'gk331_stock_offer_rng.c', 'gk331_stock_offer_rng.h',
 'gk331_app_offer_bridge.c', 'gk331_app_offer_bridge.h',
 'test_gk331_public.c',
)
REQUIRED = (
 'CMakeLists.txt', 'gk1_app_state.c', 'gk1_app_state.h',
 'gk178_source_rng.c', 'gk178_source_rng.h',
 'gk82_stock_cartridge.c', 'gk82_stock_cartridge.h',
 'gk1_source_identity.c', 'gk77_stock_spring.c', 'gk61_market.c',
)
MARKER = '# EXEC0331 exact-USA-stock Trade source RNG offer masks'
APPEND = '\n' + MARKER + '''
target_sources(gk1_app_state PRIVATE gk331_stock_offer_rng.c gk331_app_offer_bridge.c)
add_executable(test_gk331_public test_gk331_public.c)
target_link_libraries(test_gk331_public PRIVATE gk1_app_state)
add_test(NAME GK1_331_OriginalStockTradeOfferSourceGateNoFabricatedMerchant COMMAND test_gk331_public)
'''

def stage(target: Path) -> bool:
 target = target.resolve()
 if not target.is_dir():
  raise ValueError('missing inherited native application')
 for name in REQUIRED:
  path=target/name
  if not path.is_file() or not path.stat().st_size:
   raise ValueError('missing nonempty inherited source: '+name)
 source = {name:(HERE/name).read_bytes() for name in SOURCES}
 if any(not raw or b'\r' in raw for raw in source.values()):
  raise ValueError('invalid empty / CR source')
 for name, raw in source.items():
  digest = hashlib.sha256(raw).hexdigest()
  if len(digest)!=64:
   raise ValueError('invalid SHA')
 existing={}
 missing=[]
 for name,raw in source.items():
  dest=target/name
  if dest.exists():
   if not dest.is_file() or dest.read_bytes().replace(b'\r\n',b'\n')!=raw:
    raise ValueError('conflicting source '+name)
   existing[name]=raw
  else:
   missing.append(name)
 cm=target/'CMakeLists.txt'
 old=cm.read_bytes()
 if b'add_library(gk1_app_state' not in old:
  raise ValueError('wrong inherited CMake application')
 decoded=old.decode('utf-8')
 if MARKER in decoded:
  if APPEND.strip() not in decoded or missing:
   raise ValueError('partial/corrupt preexisting CMake integration')
  return False
 if 'test_gk331_public' in decoded:
  raise ValueError('marker missing for preexisting new test')
 created=[]
 pending=target/'.gk331_cmake_pending'
 try:
  for name in missing:
   path=target/name
   with path.open('xb') as f:f.write(source[name])
   created.append(path)
  with pending.open('xb') as f:f.write(old+APPEND.encode('utf-8'))
  os.replace(pending,cm)
  return True
 except BaseException:
  for path in created:path.unlink(missing_ok=True)
  pending.unlink(missing_ok=True)
  raise

if __name__=='__main__':
 if len(sys.argv)!=2:
  print('usage: stage331.py /path/to/latest/native/app',file=sys.stderr)
  raise SystemExit(2)
 try:
  changed=stage(Path(sys.argv[1]))
 except (OSError,ValueError,UnicodeError) as exc:
  print('EXEC0331 REFUSED: '+str(exc),file=sys.stderr)
  raise SystemExit(1)
 print('EXEC0331 source staged' if changed else 'EXEC0331 already staged, identical')
