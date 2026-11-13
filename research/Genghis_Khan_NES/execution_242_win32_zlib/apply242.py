#!/usr/bin/env python3
"""EXEC0242 strict copyright-clean 230->242 native app overlay; fail closed."""
import hashlib,json,os,subprocess,sys
from pathlib import Path

def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def main(argv):
 if len(argv)!=2:raise SystemExit('Usage: apply242.py <parent directory containing execution_199>')
 here=Path(__file__).resolve().parent
 dst=Path(argv[1]).resolve()
 source=dst/'execution_199'
 meta=json.loads((here/'GK1_EXEC0242_Source_SHA256.json').read_text())
 patch=here/'GK1_EXEC0242_Combined_EXEC0230_to_EXEC0242_CopyrightClean.patch'
 if not source.is_dir() or sha(patch)!=meta['patch_sha256']:
  raise SystemExit('EXEC0242: source root or exact patch missing/corrupted')
 for name,want in meta['pre_sha256'].items():
  p=source/name
  if not p.is_file() or sha(p)!=want:
   raise SystemExit('EXEC0242: predecessor source mismatch '+name)
 for name in meta['post_sha256']:
  p=source/name
  if name not in meta['pre_sha256'] and p.exists():
   raise SystemExit('EXEC0242: new source unexpectedly already exists '+name)
 cmd=['git','-C',str(dst),'apply','--ignore-space-change']
 for check in [True,False]:
  r=subprocess.run(cmd+(['--check'] if check else [])+[str(patch)],capture_output=True,text=True)
  if r.returncode:raise SystemExit('EXEC0242: '+('precheck' if check else 'apply')+' failed: '+r.stderr[:500])
 for name,want in meta['post_sha256'].items():
  if sha(source/name)!=want:raise SystemExit('EXEC0242: post-hash mismatch '+name)
 print('EXEC0242 verified exact native app replay: %d changed/new files, private ROM absent'%len(meta['post_sha256']))
if __name__=='__main__': main(sys.argv)
