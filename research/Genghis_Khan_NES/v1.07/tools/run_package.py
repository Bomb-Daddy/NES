#!/usr/bin/env python3
"""Strict isolated-process runner. Private inputs remain outside the package."""
from pathlib import Path
import argparse,os,sys,json,subprocess,re,hashlib,time,platform
ROOT=Path(__file__).resolve().parent
SUITES={
 'scenario_setup':('batches/scenario_economy_data/replay_setup.py',15,['GK_ROM']),
 'roi':('batches/economic_roi/test_roi.py',13,['GK_ROM','GK_ROI_EVIDENCE']),
 'v106_source':('output/research/Genghis_Khan_NES/v1.06/tools/run_all.py',63,['GK_ROM','GK_SRAM']),
 'v106_full':('output/research/Genghis_Khan_NES/v1.06/tools/run_all.py',72,['GK_ROM','GK_SRAM','GK_EVIDENCE']),
 'domestic':('batches/domestic_source/test_domestic.py',27,['GK_ROM','GK_SRAM','GK_ARCHIVED_CAPTURES']),
 'trade':('batches/trade_source/test_trade.py',38,['GK_ROM','GK_SRAM']),
 'seasonal_core':('batches/seasonal_source/test_annual.py',26,['GK_ROM']),
 'seasonal_extension':('batches/seasonal_source/test_dependencies.py',24,['GK_ROM']),
 'logistics':('batches/logistics_source/test_logistics.py',37,['GK_ROM','GK_SRAM']),
 'commodity':('batches/commodity_identity/test_identity.py',12,['GK_ROM','GK_MANUAL','GK_EVIDENCE']),
 'scenario':('batches/scenario_economy_data/test_scenarios.py',13,['GK_ROM']),
}
DEFAULT=['v106_full','domestic','trade','seasonal_core','seasonal_extension','logistics','commodity','scenario','scenario_setup','roi']
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
 parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--suite',action='append',choices=list(SUITES));parser.add_argument('--list',action='store_true');parser.add_argument('--output',type=Path,default=ROOT/'results');args=parser.parse_args()
 if args.list:
  print(json.dumps({k:{'script':v[0],'expected_methods':v[1],'private_env':v[2]} for k,v in SUITES.items()},indent=2));return 0
 selected=args.suite or DEFAULT;required=sorted({var for k in selected for var in SUITES[k][2]})
 specs=json.loads((ROOT/'PRIVATE_INPUT_REQUIREMENTS.json').read_text());verified={};errors=[]
 for var in required:
  if not os.environ.get(var):errors.append(f'Missing required environment variable {var}');continue
  p=Path(os.environ[var]).resolve();spec=specs[var];records=[]
  for entry in spec['files']:
   f=p/entry['relative'] if spec['kind']=='directory' else p
   if not f.is_file():errors.append(f'{var}: missing required file {entry["relative"]}');continue
   actual=sha(f)
   if actual!=entry['sha256']:errors.append(f'{var}: SHA-256 mismatch for {entry["relative"]}');continue
   records.append({'relative':entry['relative'],'bytes':f.stat().st_size,'sha256':actual})
  verified[var]=records
 if errors:
  print('\n'.join(errors),file=sys.stderr);return 2
 out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
 receipt={'python':sys.version,'executable':sys.executable,'platform':platform.platform(),'working_directory':str(ROOT),'input_hashes':verified,'selected':selected,'results':[],'evidence_scope':'Source replay and explicitly included archived-evidence checks; not new emulator or port validation'}
 for name in selected:
  script,expected,_=SUITES[name];env=os.environ.copy();env['PYTHONDONTWRITEBYTECODE']='1';env.pop('PYTHONPATH',None)
  if name=='v106_source':env.pop('GK_EVIDENCE',None)
  start=time.monotonic();command=[sys.executable,str(ROOT/script)]
  # Each process has its own module namespace, avoiding inspect_source collisions.
  completed=subprocess.run(command,cwd=ROOT,env=env,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
  (out/(name+'.stdout')).write_bytes(completed.stdout);(out/(name+'.stderr')).write_bytes(completed.stderr)
  combined=(completed.stdout+completed.stderr).decode('utf-8','replace');match=re.findall(r'Ran (\d+) tests? in [\d.]+s',combined)
  count=int(match[-1]) if match else None;okline=bool(re.search(r'^OK\s*$',combined,re.M))
  if name=='scenario_setup':
   setup_match=re.findall(r'^PASS (\d+) condition-labelled original country setup replays$',combined,re.M)
   count=int(setup_match[-1]) if setup_match else None;okline=bool(setup_match)

  good=completed.returncode==0 and count==expected and okline
  row={'suite':name,'command':command,'exit_code':completed.returncode,'count_kind':'setup_cases' if name=='scenario_setup' else 'unittest_methods','expected_count':expected,'observed_count':count,'unittest_ok_line':okline,'pass':good,'seconds':time.monotonic()-start,'stdout_sha256':sha(out/(name+'.stdout')),'stderr_sha256':sha(out/(name+'.stderr'))}
  receipt['results'].append(row);print(f'{name}: '+('PASS' if good else 'FAIL')+f' methods={count}/{expected}, exit={completed.returncode}',flush=True)
  (out/'RUN_RECEIPT.json').write_text(json.dumps(receipt,indent=2))
 receipt['total_methods']=sum((r['observed_count'] or 0) for r in receipt['results'] if r['count_kind']=='unittest_methods');receipt['total_setup_cases']=sum((r['observed_count'] or 0) for r in receipt['results'] if r['count_kind']=='setup_cases');receipt['all_pass']=all(r['pass'] for r in receipt['results']);(out/'RUN_RECEIPT.json').write_text(json.dumps(receipt,indent=2))
 return 0 if receipt['all_pass'] else 1
if __name__=='__main__':raise SystemExit(main())
