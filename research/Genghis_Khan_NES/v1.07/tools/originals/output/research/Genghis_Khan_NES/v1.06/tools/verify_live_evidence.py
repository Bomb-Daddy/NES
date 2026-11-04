"""Assert facts from captured original-game SRAM and traces, not a game model.
Run with GK_EVIDENCE=/path/to/evidence python verify_live_evidence.py.
The private evidence archive is linked from the accompanying report.
"""
from pathlib import Path
import os,json,hashlib,unittest
ROOT=Path(os.environ['GK_EVIDENCE'])
def ram(session,label):return (ROOT/session/(label+'.sram')).read_bytes()
def word(b,a):return int.from_bytes(b[a-0x6000:a-0x6000+2],'little')
def byte(b,a):return b[a-0x6000]
def facts(b):return tuple(word(b,a) for a in (0x6e0d,0x6df3,0x6df9,0x6e03,0x7a8a))+(byte(b,0x7b36),)
def roundtrip_ok(a,b,c):return facts(a)==(439,26,50,208,101,3) and facts(b)==(432,27,50,208,101,2) and facts(c)==(437,26,50,208,101,1)
class CapturedEvidenceChecks(unittest.TestCase):
 def test_all_snapshot_hashes(self):
  count=0
  for f in ROOT.glob('*/*.json'):
   s=json.loads(f.read_text())
   if isinstance(s,dict) and 'sram_sha256' in s:
    with self.subTest(file=str(f)):
     b=f.with_suffix('.sram').read_bytes();self.assertEqual(len(b),8192)
     self.assertEqual(hashlib.sha256(b).hexdigest(),s['sram_sha256'])
     self.assertTrue(f.with_suffix('.png').exists())
    count+=1
  self.assertGreater(count,50)
 def test_repeated_cold_start(self):
  for s in ('live_trade','live_sale','live_trace','roundtrip'):
   self.assertEqual(hashlib.sha256(ram(s,'baseline')).hexdigest(),'dd22847140cc0397ab8f42e29f1832681628ed52bfa17e95041c288afc4ef773')
 def test_actual_buy_and_sell_same_stock(self):
  a,b,c=[ram('roundtrip',l) for l in ('baseline','buy1_result','sale1_result')]
  self.assertTrue(roundtrip_ok(a,b,c))
 def test_mutated_purchase_stock_is_rejected(self):
  a,b,c=[ram('roundtrip',l) for l in ('baseline','buy1_result','sale1_result')]
  altered=bytearray(b);altered[0x6df3-0x6000]=26
  self.assertFalse(roundtrip_ok(a,altered,c))
 def test_buy11_and_hire8(self):
  a=ram('live_trade','return_menu');b=ram('live_trade','hire8_result')
  self.assertEqual((word(a,0x6e0d),word(a,0x6df3),word(a,0x6de5),byte(a,0x7b36)),(368,37,61,2))
  self.assertEqual((word(b,0x6e0d),word(b,0x6df3),word(b,0x6de5),byte(b,0x7b36)),(272,37,69,1))
  self.assertEqual((word(a,0x6e0f),word(b,0x6e0f)),(118,113))
 def test_second_commodity_is_not_food(self):
  a=ram('roundtrip','sale1_result');b=ram('roundtrip','food1_commit')
  self.assertEqual((word(a,0x6df9),word(b,0x6df9)),(50,51))
  self.assertEqual((word(a,0x6e03),word(b,0x6e03)),(208,208))
  self.assertEqual((word(a,0x6e0d),word(b,0x6e0d)),(437,434))
 def test_natural_summer_transition(self):
  a=ram('roundtrip','food_settled');b=ram('roundtrip','summer_progress2')
  self.assertEqual((word(a,0x7a8a),word(b,0x7a8a)),(101,93))
  self.assertEqual((byte(a,0x7b36),byte(b,0x7b36)),(0,3))
  for addr in (0x6df3,0x6df9,0x6e03,0x6e0d):self.assertEqual(word(a,addr),word(b,addr))
 def test_live_china_rate_doses_and_return_control(self):
  traces=[json.loads(l) for l in (ROOT/'roundtrip_vm_trace.jsonl').read_text().splitlines()]
  samples=[(69,'controlled_islam_buy_rate69_actual',180),(0,'controlled_islam_buy_rate0',180),(101,'controlled_islam_buy_rate101',180),(123,'controlled_islam_buy_rate123',180),(69,'controlled_islam_buy_rate69_repeat',180),(93,'controlled_islam_buy_rate93_restored',180)]
  for rate,label,window in samples:
   s=json.loads((ROOT/'roundtrip'/(label+'.json')).read_text());b=ram('roundtrip',label)
   rows=[t for t in traces if s['frame']-window<=t['frame']<=s['frame'] and t['vm_pc']==0xdd00]
   with self.subTest(rate=rate,label=label):
    self.assertEqual(word(b,0x7a8a),rate)
    self.assertEqual({(t['table_row'],t['table_column'],t['a']) for t in rows},{(0,3,18*(rate+100)//100),(0,5,11*(rate+100)//100),(0,7,8*(rate+100)//100)})
    self.assertTrue(all(t['rate']==rate for t in rows))
    self.assertEqual((word(b,0x6e0d),word(b,0x6e03),byte(b,0x7b36)),(434,208,3))
 def test_clean_start_rate190_price_quote(self):
  b=ram('rate190','china_buy_rate190');s=json.loads((ROOT/'rate190/china_buy_rate190.json').read_text())
  rows=[json.loads(l) for l in (ROOT/'rate190_vm_trace.jsonl').read_text().splitlines()]
  rows=[t for t in rows if s['frame']-180<=t['frame']<=s['frame'] and t['vm_pc']==0xdd00]
  self.assertEqual({(t['table_row'],t['table_column'],t['a']) for t in rows},{(0,2,92),(0,5,31)})
  self.assertTrue(all(t['rate']==190 for t in rows))
  self.assertEqual((word(b,0x7a8a),word(b,0x6e0d),byte(b,0x7b36)),(190,439,3))
if __name__=='__main__':unittest.main(verbosity=2)
