"""Rebuild exact Enhanced Q03 candidate only from verified Run004 full source ZIP.
This is the separate original-game Enhanced tree; never the NES canonical source.
"""
from pathlib import Path
import zipfile, hashlib, json, sys
BASE=Path('/mnt/data/ROTK2_Enhanced46_BPF_Run004_Q14_Remote_Refuge_Source_v1.03.zip')
ROOT=Path('/mnt/data/enhanced_run004_source')
OUT=Path('/mnt/data/rotk_enh_q03_run015/ROTK2_Enhanced46_BPF_Run005_Q03_ResidentDefenders_FullSource_v1.00.zip')
PARENT_SHA='7b2891d3c7d7b6ef371b2e06926fce59bda1b0f480160ccb8fc32c9e286ec229'
PATCHED=['third_branch/officer_battle_adapter.py','third_branch/army_cohorts.py']
ADDED=['tests/test_bpf_q03_resident_enhanced.py']
assert hashlib.sha256(BASE.read_bytes()).hexdigest()==PARENT_SHA,'WRONG ENHANCED PARENT SOURCE!'
with zipfile.ZipFile(BASE) as prev,zipfile.ZipFile(OUT,'w') as target:
    previous=prev.namelist()
    assert all(p in previous for p in PATCHED)
    assert all(p not in previous for p in ADDED)
    entries=[]
    for meta in prev.infolist():
        body=(ROOT/meta.filename).read_bytes() if meta.filename in PATCHED else prev.read(meta.filename)
        target.writestr(meta,body)
        entries.append({'path':meta.filename,'sha256':hashlib.sha256(body).hexdigest(),'modified':meta.filename in PATCHED})
    for p in ADDED:
        body=(ROOT/p).read_bytes()
        info=zipfile.ZipInfo(p,date_time=(2026,11,6,12,0,0));info.compress_type=zipfile.ZIP_DEFLATED
        target.writestr(info,body)
        entries.append({'path':p,'sha256':hashlib.sha256(body).hexdigest(),'new':True})
with zipfile.ZipFile(OUT) as final:
    assert final.testzip() is None
    assert len(final.namelist())==len(entries)
    for row in entries:
        assert hashlib.sha256(final.read(row['path'])).hexdigest()==row['sha256']
report={'project':'Original-KOEI Enhanced (not canonical NES)','parent':str(BASE),'parent_sha256':PARENT_SHA,'child_sha256':hashlib.sha256(OUT.read_bytes()).hexdigest(),'size':OUT.stat().st_size,'entries':len(entries),'patch':PATCHED,'new':ADDED,'entries_manifest':entries,'test_sources':['Q03 original 19000+1000=666; Classic port same arithmetic; static SNES/NES reference']}
OUT.with_suffix('.manifest.json').write_text(json.dumps(report,indent=2))
print('PARENT',PARENT_SHA,'CHILD',report['child_sha256'],'SIZE',report['size'],'ENTRIES',len(entries))
