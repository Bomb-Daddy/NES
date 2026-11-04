#!/usr/bin/env python3
"""Strict additive Move Base suite; does not invoke or change run_package.py."""
from pathlib import Path
import argparse,hashlib,json,os,platform,re,subprocess,sys,time
HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[1]
INPUTS={
 'GK_ROM':(262160,'ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3'),
 'GK_SRAM':(8192,'dd22847140cc0397ab8f42e29f1832681628ed52bfa17e95041c288afc4ef773'),
}
def sha(path):return hashlib.sha256(path.read_bytes()).hexdigest()
def completed_suite(stdout,stderr,exit_code):
 text=(stdout+stderr).decode('utf-8','replace')
 matches=re.findall(r'^Ran (\d+) tests? in [\d.]+s$',text,re.M)
 count=int(matches[-1]) if len(matches)==1 else None
 ok=bool(re.search(r'^OK\s*$',text,re.M))
 return {'expected_count':19,'observed_count':count,'unittest_ok_line':ok,
         'pass':exit_code==0 and count==19 and ok}
def main():
 parser=argparse.ArgumentParser(description=__doc__)
 parser.add_argument('--output',type=Path,default=HERE/'results')
 args=parser.parse_args();verified={};errors=[]
 for key,(size,expected) in INPUTS.items():
  value=os.environ.get(key)
  if not value:errors.append('Missing required environment variable '+key);continue
  path=Path(value).resolve()
  if not path.is_file():errors.append(key+': required file is missing');continue
  actual=sha(path)
  if path.stat().st_size!=size or actual!=expected:errors.append(key+': size/SHA-256 mismatch');continue
  verified[key]={'bytes':size,'sha256':actual}
 provenance=json.loads((HERE/'SOURCE_PROVENANCE.json').read_text())
 source_hashes={}
 for entry in provenance['executed_sources']:
  path=ROOT/entry['relative']
  if not path.is_file() or sha(path)!=entry['sha256']:
   errors.append('Executed source SHA-256 mismatch: '+entry['relative'])
  else:source_hashes[entry['relative']]=entry['sha256']
 if errors:
  print('\n'.join(errors),file=sys.stderr);return 2
 output=args.output.resolve();output.mkdir(parents=True,exist_ok=True)
 env=os.environ.copy();env['PYTHONDONTWRITEBYTECODE']='1';env.pop('PYTHONPATH',None)
 command=[sys.executable,str(HERE/'test_base.py')];start=time.monotonic()
 try:
  result=subprocess.run(command,cwd=ROOT,env=env,capture_output=True,timeout=60)
  stdout,stderr,code=result.stdout,result.stderr,result.returncode
 except subprocess.TimeoutExpired as failure:
  stdout,stderr,code=failure.stdout or b'',failure.stderr or b'',124
  stderr+=b'\nRunner timeout after 60 seconds; suite incomplete.\n'
 (output/'base_relocation.stdout').write_bytes(stdout)
 (output/'base_relocation.stderr').write_bytes(stderr)
 receipt={'suite':'base_relocation_additive','scope':'VERIFIED_STATIC bounded original-instruction suite; no new emulator execution',
  'python':sys.version,'executable':sys.executable,'platform':platform.platform(),
  'command':command,'cwd':str(ROOT),'input_hashes':verified,'executed_source_hashes':source_hashes,
  'exit_code':code,'seconds':time.monotonic()-start,
  'stdout_sha256':sha(output/'base_relocation.stdout'),'stderr_sha256':sha(output/'base_relocation.stderr'),
  **completed_suite(stdout,stderr,code)}
 (output/'RUN_RECEIPT.json').write_text(json.dumps(receipt,indent=2)+'\n')
 print('base_relocation: '+('PASS' if receipt['pass'] else 'FAIL')+
       f" methods={receipt['observed_count']}/19, exit={code}")
 return 0 if receipt['pass'] else 1
if __name__=='__main__':raise SystemExit(main())
