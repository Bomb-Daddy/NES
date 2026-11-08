#!/usr/bin/env python3
"""Portable Linux executable checks only; not ROM, Windows or Xbox evidence."""
import hashlib, os, pathlib, subprocess, sys, tempfile
exe,csv,desktop=map(pathlib.Path,sys.argv[1:4])
assert exe.is_file() and csv.is_file() and desktop.is_file()
assert hashlib.sha256(csv.read_bytes()).hexdigest()=="fa3958f156f09d4b4c15bb559c12aa357fc617a9b1f9217505fe07e8800de581"
entry=desktop.read_text()
assert "Name=Genghis Khan I Research 0.80.0-dev1" in entry
assert "Exec=gk1-native-research\n" in entry
assert "Terminal=false\n" in entry
with tempfile.TemporaryDirectory(prefix='gk1_exec54_') as tmp:
    def run(*args,env=None):
        return subprocess.run([str(exe),*args],cwd=tmp,env=env,capture_output=True,text=True,timeout=15)
    v=run('--version')
    assert v.returncode==0 and '0.80.0-dev1' in v.stdout and 'not full game' in v.stdout
    for sc,num in ((1,14),(2,27)):
        r=run('--scenario-report',str(sc))
        assert r.returncode==0,(r.stdout,r.stderr)
        lines=r.stdout.splitlines()
        assert f'countries={num} phase=PRE_SETUP_TEMPLATE' in lines[0]
        assert len(lines)==num+1, (sc,len(lines))
    bad_env=os.environ.copy()
    bad_env['GK1_SCENARIO_CSV']=str(pathlib.Path(tmp)/'tampered.csv')
    pathlib.Path(bad_env['GK1_SCENARIO_CSV']).write_text('invalid original country data\n')
    bad=run('--scenario-report','1',env=bad_env)
    assert bad.returncode==8 and 'FAILED' in bad.stderr
    assert not list(pathlib.Path(tmp).glob('*.gkp')) # no silent save on CLI check
print('EXEC0055 installed-style native CLI identity/2 scenarios/corrupted override/desktop version: PASS (Linux)')
