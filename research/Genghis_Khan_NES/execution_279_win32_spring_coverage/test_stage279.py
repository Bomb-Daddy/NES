#!/usr/bin/env python3
"""Hostile EXEC0279 stage QA. Requires copyright-clean complete EXEC0277 root."""
import argparse, pathlib, hashlib, shutil, subprocess, tempfile, sys

def run(src, review, patch):
    return subprocess.run([sys.executable,str(pathlib.Path(__file__).with_name('stage279.py')),
                           '--source-root',str(src),'--review-root',str(review),'--patch',str(patch)],
                          capture_output=True,text=True)

def all_hashes(root, names):
    return {name:hashlib.sha256((root/'execution_199'/name).read_bytes()).hexdigest() for name in names}

def main():
    p=argparse.ArgumentParser()
    for n in ('source_root','review_root','patch'):
        p.add_argument('--'+n.replace('_','-'),type=pathlib.Path,required=True)
    a=p.parse_args()
    names=['CMakeLists.txt','gk1_app_state.c','gk1_app_state.h','gk1_native_shell.c']
    checks=0
    with tempfile.TemporaryDirectory(prefix='gk279qa_') as temp:
        t=pathlib.Path(temp)
        root=t/'clean'
        # Only the complete source tree is copied, never a private ROM.
        shutil.copytree(a.source_root,root)
        baseline=all_hashes(root,names)
        ok=run(root,a.review_root,a.patch)
        assert ok.returncode==0,ok.stderr
        checks+=1
        assert all_hashes(root,names)!=baseline
        checks+=1
        assert (root/'execution_199'/'gk278_original_spring_coverage.c').is_file()
        checks+=1
        again=run(root,a.review_root,a.patch)
        assert again.returncode!=0 and 'predecessor bytes changed' in again.stderr
        checks+=1
        mutated=t/'mutated';shutil.copytree(a.source_root,mutated)
        f=mutated/'execution_199'/'gk1_app_state.c'
        f.write_bytes(f.read_bytes()+b'\n/* tampered */\n')
        before=all_hashes(mutated,names)
        fail=run(mutated,a.review_root,a.patch)
        assert fail.returncode!=0 and 'predecessor bytes changed' in fail.stderr
        assert all_hashes(mutated,names)==before
        checks+=2
        missing=t/'missing';shutil.copytree(a.source_root,missing)
        badreview=t/'review';shutil.copytree(a.review_root,badreview)
        (badreview/'test_gk278_original_spring_coverage.c').write_bytes(b'forged')
        before=all_hashes(missing,names)
        fail=run(missing,badreview,a.patch)
        assert fail.returncode!=0 and 'reviewed source mismatch' in fail.stderr
        assert all_hashes(missing,names)==before
        checks+=2
        invalid=t/'patch';invalid.write_bytes(a.patch.read_bytes().replace(b'gk278_original_spring_coverage.c',b'forged_does_not_exist_source.c',1))
        before=all_hashes(missing,names)
        fail=run(missing,a.review_root,invalid)
        assert fail.returncode!=0 and 'integration patch digest mismatch' in fail.stderr
        assert all_hashes(missing,names)==before
        checks+=2
    print(f'VERIFIED_STATIC EXEC0279 hostile staging {checks}/{checks} PASS; real Win32 CI not inferred')

if __name__ == '__main__':
    try: main()
    except Exception as e:
        print(f'GK279 HOSTILE QA FAILED: {e}',file=sys.stderr)
        raise
