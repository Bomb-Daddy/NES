#!/usr/bin/env python3
"""Reproducibly overlay EXEC0239 onto an already extracted EXEC0238 source tree."""
import hashlib,json,sys,subprocess
from pathlib import Path
here=Path(__file__).resolve().parent
manifest=json.loads((here/'GK1_EXEC0239_SourceSHA256.json').read_text())
if len(sys.argv)!=2:sys.exit('usage: python3 apply239.py ROOT_CONTAINING_source')
root=Path(sys.argv[1]).resolve()
if not (root/'source/execution_199/CMakeLists.txt').exists():sys.exit('wrong source root')
for entry in manifest['changed_paths']:
 p=root/entry['path']
 expected=entry['previous_sha256']
 if expected:
  if not p.is_file() or hashlib.sha256(p.read_bytes()).hexdigest()!=expected:sys.exit('SOURCE_DRIFT '+entry['path'])
 elif p.exists():sys.exit('NEW_FILE_EXISTS '+entry['path'])
patch=here/'GK1_EXEC0239_Integration.patch'
check=subprocess.run(['git','apply','--check','--unsafe-paths',str(patch)],cwd=root)
if check.returncode:sys.exit('PATCH_PRECHECK_FAILED')
run=subprocess.run(['git','apply','--unsafe-paths',str(patch)],cwd=root)
if run.returncode:sys.exit('PATCH_FAILED')
for entry in manifest['changed_paths']:
 p=root/entry['path']
 if hashlib.sha256(p.read_bytes()).hexdigest()!=entry['current_sha256']:sys.exit('POST_HASH_FAILED '+entry['path'])
print('EXEC0239 '+str(len(manifest['changed_paths']))+' changed paths SHA256 PASS')
