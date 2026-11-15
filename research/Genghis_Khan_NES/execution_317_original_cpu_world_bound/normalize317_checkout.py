#!/usr/bin/env python3
"""EXEC0317: SHA-pinned GitHub Windows checkout normalization ONLY for
reviewed EXEC0316 and EXEC0317 C11 sources; never trust arbitrary file names.
"""
from pathlib import Path
import hashlib
import os
import sys
import tempfile
PINS={
 'gk316_cpu_claims_ingest.c':'15d093cb07ecbd18ec95ff6c4f58f1fe46fece19259e45dad9c693623a8cfe3b',
 'gk316_cpu_claims_ingest.h':'104217c7f3d11dc7992766996b24f48150553ad47dfc15b3377bf01e8f0f4d8e',
 'test_gk316_cpu_claims.c':'e91a23234e1ff69d0992c85024f5b6f202d611363b5b0a022c0ecfb99e06cbc5',
 'test_gk316_controller_fuzz.c':'182ae7d066301217ab9bc71a9b4231016a965d827f72bbded17c27e20063e8c2',
 'test_gk317_world_bound.c':'2f751ea6c6367f57e6e6a34497cb05d37b9c37684cf88f29933ae93c621213e6'
}
def sha(b):return hashlib.sha256(b).hexdigest()
def canonical(raw):
 if b'\0' in raw or b'\r\r\n' in raw or b'\r' in raw.replace(b'\r\n',b''):
  raise ValueError('malformed binary, CRCRLF or mixed checkout')
 return raw.replace(b'\r\n',b'\n')
def normalize(path):
 path=Path(path)
 if path.name not in PINS:raise ValueError('unreviewed source filename')
 raw=path.read_bytes()
 clean=canonical(raw)
 if sha(clean)!=PINS[path.name]:raise ValueError('source SHA mismatch, refuse checkout')
 if raw!=clean:
  fd,tmp=tempfile.mkstemp(dir=path.parent,prefix='.gk317-normalize-')
  try:
   with os.fdopen(fd,'wb') as f:
    f.write(clean);f.flush();os.fsync(f.fileno())
   os.replace(tmp,path)
  finally:
   if os.path.exists(tmp):os.unlink(tmp)
 if path.read_bytes()!=clean:raise OSError('source normalization readback mismatch')
 return sha(clean)
if __name__=='__main__':
 if len(sys.argv)!=2:sys.exit('Usage: normalize317_checkout.py FILE_PATH')
 try:print('EXEC0317 SOURCE SHA PIN VERIFIED',normalize(sys.argv[1]))
 except (ValueError,OSError) as e:sys.exit('EXEC0317 SOURCE REJECT: '+str(e))
