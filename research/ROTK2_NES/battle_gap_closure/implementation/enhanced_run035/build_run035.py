"""Reproducible Enhanced Run035 full source archive from immutable Run034.

Never replaces Run034, Classic NES SUP-0016, or the DOS tree.
"""
from pathlib import Path
import hashlib, json, zipfile, sys
PARENT_SHA='79c5bb7d98ef6376a34b7ea56d6d670c9c5d452439a8602d2c7b8c555d3d0d8c'
ROOT=Path(__file__).resolve().parent
PARENT=Path(sys.argv[1]) if len(sys.argv)>1 else Path('/mnt/data/ROTK2_Enhanced_Run034_FullSource_v1.00.zip')
MOD=ROOT/'parent'
OUT=Path(sys.argv[2]) if len(sys.argv)>2 else ROOT/'output/ROTK2_Enhanced_Run035_Q10_SwitchedBitAutomaticReturn_FullSource_v1.00.zip'
REPLACED={'third_branch/officer_battle_adapter.py','tests/test_build16_q10_bribe.py'}
ADDED='tests/test_run035_q10_return_requires_switched_flag.py'
def sha(b):return hashlib.sha256(b).hexdigest()
if sha(PARENT.read_bytes())!=PARENT_SHA:raise SystemExit('BLOCKED: immutable Run034 parent hash wrong')
OUT.parent.mkdir(parents=True,exist_ok=True)
with zipfile.ZipFile(PARENT,'r') as src,zipfile.ZipFile(OUT,'w',allowZip64=True) as dst:
    assert src.testzip() is None and len(src.namelist())==552
    for info in src.infolist():
        data=(MOD/info.filename).read_bytes() if info.filename in REPLACED else src.read(info.filename)
        dst.writestr(info,data,compress_type=info.compress_type,compresslevel=6)
    info=zipfile.ZipInfo(ADDED,date_time=(1980,1,1,0,0,0))
    info.compress_type=zipfile.ZIP_DEFLATED
    info.create_system=3
    info.external_attr=0o100644<<16
    dst.writestr(info,(MOD/ADDED).read_bytes(),compresslevel=6)
with zipfile.ZipFile(OUT) as z:
    bad=z.testzip();assert bad is None and len(z.namelist())==553
    old=zipfile.ZipFile(PARENT)
    same=sum(z.read(n)==old.read(n) for n in old.namelist())
    assert same==550, (same,len(old.namelist()))
    old.close()
print(json.dumps({'parent_sha256':PARENT_SHA,'new_sha256':sha(OUT.read_bytes()),'byte_count':OUT.stat().st_size,'entries':553,'inherited_identical':same,'inherited_total':552,'changed_files':sorted(REPLACED),'added_files':[ADDED]},indent=2))
