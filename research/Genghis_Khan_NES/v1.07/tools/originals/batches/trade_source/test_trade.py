import unittest,json,hashlib
from replay_trade import *
SELL_P=[[30,15,7,10,18,20,20,23,10,12,40],[40,5,8,7,23,15,15,25,17,3,35],[30,8,10,15,10,25,15,1,6,3,45]]
BUY_P=[[20,5,15,10,10,10,20,15,20,3,40,12,10,10,25,10,10],[1,20,10,7,5,20,10,10,7,20,35,7,9,20,20,10,15],[10,20,7,5,10,15,10,5,15,20,40,25,15,5,15,10,10]]
def next_rng(seed):
 new=(-(seed|1)*0xe0900e0187c5)&((1<<48)-1)
 return new, (new>>32)&0x7fff
def model_offers(seed):
 sell=[];buy=[]
 for m in range(3):
  for n,prob,out in ((11,SELL_P[m],sell),(17,BUY_P[m],buy)):
   seed,r=next_rng(seed);forced=r%n;row=[]
   for i in range(n):
    if i==forced:row.append(1)
    else:seed,r=next_rng(seed);row.append(int(r%100<prob[i]))
   out.append(row)
 return sell,buy,seed
class TradeChecks(unittest.TestCase):
 def test_private_source_hash(self):self.assertEqual(hashlib.sha256(h.ROM.read_bytes()).hexdigest(),h.EXPECTED)
 def test_rng_source_vs_independent_48bit_model(self):
  for seed in (0,1,2,0x010203040506,0xffffffffffff,0x800000000000,0x123456789abc):
   for n in (1,2,11,17,100,999):
    new,r=next_rng(seed);actual,new2,_=rng(seed,n);self.assertEqual((actual,new2),(r%n,new))
 def test_rng_zero_bound_does_not_advance_seed(self):
  c=prepare(0xdb98,[0],seed=9);run(c,0xdb9d);self.assertEqual(word(c,8),0);self.assertEqual(int.from_bytes(c.m[0x84:0x8a],'big'),9)
 def test_weighted_average_boundaries(self):
  for a,t,b,n in [(0,0,100,0),(999,0,100,1),(110,103,100,8),(124,103,100,8),(0,998,999,1),(999,998,0,1),(100,500,100,499),(1500,1,1500,1)]:
   self.assertEqual(avg(a,t,b,n),min(999,(a*t+b*n)//(t+n)) if t+n else 0)
 def test_average_uses_all_five_manpower_fields(self):
  for k in range(5):
   counts=[0]*5;counts[k]=103;r=hire(110,124,118,counts,8)
   self.assertEqual((r['arms'],r['skill'],r['morale']),(109,122,113));self.assertEqual(r['counts'],counts)
 def test_hire_morale_seed_grid(self):
  for seed in range(20):
   _,draw=next_rng(seed);r=hire(110,124,118,[61,5,4,6,27],8,seed)
   self.assertEqual(r['morale'],(118*103+(50+draw%11)*8)//111)
 def test_hire_zero_old_manpower(self):
  r=hire(999,999,999,[0]*5,8);self.assertEqual((r['arms'],r['skill'],r['morale']),(100,100,50))
 def test_hire_total_cap(self):
  for total in (0,1,103,998,999):
   for gold in (0,1,12,439,999):self.assertEqual(buy_cap(gold,120,999-total),min(999-total,gold*10//120))
 def test_hire_complete_commit_reproduces_observed_state(self):
  r=hire_commit(110,124,118,[61,5,4,6,27],8,368,120)
  self.assertEqual({k:r[k] for k in ('arms','skill','morale','troops','gold')},{'arms':109,'skill':122,'morale':113,'troops':69,'gold':272})
 def test_hire_mutated_incoming_arms(self):
  orig=hire(110,124,118,[61,5,4,6,27],8);altered=hire(110,124,118,[61,5,4,6,27],8,mutation={0x8694:0})
  self.assertNotEqual(orig['arms'],altered['arms']);self.assertEqual(orig['skill'],altered['skill'])
 def test_sale_rounding_grid_and_cap(self):
  for gold in (0,1,432,998,999):
   for quote in (1,7,18,50,255,999):
    stock=999;cap=sale_cap(gold,stock,quote);self.assertEqual(cap,min(stock,(999-gold)*10//quote))
    for q in set((0,1,cap)):
     if q<=cap:
      r=sale(gold,stock,q,quote);self.assertEqual((r['gold'],r['stock']),(min(999,gold+q*quote//10),stock-q))
 def test_sale_max_stock_constrains_quantity(self):self.assertEqual(sale_cap(0,3,7),3)
 def test_sale_observed_fur_examples(self):
  for n,credit in ((1,0),(7,4),(9,6)):self.assertEqual(sale(100,10,n,7)['gold'],100+credit)
 def test_sale_unreachable_overcap_saturates_credit(self):self.assertEqual(sale(998,20,10,50)['gold'],999)
 def test_sale_mutated_divisor(self):self.assertNotEqual(sale(100,20,7,7),sale(100,20,7,7,mutation={0x8c2a:0x55}))
 def test_sale_counterexample_zero_gold_capacity(self):
  self.assertEqual(sale_cap(999,10,1),0) # conservative cap disallows even zero-credit single-unit sale
 def test_buy_affordability_grid(self):
  for g in (0,1,7,30,439,999):
   for p in (1,7,18,64,120,500):
    for cap in (0,1,26,999):self.assertEqual(buy_cap(g,p,cap),min(cap,10*g//p))
 def test_weapons_all_six_base_values(self):
  for i,b in enumerate((20,80,100,40,60,150),11):
   for qty in (1,2,9,99):
    self.assertEqual(weapon_gain(110,103,qty,i,b),min(999,110+(9*b*(qty+1))//103))
 def test_weapon_plus_one_matters(self):self.assertEqual(weapon_gain(110,103,1,11,20),113)
 def test_weapon_capacity_and_gold_bound(self):
  for arms,total,base,gold,quote in ((110,103,20,439,40),(998,1,150,999,500),(0,999,100,999,200),(999,103,20,439,40)):
   self.assertEqual(weapon_cap(arms,total,11,base,gold,quote),min(999,total*(999-arms)//base,gold*10//quote))
 def test_weapon_quantity_plus_one_mutation(self):self.assertNotEqual(weapon_gain(110,103,1,11,20),weapon_gain(110,103,1,11,20,mutation={0x8d0d:0xd1}))
 def test_offer_source_arrays_vs_independent_model(self):
  for seed in (0,1,0x010203040506,0xffffffffffff):
   r=offers(seed);sell,buy,end=model_offers(seed);self.assertEqual((r['sell'],r['buy'],r['seed']),(sell,buy,end))
 def test_offer_at_least_one_and_boolean(self):
  r=offers(0x123456789abc)
  for row in r['sell']+r['buy']:self.assertGreaterEqual(sum(row),1);self.assertLessEqual(set(row),{0,1})
 def test_offer_masks_are_unchanged_by_sale_commit(self):
  c=prepare(0x8c20,[COUNTRY],words={COUNTRY+64:100,COUNTRY+34:10},locals={-4:1,-12:COUNTRY+34,-8:7,-10:COUNTRY+64});c.m[0x7a8c:0x7b34]=bytes([1,0])*84;before=bytes(c.m[0x7a8c:0x7b34]);run(c,0x8c33);self.assertEqual(bytes(c.m[0x7a8c:0x7b34]),before)
 def test_merchant_presence_mongol_formula(self):
  for e,m in ((0,0),(185,118),(999,999)):
   seed=0x010203040506;expected=[]
   for cutoff in (25,15,40):seed,r=next_rng(seed);expected.append(int(r%100>cutoff+(e*m)//50000))
   got=presence(e,m);self.assertEqual((got['flags'],got['seed']),(expected,seed))
 def test_merchant_presence_world_table_countdown(self):
  for remaining in (1,14,26,27):
   seed=0x010203040506;expected=[]
   bank=PRG[16384:2*16384]
   for row in range(3):
    a=0xb1c6-0x8000+row*54+remaining*2;cutoff=int.from_bytes(bank[a:a+2],'little');seed,r=next_rng(seed);expected.append(int(r%100>cutoff))
   self.assertEqual(presence(0,0,27,remaining)['flags'],expected)
 def test_judgment_cost_and_saturating_debit(self):
  for value in (0,1,4,5,6,77,999):self.assertEqual(ability(value),[value,max(0,value-5),value,value,value,value])
 def test_judgment_eligibility_boundary(self):
  for value in (0,1,4,5,6,77):self.assertEqual(ability(value,'gate'),value>=5)
 def test_trade_result_code_orders_cost(self):
  c=prepare(0x8e46,words={0x7b36:3});run(c,0x8e4d);self.assertEqual(word(c,0x7b36),2)
 def test_trade_cancel_skips_ability_and_orders(self):
  c=prepare(0x8e38,words={0x7b36:3});put(c,8,255);run(c,0x8e50);self.assertEqual(word(c,0x7b36),3)
 def test_original_baseline_ability_table(self):
  b=(ROOT/'evidence/roundtrip/baseline.sram').read_bytes();self.assertEqual(hashlib.sha256(b).hexdigest(),'dd22847140cc0397ab8f42e29f1832681628ed52bfa17e95041c288afc4ef773');self.assertEqual(list(b[0x7e4e-0x6000:0x7e54-0x6000]),[0,5,0,0,0,0])
 def test_offer_threshold_negative_control(self):
  for chance in (0,100):
   mutation={}
   for a,n in ((0xafb2,33),(0xaff4,51)):
    for i in range(n):mutation[a+2*i]=chance;mutation[a+2*i+1]=0
   r=offers(0x010203040506,mutation)
   for row in r['sell']+r['buy']:self.assertEqual(sum(row),1 if chance==0 else len(row))
 def test_buy_complete_commodity_commit(self):
  r=buy_commit(439,26,11,64,index=2);self.assertEqual((r['gold'],r['stock']),(368,37));self.assertTrue(r['merchant_masks_unchanged'])
 def test_buy_complete_weapon_commit(self):
  r=buy_commit(439,0,1,40,index=11);self.assertEqual((r['gold'],r['arms']),(435,113));self.assertTrue(r['merchant_masks_unchanged'])
 def test_quantity_lower_upper_and_cancel_gate(self):
  for value in (0,1,5,6,65535):
   c=prepare(0xd3a1,[1,5],locals={-10:value})
   for _ in range(1000):
    if c.pc==0xefc9 and c.stream() in (0xd37c,0xd3b4):break
    c.step()
   self.assertEqual(c.stream()==0xd3b4,value in (1,5,65535))
 def test_any_merchant_present_gate(self):
  for flags in ((0,0,0),(1,0,0),(0,1,0),(0,0,1),(1,1,1)):
   c=prepare(0x8daf,[COUNTRY],words={COUNTRY+75+2*i:f for i,f in enumerate(flags)})
   for _ in range(10000):
    if c.pc==0xefc9 and c.stream() in (0x8dce,0x8e55):break
    c.step()
   self.assertEqual(c.stream()==0x8e55,any(flags))
 def test_selected_merchant_present_gate(self):
  for m in range(3):
   for enabled in (0,1):
    c=prepare(0x8df5,[COUNTRY],words={COUNTRY+75+2*m:enabled},locals={-2:m})
    for _ in range(1000):
     if c.pc==0xefc9 and c.stream() in (0x8dff,0x8e14):break
     c.step()
    self.assertEqual(c.stream()==0x8e14,bool(enabled))
if __name__=='__main__':unittest.main(verbosity=2)
