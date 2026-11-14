#!/usr/bin/env python3
"""Independent hostile source-stage tests using a verified predecessor ZIP.
No original NES ROM or Mesen data is included or read in this test.
"""
from pathlib import Path
from tempfile import TemporaryDirectory
import hashlib
import json
import sys
import zipfile
from stage287 import stage, MANIFEST, CHANGED, NEW_TEST, TEST_NAME

def sha(b): return hashlib.sha256(b).hexdigest()

def make_app(zip_path, root, crlf=False):
    app = root/'execution_199'
    (app/'packaging').mkdir(parents=True)
    with zipfile.ZipFile(zip_path) as z:
        for name in (*CHANGED,'CMakeLists.txt'):
            matches=[x for x in z.namelist() if x=='execution_199/'+name]
            assert len(matches)==1,(name,matches)
            b=z.read(matches[0]); assert b
            if crlf: b=b.replace(b'\n',b'\r\n')
            (app/name).write_bytes(b)
    manifest=json.loads(MANIFEST.read_text())
    old=next(x['previous_sha256'] for x in manifest['changed'] if x['path'].endswith('/gk1_app_state.c'))
    (app/'packaging/test_gk265_current_win32_source.py').write_text("EXPECTED_APP_SHA = '"+old+"'\n")
    return app

def run(zippath):
    checks=0
    manifest=json.loads(MANIFEST.read_text()); entries={Path(i['path']).name:i for i in manifest['changed']}
    with TemporaryDirectory(prefix='gk287-') as temporary:
        base=Path(temporary)
        for crlf in (False,True):
            root=base/('crlf' if crlf else 'lf')
            app=make_app(zippath,root,crlf)
            stage(app);checks+=1
            for f in CHANGED:
                assert sha((app/f).read_bytes())==entries[f]['new_sha256'],f;checks+=1
            assert sha((app/NEW_TEST).read_bytes())==entries[NEW_TEST]['new_sha256'];checks+=1
            cm=(app/'CMakeLists.txt').read_text()
            assert cm.count('add_test(NAME '+TEST_NAME)==1
            assert cm.count('target_link_libraries(test_gk286_c289_scenario_binding PRIVATE gk1_app_state)')==1;checks+=2
            assert "EXPECTED_APP_SHA = '"+entries['gk1_app_state.c']['new_sha256']+"'" in (app/'packaging/test_gk265_current_win32_source.py').read_text();checks+=1
            post={f:(app/f).read_bytes() for f in (*CHANGED,NEW_TEST,'CMakeLists.txt')}
            try: stage(app)
            except ValueError: pass
            else: raise AssertionError('Duplicate staging accepted')
            assert all((app/f).read_bytes()==post[f] for f in post);checks+=1
            # Every protected predecessor SHA, CMake registry, and pinned gate
            # independently mutated: stage must fail before a single write.
            for victim in (*CHANGED,'CMakeLists.txt','packaging/test_gk265_current_win32_source.py'):
                badroot=base/('bad'+str(checks))
                badapp=make_app(zippath,badroot,crlf)
                p=badapp/victim
                p.write_bytes(p.read_bytes()+b'\n/* mutation */\n')
                saved={f:(badapp/f).read_bytes() for f in (*CHANGED,'CMakeLists.txt','packaging/test_gk265_current_win32_source.py')}
                if victim=='CMakeLists.txt':
                    # Additional text is not necessarily disqualifying; remove
                    # required linked test to make this an authoritative negative.
                    t=p.read_text().replace('GK1_285_SelectedDifficulty_OriginalC289_CausalAndAtomic','GK1_285_INVALID')
                    p.write_text(t)
                    saved[victim]=p.read_bytes()
                if victim=='packaging/test_gk265_current_win32_source.py':
                    p.write_text(p.read_text().replace("EXPECTED_APP_SHA = '", "EXPECTED_APP_SHA = 'stale-"))
                    saved[victim]=p.read_bytes()
                try: stage(badapp)
                except ValueError: pass
                else: raise AssertionError('Modified predecessor accepted '+victim)
                assert all((badapp/f).read_bytes()==saved[f] for f in saved),victim
                assert not (badapp/NEW_TEST).exists();checks+=3
    print(f'GK1_EXEC0287 hostile Windows source preflight: {checks} checks PASS; no ROM, no game turn fabricated')

if __name__=='__main__':
    if len(sys.argv)!=2: raise SystemExit('Usage: test_stage287.py <EXEC0285 source ZIP>')
    run(Path(sys.argv[1]))