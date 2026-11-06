import dataclasses, json, os, unittest
from pathlib import Path
from movement_legality_contract import *

INPUTS = (
 ('insufficient_mobility',2,Occupancy.EMPTY,Disposition.REJECT),
 ('exact_mobility',4,Occupancy.EMPTY,Disposition.COMMIT_FINISHED),
 ('friendly_occupied',10,Occupancy.FRIENDLY_SLOT1,Disposition.FRIENDLY_COMBINE_OBSERVED),
 ('enemy_occupied',10,Occupancy.ENEMY_SLOT10,Disposition.REJECT),
)
ROOT=Path(os.environ.get('GK1_EXEC0033_PRIVATE','/not-present'))

class TestPortableOracle(unittest.TestCase):
 def test_all_four_bounded_results(self):
  for name,mob,occ,disp in INPUTS:
   with self.subTest(case=name):
    got=known_checkpoint_observation(name,40,3,mob,occ)
    self.assertEqual(got.disposition,disp)
    self.assertEqual(got.case,name)
 def test_low_mobility_does_not_consume_rng_or_increment_turn(self):
  got=known_checkpoint_observation('insufficient_mobility',40,3,2,Occupancy.EMPTY)
  self.assertEqual((got.xy0,got.soldiers0,got.mobility0,got.completion_word,got.rng_hex),((7,7),40,2,0,'c7c3f28f2363'))
 def test_exact_mobility_auto_finish(self):
  got=known_checkpoint_observation('exact_mobility',40,3,4,Occupancy.EMPTY)
  self.assertEqual((got.xy0,got.soldiers0,got.mobility0,got.completion_word),((8,7),34,10,1))
 def test_friendly_destination_is_not_assumed_blocked(self):
  got=known_checkpoint_observation('friendly_occupied',40,3,10,Occupancy.FRIENDLY_SLOT1)
  self.assertEqual((got.soldiers0,got.soldiers1,got.xy1,got.completion_word),(74,0,(8,7),2))
 def test_enemy_destination_rejected(self):
  got=known_checkpoint_observation('enemy_occupied',40,3,10,Occupancy.ENEMY_SLOT10)
  self.assertEqual((got.xy0,got.soldiers0,got.rng_hex,got.completion_word),((7,7),40,'c7c3f28f2363',0))
 def test_unverified_cases_refused(self):
  bad=(('other',40,3,2,Occupancy.EMPTY),('exact_mobility',40,1,4,Occupancy.EMPTY),
       ('exact_mobility',39,3,4,Occupancy.EMPTY),('exact_mobility',40,3,3,Occupancy.EMPTY),
       ('friendly_occupied',40,3,10,Occupancy.EMPTY),('enemy_occupied',40,3,10,Occupancy.FRIENDLY_SLOT1),
       ('exact_mobility',True,3,4,Occupancy.EMPTY),('exact_mobility',40,3,4,'empty'))
  for args in bad:
   with self.subTest(args=args),self.assertRaises(UnverifiedMoveState): known_checkpoint_observation(*args)
 def test_mutations_caught(self):
  o=known_checkpoint_observation('exact_mobility',40,3,4,Occupancy.EMPTY)
  d={'case':o.case,'phase':'after_idle','rom_sha256':ROM_SHA256,
     'checkpoint_sha256':CHECKPOINT_SHA256,'unit0_soldiers':34,'unit0_xy':[8,7],
     'unit0_mobility':10,'unit1_soldiers':40,'unit1_xy':[6,9],
     'completion_word':1,'rng':'a31e4ecd8fd1','gold':403,'food':198,'target_terrain_byte':3}
  verify_observed_phase(o,d)
  for key,new in [('unit0_soldiers',35),('unit0_xy',[7,7]),('completion_word',0),
                  ('rng','c7c3f28f2363'),('phase','selected_unit'),('unit0_mobility',0)]:
    altered=dict(d);altered[key]=new
    with self.subTest(field=key),self.assertRaises(AssertionError): verify_observed_phase(o,altered)

class TestNativeEvidence(unittest.TestCase):
 def test_four_native_results(self):
  if not ROOT.is_dir(): self.skipTest('licensed private SRAM not installed')
  for name,mob,occ,disp in INPUTS:
   with self.subTest(case=name):
    o=known_checkpoint_observation(name,40,3,mob,occ)
    d=json.loads((ROOT/name/'after_idle.json').read_text())
    verify_observed_phase(o,d)
 def test_reject_native_snapshot_mutation(self):
  if not ROOT.is_dir(): self.skipTest('licensed private SRAM not installed')
  o=known_checkpoint_observation('friendly_occupied',40,3,10,Occupancy.FRIENDLY_SLOT1)
  d=json.loads((ROOT/'friendly_occupied'/'after_idle.json').read_text())
  d['unit1_soldiers']=40
  with self.assertRaises(AssertionError):verify_observed_phase(o,d)

if __name__ == '__main__':unittest.main()

class TestPrivateIntegrity(unittest.TestCase):
 def test_complete_source_receipt_28_snapshots(self):
  if not ROOT.is_dir():self.skipTest('licensed private SRAM not installed')
  from audit_private_evidence import check
  r=check(ROOT)
  self.assertEqual(r['status'],'PASS')
  self.assertEqual(sum(v['stages_checked'] for v in r['cases'].values()),28)
 def test_corrupted_snapshot_detected(self):
  if not ROOT.is_dir():self.skipTest('licensed private SRAM not installed')
  from audit_private_evidence import hashbytes
  p=ROOT/'friendly_occupied'/'after_idle.sram'
  b=bytearray(p.read_bytes());b[0x60]^=1
  recorded=json.loads((ROOT/'friendly_occupied'/'after_idle.json').read_text())
  self.assertNotEqual(hashbytes(b),recorded['sram_sha256'])
