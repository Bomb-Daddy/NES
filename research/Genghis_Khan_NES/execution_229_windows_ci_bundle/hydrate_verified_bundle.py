#!/usr/bin/env python3
"""EXEC0229: fail-closed reassembly of complete source for real Win32 CI."""
import argparse, base64, hashlib, io, json, os, re, sys, tempfile, zipfile
from pathlib import Path, PurePosixPath

REQUIRED = {'execution_199/CMakeLists.txt', 'execution_199/gk1_app_state.c',
            'execution_199/gk1_native_shell.c', 'execution_199/gk228_win32_smoke.c',
            'execution_199/packaging/test_gk228_windows_contract.py',
            'execution_039/gk1_battle_session.c',
            'execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv'}
FORBIDDEN = {'.nes','.srm','.sav','.mss','.rom','.bin','.png','.jpg','.jpeg','.wav','.mp3'}
class BundleError(RuntimeError): pass

def hydrate(parts, dest, manifest):
    m=json.loads(manifest.read_text('utf8'))
    h,n,k,q=(m[x] for x in ('archive_sha256','archive_size_bytes','chunk_count','entry_count'))
    if not (isinstance(h,str) and re.fullmatch('[a-f0-9]{64}',h) and
            isinstance(n,int) and n>0 and isinstance(k,int) and 0<k<=999 and
            isinstance(q,int) and q>0): raise BundleError('bad source manifest')
    expected=[f'part_{i:03}.txt' for i in range(k)]
    actual=sorted(p.name for p in parts.iterdir() if p.is_file())
    if actual!=expected: raise BundleError('missing, extra or out-of-order source chunk')
    try: raw=base64.b64decode(b''.join((parts/p).read_bytes() for p in expected),validate=True)
    except (ValueError,base64.binascii.Error) as e: raise BundleError('invalid base64') from e
    if len(raw)!=n or hashlib.sha256(raw).hexdigest()!=h: raise BundleError('bad source SHA or length')
    with zipfile.ZipFile(io.BytesIO(raw)) as z:
        entries=z.infolist()
        if len(entries)!=q or z.testzip() is not None: raise BundleError('bad ZIP count/CRC')
        names=set()
        for e in entries:
            p=PurePosixPath(e.filename)
            if (not e.filename or e.filename.startswith('/') or '\\' in e.filename or
                '..' in p.parts or '.' in p.parts or ':' in p.parts[0] or
                e.filename in names or (e.external_attr>>16)&0o170000==0o120000):
                raise BundleError('unsafe or duplicate ZIP path')
            if p.suffix.lower() in FORBIDDEN: raise BundleError('licensed gameplay binary')
            names.add(e.filename)
        if not REQUIRED.issubset(names): raise BundleError('incomplete native game source')
        if dest.exists(): raise BundleError('refusing to overwrite checkout')
        dest.parent.mkdir(parents=True,exist_ok=True)
        with tempfile.TemporaryDirectory(prefix='gk229.',dir=dest.parent) as tmp:
            stage=Path(tmp)/'source';stage.mkdir()
            z.extractall(stage)
            os.rename(stage,dest)
    return {'sha256':h,'bytes':n,'chunks':k,'entries':q}

def main():
    p=argparse.ArgumentParser()
    for name in ('parts','dest','manifest'): p.add_argument('--'+name,required=True,type=Path)
    a=p.parse_args()
    try: print(json.dumps(hydrate(a.parts,a.dest,a.manifest),sort_keys=True))
    except (BundleError,OSError,KeyError,ValueError,TypeError,zipfile.BadZipFile) as e:
        print('EXEC0229 SOURCE REFUSED: '+str(e),file=sys.stderr);return 1
    return 0
if __name__=='__main__':sys.exit(main())
