#!/usr/bin/env python3
"""Runs original instructions and callees; synthetic CPU state, not full console gameplay."""
from pathlib import Path
import hashlib,json,unittest
from source_cpu import Cpu
from inspect_source import ROM
ROOT=Path(__file__).resolve().parent
B=0x6dcd
FIELDS={'arms':0x14,'skill':0x16,'troops':0x18,'economy':0x1a,'town':0x1c,'defense':0x1e,'masons':0x20,'food':0x36,'commodity_history':0x38,'artisans':0x3a,'food_history':0x3c,'foodworkers':0x3e,'gold':0x40,'morale':0x42,'tax':0x44,'extra_used':0x46,'residents':0x49}
SEED=0x123456789abc
CASES=[]
def init(start,fields=None,args=(),seed=SEED):
 c=Cpu();c.setup(start,args);c.m[0x84:0x8a]=seed.to_bytes(6,'big');c.setword(0x65b9,1);c.setword(B+0x51,0x6800);c.m[0x681d]=0;c.m[0x681e]=100;c.setword(0x79de,0)
 for k,v in (fields or {}).items():c.setword(B+FIELDS[k],v)
 return c

def capture(label,c,inputs,stop):
 before={k:c.word(B+v) for k,v in FIELDS.items()};c.run(stop)
 out={k:c.word(B+v) for k,v in FIELDS.items()};CASES.append({'label':label,'inputs':inputs,'before':before,'after':out,'cpu_steps':c.count,'vm_trace':c.vmtrace,'memory_writes':c.writes})
 return out

def rng_model(seed):
 nxt=(-((seed|1)*0xe0900e0187c5))&((1<<48)-1)
 return nxt,(nxt>>32)&0x7fff

def food_model(workers,food):
 w=list(workers);short=False
 while sum(w)>2*food:
  short=True;pct=(food*200)//sum(w);w=[x*pct//100 for x in w]
 return w,0 if short else food-sum(w)//2

