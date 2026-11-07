#!/usr/bin/env python3
"""Rebuild Run020 from exact immutable Run019 B ZIP; no Classic/DOS sources."""
from pathlib import Path
from hashlib import sha256
from zipfile import ZipFile, ZipInfo, ZIP_DEFLATED
import json, sys, difflib
BASE=Path('/mnt/data/rotk_enh_run020_work')
PARENT=BASE/'Run019_DriveVerified_9a47_ImmutableParent.zip'
WORK=BASE/'working'
OUT=BASE/'output'
PARENT_SHA='9a47e07278d5d98a9f3cedcdc0e366a4dbb435241ce9f1777c607def32e65ad6'
NEW_NAME='ROTK2_Enhanced_Run020_Q09_PlayerDefenderInitialSelection_FullSource_v1.00.zip'
CHANGES=['third_branch/officer_battle_adapter.py','tests/test_run020_q09_human_defender_selected.py']
def run():
    OUT.mkdir(exist_ok=True)
    assert sha256(PARENT.read_bytes()).hexdigest()==PARENT_SHA
    p=OUT/NEW_NAME
    with ZipFile(PARENT) as base, ZipFile(p,'w') as new:
        original=base.namelist()
        assert CHANGES[0] in original and CHANGES[1] not in original
        for item in base.infolist():
            raw=(WORK/item.filename).read_bytes() if item.filename==CHANGES[0] else base.read(item)
            new.writestr(item,raw)
        info=ZipInfo(CHANGES[1], date_time=(2026,11,7,0,0,0))
        info.compress_type=ZIP_DEFLATED
        info.create_system=3; info.external_attr=0o644 << 16
        new.writestr(info,(WORK/CHANGES[1]).read_bytes())
    with ZipFile(p) as obj:
        assert len(obj.infolist())==538 and obj.testzip() is None
        same=sum(obj.read(x)==ZipFile(PARENT).read(x) for x in original)
        assert same==536
    manifest={'product':'Original-KOEI Enhanced only','run':'Run020','direct_parent':'Drive-verified separate Run019 B',
       'parent_full_zip_sha256':PARENT_SHA,'new_full_zip_sha256':sha256(p.read_bytes()).hexdigest(),
       'size':p.stat().st_size,'zip_members':538,'parent_unmodified_members':536,'changed':CHANGES,
       'notes':'Do not treat parallel Run019 A (5887f9...) as the direct parent. Source-parity partial.'}
    (OUT/'RUN020_SOURCE_MANIFEST.json').write_text(json.dumps(manifest,indent=2)+'\n')
    print(json.dumps(manifest,indent=2))
    old=ZipFile(PARENT).read(CHANGES[0]).decode().splitlines(keepends=True)
    updated=(WORK/CHANGES[0]).read_text().splitlines(keepends=True)
    patch=''.join(difflib.unified_diff(old,updated,fromfile='Run019B/'+CHANGES[0],tofile='Run020/'+CHANGES[0]))
    (OUT/'RUN020_Q09_PRODUCTION.patch').write_text(patch)
    print('diff lines',len(patch.splitlines()))
if __name__=='__main__':run()