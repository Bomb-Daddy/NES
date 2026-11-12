#!/usr/bin/env python3
"""Hostile source-bundle tests; no Mesen or copyrighted ROM required."""
import base64
import hashlib
from pathlib import Path
import json
import tempfile
import zipfile
from hydrate_verified_bundle import hydrate, BundleError, REQUIRED


def make(directory, *, bad_path=False, binary=False):
    archive = directory / 'source.zip'
    with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED) as z:
        for name in REQUIRED:
            z.writestr(name, name)
        if bad_path: z.writestr('../escape.txt','bad')
        if binary: z.writestr('execution_199/raw.nes', 'bad')
    blob = archive.read_bytes()
    b64 = base64.b64encode(blob)
    parts = directory / 'parts'; parts.mkdir()
    pieces = [b64[i:i+120] for i in range(0, len(b64), 120)]
    for i, part in enumerate(pieces):
        (parts/f'part_{i:03}.txt').write_bytes(part)
    m = {'archive_sha256':hashlib.sha256(blob).hexdigest(),
         'archive_size_bytes':len(blob), 'chunk_count':len(pieces),
         'entry_count':len(REQUIRED)+int(bad_path)+int(binary)}
    manifest = directory/'manifest.json';manifest.write_text(json.dumps(m))
    return parts, manifest


def rejected(parts, dest, manifest):
    try: hydrate(parts,dest,manifest)
    except BundleError: pass
    else: raise AssertionError('unexpected acceptance')
    assert not dest.exists()


def main():
    with tempfile.TemporaryDirectory() as t:
        root=Path(t); a=root/'valid';a.mkdir()
        parts,manifest=make(a)
        out=root/'out'
        assert hydrate(parts,out,manifest)['entries']==len(REQUIRED)
        assert (out/'execution_199/gk1_native_shell.c').is_file()
        try: hydrate(parts,out,manifest)
        except BundleError: pass
        else: raise AssertionError('overwrite accepted')
        print('PASS valid exact complete native source and no overwrite')
        p=(parts/'part_000.txt');original=p.read_bytes();p.write_bytes(original[:-1]+b'!')
        rejected(parts,root/'bad-corrupt',manifest);p.write_bytes(original)
        print('PASS hostile modified-chunk refusal')
        removed=parts/'part_001.txt'; saved=removed.read_bytes();removed.unlink()
        rejected(parts,root/'bad-missing',manifest);removed.write_bytes(saved)
        print('PASS missing-chunk refusal')
        (parts/'part_999.txt').write_text('AA==')
        rejected(parts,root/'bad-extra',manifest)
        (parts/'part_999.txt').unlink()
        print('PASS extra-chunk refusal')
        m=json.loads(manifest.read_text());m['archive_sha256']='f'*64;manifest.write_text(json.dumps(m))
        rejected(parts,root/'bad-hash',manifest)
        print('PASS altered-source SHA refusal')
        for label, mode in [('zip-slip','bad_path'),('ROM','binary')]:
            where=root/label;where.mkdir();parts2,manifest2=make(where,**{mode:True})
            rejected(parts2,root/(label+'-out'),manifest2)
            print('PASS rejected',label)
    return 0


if __name__=='__main__':raise SystemExit(main())
