"""EXEC0033: strict private-source receipt verifier for four controlled native cases."""
from pathlib import Path
import argparse,hashlib,json
from movement_legality_contract import *

STAGES=('original_checkpoint_loaded','controlled_intervention','selected_unit','selected_destination','after_attempt','after_idle','after_b')
CASES={
    'insufficient_mobility':(2,Occupancy.EMPTY,(0x65,)),
    'exact_mobility':(4,Occupancy.EMPTY,(0x65,)),
    'friendly_occupied':(10,Occupancy.FRIENDLY_SLOT1,(0x6d,0x6e)),
    'enemy_occupied':(10,Occupancy.ENEMY_SLOT10,(0xd0,0xd1)),
}
class CorruptEvidence(AssertionError):pass

def hashbytes(b):return hashlib.sha256(b).hexdigest()
def check(root:Path):
  receipt={'cases':{},'rom_sha256':ROM_SHA256,'checkpoint_sha256':CHECKPOINT_SHA256,'status':'PASS',
           'provenance':'controlled approved combined NES ROM; MesenCE original first-battle checkpoint; SRAM interventions disclosed; not Windows QA'}
  for name,(mob,occ,offsets) in CASES.items():
    base=root/name
    entries={}
    for stage in STAGES:
      d=json.loads((base/(stage+'.json')).read_text())
      s=(base/(stage+'.sram')).read_bytes(); z=(base/(stage+'.zp')).read_bytes()
      if len(s)!=8192 or len(z)!=256:raise CorruptEvidence(f'{name}/{stage} wrong byte lengths')
      if d.get('rom_sha256')!=ROM_SHA256 or d.get('checkpoint_sha256')!=CHECKPOINT_SHA256:
        raise CorruptEvidence(f'{name}/{stage} wrong reference identities')
      if d.get('case')!=name or d.get('phase')!=stage:raise CorruptEvidence(f'{name}/{stage} mismatched identity')
      if hashbytes(s)!=d['sram_sha256'] or hashbytes(z)!=d['zp_sha256']:
        raise CorruptEvidence(f'{name}/{stage} corrupt SRAM/zero-page bytes')
      expect={
       'unit0_soldiers':int.from_bytes(s[0x60:0x62],'little'),
       'unit0_xy':list(s[0x62:0x64]),'unit0_mobility':s[0x65],
       'unit1_soldiers':int.from_bytes(s[0x6b:0x6d],'little'),
       'unit1_xy':list(s[0x6d:0x6f]),'unit1_mobility':s[0x70],
       'target_terrain_byte':s[0x1d4],
       'completion_word':int.from_bytes(s[0x13e:0x140],'little'),
       'rng':z[0x84:0x8a].hex(),
       'vm_pc_le':hex(z[6]+(z[7]<<8)),
       'gold':int.from_bytes(s[0xe0d:0xe0f],'little'),
       'food':int.from_bytes(s[0xe03:0xe05],'little'),
      }
      for k,v in expect.items():
        if d.get(k)!=v:raise CorruptEvidence(f'{name}/{stage} {k} numeric mismatch')
      entries[stage]=(s,z,d)
    before=entries['original_checkpoint_loaded'][0]
    controlled=entries['controlled_intervention'][0]
    changed={i for i,(a,b) in enumerate(zip(before,controlled)) if a!=b}
    if changed!=set(offsets):raise CorruptEvidence(f'{name}: intervention touched {changed}, expected {set(offsets)}')
    if before[0x60:0x66]!=bytes([40,0,7,7,before[0x64],10]):
      raise CorruptEvidence(f'{name}: invalid initial unit0 checkpoint')
    # The exact intervention values matter. No other SRAM bytes may change before acting.
    if name.endswith('mobility'):
      if controlled[0x65]!=mob: raise CorruptEvidence('wrong intervention mobility')
    elif name=='friendly_occupied':
      if controlled[0x6d:0x6f]!=b'\x08\x07':raise CorruptEvidence('wrong friendly intervention')
    elif name=='enemy_occupied':
      if controlled[0xd0:0xd2]!=b'\x08\x07':raise CorruptEvidence('wrong enemy intervention')
    observed=known_checkpoint_observation(name,40,3,mob,occ)
    verify_observed_phase(observed,entries['after_idle'][2])
    initial=entries['controlled_intervention'][2]
    final=entries['after_idle'][2]
    for key in ('gold','food','target_terrain_byte'):
      if initial[key]!=final[key]: raise CorruptEvidence(f'{name}: strategic/tile mutation unexpectedly changed {key}')
    # Exhaustive unchanged unit-record audit; only 0 and (when friendly) 1 may change.
    old,new=entries['controlled_intervention'][0],entries['after_idle'][0]
    unit_changes=[j for j in range(20) if old[0x60+j*11:0x60+(j+1)*11]!=new[0x60+j*11:0x60+(j+1)*11]]
    expected=[0,1] if name=='friendly_occupied' else ([0] if name=='exact_mobility' else [])
    if unit_changes!=expected:raise CorruptEvidence(f'{name}: unit records changed {unit_changes}, expected {expected}')
    receipt['cases'][name]={
      'intervention_sram_offsets_hex':[hex(i+0x6000) for i in sorted(changed)],
      'changed_unit_indices':unit_changes,
      'original_sram_sha256':hashbytes(before),
      'final_sram_sha256':hashbytes(new),
      'final_zp_sha256':hashbytes(entries['after_idle'][1]),
      'phase':'after_idle',
      'native_case_result':{'soldiers0':observed.soldiers0,'xy0':list(observed.xy0),'mobility0':observed.mobility0,
        'completion_word':observed.completion_word,'rng':observed.rng_hex},
      'stages_checked':len(entries),
    }
  return receipt

if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--private',required=True,type=Path);p.add_argument('--out',type=Path)
 args=p.parse_args();r=check(args.private);x=json.dumps(r,indent=2,sort_keys=True)+'\n'
 if args.out:args.out.write_text(x)
 print(x)
