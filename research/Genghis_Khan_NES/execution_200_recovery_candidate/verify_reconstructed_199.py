#!/usr/bin/env python3
"""Narrow source provenance gate: 619 original matches, exactly 12 expected mismatches.
This explicitly does NOT prove exact published 199 source or complete NES parity.
"""
import argparse, hashlib, json, pathlib, sys
EXPECTED_MISMATCH = {
    'README_EXEC0197.md',
    '__pycache__/gk139_archived_state_extract.cpython-313.pyc',
    'gk134_stock_command_route.c','gk134_stock_command_route.h',
    'gk187_classic_assign.c','gk187_classic_assign.h',
    'gk189_classic_cold_save.c','gk1_app_state.c','gk1_app_state.h',
    'gk1_native_shell.c','test_gk134_stock_command_route.c',
    'test_gk197_classic_tax.c',
}
def audit(source, manifest):
    obj=json.loads(pathlib.Path(manifest).read_text())
    hashes=obj.get('file_sha256', {})
    if obj.get('execution')!='0199' or len(hashes)!=631:
        return False, 'manifest scope is not EXEC0199/631'
    mismatched=set(); missing=[]; matched=0
    for name,expected in hashes.items():
        path=pathlib.Path(name)
        if path.is_absolute() or '..' in path.parts:
            return False, 'unsafe path in manifest'
        file=pathlib.Path(source)/path
        if not file.is_file():
            missing.append(name);continue
        if hashlib.sha256(file.read_bytes()).hexdigest()==expected:
            matched+=1
        else:mismatched.add(name)
    ok = matched==619 and not missing and mismatched==EXPECTED_MISMATCH
    report={'matched':matched,'mismatched':sorted(mismatched),'missing':missing,
            'historically_byte_identical':False,'certified_source':False}
    return ok, json.dumps(report,sort_keys=True)
def main():
    p=argparse.ArgumentParser();p.add_argument('source');p.add_argument('manifest');args=p.parse_args()
    ok,report=audit(args.source,args.manifest)
    print(('RECOVERY-CANDIDATE-PASS' if ok else 'RECOVERY-CANDIDATE-FAIL')+' '+report)
    return 0 if ok else 1
if __name__=='__main__':sys.exit(main())