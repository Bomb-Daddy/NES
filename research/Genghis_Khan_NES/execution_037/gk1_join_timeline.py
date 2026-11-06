#!/usr/bin/env python3
"""EXEC0037: temporal audit of already-authorized modified-ROM Mesen snapshots.

This does not execute or emulate the NES, reveal ROM bytes, interpret unknown
scratch fields, or prove which banked VM routine implements a friendly join.
The seven named snapshots are successive, different VM phases; the event timing
is bracketed, not assigned to a specific instruction.
"""
from __future__ import annotations
import argparse
import hashlib
import io
import json
from pathlib import Path
import zipfile

ROM_SHA='4764d46359581be1e7d12bcb85bd375c84cd4497d1084d9d306a6e884afbe7c5'
CHECKPOINT_SHA='e1b6266c963a48d736627812d024910b6d600c2ad8571813eb3b0999e0f2938b'
PRIVATE_ZIP_SHA='aec170e8661a0011e373024caeb858f2657b716574b5f8555b63703e5cfd28f1'
STAGES=('original_checkpoint_loaded','controlled_intervention','selected_unit',
        'selected_destination','after_attempt','after_idle','after_b')
CASES=('insufficient_mobility','exact_mobility','friendly_occupied','enemy_occupied')
class EvidenceError(ValueError): pass

def sha(x: bytes) -> str: return hashlib.sha256(x).hexdigest()
def unit(r: bytes, i: int):
    j=0x60+11*i
    return {'soldiers':int.from_bytes(r[j:j+2],'little'),
            'x':r[j+2],'y':r[j+3],'mobility':r[j+5]}
def scratch(r: bytes, cpu: int) -> int: return r[cpu-0x6000]
def word(r: bytes) -> int: return int.from_bytes(r[0x13e:0x140],'little')

def inspect_archive(blob: bytes, require_zip_sha: bool=True) -> dict:
    if require_zip_sha and sha(blob)!=PRIVATE_ZIP_SHA:
        raise EvidenceError('private evidence ZIP SHA256 mismatch')
    try: z=zipfile.ZipFile(io.BytesIO(blob),'r')
    except (OSError,zipfile.BadZipFile) as exc: raise EvidenceError('not a ZIP') from exc
    if z.testzip() is not None: raise EvidenceError('ZIP CRC failure')
    found={}
    for case in CASES:
        found[case]={}
        for stage in STAGES:
            base=f'private/{case}/{stage}'
            try:
                raw=z.read(base+'.sram'); zp=z.read(base+'.zp')
                j=json.loads(z.read(base+'.json'))
            except (KeyError,ValueError) as exc:
                raise EvidenceError(f'missing/invalid stage {case}:{stage}') from exc
            if len(raw)!=8192 or len(zp)!=256: raise EvidenceError('snapshot size')
            if j.get('case')!=case or j.get('phase')!=stage:
                raise EvidenceError('phase or case label mismatch')
            if j.get('rom_sha256')!=ROM_SHA or j.get('checkpoint_sha256')!=CHECKPOINT_SHA:
                raise EvidenceError('wrong oracle source identity')
            if sha(raw)!=j.get('sram_sha256') or sha(zp)!=j.get('zp_sha256'):
                raise EvidenceError('snapshot hash mismatch')
            a,b=unit(raw,0),unit(raw,1)
            if (j['unit0_soldiers'],j['unit0_xy'],j['unit0_mobility'])!=(a['soldiers'],[a['x'],a['y']],a['mobility']) or (j['unit1_soldiers'],j['unit1_xy'],j['unit1_mobility'])!=(b['soldiers'],[b['x'],b['y']],b['mobility']):
                raise EvidenceError('unit metadata mismatch')
            if word(raw)!=j['completion_word'] or int(j['vm_pc_le'],16)!=int.from_bytes(zp[6:8],'little'):
                raise EvidenceError('scratch/VM metadata mismatch')
            if j['target_terrain_byte']!=raw[0x1d4]: raise EvidenceError('terrain metadata mismatch')
            found[case][stage]={'sram':raw,'zp':zp,'meta':j,'unit0':a,'unit1':b,
                  'word':word(raw),'raw614c':scratch(raw,0x614c),
                  'raw614e':scratch(raw,0x614e),'vm_source_pointer':int.from_bytes(zp[6:8],'little')}
    def eq(c,s,t,f):return found[c][s][f]==found[c][t][f]
    c='friendly_occupied'; f=found[c]
    for stage in ('controlled_intervention','selected_unit','selected_destination'):
        s=f[stage]
        if (s['unit0']['soldiers'],s['unit1']['soldiers'],s['word'],s['raw614e'],s['raw614c'])!=(40,40,0,5,14):
            raise EvidenceError('join already committed before witnessed attempt')
    a=f['after_attempt']; b=f['after_idle']; d=f['after_b']; pre=f['selected_destination']
    for s in (a,b,d):
        if (s['unit0']['soldiers'],s['unit0']['x'],s['unit0']['y'],
            s['unit1']['soldiers'],s['unit1']['x'],s['unit1']['y'],s['word'],s['raw614e'])!=(74,8,7,0,8,7,2,3):
            raise EvidenceError('join/word2/614e evidence mismatch')
    if not (pre['raw614c']==14 and a['raw614c']==14 and b['raw614c']==22 and d['raw614c']==22):
        raise EvidenceError('late scratch614c timing mismatch')
    for case in ('insufficient_mobility','enemy_occupied'):
        for stage in ('controlled_intervention','after_attempt','after_idle','after_b'):
            s=found[case][stage]
            if (s['unit0']['soldiers'],s['unit0']['x'],s['word'],s['raw614e'])!=(40,7,0,5):
                raise EvidenceError(f'negative-control unaccepted movement: {case}/{stage}')
    for stage in ('after_attempt','after_idle','after_b'):
        s=found['exact_mobility'][stage]
        if (s['unit0']['soldiers'],s['unit0']['x'],s['word'],s['raw614e'])!=(34,8,1,5):
            raise EvidenceError('no-join exact-mobility comparator')
    # Narrow, byte-level observation: a late 0x614C write follows the already
    # visible friendly join. It does not prove causality or source instructions.
    return {
      'audit':'EXEC0037 archived modified-ROM temporal bracket',
      'method':'hash-verified stages, not new Mesen execution',
      'rom_sha256':ROM_SHA,'checkpoint_sha256':CHECKPOINT_SHA,'evidence_sha256':sha(blob),
      'cases':len(CASES),'snapshots':len(CASES)*len(STAGES),
      'friendly_join_first_observed':'after_attempt',
      'friendly_join_before':'selected_destination',
      'units_first_observed':{'mover_soldiers':74,'partner_soldiers':0,'both_at':[8,7]},
      'word_613e_first_observed':2,
      'scratch_614e_first_observed':{'before':5,'after_attempt':3},
      'scratch_614c_later':{'after_attempt':14,'after_idle':22},
      'negative_controls':['insufficient_mobility','enemy_occupied','exact_mobility'],
      'unproven':['which VM writer increments word twice','meaning of scratch $614E/$614C',
                  'general merge legality','sequence inside selected_destination→after_attempt',
                  'Windows gameplay parity']}

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument('--private',type=Path,required=True)
    ap.add_argument('--out',type=Path,default=None)
    args=ap.parse_args()
    result=inspect_archive(args.private.read_bytes())
    t=json.dumps(result,sort_keys=True,indent=2)+'\n'
    if args.out: args.out.write_text(t)
    print(t,end='')
if __name__=='__main__':main()
