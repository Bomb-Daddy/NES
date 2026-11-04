"""Reassemble the12 privately downloaded ZIP volumes; verify every byte."""
from pathlib import Path
import json,hashlib,zipfile,os
root=Path(__file__).resolve().parent
m=json.loads((root/'GK1_Evidence_Parts_v1.07.json').read_text())
out=root/m['archive'];temp=out.with_suffix('.building')
with temp.open('wb') as f:
 for p in m['parts']:
  v=root/p['volume'];b=v.read_bytes()
  assert len(b)==p['volume_bytes'] and hashlib.sha256(b).hexdigest()==p['volume_sha256'],v
  with zipfile.ZipFile(v) as z:
   assert z.testzip() is None;data=z.read(p['name'])
   assert len(data)==p['bytes'] and hashlib.sha256(data).hexdigest()==p['sha256'],p['name']
   f.write(data)
assert temp.stat().st_size==m['archive_bytes'] and hashlib.sha256(temp.read_bytes()).hexdigest()==m['archive_sha256']
with zipfile.ZipFile(temp) as z:
 assert z.testzip() is None
 manifest=json.loads(z.read('EVIDENCE_MANIFEST_v1.07.json'))
 for name,p in manifest['files'].items():
  b=z.read(name);assert len(b)==p['bytes'] and hashlib.sha256(b).hexdigest()==p['sha256'],name
os.replace(temp,out);print(f'PASS: {out.name}; {len(manifest["files"])} manifest files verified')
