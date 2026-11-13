#!/usr/bin/env python3
import pathlib,sys,subprocess,tempfile,zipfile,json,hashlib
here=pathlib.Path(__file__).resolve().parent
manifest=json.loads((here/'GK1_EXEC0239_SourceSHA256.json').read_text())
assert len(sys.argv)==2
zip_path=pathlib.Path(sys.argv[1]); checks=0
with tempfile.TemporaryDirectory() as td:
 root=pathlib.Path(td)
 with zipfile.ZipFile(zip_path) as z:
  z.extractall(root,members=[n for n in z.namelist() if n.startswith('source/')])
 cp=subprocess.run([sys.executable,str(here/'apply239.py'),str(root)],capture_output=True,text=True)
 assert cp.returncode==0,cp.stderr;checks+=1
 for entry in manifest['changed_paths']:
  assert hashlib.sha256((root/entry['path']).read_bytes()).hexdigest()==entry['current_sha256'];checks+=1
 cp=subprocess.run([sys.executable,str(here/'apply239.py'),str(root)],capture_output=True,text=True)
 assert cp.returncode!=0; checks+=1
 (root/'source/execution_199/gk239_owner_world_mss.c').unlink()
 cp=subprocess.run([sys.executable,str(here/'apply239.py'),str(root)],capture_output=True,text=True)
 assert cp.returncode!=0;checks+=1
with tempfile.TemporaryDirectory() as td:
 root=pathlib.Path(td)
 with zipfile.ZipFile(zip_path) as z:z.extractall(root,members=[n for n in z.namelist() if n.startswith('source/')])
 target=root/'source/execution_199/CMakeLists.txt'
 target.write_bytes(target.read_bytes().replace(b'cmake_minimum_required',b'cmake_MINIMUM_required',1))
 cp=subprocess.run([sys.executable,str(here/'apply239.py'),str(root)],capture_output=True,text=True)
 assert cp.returncode!=0;checks+=1
 print('EXEC0239 hostile QA',checks,'controls PASS')
