#!/usr/bin/env python3
"""EXEC0240 exact, fail-closed integration from copyright-clean EXEC0239 ZIP."""
import sys,zipfile,hashlib,json,pathlib,subprocess
if len(sys.argv)!=3: raise SystemExit('usage: python apply240.py EXEC0239.zip EMPTY_OUTPUT_DIR')
root=pathlib.Path(__file__).resolve().parent
manifest=json.loads((root/'GK1_EXEC0240_SHA256_Source.json').read_text())
src=pathlib.Path(sys.argv[1]); dest=pathlib.Path(sys.argv[2])
if hashlib.sha256(src.read_bytes()).hexdigest()!=manifest['predecessor_archive_sha256']:
 raise SystemExit('REFUSED: predecessor ZIP SHA mismatch')
if dest.exists() and any(dest.iterdir()):raise SystemExit('REFUSED: output directory not empty')
dest.mkdir(parents=True,exist_ok=True)
with zipfile.ZipFile(src) as z:
 for member in z.infolist():
  p=pathlib.PurePosixPath(member.filename)
  if p.is_absolute() or '..' in p.parts:raise SystemExit('REFUSED: unsafe archive path')
 z.extractall(dest)
for name,identity in manifest['files'].items():
 target=dest/name
 if identity['predecessor_sha256']:
  if hashlib.sha256(target.read_bytes()).hexdigest()!=identity['predecessor_sha256']:
   raise SystemExit('REFUSED: original file differs: '+name)
 elif target.exists():raise SystemExit('REFUSED: new file unexpectedly exists: '+name)
subprocess.run(['git','apply','--check',str(root/'GK1_EXEC0240_Integration.patch')],cwd=dest,check=True)
subprocess.run(['git','apply',str(root/'GK1_EXEC0240_Integration.patch')],cwd=dest,check=True)
for name,identity in manifest['files'].items():
 if hashlib.sha256((dest/name).read_bytes()).hexdigest()!=identity['result_sha256']:
  raise SystemExit('REFUSED: bad patched result: '+name)
print('EXEC0240: exact predecessor + four source paths matched; no private data imported')
