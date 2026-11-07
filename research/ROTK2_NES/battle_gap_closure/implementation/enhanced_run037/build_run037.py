"""Original-KOEI Enhanced deterministic Run037 builder; Classic SUP-0016/DOS untouched.

This builder requires byte-exact Run036 and patched source files present
under parent/. A source hash mismatch aborts before any new artifact write.
"""
from pathlib import Path
from hashlib import sha256
from zipfile import ZipFile, ZipInfo, ZIP_DEFLATED
import sys, json
HERE=Path(__file__).resolve().parent
PARENT=Path(sys.argv[1]) if len(sys.argv)>1 else HERE/'output/ROTK2_Enhanced_Run036_rebuilt.zip'
TREE=HERE/'parent'
OUT=Path(sys.argv[2]) if len(sys.argv)>2 else HERE/'output/ROTK2_Enhanced_Run037_Q08_EmptyHexFire_FullSource_v1.00.zip'
PARENT_SHA='7b4e03d2f335a12fb5f5950e552b0fcd7e37f21abcfc425b8f68405211a92ecc'
REPLACED={'third_branch/officer_battle_adapter.py','third_branch/tactical_view.py'}
NEW='tests/test_run037_q08_fire_empty_hex.py'
def digest(p):return sha256(Path(p).read_bytes()).hexdigest()
if digest(PARENT)!=PARENT_SHA:raise SystemExit('BLOCKED: wrong Enhanced Run036 parent SHA '+digest(PARENT))
OUT.parent.mkdir(parents=True,exist_ok=True)
with ZipFile(PARENT) as src, ZipFile(OUT,'w',allowZip64=True) as dst:
    if src.testzip() is not None or len(src.namelist())!=554:raise SystemExit('BLOCKED: corrupted Run036 parent')
    if NEW in src.namelist() or not REPLACED.issubset(src.namelist()):raise SystemExit('BLOCKED: wrong source topology')
    for info in src.infolist():
        data=(TREE/info.filename).read_bytes() if info.filename in REPLACED else src.read(info.filename)
        dst.writestr(info,data,compress_type=info.compress_type,compresslevel=6)
    info=ZipInfo(NEW,date_time=(1980,1,1,0,0,0))
    info.compress_type=ZIP_DEFLATED
    info.create_system=3
    info.external_attr=0o100644<<16
    dst.writestr(info,(TREE/NEW).read_bytes(),compresslevel=6)
with ZipFile(PARENT) as src, ZipFile(OUT) as dst:
    if dst.testzip() is not None or len(dst.namelist())!=555:raise SystemExit('BLOCKED: invalid Run037 ZIP')
    same=sum(src.read(n)==dst.read(n) for n in src.namelist())
    if same!=552:raise SystemExit('BLOCKED: unapproved parent changes '+str(same))
report={'product':'Original-KOEI Enhanced','parent_run':'Run036','parent_sha256':PARENT_SHA,
        'run':'Run037','sha256':digest(OUT),'bytes':OUT.stat().st_size,'crc_clean_members':555,
        'inherited_unchanged':same,'inherited_total':554,'production_modified':sorted(REPLACED),
        'test_added':NEW,'source_status':'TESTED_PORT_PARTIAL_NOT_WINDOWS_RELEASE'}
print(json.dumps(report,indent=2))
(HERE/'output/RUN037_SOURCE_MANIFEST.json').write_text(json.dumps(report,indent=2)+'\n')