class Annual(unittest.TestCase):
 def test_01_rom_hash(self):
  self.assertEqual(hashlib.sha256(ROM.read_bytes()).hexdigest(),'ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3')
 def test_02_random_service_original_native(self):
  for seed in [0,1,2,3,SEED,0xffffffffffff]:
   for n in [0,1,2,9,11,100,101,999,65535]:
    c=init(0xdb98,seed=seed,args=[n]);out=c.run(0xdb9d if n==0 else 0xdba6);new,r=rng_model(seed)
    self.assertEqual(out,r%n if n else 0);self.assertEqual(int.from_bytes(c.m[0x84:0x8a],'big'),new if n else seed)
 def test_03_seasonal_rate_decay_and_floor(self):
  for r in [0,49,50,51,69,99,100,101,123,150,190,255,999]:
   for season in [0,1,2]:
    for seed in [0,2,SEED,0xffffffffffff]:
     c=init(0x8eb4,seed=seed);c.setword(0x65bd,season);c.setword(0x65bb,1175);c.setword(0x7a8a,r);c.setword(0x7a88,1)
     c.run(0x8f11,trace=False);ns,rnd=rng_model(seed);delta=r*((rnd%9)+1)//100
     expected=max(50,(r-delta)&65535)
     self.assertEqual(c.word(0x7a8a),expected);self.assertEqual(c.word(0x65bd),season+1);self.assertEqual(c.word(0x65bb),1175)
  c=init(0x8eb4);c.setword(0x65bd,0);c.setword(0x7a8a,101);c.setword(0x7a88,1);capture('natural_rate_source_replay',c,{'rate':101,'season':0,'seed':SEED},0x8f11)
 def test_04_spring_rate_generation(self):
  for difficulty in [0,1,2,3,4,5,10]:
   for seed in [0,1,2,SEED,0x0123456789ab,0xffffffffffff]:
    c=init(0x8eb4,seed=seed);c.setword(0x65bd,3);c.setword(0x65bb,1175);c.setword(0x7a8a,101);c.setword(0x7a88,difficulty);c.run(0x8f11,trace=False)
    ns,r=rng_model(seed);self.assertEqual(c.word(0x7a8a),50+r%(94+7*difficulty));self.assertEqual((c.word(0x65bd),c.word(0x65bb)),(0,1176))
 def test_05_decay_random_mod1_mutation(self):
  c=init(0x8eb4);c.setword(0x65bd,0);c.setword(0x7a8a,101);c.m[0x8eec]=0x62;c.run(0x8f11)
  # With modulus2 some seeds take increase branch; seed search is synthetic input selection.
  found=False
  for seed in range(12):
   c=init(0x8eb4,seed=seed);c.setword(0x65bd,0);c.setword(0x7a8a,101);c.m[0x8eec]=0x62;c.run(0x8f11,trace=False)
   if c.word(0x7a8a)>101:found=True;break
  self.assertTrue(found)
 def test_06_seasonal_labor_history_and_development(self):
  for econ in [0,1,24,25,26,99,100,998,999]:
   for staff in [0,1,19,40,999]:
    f={'economy':econ,'town':staff,'defense':econ,'masons':staff,'artisans':staff,'foodworkers':staff,'commodity_history':750,'food_history':800}
    c=init(0x9034,f);c.setword(0x71f4,B);o=capture('labor',c,f,0x907a)
    self.assertEqual(o['economy'],min(999,econ-econ*4//100+staff));self.assertEqual(o['defense'],o['economy']);self.assertEqual(o['commodity_history'],min(999,750+staff));self.assertEqual(o['food_history'],min(999,800+staff))
 def test_07_four_season_history_not_only_final_staff(self):
  def path(staff):
   c=init(0x9034,{'food_history':0,'commodity_history':0,'food':10});c.m[0x7692]=2
   for n in staff:
    c.setup(0x9034);c.setword(0x71f4,B);c.setword(B+0x3e,n);c.setword(B+0x3a,n);c.run(0x907a,trace=False)
   c.setup(0x8e55);capture('four_season_harvest_history',c,{'staff_history':staff},0x8e9c);return c.word(B+0x36),c.word(B+0x26)
  self.assertEqual(path([10,20,30,40]),(110,100));self.assertEqual(path([0,0,0,40]),(50,40));self.assertEqual(path([25]*4),(110,100))
 def test_08_harvest_caps_specialty_reset_controls(self):
  for specialty in [0,2,5,9]:
   for history in [0,1,300,999]:
    f={'food':900,'food_history':history,'commodity_history':history,'extra_used':1,'foodworkers':77,'artisans':88}
    c=init(0x8e55,f);c.m[0x7692]=specialty
    for i in range(10):c.setword(B+0x22+2*i,900)
    o=capture('harvest',c,{'specialty':specialty,'history':history},0x8e9c)
    self.assertEqual(o['food'],min(999,900+history));self.assertEqual(o['food_history'],0);self.assertEqual(o['commodity_history'],0);self.assertEqual(o['extra_used'],0)
    self.assertEqual((o['foodworkers'],o['artisans']),(77,88))
    for i in range(10):self.assertEqual(c.word(B+0x22+2*i),min(999,900+history) if i==specialty else 900)
 def test_09_food_consumption_shortage_rounding(self):
  groups=[[0]*5,[1]*5,[1,0,0,0,0],[61,56,15,22,33],[100,200,300,200,199],[999,0,0,0,0]]
  for workers in groups:
   for food in [0,1,2,3,10,50,93,94,499,500,999]:
    f=dict(zip(['troops','town','masons','artisans','foodworkers'],workers));f['food']=food
    c=init(0x809e,f,[B]);o=capture('spring_food',c,f,0x8105);expected,left=food_model(workers,food)
    self.assertEqual([o[x] for x in ['troops','town','masons','artisans','foodworkers']],expected);self.assertEqual(o['food'],left)
 def test_10_spring_tax_payroll(self):
  for econ,tax,morale,gold,troops in [(0,0,0,0,61),(999,100,999,999,1),(207,40,118,439,61),(100,50,0,0,1),(100,50,117,0,51),(100,50,116,0,51),(999,1,8,0,5),(999,99,9,0,999),(1,1,0,0,0)]:
   f=dict(economy=econ,tax=tax,morale=morale,gold=gold,troops=troops,food=999,town=31)
   c=init(0x810b,f,[B]);o=capture('spring_tax_payroll',c,f,0x8168)
   receipt=(econ*tax//100)*(12+min(13,morale//9))//25;available=gold+receipt;paid=min(troops,available)
   self.assertEqual((o['gold'],o['troops']),(min(999,available-paid),paid));self.assertEqual(o['food'],999)
 def test_11_tax_ruler_population_negative_controls(self):
  f=dict(economy=500,tax=60,morale=90,gold=100,troops=61)
  result=[]
  for workers,ruler in [(0,0),(999,999)]:
   c=init(0x810b,{**f,'town':workers,'foodworkers':workers},[B]);c.setword(0x680e,ruler);c.run(0x8168,trace=False);result.append((c.word(B+0x40),c.word(B+0x18)))
  self.assertEqual(result[0],result[1])
 def test_12_annual_morale_tax_effect(self):
  for morale in [0,1,49,50,99,100,118,999]:
   for tax in [0,1,49,50,51,99,100]:
    f=dict(morale=morale,tax=tax);c=init(0x898e,f);c.run(0x89c0,trace=False);pct=2 if tax<50 else tax-48;self.assertEqual(c.word(B+0x42),morale-morale*pct//100)
 def test_13_population_growth_cap_and_distribution(self):
  for p in [0,1,18,19,20,32,33,34,98,99,100,167,300,998,999]:
   for player in [True,False]:
    for diff in [1,5]:
     workers=[p//5+(i<p%5) for i in range(5)]
     f=dict(zip(['troops','town','masons','artisans','foodworkers'],workers));f['residents']=100
     c=init(0x8a28,f);c.setword(0x7a88,diff);c.setword(0x79de,0 if player else 255);c.m[0x681e]=100
     o=capture('annual_population',c,{'p':p,'player':player,'difficulty':diff},0x8ad7)
     gain=1 if p<20 else min(999,p*(103+(0 if player else min(3,diff)))//100)-p
     expected=workers[:]
     if p<20:expected[0]+=1
     else:
      q,r=divmod(gain,5);expected=[w+q for w in workers]
      for idx in [0,2,1,3][:r]:expected[idx]+=1
     self.assertEqual([o[x] for x in ['troops','town','masons','artisans','foodworkers']],expected);self.assertEqual(o['residents'],106)
 def test_14_growth_officer_vs_distance_ownership_control(self):
  # AI non-leader without player neighbor gets base3%; lord-status100 gets difficulty bonus.
  f=dict(troops=100,residents=999)
  c=init(0x8a28,f);c.setword(0x7a88,5);c.setword(0x79de,255);c.m[0x681e]=101
  # Neighbor list index for country0 starts table entry returned from DE89: initialize terminator after obtaining pointer.
  probe=init(0xde8e,args=[B]);probe.run(0xde99);ptr=probe.word(8);c.m[ptr]=255
  c.run(0x8ad7);self.assertEqual(sum(c.word(B+FIELDS[k]) for k in ['troops','town','masons','artisans','foodworkers']),103);self.assertEqual(c.word(B+0x49),999)
 def test_15_instruction_mutation_development(self):
  f=dict(economy=100,town=10);c=init(0x9034,f);c.setword(0x71f4,B);c.m[0x904e]=0x65;c.run(0x907a,trace=False);self.assertEqual(c.word(B+0x1a),105);self.assertNotEqual(c.word(B+0x1a),106)
 def test_16_annual_trained_resident_percentage(self):
  for population in [0,1,19,20,21,100,999]:
   for trained in [0,1,20,49,50]:
    c=init(0x8008,{'residents':population});c.m[B+0x48]=trained;c.run(0x8063,trace=False)
    if population<20:
     _,r=rng_model(SEED);expected=20+r%11
    else:
     a=population*(trained+3)//100+1;b=population*105//100+1;expected=min(50,a*100//b)
    self.assertEqual(c.m[B+0x48],expected)
 def test_17_summer_and_winter_depreciation(self):
  for val in [0,1,14,15,99,100,999]:
   c=init(0x83ef,{'skill':val,'arms':val});c.run(0x840e,trace=False);self.assertEqual(c.word(B+0x16),val*93//100);self.assertEqual(c.word(B+0x14),val)
   c=init(0x83ca,{'skill':val,'arms':val});c.run(0x83e9,trace=False);self.assertEqual(c.word(B+0x14),val*97//100);self.assertEqual(c.word(B+0x16),val)
 def test_18_disaster_effect_slices(self):
  for player in [True,False]:
   for difficulty in [1,5]:
    c=init(0x868c,{'economy':333,'defense':777,'residents':500});c.m[B+0x48]=40;c.setword(0x71f6,B);c.setword(0x7a88,difficulty);c.setword(0x79de,0 if player else 255)
    o=capture('summer_storm_effect',c,{'player':player,'difficulty':difficulty},0x86b1)
    seed,r1=rng_model(SEED);seed,r2=rng_model(seed);seed,r3=rng_model(seed);seed,r4=rng_model(seed);bonus=difficulty*3//2 if player else 0
    self.assertEqual(o['defense'],777-777*(25+r1%6+bonus)//100);self.assertEqual(o['economy'],333-333*(25+r2%6+bonus)//100);self.assertEqual(o['residents'],500*(80+r3%11)//100);self.assertEqual(c.m[B+0x48],40*(80+r4%11)//100)
    c=init(0x874a,{'economy':333,'food_history':400,'commodity_history':300,'residents':500});c.m[B+0x48]=40;c.setword(0x71f6,B);c.setword(0x7a88,difficulty);c.setword(0x79de,0 if player else 255)
    o=capture('winter_cold_effect',c,{'player':player,'difficulty':difficulty},0x876a)
    seed,r1=rng_model(SEED);seed,r2=rng_model(seed);seed,r3=rng_model(seed)
    self.assertEqual(o['economy'],333-333*(20+r1%6+bonus)//100);self.assertEqual((o['food_history'],o['commodity_history']),(0,0));self.assertEqual(o['residents'],500*(80+r2%11)//100);self.assertEqual(c.m[B+0x48],40*(80+r3%11)//100)
 def test_19_sickness_workforce_reduction(self):
  f=dict(troops=100,town=200,masons=50,artisans=80,foodworkers=70,economy=333,morale=140,residents=500)
  c=init(0x893e,f);c.setword(0x71f6,B);c.setword(0x7a88,1);c.m[B+0x48]=40
  o=capture('sickness_effect',c,f,0x896e);seed,r1=rng_model(SEED);seed,r2=rng_model(seed);seed,r3=rng_model(seed);seed,r4=rng_model(seed);seed,r5=rng_model(seed);bonus=1
  self.assertEqual(o['economy'],333-333*(10+r1%6+bonus)//100);self.assertEqual(o['morale'],140-140*(20+r2%6+bonus)//100)
  for field in ['troops','town','masons','artisans','foodworkers']:self.assertEqual(o[field],f[field]-f[field]*(15+r3%6+bonus)//100)
  self.assertEqual(o['residents'],500*(80+r4%11)//100);self.assertEqual(c.m[B+0x48],40*(80+r5%11)//100)
 def test_20_ai_food_purchase_and_human_exclusion(self):
  for player in [True,False]:
   for diff in [0,5]:
    for food,gold,troops in [(0,100,61),(90,100,61),(91,100,61),(0,30,61),(0,31,61),(0,32,61),(500,999,100),(1,999,999)]:
     f=dict(food=food,gold=gold,troops=troops);c=init(0x82b6,f);c.setword(0x79de,0 if player else 255);c.setword(0x7a88,diff);o=capture('ai_food_purchase',c,{'player':player,'difficulty':diff,**f},0x832b)
     deficit=troops*150//100-food;surplus=gold-troops//2;qty=min(deficit,2*surplus) if deficit>0 and surplus>0 and not player and diff==5 else 0
     self.assertEqual((o['food'],o['gold']),(min(999,food+qty),gold-qty//2))
 def test_21_country_count_growth_both_scenarios(self):
  # Real loop count14/27; synthetic ownership and country records, relocated harness frame.
  for count in [14,27]:
   c=init(0x8a28);c.setword(4,0x7e00);c.setword(2,0x7d00);c.setword(0x65b9,count);c.setword(0x7a88,1)
   for idx in range(count):
    base=B+83*idx;c.setword(base+0x18,100);c.setword(base+0x49,100);c.setword(base+0x51,0x6000)
   c.m[0x601d]=0;c.m[0x601e]=100;c.run(0x8ad7,maxsteps=1000000,trace=False)
   for idx in range(count):
    base=B+83*idx;self.assertEqual(sum(c.word(base+FIELDS[k]) for k in ['troops','town','masons','artisans','foodworkers']),103);self.assertEqual(c.word(base+0x49),106)

 def test_22_ai_seasonal_gold_subsidy_both_scenarios(self):
  for count in [14,27]:
   for season_parameter in [0,3]:
    for difficulty in [0,1,5]:
     c=init(0x81ff,args=[season_parameter]);c.setword(4,0x7e00);c.setword(2,0x7d00);c.setword(0x7e0b,season_parameter);c.setword(0x65b9,count);c.setword(0x65bd,3);c.setword(0x7a88,difficulty)
     for slot in range(4):c.setword(0x79de+slot*2,0 if slot==0 else 255)
     for idx in range(count):
      base=B+83*idx;lord=0x6000+idx*32;c.setword(base+0x51,lord);c.m[lord+0x1d]=0 if idx==0 else 1;c.setword(base+0x40,900)
     c.run(0x829a,maxsteps=2000000,trace=False);seed=SEED
     self.assertEqual(c.word(B+0x40),900)
     for idx in range(1,count):
      seed,r=rng_model(seed);grant=(r%3+1)*(season_parameter+(2 if count==14 else 3))*difficulty
      self.assertEqual(c.word(B+idx*83+0x40),min(999,900+grant))
 def test_23_no_humans_no_ai_subsidy(self):
  c=init(0x81ff,{'gold':123},[3])
  for slot in range(4):c.setword(0x79de+slot*2,255)
  c.run(0x8207,trace=False);self.assertEqual(c.word(B+0x40),123);self.assertEqual(int.from_bytes(c.m[0x84:0x8a],'big'),SEED)
 def test_24_rng_adjacent_byte_negative_control(self):
  results=[]
  for val in [0,255]:
   c=init(0xdb98,args=[101]);c.m[0x83]=val;c.run(0xdba6,trace=False);results.append(c.word(8))
  self.assertEqual(results[0],results[1])

 def test_25_per_country_merchant_flags(self):
  for count,remaining in [(14,14),(14,1),(27,27),(27,1)]:
   f=dict(economy=333,morale=118);c=init(0x907a,f);c.setword(0x65b9,count);c.setword(0x71f4,B);c.setword(0x71fe,remaining);c.run(0x90cb)
   seed=SEED;expect=[]
   for merchant in range(3):
    seed,r=rng_model(seed);threshold=c.word(0xb1c6+54*merchant+(2*remaining if count==27 else 0))+(333*118//20000);expect.append(int(r%100>threshold))
   self.assertEqual([c.word(B+0x4b+2*i) for i in range(3)],expect)
 def test_26_global_offer_mask_refresh(self):
  c=init(0x8331);c.run(0x83c4);seed=SEED
  for merchant in range(3):
   for columns,dest,table in [(11,0x7a8c,0xafb2),(17,0x7ace,0xaff4)]:
    seed,r=rng_model(seed);forced=r%columns;expect=[]
    for item in range(columns):
     if item==forced:expect.append(1)
     else:
      seed,r=rng_model(seed);expect.append(int(r%100<c.word(table+2*columns*merchant+2*item)))
    self.assertEqual([c.word(dest+2*columns*merchant+2*i) for i in range(columns)],expect)
    self.assertIn(1,expect)

if __name__=='__main__':
 r=unittest.TextTestRunner(verbosity=2).run(unittest.defaultTestLoader.loadTestsFromTestCase(Annual))
 (ROOT/'annual_source_replays.json').write_text(json.dumps(CASES,indent=2))
 print('Executed source-replay records:',len(CASES),'original CPU instructions recorded:',sum(x['cpu_steps'] for x in CASES))
 raise SystemExit(0 if r.wasSuccessful() else 1)
