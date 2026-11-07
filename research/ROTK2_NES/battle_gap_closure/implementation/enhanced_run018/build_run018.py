"""Reproducible Enhanced Run018 full source from immutable Run017 ZIP + 3 files.
Actual parent SHA verification; not a finished release. Original-game only.
NOTE: adjust parent path for the developer's working environment without
changing the pinned hash. The canonical Drive Run017 full ZIP is the parent.
"""
from pathlib import Path
import hashlib, zipfile, json, difflib

root=Path(__file__).parent
parent=Path('/mnt/data/rotk_enh_run017_work/output/ROTK2_Enhanced_Run017_Q14_SourceGatedRulerSuccession_FullSource_v1.00.zip')
parent_sha='d052013bff0cad1a3fc9be9902647f9d087aba19f6c19609d9aaa9092ec115a6'
modified=('third_branch/officer_battle_adapter.py','third_branch/integrated.py')
new='tests/test_run018_q09_attacked_arrival.py'
if hashlib.file_digest(parent.open('rb'),'sha256').hexdigest()!=parent_sha:
    raise RuntimeError('PARENT HASH MISMATCH')
outdir=root/'output';outdir.mkdir(exist_ok=True)
output=outdir/'ROTK2_Enhanced_Run018_Q09_AttackedLiveArrival_Partial_FullSource_v1.00.zip'
file_manifest=[];changes=[]
with zipfile.ZipFile(parent,'r') as source,zipfile.ZipFile(output,'w') as dest:
    assert source.testzip() is None
    originals=set(source.namelist())
    assert new not in originals
    for zi in source.infolist():
        before=source.read(zi)
        body=(root/'patched'/zi.filename).read_bytes() if zi.filename in modified else before
        dest.writestr(zi,body)
        file_manifest.append({'path':zi.filename,'sha256':hashlib.sha256(body).hexdigest(),'changed':body!=before})
        if body!=before:
            assert zi.filename in modified
            diff=''.join(difflib.unified_diff(before.decode('utf-8').splitlines(keepends=True),
                body.decode('utf-8').splitlines(keepends=True),
                fromfile='Run017/'+zi.filename,tofile='Run018/'+zi.filename))
            changes.append(diff)
    assert sum(r['changed'] for r in file_manifest)==2
    zi=zipfile.ZipInfo(new,date_time=(2026,11,6,0,0,0))
    zi.compress_type=zipfile.ZIP_DEFLATED
    zi.external_attr=0o644<<16
    body=(root/'patched'/new).read_bytes()
    dest.writestr(zi,body)
    file_manifest.append({'path':new,'sha256':hashlib.sha256(body).hexdigest(),'changed':True,'new':True})
with zipfile.ZipFile(output) as z:
    assert z.testzip() is None
    assert len(z.namelist())==535
outsha=hashlib.file_digest(output.open('rb'),'sha256').hexdigest()
manifest={'project':'Original-KOEI Enhanced, NOT NES Classic','version':'Run018',
  'classification':'TESTED_PORT/PARTIAL pending release acceptance',
  'parent_version':'Run017','parent_sha256':parent_sha,
  'source_sha256':outsha,'size':output.stat().st_size,'entries':len(file_manifest),
  'source_rule':'hsovl2 D648/D69A original source; not original-emulator replay',
  'changed_files':list(modified)+[new],'files':file_manifest}
(outdir/'RUN018_SOURCE_MANIFEST.json').write_text(json.dumps(manifest,indent=2)+'\n')
(outdir/'RUN018_PRODUCTION.patch').write_text('\n'.join(changes))
print('SOURCE',output,'SHA256',outsha,'size',output.stat().st_size,'entries',len(file_manifest))
