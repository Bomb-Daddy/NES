"""Build Enhanced Run029 ONLY from immutable, SHA-pinned Original-KOEI Run028.
No Classic NES, DOS, GitHub old-game source trees are inputs.
"""
from pathlib import Path
import hashlib,json,zipfile,sys
ROOT=Path('/mnt/data/rotk_enh_run029_work')
PARENT=Path('/mnt/data/rotk_enh_run028_recovered/ROTK2_Enhanced_Run028_FullSource_v1.00.zip')
SOURCE=ROOT/'source'
OUTPUT=ROOT/'output'/'ROTK2_Enhanced_Run029_Q14_PrisonerModalIdentity_FullSource_v1.00.zip'
OLD_HASH='95f5660ff01e3f92fcb449876a0779f388f6fe4c7240dba9c0d4f7de247db907'
CHANGE='third_branch/tactical_view.py'
TEST='tests/test_run029_q14_prisoner_modal_identity.py'

def sha(data):return hashlib.sha256(data).hexdigest()
def sha_file(path):
    h=hashlib.sha256()
    with path.open('rb') as f:
        for chunk in iter(lambda:f.read(1048576),b''):h.update(chunk)
    return h.hexdigest()

def build(dest=OUTPUT):
    if sha_file(PARENT)!=OLD_HASH:raise RuntimeError('PARENT SOURCE HASH MISMATCH')
    dest.parent.mkdir(parents=True,exist_ok=True)
    with zipfile.ZipFile(PARENT) as before,zipfile.ZipFile(dest,'w',allowZip64=True) as result:
        old=before.namelist()
        if CHANGE not in old or TEST in old:raise RuntimeError('unexpected parent contents')
        for info in before.infolist():
            data=SOURCE.joinpath(info.filename).read_bytes() if info.filename==CHANGE else before.read(info.filename)
            result.writestr(info,data)
        info=zipfile.ZipInfo(TEST,date_time=(2026,11,7,0,0,0))
        info.create_system=3
        info.external_attr=0o100644<<16
        info.compress_type=zipfile.ZIP_DEFLATED
        result.writestr(info,(SOURCE/TEST).read_bytes())
    with zipfile.ZipFile(PARENT) as before,zipfile.ZipFile(dest) as after:
        assert after.testzip() is None
        assert len(after.namelist())==len(before.namelist())+1
        unchanged=[n for n in before.namelist() if before.read(n)==after.read(n)]
        assert len(unchanged)==len(before.namelist())-1, (len(unchanged),len(before.namelist()))
        assert len(after.namelist())==547
    return {'version':'Enhanced Run029','parent_version':'Enhanced Run028','parent_sha256':OLD_HASH,
            'new_sha256':sha_file(dest),'bytes':dest.stat().st_size,'zip_entries':547,
            'unchanged_parent_entries':len(unchanged),'parent_entries':len(old),
            'changed_production_file':CHANGE,'new_test_file':TEST,
            'environment':'Linux Python 3.13 / Xvfb, not Windows/Xbox',
            'source_scope':'Original-KOEI Enhanced ONLY',
            'original_english_NES_event_goldens':'NOT RUN',
            'status':'TESTED_PORT PARTIAL'}
if __name__=='__main__':
    manifest=build(Path(sys.argv[1]) if len(sys.argv)>1 else OUTPUT)
    (ROOT/'output').mkdir(exist_ok=True,parents=True)
    (ROOT/'output'/'RUN029_SOURCE_MANIFEST.json').write_text(json.dumps(manifest,indent=2)+'\n')
    print(json.dumps(manifest,indent=2))
