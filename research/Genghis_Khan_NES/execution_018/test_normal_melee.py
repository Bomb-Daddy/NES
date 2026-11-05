import unittest
from normal_melee import Fighter, strength, role_bonus, share, casualty, normal_melee
A=Fighter(40,124,102,25,25,25,25,100,0,2)
D=Fighter(70,121,130,96,96,96,96,100,0,3)
class NormalMeleeTests(unittest.TestCase):
 def test_first_native_controller_case(self):
  draws=iter([14,3]);out=normal_melee(A,D,lambda n:next(draws))
  self.assertEqual(out['attacker_strength'],44);self.assertEqual(out['defender_strength'],84)
  self.assertEqual(out['attacker_share'],39)
  self.assertEqual((out['attacker_remaining'],out['defender_remaining']),(24,60))
 def test_role_bonus(self):
  self.assertEqual([role_bonus(x) for x in [100,101,102,0]],[2,1,0,0])
 def test_strength_and_integer_floor(self):
  self.assertEqual(strength(A),44);self.assertEqual(strength(D),84)
 def test_ratio_clamp_and_zero_sum(self):
  self.assertEqual(share(0,0),50);self.assertEqual(share(0,15),1);self.assertEqual(share(15,0),99)
  self.assertEqual(share(32768,32768),99) # 16-bit sum wraps; original divide-zero returns FFFF.
 def test_loss_minimum_and_clamp(self):
  self.assertEqual(casualty(40,39,14),16)
  self.assertEqual(casualty(70,61,3),10)
  self.assertEqual(casualty(1,1,0),1)
 def test_bad_draw_is_rejected(self):
  with self.assertRaises(ValueError):normal_melee(A,D,lambda n:n)

class CapturedAndSourceVectors(unittest.TestCase):
 def test_new_native_cases(self):
  import json
  from pathlib import Path
  for c in json.loads(Path(__file__).with_name('native_melee_cases.json').read_text()):
   with self.subTest(case=c['case']):
    draws=iter(c['draw_values'])
    got=normal_melee(Fighter(**c['attacker']),Fighter(**c['defender']),lambda n:next(draws),defending_country_defense=c['defending_country_defense'])
    self.assertEqual(got,c['expected'])
    self.assertEqual(next(draws,None),None)
 def test_new_source_vectors(self):
  import json
  from pathlib import Path
  for v in json.loads(Path(__file__).with_name('source_arithmetic_vectors.json').read_text()):
   with self.subTest(vector=v):
    if v['kind']=='strength':self.assertEqual(strength(Fighter(**v['input'])),v['expected'])
    elif v['kind']=='share':self.assertEqual(share(v['a'],v['b']),v['expected'])
    else:self.assertEqual(v['n']-casualty(v['n'],v['share'],v['draw']),v['expected_remaining'])
 def test_wrong_matchup_rejected(self):
  import normal_melee as m
  old=m.MATCHUP
  try:
   m.MATCHUP=((4,5,5),(4,3,3),(4,4,3))
   draws=iter([14,3]);v=m.normal_melee(A,D,lambda n:next(draws))
   self.assertNotEqual((v['attacker_remaining'],v['defender_remaining']),(24,60))
  finally:m.MATCHUP=old

if __name__=='__main__':unittest.main()
