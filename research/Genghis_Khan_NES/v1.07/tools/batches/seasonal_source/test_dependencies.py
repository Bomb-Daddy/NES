#!/usr/bin/env python3
"""Source replay extensions: AI economy, unrest, robbery, population ledgers."""
from pathlib import Path
import json,unittest
from source_cpu import Cpu
from inspect_source import prg
from test_annual import B,FIELDS,SEED,rng_model
OUT=Path(__file__).resolve().parent;RECORDS=[]
def init(bank,start,f=None,args=(),seed=SEED,player=False):
 c=Cpu(bank);c.setup(start,args);c.m[0x84:0x8a]=seed.to_bytes(6,'big');c.setword(0x65b9,1);c.setword(B+0x51,0x6800);c.m[0x681d]=0;c.m[0x681e]=100;c.setword(0x79de,0 if player else 255);c.setword(0x7a88,1)
 for k,v in (f or {}).items():c.setword(B+FIELDS[k],v)
 return c

def run(c,stops,label='',inputs=None):
 if isinstance(stops,int):stops=(stops,)
 before={k:c.word(B+v) for k,v in FIELDS.items()}
 while c.count<2000000:
  if c.pc==0xefc9:
   p=c.stream();c.vmtrace.append({'pc':f'{p:04X}','op':f'{c.m[p]:02X}','a':c.word(8),'b':c.word(12),'frame':f'{c.word(4):04X}','stack':f'{c.word(2):04X}','step':c.count})
   if p in stops and c.word(4)==0x7200:
    if label:RECORDS.append({'label':label,'inputs':inputs,'before':before,'after':{k:c.word(B+v) for k,v in FIELDS.items()},'cpu_steps':c.count,'trace':c.vmtrace,'writes':c.writes})
    return c.word(8)
  c.step()
 raise RuntimeError('Budget')
class Rng:
 def __init__(self,seed=SEED):self.seed=seed
 def draw(self,n):
  if not n:return 0
  self.seed,v=rng_model(self.seed);return v%n

def revolt_model(tax,morale,d,seed):
 r=Rng(seed)
 if tax>50+r.draw(210-10*d):return 1,r.seed
 threshold=25+d+r.draw(16)
 return int(morale+r.draw(101)<threshold),r.seed

def theft_model(P,F,M,T,d,seed):
 r=Rng(seed);danger=True
 if M*T//100+r.draw(201)>16:
  if F//90<P//90+r.draw(176-5*d):danger=False
 if not danger:return 0,F,r.seed
 if r.draw(100)>70+6*d:return 0,F,r.seed
 left=F*(55+r.draw(21))//100;left=max(0,left-(5+r.draw(11)));return 1,left,r.seed

