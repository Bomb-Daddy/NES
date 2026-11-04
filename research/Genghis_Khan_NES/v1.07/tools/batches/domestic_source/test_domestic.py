import unittest, hashlib, itertools
from replay_domestic import *

POOL=('troops','town','masons','food_workers','artisans')
def total(f):return sum(f[k] for k in POOL)
def loss(e,x,q):return x//2+((((e-q)*x*10)&65535)//e)//20
class DomesticTests(unittest.TestCase):
 def test_identity(self):
  self.assertEqual(hashlib.sha256(h.ROM.read_bytes()).hexdigest(),h.EXPECTED)
  self.assertEqual(hashlib.sha256(SRAM.read_bytes()).hexdigest(),'dd22847140cc0397ab8f42e29f1832681628ed52bfa17e95041c288afc4ef773')
 def test_extra_tax_wrap_formula(self):
  for e,x,q in itertools.product((1,185,999),(1,61,103,118,999),(0,1)):
   if q<=e:
    r=run(0x8914,0x892e,[COUNTRY,x,q],fields={'economy':e});self.assertEqual(r['a'],loss(e,x,q))
 def test_tax_against_archived_country_fields(self):
  for q in (1,2,9):
   r=run(0x89a1,0x8a04,[COUNTRY],locals_={-2:q},rng=[11])
   b=(Path(os.environ['GK_ARCHIVED_CAPTURES'])/f'tax_extra_{q}_v102/after.sram').read_bytes()
   for k,o in FIELDS.items():self.assertEqual(r['fields'][k],b[COUNTRY+o-0x6000] if k=='eligibility' else int.from_bytes(b[COUNTRY+o-0x6000:COUNTRY+o-0x6000+2],'little'),(q,k))
 def test_tax_food_rng_boundaries_and_caps(self):
  for q in (1,9,185):
   for r0 in (0,10,11,21,98,99):
    r=run(0x89a1,0x8a04,[COUNTRY],locals_={-2:q},rng=[r0])
    self.assertEqual(r['fields']['food'],min(999,208+q*(r0+1)//100));self.assertEqual(r['fields']['economy'],185-q)
  r=run(0x89a1,0x8a04,[COUNTRY],locals_={-2:9},rng=[99],fields={'gold':998,'food':998})
  self.assertEqual((r['fields']['gold'],r['fields']['food']),(999,999))
 def test_tax_loss_mutation(self):
  a=run(0x8914,0x892e,[COUNTRY,118,1])['a'];b=run(0x8914,0x892e,[COUNTRY,118,1],mutations={0x8925:0x59})['a'];self.assertNotEqual(a,b)
 def test_tax_limit_original_path(self):
  for e,m,r0 in itertools.product((1,185,999),(0,118,999),(0,99)):
   r=run(0x8944,0x8981,[COUNTRY],fields={'economy':e,'morale':m},rng=[r0]);expected=min(e,1+((e*(r0+1)//100)*(89+m+61)//3000))
   self.assertEqual(r['a'],expected)
 def test_workforce_reduction(self):
  for loss_n in (0,1,35,37,50,102,103):
   r=run(0x88bd,0x890e,[COUNTRY,loss_n]);xs=[61,5,4,27,6];target=sum(xs)-loss_n
   while sum(xs)>target:
    p=target*100//sum(xs);xs=[x*p//100 for x in xs]
   self.assertEqual([r['fields'][k] for k in POOL],xs)
   self.assertEqual(r['fields']['civilians'],34)
   if sum(xs)==0:self.assertEqual((r['fields']['arms'],r['fields']['skill']),(0,0))
 def test_lower_tax(self):
  for old,new,r0 in itertools.product((35,100),(1,10),(0,2)):
   r=run(0x878c,0x87a9,[COUNTRY,new],fields={'tax':old},rng=[r0]);self.assertEqual((r['fields']['tax'],r['fields']['morale']),(new,min(999,118+(old-new)*(r0+2))))
 def test_raise_tax_all_resistance_branches(self):
  for stat,old,new,r0,r1 in [(89,35,40,0,0),(999,35,40,0,2),(300,35,40,20,1),(89,0,10,0,0),(89,35,100,10,2)]:
   margin=stat//5-new;percent=100 if margin>50 else 40+r0%21 if margin>0 else 10+r0%11
   draws=[r1] if margin>50 else [r0,r1]
   r=run(0x87af,0x880e,[COUNTRY,new],fields={'tax':old},segments={0x65cd:stat},rng=draws)
   self.assertEqual(r['fields']['tax'],min(100,old+(new-old)*percent//100));self.assertEqual(r['fields']['morale'],max(0,118-(new-old)*(r1%3+2)))
 def test_give_gold_food_item(self):
  for idx,q in itertools.product((255,0,2,10),(1,4,5,8,9,25)):
   r=run(0x8a85,0x8ad3,[COUNTRY,idx,q]);data=SRAM.read_bytes();tab=list(data[0x7de5-0x6000:0x7de5-0x6000+51])
   if idx==255:v=q;stock='gold'
   else:
    avg=sum(max(1,(tab[m*17+idx]*201//100)*90//100) for m in range(3))//3;v=min(999,q*avg//10)+1;stock='food' if idx==10 else None
   self.assertEqual(r['fields']['morale'],min(999,118+30*v//103+1))
   if stock:self.assertEqual(r['fields'][stock],(439 if idx==255 else 208)-q)
 def test_gift_runtime_cases(self):
  for kind,idx in [('food',10),('gold',255)]:
   for q in (1,8,9):
    name=f'give_food{q}_v102' if kind=='food' else f'give_gold{q}_done'
    b=(Path(os.environ['GK_ARCHIVED_CAPTURES'])/name/'after.sram').read_bytes()
    r=run(0x8a85,0x8ad3,[COUNTRY,idx,q]);self.assertEqual(r['fields']['morale'],int.from_bytes(b[0xe0f:0xe11],'little'))
 def test_gift_caps_and_mutation(self):
  a=run(0x8a85,0x8ad3,[COUNTRY,255,100],fields={'morale':998})
  self.assertEqual(a['fields']['morale'],999)
  a=run(0x8a85,0x8ad3,[COUNTRY,255,8]);b=run(0x8a85,0x8ad3,[COUNTRY,255,8],mutations={0x8ab5:20});self.assertNotEqual(a['fields']['morale'],b['fields']['morale'])
 def test_labor_transfer_conservation(self):
  for k in ('town','masons','food_workers','artisans'):
   initial={'town':5,'masons':4,'food_workers':27,'artisans':6}[k];n=61+initial
   for new in (0,1,n):
    r=run(0x9f61,0x9f6c,[COUNTRY],bank=2,locals_={-12:n,-10:new,-14:COUNTRY+FIELDS[k]});self.assertEqual(total(r['fields']),103);self.assertEqual(r['fields'][k],new);self.assertEqual(r['fields']['troops'],n-new)
 def test_labor_morale_threshold(self):
  for troops,original in itertools.product((60,65,66,90,100),(0,65,100)):
   f={'troops':troops,'town':100-troops,'masons':0,'food_workers':0,'artisans':0}
   r=run(0x9f91,0x9fc6,[COUNTRY],bank=2,locals_={-20:COUNTRY+0x18,-18:original},fields=f)
   expected=118*(100-2*(troops-65))//100 if troops>65 and troops>original else 118
   self.assertEqual(r['fields']['morale'],expected)
 def test_train_people_gain_and_cap(self):
  for eligible,level,r0 in itertools.product((0,32,49),(1,2,6),(0,1)):
   r=run(0xa5e9,0xa617,[COUNTRY],bank=2,locals_={-2:COUNTRY+0x48},fields={'eligibility':eligible},segments={0x7a88:level},rng=[r0]);delta=(2+(89+129)//600+r0)*(100-6*min(5,level-1))//100
   self.assertEqual(r['fields']['eligibility'],min(50,eligible+delta));self.assertEqual(r['fields']['civilians'],34);self.assertEqual(total(r['fields']),103)
 def test_draft_limit_and_conservation(self):
  for pop,eligible in itertools.product((1,34,999),(0,32,50)):
   a=pop*eligible//100;lim=min(a,999-103)
   r=run(0x90a2,0x90c3,[COUNTRY],fields={'civilians':pop,'eligibility':eligible});self.assertEqual(r['a'],lim)
   if not lim:continue
   for q,random in itertools.product((1,lim),(0,2)):
    r=run(0x90ea,0x9125,[COUNTRY],locals_={-2:q,-4:a},fields={'civilians':pop,'eligibility':eligible},rng=[random]);f=r['fields']
    self.assertEqual(total(f)+f['civilians'],103+pop)
    self.assertEqual(f['eligibility'],max(0,100*(a-q)//(pop-q)-random))
    for k,x in [('arms',110),('skill',124),('morale',118)]:self.assertEqual(f[k],(x*103+q)//(103+q))
 def test_train_people_guards(self):
  for eligible,pop,stop in [(50,34,0xa5cd),(51,34,0xa5cd),(32,0,0xa5df),(32,34,0xa5e9)]:
   r=run(0xa5c1,stop,[COUNTRY],bank=2,fields={'eligibility':eligible,'civilians':pop})
   self.assertFalse(r['changes'])
 def test_extra_tax_once_used_guard(self):
  for cap,used,stop in [(0,0,0x8a0b),(10,1,0x8a0b),(10,0,0x898c)]:
   r=run(0x8981,stop,[COUNTRY],locals_={-4:cap},fields={'tax_used':used});self.assertFalse(r['changes'])
 def test_train_people_mutation(self):
  a=run(0xa5e9,0xa617,[COUNTRY],bank=2,locals_={-2:COUNTRY+0x48},fields={'eligibility':49},rng=[1])
  b=run(0xa5e9,0xa617,[COUNTRY],bank=2,locals_={-2:COUNTRY+0x48},fields={'eligibility':49},rng=[1],mutations={0xa60c:49})
  self.assertEqual((a['fields']['eligibility'],b['fields']['eligibility']),(50,49))
 def test_labor_penalty_mutation(self):
  kw=dict(bank=2,locals_={-20:COUNTRY+0x18,-18:61},fields={'troops':66,'town':34,'masons':0,'food_workers':0,'artisans':0})
  a=run(0x9f91,0x9fc6,[COUNTRY],**kw)
  b=run(0x9f91,0x9fc6,[COUNTRY],**kw,mutations={0x9fa3:66})
  self.assertEqual((a['fields']['morale'],b['fields']['morale']),(115,118))
 def test_train_troops_gain_caps_and_stored_word(self):
  for troops,skill,level,r0 in itertools.product((0,61,957),(0,124,998,999),(1,6),(0,10)):
   n=troops+42
   r=run(0xa4f9,0xa50d,[COUNTRY],bank=2,fields={'troops':troops,'skill':skill},segments={0x7a88:level},rng=[r0])
   raw=15+r0+(89+61+129)//40;gain=(999*raw//(n+999))*(100-4*min(5,level-1))//100
   self.assertEqual(r['fields']['skill'],min(999,skill+gain));self.assertEqual(r['fields']['troops'],troops)
   self.assertEqual({d['address'] for d in r['changes']} - {'6DE3','6DE4'},set())
 def test_train_troops_repetition_with_leadership_cost(self):
  skill=124;lead=89
  for i in range(5):
   r=run(0xa4f9,0xa50d,[COUNTRY],bank=2,fields={'skill':skill},segments={0x65cd:lead},rng=[0])
   expected=min(999,skill+999*(15+(lead+61+129)//40)//1102)
   self.assertEqual(r['fields']['skill'],expected);skill=expected;lead-=5
  self.assertGreater(skill,124)
 def test_skill_seasonal_leaf_after_training(self):
  trained=run(0xa4f9,0xa50d,[COUNTRY],bank=2,rng=[0])['fields']['skill']
  decayed=run(0x808e,0x8098,[COUNTRY+0x16,93],bank=1,fields={'skill':trained})
  self.assertEqual((trained,decayed['fields']['skill']),(143,132))
  self.assertEqual({d['address'] for d in decayed['changes']},{'6DE3'})
 def test_train_troops_mutation_and_archive(self):
  a=run(0xa4f9,0xa50d,[COUNTRY],bank=2,rng=[0]);b=run(0xa4f9,0xa50d,[COUNTRY],bank=2,rng=[0],mutations={0xa2bc:0x7e})
  self.assertEqual(a['fields']['skill'],143);self.assertNotEqual(a['fields']['skill'],b['fields']['skill'])
  b=(Path(os.environ['GK_ARCHIVED_CAPTURES'])/'train_troops_complete_v102/after.sram').read_bytes()
  self.assertEqual(a['fields']['skill'],int.from_bytes(b[0xde3:0xde5],'little'))
 def test_economic_ability_gates_and_saturation(self):
  data=SRAM.read_bytes()
  for code in (0,1,2,3,4,5,6,7,8,9,15,22,23):
   costs=list(data[0x1e18+6*code:0x1e1e+6*code])
   stats={0x65cd+i*2:v for i,v in enumerate(costs)}
   a=run(0x8280,0x82ba,[code],segments=stats);self.assertFalse(a['changes'])
   for i,v in enumerate(costs):
    if v:
     b=run(0x8280,0x829d,[code],segments={**stats,0x65cd+i*2:v-1});self.assertFalse(b['changes'])
   c=run(0x8247,0x827a,[code],segments={p:0 for p in stats});self.assertFalse(c['changes'])
   d=run(0x8247,0x827a,[code],segments=stats)
   changed={int(x['address'],16):x['after'] for x in d['changes']}
   self.assertEqual([changed.get(p,v) for p,v in stats.items()],[0]*6)
 def test_ability_table_mutation_is_detected(self):
  a=run(0x8247,0x827a,[4]);b=run(0x8247,0x827a,[4],mutations={0x7e18+6*4+2:6})
  before={d['address']:d['after'] for d in a['changes']};after={d['address']:d['after'] for d in b['changes']};self.assertEqual((before['65D1'],after['65D1']),(107,106))
 def test_ability_costs_original_replay(self):
  costs={0:[5,0,0,5,0,0],1:[10,0,0,10,0,0],2:[5,0,5,0,0,0],4:[0,0,5,0,0,0],8:[0,0,0,5,0,0],15:[20,0,0,10,0,10]}
  for code,cost in costs.items():
   r=run(0x8247,0x827a,[code]);changes={int(d['address'],16):d['after'] for d in r['changes']};values=[89,77,112,61,170,129]
   self.assertEqual([changes.get(0x65cd+i*2,values[i]) for i in range(6)],[max(0,x-y) for x,y in zip(values,cost)])

if __name__=='__main__':
 result=unittest.TextTestRunner(verbosity=2).run(unittest.defaultTestLoader.loadTestsFromTestCase(DomesticTests));raise SystemExit(not result.wasSuccessful())
