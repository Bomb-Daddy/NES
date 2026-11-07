"""Deterministic append-only Enhanced Run036 from immutable Run035.

Note: this only builds Original-KOEI Enhanced. Not Classic NES SUP-0016/DOS.
"""
import hashlib, json, zipfile, sys
from pathlib import Path
ROOT=Path(__file__).resolve().parent
PARENT=Path(sys.argv[1]) if len(sys.argv)>1 else Path('/mnt/data/rotk_run035_work/output/ROTK2_Enhanced_Run035_Q10_SwitchedBitAutomaticReturn_FullSource_v1.00.zip')
TREE=ROOT/'parent'
OUT=Path(sys.argv[2]) if len(sys.argv)>2 else ROOT/'output/ROTK2_Enhanced_Run036_Q10_LiveListHeadProtection_FullSource_v1.00.zip'
EXPECTED_PARENT='48b4ed72b99141fe9672dcabbeb32cf417d498cb75461e5270e67bf0aceebf94'
REPLACED={'third_branch/officer_battle_adapter.py','third_branch/tactical_view.py'}
NEW='tests/test_run036_q10_live_list_head.py'
def sha(path):
    h=hashlib.sha256()
    with open(path,'rb') as fp:
        for chunk in iter(lambda:fp.read(1024*1024),b''):h.update(chunk)
    return h.hexdigest()
if sha(PARENT)!=EXPECTED_PARENT:
    raise SystemExit('BLOCKED: parent SHA mismatch: '+sha(PARENT))
OUT.parent.mkdir(parents=True,exist_ok=True)
with zipfile.ZipFile(PARENT) as src,zipfile.ZipFile(OUT,'w',allowZip64=True) as dst:
    assert src.testzip() is None and len(src.namelist())==553
    for info in src.infolist():
        data=(TREE/info.filename).read_bytes() if info.filename in REPLACED else src.read(info.filename)
        dst.writestr(info,data,compress_type=info.compress_type,compresslevel=6)
    info=zipfile.ZipInfo(NEW,date_time=(1980,1,1,0,0,0))
    info.compress_type=zipfile.ZIP_DEFLATED
    info.create_system=3
    info.external_attr=0o100644<<16
    dst.writestr(info,(TREE/NEW).read_bytes(),compresslevel=6)
with zipfile.ZipFile(OUT) as z,zipfile.ZipFile(PARENT) as p:
    assert z.testzip() is None and len(z.namelist())==554
    assert not (set(REPLACED) - set(p.namelist())) and NEW not in p.namelist()
    same=sum(z.read(n)==p.read(n) for n in p.namelist())
    assert same==551,(same,len(p.namelist()))
print(json.dumps({'parent_sha256':EXPECTED_PARENT,'source_sha256':sha(OUT),
    'bytes':OUT.stat().st_size,'zip_entries':554,'inherited_identical':same,
    'inherited_total':553,'production_modified':sorted(REPLACED),'test_added':NEW},indent=2))
