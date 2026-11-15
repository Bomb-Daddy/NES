#!/usr/bin/env python3
"""EXEC0299: fix the actual EXEC0298 MSVC LNK2019, without changing gameplay.

The Win32 stage linked new gk296_stock_seed_origin.c to *old* EXEC0294
 gk294_seed_writer_attest.c.  The missing symbol is
 gk294_attest_seed_writers_masked.  Apply only the reviewed EXEC0296
 contract change, verifying exact old/new source hashes.
"""
import hashlib
import os
import sys
import tempfile
from pathlib import Path

BEFORE = {
 'gk294_seed_writer_attest.c': '905e8cf5c9b4867a3995b4cd3655c61ce95ecbd5e9f41d4baca14bd39d6eed71',
 'gk294_seed_writer_attest.h': '33d5ccad8f5314a344f5a64a68fb59f12e70c38195b32a3c0b6684d3d781c479',
}
AFTER = {
 'gk294_seed_writer_attest.c': 'cf38c32f5b3358fbeab4a2a84e4f1936b32c797d8497efc1bee9c9311708f964',
 'gk294_seed_writer_attest.h': '2642bf11bc03b3112427f54749ea60eb5508bea9d21490aa9c6531ef4dc5c2cc',
}
APP_HASH = '79ed178dec0a345886932bda2f887645c427b4b12e721438d937b7f97dbcf581'
HEADER_OLD = '''Gk294Status gk294_attest_seed_writers(const char *private_rom,
 const Gk294Write *claimed, size_t count, Gk294SourceProof *out);'''
HEADER_NEW = '''/* EXEC0296: authenticate exactly the supplied source-write target mask.
 * 0x0f is the genuine observed fresh-boot four-writer set; bytes $88/$89
 * need separate RAM-origin evidence. No substitution for full 0x3f proof. */
Gk294Status gk294_attest_seed_writers_masked(const char *private_rom,
 const Gk294Write *claimed, size_t count, uint8_t required_mask,
 Gk294SourceProof *out);
''' + HEADER_OLD
C_OLD = '''Gk294Status gk294_attest_seed_writers(const char *rom,
 const Gk294Write *writes,size_t count,Gk294SourceProof *out){
 if(!rom||!*rom||!writes||!out||count==0||count>GK294_MAX_WRITES)'''
C_NEW = '''Gk294Status gk294_attest_seed_writers_masked(const char *rom,
 const Gk294Write *writes,size_t count,uint8_t required_mask,
 Gk294SourceProof *out){
 if(!rom||!*rom||!writes||!out||count==0||count>GK294_MAX_WRITES||
    required_mask==0u||(required_mask&0xc0u)!=0u)'''
TAIL = '''
/* Keep EXEC0294's original all-six contract unchanged for older users/tests. */
Gk294Status gk294_attest_seed_writers(const char *rom,
 const Gk294Write *writes,size_t count,Gk294SourceProof *out){
 return gk294_attest_seed_writers_masked(rom,writes,count,0x3fu,out);
}
'''

def sha(data):
 return hashlib.sha256(data).hexdigest()

def canonical(data):
 clean=data.replace(b'\r\n',b'\n')
 if b'\r' in clean: raise ValueError('Lone CR or malformed source line endings')
 return clean

def replace_one(text,old,new):
 if text.count(old)!=1 or new in text: raise ValueError('Ambiguous, duplicate or modified source')
 return text.replace(old,new,1)

def stage(app_dir):
 root=Path(app_dir)
 if not root.is_dir(): raise ValueError('Complete current native C11 app missing')
 # Validate actual EXEC0298 postimage, not an older, unrelated prototype.
 app=(root/'gk1_app_state.c')
 if not app.is_file() or sha(canonical(app.read_bytes()))!=APP_HASH:
  raise ValueError('Wrong EXEC0298 native-app predecessor')
 cm=canonical((root/'CMakeLists.txt').read_bytes()).decode('utf-8')
 if ('gk294_seed_writer_attest.c' not in cm or
     'gk296_stock_seed_origin.c' not in cm or
     'GK1_296_OriginalFourWriterOriginAndUnknownTailRollback' not in cm or
     'GK1_297_StockResetInitializerAndUnknownC289Preseed' not in cm):
  raise ValueError('Missing actual linked Win32 source or registered tests')
 dep=canonical((root/'gk296_stock_seed_origin.c').read_bytes())
 if b'gk294_attest_seed_writers_masked(' not in dep:
  raise ValueError('Not the defective EXEC0298 caller')
 outputs={}; originals={}
 for name in BEFORE:
  raw=(root/name).read_bytes()
  data=canonical(raw)
  if sha(data)!=BEFORE[name]: raise ValueError('Incorrect or already patched '+name)
  text=data.decode('utf-8')
  if name.endswith('.h'):
   text=replace_one(text,HEADER_OLD,HEADER_NEW)
  else:
   text=replace_one(text,C_OLD,C_NEW)
   text=replace_one(text,'if(mask!=0x3fu)return GK294_INCOMPLETE;',
                    'if(mask!=required_mask)return GK294_INCOMPLETE;')
   text=text+TAIL
  output=text.encode('utf-8')
  if sha(output)!=AFTER[name]: raise ValueError('Reviewed EXEC0296 postimage mismatch '+name)
  originals[name]=raw;outputs[name]=output
 # No files are touched before *all* validation. Roll back on I/O error.
 touched=[]
 try:
  for name,output in outputs.items():
   target=root/name
   with tempfile.NamedTemporaryFile(prefix='.gk299_',dir=root,delete=False) as f:
    tmp=Path(f.name);f.write(output)
   try: os.replace(tmp,target)
   finally: tmp.unlink(missing_ok=True)
   touched.append(name)
 except Exception:
  for name in reversed(touched):
   (root/name).write_bytes(originals[name])
  raise
 for name,expected in AFTER.items():
  if sha((root/name).read_bytes())!=expected: raise RuntimeError('Post-write verification failed')
 print('EXEC0299 VERIFIED_STATIC: exact masked-write symbol now defined and linked to C11 app; Spring still open')

if __name__=='__main__':
 if len(sys.argv)!=2: sys.exit('Usage: stage299.py <complete-C11-app>/execution_199')
 try: stage(sys.argv[1])
 except Exception as exc: sys.exit('EXEC0299 REFUSED: '+str(exc))