class Dependencies(unittest.TestCase):
 def test_01_popular_revolt_gates(self):
  for tax in [0,1,49,50,51,99,100]:
   for morale in [0,1,25,40,50,99,100,999]:
    for d in [1,5]:
     for seed in [0,SEED]:
      c=init(0,0x864a,{'tax':tax,'morale':morale},[B],seed);c.setword(0x7a88,d);got=run(c,[0x8686,0x8688]);want,state=revolt_model(tax,morale,d,seed)
      self.assertEqual(got,want);self.assertEqual(int.from_bytes(c.m[0x84:0x8a],'big'),state)
 def test_02_theft_gates_and_loss(self):
  for P,F,M,T,d,seed in [(0,0,0,0,1,0),(100,999,0,0,1,SEED),(100,100,999,50,1,SEED),(0,999,999,50,5,0),(999,999,0,0,5,SEED),(10,1,0,0,1,SEED),(1,5,0,0,5,2),(10,999,1,0,1,3)]:
   c=init(0,0x829a,{'troops':P,'food':F,'morale':M},[B],seed);c.m[B+0x48]=T;c.setword(0x7a88,d);c.m[0x681e]=101
   got=run(c,[0x82e3,0x82f8,0x8345],'provision_theft',dict(P=P,F=F,M=M,T=T,d=d,seed=seed));flag,left,state=theft_model(P,F,M,T,d,seed)
   self.assertEqual((got,c.word(B+0x36)),(flag,left));self.assertEqual(int.from_bytes(c.m[0x84:0x8a],'big'),state)
 def test_03_theft_gold_and_history_negative(self):
  f=dict(troops=100,food=999,morale=0,gold=700,commodity_history=99,food_history=88)
  c=init(0,0x829a,f,[B]);c.m[0x681e]=101;run(c,[0x82e3,0x82f8,0x8345])
  self.assertEqual((c.word(B+0x40),c.word(B+0x38),c.word(B+0x3c)),(700,99,88))
 def test_04_revolt_mutation_control(self):
  observed=False
  for seed in range(40):
   c=init(0,0x864a,{'tax':50,'morale':30},[B],seed);base=run(c,[0x8686,0x8688])
   c=init(0,0x864a,{'tax':50,'morale':30},[B],seed);c.m[0x8671]=0x64;changed=run(c,[0x8686,0x8688])
   if base!=changed:observed=True;break
  self.assertTrue(observed)
 def test_05_ai_labor_rows_and_conservation(self):
  rows=[[5,1,8,1],[20,5,30,5],[18,7,30,5],[15,8,30,7]]
  for econ in [0,299,300,499,500,999]:
   for P in [0,1,150,151,999]:
    for season in [0,3]:
     for seed in [0,SEED]:
      f=dict(troops=P,economy=econ,gold=999);c=init(2,0x8177,f,[B],seed);c.setword(0x65bd,season);ret=run(c,0x8226)
      r=Rng(seed);row=1 if econ<300 else 2 if econ<500 else 3
      if r.draw(5)==0 and P>150 and season!=3:row=0
      vals=[P*x//100 for x in rows[row]];expect=[P-sum(vals),vals[0],vals[1],vals[3],vals[2]]
      self.assertEqual([c.word(B+FIELDS[k]) for k in ['troops','town','masons','artisans','foodworkers']],expect);self.assertEqual(ret,int(row!=0))
 def test_06_ai_labor_human_flag_blocks_special_row(self):
  found=False
  for seed in range(50):
   if Rng(seed).draw(5)==0:
    c=init(2,0x8177,dict(troops=200,economy=100,gold=999),[B],seed);c.setword(0x7b70,1);run(c,0x8226);self.assertEqual(c.word(B+0x18),80);found=True;break
  self.assertTrue(found)
 def test_07_ai_winter_tax_free_set(self):
  for P in [600,601,999]:
   for old in [0,35,50,100]:
    c=init(2,0x8294,dict(troops=P,tax=old,morale=100),[B]);c.setword(0x65bd,3)
    # P600 path may invoke normal tax helper; P>600 must direct-return before morale path.
    if P>600:
     run(c,0x82ba,'ai_winter_tax',dict(P=P,oldTax=old));self.assertEqual((c.word(B+0x44),c.word(B+0x42)),(50+Rng().draw(6),100))
 def test_08_ai_tax_normal_band_noop(self):
  for tax in [35,38,43,50]:
   c=init(2,0x8294,dict(troops=100,tax=tax,morale=100),[B]);run(c,0x82eb);self.assertEqual((c.word(B+0x44),c.word(B+0x42)),(tax,100));self.assertEqual(int.from_bytes(c.m[0x84:0x8a],'big'),SEED)
 def test_09_ai_tax_lowering(self):
  for old,new,morale in [(50,40,100),(100,38,900),(40,39,0)]:
   c=init(2,0xa1e0,dict(tax=old,morale=morale),[B,new]);run(c,0xa1fd);self.assertEqual(c.word(B+0x44),new);self.assertEqual(c.word(B+0x42),min(999,morale+(old-new)*(2+Rng().draw(3))))
 def test_10_ai_tax_increase_limited_by_ruler(self):
  for judge in [0,100,300,999]:
   for old,target in [(0,40),(34,40),(38,43)]:
    c=init(2,0xa203,dict(tax=old,morale=100),[B,target]);c.setword(0x680e,judge);run(c,0xa262,'ai_tax_increase',dict(judge=judge,old=old,target=target))
    r=Rng();gap=judge//5-target;pct=100 if gap>50 else 40+r.draw(21) if gap>0 else 10+r.draw(11);delta=target-old
    self.assertEqual(c.word(B+0x44),min(100,old+delta*pct//100));self.assertEqual(c.word(B+0x42),max(0,100-delta*(2+r.draw(3))))
 def test_11_ai_hire_price_quality_and_capacity(self):
  for P,troops,gold,rate in [(100,61,439,101),(999,100,999,50),(100,100,200,101),(100,100,201,101),(0,0,999,150),(900,50,999,50)]:
   f=dict(troops=troops,town=P-troops,gold=gold,arms=110,skill=124,morale=118);c=init(2,0x8a9d,f,[B]);c.setword(0x7a8a,rate);run(c,0x8ae3,'ai_hire',dict(P=P,**f,rate=rate))
   price=60*(rate+100)//100;q=min(999-P,gold*10//price)//2 if gold>2*troops else 0
   self.assertEqual(c.word(B+0x18),troops+q);self.assertEqual(c.word(B+0x40),(gold*10-q*price)//10)
   if q:
    for key,val,newval in [('arms',110,100),('skill',124,100),('morale',118,50+Rng().draw(11))]:self.assertEqual(c.word(B+FIELDS[key]),(P*val+q*newval)//(P+q))
 def test_12_ai_arms_buy_fixed_column_and_gain(self):
  for P,arms,gold,leader in [(100,100,999,True),(100,299,999,True),(100,300,999,True),(100,199,999,False),(10,100,999,True)]:
   c=init(2,0x8b2e,dict(troops=P,arms=arms,gold=gold),[B]);c.m[0x681e]=100 if leader else 101;c.setword(0x7a8a,100);c.m[0x7de5+14]=40;c.m[0x7df4]=60
   run(c,0x8baf,'ai_weapon_purchase',dict(P=P,arms=arms,gold=gold,leader=leader));price=80
   q=min(min(999,P*(999-arms)//60),gold*10//price)//2 if P>10 and arms<(300 if leader else 200) else 0
   self.assertEqual(c.word(B+0x40),(gold*10-q*price)//10);self.assertEqual(c.word(B+0x14),min(999,arms+60*9*(q+1)//P) if q else arms)
 def test_13_ai_gifts_extra_morale_bonus(self):
  for player in [False,True]:
   c=init(2,0xa18c,dict(troops=100,gold=100,morale=10),[B,255,20],player=player);run(c,0xa1da,'gift_ai_bonus',{'player':player})
   self.assertEqual(c.word(B+0x40),80);self.assertEqual(c.word(B+0x42),10+20*30//100+1+(0 if player else 10))
 def test_14_ai_gift_selects_largest_good_and_spends_entire_stock(self):
  for morale in [149,150]:
   c=init(2,0x8ae9,dict(troops=100,morale=morale),[B]);c.setword(0x7a8a,100)
   for i in range(51):c.m[0x7de5+i]=10
   for i,x in enumerate([10,20,20,1,0,0,0,0,0,0]):c.setword(B+0x22+2*i,x)
   run(c,0x8b28,'ai_gift_largest',{'morale':morale})
   if morale<150:self.assertEqual(c.word(B+0x24),0);self.assertEqual(c.word(B+0x26),20);self.assertEqual(c.word(B+0x42),149+(37*30//100+1+10))
   else:self.assertEqual(c.word(B+0x24),20)
 def test_15_major_epidemic_bounded_economic_effect(self):
  f=dict(troops=100,town=200,masons=50,artisans=80,foodworkers=70,economy=333,morale=140,residents=500)
  c=init(1,0x878a,f,[B]);c.m[0x681e]=102;c.m[B+0x48]=40;run(c,0x8852,'major_epidemic_effect',f);r=Rng();pm=45+r.draw(11);pw=45+r.draw(11);pe=45+r.draw(11)
  self.assertEqual(c.word(B+0x42),140-140*pm//100);self.assertEqual(c.word(B+0x1a),333-333*pe//100)
  for k in ['troops','town','masons','artisans','foodworkers']:self.assertEqual(c.word(B+FIELDS[k]),f[k]-f[k]*pw//100)
  self.assertEqual((c.word(B+0x49),c.m[B+0x48]),(0,0))
 def test_16_ai_summer_draft_trained_percentage_literal_division(self):
  for P,reserve,trained in [(100,500,50),(300,500,50),(990,500,50),(999,500,50)]:
   c=init(1,0x8453,dict(troops=P,residents=reserve,arms=110,skill=124,morale=118),[B]);c.m[B+0x48]=trained
   run(c,0x84b5,'ai_summer_draft',dict(P=P,reserve=reserve,trained=trained));q=min(999-P,P*30//100)
   self.assertEqual(c.word(B+0x18),P+q);self.assertEqual(c.word(B+0x49),reserve-q)
   if q:self.assertEqual(c.m[B+0x48],max(0,reserve*trained//100-q)//(reserve-q))
   else:self.assertEqual(c.m[B+0x48],trained)
 def test_17_ai_summer_commodity_supplement_gate(self):
  for total in [0,349,350]:
   c=init(1,0x81bc,args=[B]);c.setword(B+0x22,total);run(c,[0x81d6,0x81f9],'ai_commodity_subsidy',{'total':total});r=Rng()
   if total<=349:
    qty=50+r.draw(51);idx=r.draw(10);self.assertEqual(sum(c.word(B+0x22+2*i) for i in range(10)),total+qty);self.assertEqual(c.word(B+0x22+2*idx),(total if idx==0 else 0)+qty)
   else:self.assertEqual(sum(c.word(B+0x22+2*i) for i in range(10)),total)
 def test_18_extra_levy_allocated_loss_no_resident_restoration(self):
  f=dict(troops=700,town=100,masons=50,artisans=50,foodworkers=90,residents=500,arms=100,skill=100)
  c=init(3,0x88bd,f,[B,100]);c.m[B+0x48]=50;run(c,0x890e,'levy_population_loss',f)
  P=sum(c.word(B+FIELDS[k]) for k in ['troops','town','masons','artisans','foodworkers']);self.assertLessEqual(P,890);self.assertEqual(c.word(B+0x49),500);self.assertEqual(c.m[B+0x48],50)
  self.assertEqual(min(250,999-990),9);self.assertGreater(min(250,999-P),9)
 def test_19_player_draft_ledger_effect_only(self):
  for P,reserve,trained,q in [(100,500,50,1),(990,500,50,9),(0,2,50,1),(100,100,50,50)]:
   eligible=reserve*trained//100;c=init(3,0x90ea,dict(troops=P,residents=reserve,arms=110,skill=124,morale=118),[B]);c.m[B+0x48]=trained;c.setword(0x71fe,q);c.setword(0x71fc,eligible)
   run(c,0x9125,'player_draft_ledger',dict(P=P,reserve=reserve,trained=trained,q=q))
   self.assertEqual((c.word(B+0x18),c.word(B+0x49)),(P+q,reserve-q));self.assertEqual(c.m[B+0x48],max(0,(eligible-q)*100//(reserve-q)-Rng().draw(3)))
 def test_20_zero_allocated_cleanup_not_reserve(self):
  for P in [0,1]:
   c=init(1,0xdf6f,dict(troops=P,residents=500,arms=100,skill=120),[B]);run(c,0xdf82);self.assertEqual(c.word(B+0x49),500);self.assertEqual((c.word(B+0x14),c.word(B+0x16)),(100,120) if P else (0,0))
 def test_21_person_revolt_attribute_gate(self):
  for personality in [0,1,2]:
   for leaderAverage,candidateAverage in [(0,999),(999,0),(100,100),(100,200)]:
    for seed in [0,SEED,2,7,11]:
     c=init(0,0x8402,args=[B,0x6840],seed=seed);c.m[0x685a]=personality
     for j in range(6):c.setword(0x680e+j*2,leaderAverage);c.setword(0x684e+j*2,candidateAverage)
     got=run(c,[0x840c,0x8455,0x845d]);r=Rng(seed)
     if personality==0:want=0
     else:
      threshold=r.draw(51) if personality==2 else 50+r.draw(101)
      want=int(r.draw(100)<(3 if personality==2 else 1)) if candidateAverage>leaderAverage+threshold else int(r.draw(101 if personality==2 else 501)==0)
     self.assertEqual(got,want);self.assertEqual(int.from_bytes(c.m[0x84:0x8a],'big'),r.seed)
 def test_22_ai_training_source_without_gold_cost(self):
  for P,skill,attributes in [(100,349,[100]*6),(0,10,[100]*6),(100,350,[100]*6),(100,0,[200]*6),(999,100,[100,50,100,100,100,100])]:
   c=init(2,0x8a3d,dict(troops=P,skill=skill,gold=500),[B])
   for j,a in enumerate(attributes):c.setword(0x680e+2*j,a)
   run(c,0x8a97,'ai_training',dict(P=P,skill=skill,attributes=attributes));r=Rng();expected=skill
   if P and skill<350:
    gain=(attributes[0]+attributes[3]+attributes[5])//40+r.draw(11)+15;expected=min(999,skill+gain*999//(P+999))
   want=attributes[:]
   if sum(attributes)//6<150:
    idx=min(range(6),key=lambda j:attributes[j]);gain=(attributes[1]+attributes[2]+attributes[4])//40+r.draw(11)+15;want[idx]=min(999,want[idx]+gain)
   self.assertEqual(c.word(B+0x16),expected);self.assertEqual([c.word(0x680e+2*j) for j in range(6)],want);self.assertEqual(c.word(B+0x40),500)
 def test_23_ai_summer_draft_eligibility_gate(self):
  for player in [False,True]:
   for morale in [0,140,141,180,181,999]:
    f=dict(troops=100,morale=morale,residents=500);c=init(1,0x84c5,f,player=player);c.m[B+0x48]=50;c.setword(0x71fc,B)
    run(c,0x8505,'ai_summer_draft_gate',dict(player=player,morale=morale));q=30 if not player and morale>140+Rng().draw(41) else 0
    self.assertEqual(c.word(B+0x18),100+q)
 def test_24_theft_positive_and_negative_seed_grid(self):
  flags=set()
  for seed in range(128):
   c=init(0,0x829a,dict(troops=10,food=999,morale=0),[B],seed);c.m[0x681e]=101;got=run(c,[0x82e3,0x82f8,0x8345]);expect,left,state=theft_model(10,999,0,0,1,seed);self.assertEqual((got,c.word(B+0x36)),(expect,left));flags.add(got)
  self.assertEqual(flags,{0,1})

if __name__=='__main__':
 r=unittest.TextTestRunner(verbosity=2).run(unittest.defaultTestLoader.loadTestsFromTestCase(Dependencies));(OUT/'dependency_source_replays.json').write_text(json.dumps(RECORDS,indent=2));print('Records',len(RECORDS),'CPU instructions',sum(x['cpu_steps'] for x in RECORDS));raise SystemExit(0 if r.wasSuccessful() else 1)
