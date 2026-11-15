#!/usr/bin/env python3
"""Executable hostile QA of EXEC0289 source-integrity gated native Win32 stage.
Run with a copyright-clean ZIP of *actual EXEC0287 staged* C11 preimage.
"""
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import zipfile

HERE = Path(__file__).resolve().parent
REQ = ['gk1_app_state.c','gk1_app_state.h','CMakeLists.txt',
       'packaging/test_gk265_current_win32_source.py']
NEW = ['gk288_original_rng_stream.c','gk288_original_rng_stream.h',
       'test_gk288_original_rng_stream.c']
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()

def run(script,app):
    return subprocess.run([sys.executable,str(script),str(app)],capture_output=True,text=True)

def clone(root,src):
    app=root/'app';shutil.copytree(src,app);return app

def check(x,label):
    if not x: raise AssertionError(label)


def main():
    check(len(sys.argv)==2,'Usage test_stage289.py PREIMAGE.zip')
    z=Path(sys.argv[1]);check(z.is_file(),'Missing reviewed Win32 preimage')
    tests=0
    with tempfile.TemporaryDirectory(prefix='gk289_') as tmp:
        root=Path(tmp);pre=root/'pre';pre.mkdir()
        with zipfile.ZipFile(z) as arc:
            bad=arc.testzip();check(bad is None,'Bad ZIP member '+str(bad));tests+=1
            for name in REQ:
                source='execution_199/'+name
                check(source in arc.namelist(),'Required native source absent '+source);tests+=1
                f=pre/name;f.parent.mkdir(parents=True,exist_ok=True)
                f.write_bytes(arc.read(source))
        # Independently run copy of exact delivered stage script and original code.
        basedir=root/'repo';basedir.mkdir()
        stage_dir=basedir/'execution_289_win32_original_rng_stream';stage_dir.mkdir()
        upstream=basedir/'execution_288_original_rng_stream';upstream.mkdir()
        stage=stage_dir/'stage289.py';shutil.copy2(HERE/'stage289.py',stage)
        for name in NEW+['GK1_EXEC0288_Implementation.patch','GK1_EXEC0288_Source_Changed_SHA256.json']:
            shutil.copy2(HERE.parent/'execution_288_original_rng_stream'/name,upstream/name)
        manifest=json.loads((upstream/'GK1_EXEC0288_Source_Changed_SHA256.json').read_text())
        hashes={Path(e['path']).name:e['new_sha256'] for e in manifest['source_changes']}
        good=clone(root/'goodroot' if False else root,pre)
        for p in (good/n for n in NEW):check(not p.exists(),'New source already in predecessor');tests+=1
        result=run(stage,good)
        check(result.returncode==0,'Staging failed '+result.stdout+result.stderr);tests+=1
        for name in NEW+['gk1_app_state.c','gk1_app_state.h']:
            check(sha(good/name)==hashes[name],'Reviewed postimage SHA mismatch '+name);tests+=1
        cm=(good/'CMakeLists.txt').read_text()
        check(cm.split('target_include_directories(gk1_app_state',1)[0].count('gk288_original_rng_stream.c')==1,'Real app library source not unique');tests+=1
        check(cm.count('GK1_288_SourceOriginalC289MultiDrawNativeSetupAndRollback')==1,'Win32 CTest missing');tests+=1
        check((good/'packaging/test_gk265_current_win32_source.py').read_text().count(hashes['gk1_app_state.c'])==1,'Source hostile gate not updated');tests+=1
        check(run(stage,good).returncode!=0,'Duplicate stage accepted');tests+=1
        # Distinct actual-world selected stages, source registration corruption,
        # preimage damage, and unwanted pre-existing test MUST fail atomically.
        for filename,mutation in [
            ('gk1_app_state.c',lambda b:b.replace(b'GK53_SETUP_AWAIT_SOURCE',b'GK53_SETUP_PLAYERS',1)),
            ('gk1_app_state.h',lambda b:b+b'\n/* hostile extra */\n'),
            ('CMakeLists.txt',lambda b:b.replace(b'gk280_original_rate_bridge.c',b'gk280_other_bridge.c',1)),
            ('CMakeLists.txt',lambda b:b.replace(b'GK1_286_C289HistoricalTraceMustMatchSelectedCampaign',b'GK1_286_BOGUS',1)),
            ('packaging/test_gk265_current_win32_source.py',lambda b:b.replace(b'EXPECTED_APP_SHA',b'EXPECTED_FAKE_SHA',1))]:
            d=root/('reject'+str(tests));d.mkdir();app=clone(d,pre)
            p=app/filename;p.write_bytes(mutation(p.read_bytes()))
            before={name:sha(app/name) for name in REQ}
            rr=run(stage,app)
            check(rr.returncode!=0,'Corruption accepted for '+filename);tests+=1
            check(all(sha(app/name)==h for name,h in before.items()) and not any((app/name).exists() for name in NEW), 'Not atomic on '+filename);tests+=1
        d=root/'duplicate';d.mkdir();app=clone(d,pre)
        (app/NEW[0]).write_bytes(b'hostile')
        check(run(stage,app).returncode!=0,'Duplicate original source accepted');tests+=1
        # Independently refuse tampered review patch, manifest, new source test.
        for changed in ('GK1_EXEC0288_Implementation.patch','GK1_EXEC0288_Source_Changed_SHA256.json',NEW[2]):
            f=upstream/changed;original=f.read_bytes();f.write_bytes(original+b'HOSTILE')
            d=root/('upstream'+str(tests));d.mkdir();app=clone(d,pre)
            rr=run(stage,app)
            check(rr.returncode!=0 and not any((app/name).exists() for name in NEW),'Untrusted upstream accepted '+changed);tests+=1
            f.write_bytes(original)
        # Replay reproducibility on a second clean preimage.
        d=root/'replay';d.mkdir();app=clone(d,pre)
        check(run(stage,app).returncode==0,'Deterministic replay failed');tests+=1
        for name in REQ+NEW:
            check(sha(app/name)==sha(good/name),'Nonreproducible staged C11: '+name);tests+=1
    print(f'EXEC0289 hostile source integrity and deterministic Win32 integration: {tests}/{tests} PASS; private ROM not used; Win32 runtime NOT CLAIMED')
if __name__=='__main__':
    try:main()
    except (AssertionError, OSError, ValueError, zipfile.BadZipFile) as e:
        print('EXEC0289 HOSTILE FAIL '+str(e),file=sys.stderr);sys.exit(2)
