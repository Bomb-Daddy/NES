#!/usr/bin/env python3
"""Offline, independent arithmetic/integrity audit. Never imports/calls a native bridge."""
from pathlib import Path
import argparse,datetime,hashlib,json,struct,sys,traceback,collections,platform
BASE=0x6dcd
F={'arms':20,'skill':22,'troops':24,'economy':26,'town':28,'defense':30,'masons':32,**{f'good{i}':34+2*i for i in range(10)},'food':54,'commodity_history':56,'artisans':58,'food_history':60,'foodworkers':62,'gold':64,'morale':66,'tax':68,'extra_used':70,'residents':73,'merchant0':75,'merchant1':77,'merchant2':79,'lord':81}
GROUPS=['troops','town','masons','artisans','foodworkers']
TABLE=[[10,16,32,18,14,11,13,8,20,18,10,20,80,100,40,60,150],[4,37,28,22,8,14,15,5,15,29,11,15,75,110,40,70,200],[8,25,25,10,15,7,18,20,31,38,11,25,82,90,35,65,250]]
EXCLUSIONS={'tax_100_':'Superseded numeric-entry probe; requested100 was entered as1. Use tax100_correct_*.', 'uighur':'Earlier mislabelled weapon probes; excluded from weapon acceptance.', 'levy_1_second_denied':'Old repeat attempt entered Assign, not Extra Tax.', 'levy_9_second_denied':'Old repeat attempt entered Assign, not Extra Tax.'}
def sha(b):return hashlib.sha256(b).hexdigest()
def require(v,msg):
 if not v:raise AssertionError(msg)
def eq(got,want,label):require(got==want,f'{label}: observed {got!r}, expected {want!r}')
class Snapshot:
 def __init__(self,name,meta,data,zero=None):self.name=name;self.meta=meta;self.data=data;self.zero=zero
 def w(self,a):require(0x6000<=a<0x7fff,f'word outside captured SRAM: {a:04X}');return int.from_bytes(self.data[a-0x6000:a-0x6000+2],'little')
 def b(self,a):require(0x6000<=a<0x8000,f'byte outside captured SRAM: {a:04X}');return self.data[a-0x6000]
 def c(self,i=0):
  base=BASE+83*i;return {k:self.w(base+v) for k,v in F.items()}|{'trained':self.b(base+72)}
 def abilities(self,officer=None):
  officer=officer if officer is not None else self.w(BASE+81);return [self.w(officer+14+2*i) for i in range(6)]
 def p(self,i=0):d=self.c(i);return sum(d[k] for k in GROUPS)
 def human(self,i=0):lord=self.w(BASE+83*i+81);owner=self.b(lord+29);return owner in [self.w(0x79de+2*j) for j in range(4)]

def validate_bytes(name,meta,data,zero=None):
 eq(len(data),8192,name+' SRAM length');eq(sha(data),meta['sram_sha256'],name+' SRAM SHA256');s=Snapshot(name,meta,data,zero)
 for k,a in {'gold':0x6e0d,'food':0x6e03,'troops':0x6de5,'fur':0x6def,'rate':0x7a8a,'season':0x65bd,'secondary':0x7a88,'year':0x65bb,'scenario_count':0x65b9,'reserve':0x6e16}.items():
  if k in meta:eq(meta[k],s.w(a),name+' JSON '+k)
 if 'orders' in meta:eq(meta['orders'],s.b(0x7b36),name+' JSON orders(byte view)')
 if 'eligible_percent' in meta:eq(meta['eligible_percent'],s.b(0x6e15),name+' trained byte')
 for a,v in meta.get('country_words',{}).items():eq(v,s.w(int(a,16)),name+' JSON word'+a)
 lord=s.w(BASE+81)
 if 'abilities' in meta and 0x6000<=lord<=0x7fe0:eq(meta['abilities'],s.abilities(),name+' JSON abilities')
 if 'rng_seed_hex' in meta:
  require(zero is not None,name+' missing zero-page capture');eq(len(zero),256,name+' zero-page length');eq(meta['rng_seed_hex'].lower(),zero[0x84:0x8a].hex(),name+' seed metadata')
 return s

class Audit:
 def __init__(self,root,out):self.root=root;self.out=out;self.cache={};self.json_cache={};self.inputs={};self.integrity=[];self.cases=[];self.notes=[];self.current=None;self.used=set();self.inventory=[];self.profile='legacy';self.prior_attempts=[]
 def read(self,path):
  p=self.root/path;b=p.read_bytes();self.inputs[path]={'sha256':sha(b),'bytes':len(b)};return b
 def js(self,name):
  if not name.endswith('.json'):name+='.json'
  if name not in self.json_cache:self.json_cache[name]=json.loads(self.read(name))
  return self.json_cache[name]
 def snap(self,name):
  if self.current is not None:self.current.add(name)
  self.used.add(name)
  if name not in self.cache:
   meta=self.js(name);eq(meta['label'],name,'snapshot label');data=self.read(name+'.sram');zp=self.read(name+'.zeropage') if (self.root/(name+'.zeropage')).exists() else None
   s=validate_bytes(name,meta,data,zp)
   png=self.read(name+'.png');eq(png[:8],b'\x89PNG\r\n\x1a\n',name+' PNG signature');eq(struct.unpack('>II',png[16:24]),(256,240),name+' native image dimensions')
   self.cache[name]=s
  return self.cache[name]
 def test(self,group,name,fn,scope='captured state arithmetic; no automatic screen-message proof'):
  self.current=set();row={'group':group,'id':name,'scope':scope}
  try:
   details=fn() or {};summary_check(self,group,name);details['producer_summary_consistency']='verified where a summary record exists';row.update(status='PASS',details=details)
  except FileNotFoundError as e:row.update(status='MISSING',error=str(e))
  except Exception as e:row.update(status='FAIL',error=str(e),traceback=traceback.format_exc())
  row['snapshots']=sorted(self.current);self.current=None;self.cases.append(row);print(row['status'],group,name,row.get('error',''),flush=True)
 def record(self,name,key=None):
  rows=self.js(name)
  if key is None:return rows
  return {x['case']:x for x in rows}[key]
 def count(self,name,n):
  try:eq(len(self.js(name)),n,name+' record count');self.inventory.append({'file':name,'expected_records':n,'status':'PASS'})
  except Exception as e:self.inventory.append({'file':name,'expected_records':n,'status':'MISSING' if isinstance(e,FileNotFoundError) else 'FAIL','error':str(e)})
 def all_integrity(self):
  for p in sorted(self.root.glob('*.json')):
   try:r=self.js(p.name)
   except Exception as e:self.integrity.append({'file':p.name,'status':'FAIL','error':str(e)});continue
   if not isinstance(r,dict) or 'sram_sha256' not in r or 'label' not in r:continue
   try:self.snap(p.stem);self.integrity.append({'file':p.name,'status':'PASS'})
   except Exception as e:self.integrity.append({'file':p.name,'status':'MISSING' if isinstance(e,FileNotFoundError) else 'FAIL','error':str(e)})
 def costs(self,b,a,cost,orders=1,officer=None):
  eq(a.abilities(officer),[max(0,x-y) for x,y in zip(b.abilities(officer),cost)],'six ability costs');eq(a.w(0x7b36),b.w(0x7b36)-orders,'order cost')
 def unchanged(self,b,a,indices=(0,)):
  for i in indices:eq(a.c(i),b.c(i),f'country{i+1} unchanged economic record')
 def econ(self,a,expected,i=0):eq(a.c(i),expected,f'country{i+1} complete economic fields')

def quote(s,m,i,sell=False):
 # Read the actual captured base table and independently verify the published constants.
 base=s.b(0x7de5+17*m+i);eq(base,TABLE[m][i],f'base table[{m},{i}]');b=base*(s.w(0x7a8a)+100)//100;return max(1,b*90//100) if sell else b
class Rng:
 def __init__(self,s):self.s=s
 def draw(self,n):
  if not n:return 0
  self.s=(-((self.s|1)*0xe0900e0187c5))&((1<<48)-1);return ((self.s>>32)&0x7fff)%n

def trade_case(v,m,mode,i):
 label=f'matrix_m{m}_{mode}_i{i:02d}';b=v.snap(label+'_prompt');a=v.snap(label+'_result');r=next(x for x in v.js('trade_matrix_results') if x['label']==label)
 eq((r['merchant'],r['mode'],r['item'],r['quantity']),(m,mode,i,1),'trade case identity');d=b.c();q=quote(b,m,i,mode=='sell');eq(r['quote_tenths'],q,'reported quote')
 d['gold']+=q//10 if mode=='sell' else -((q+9)//10)
 if i<11:d['food' if i==10 else 'good'+str(i)]+=1 if mode=='buy' else -1
 else:d['arms']=min(999,d['arms']+18*TABLE[0][i]//b.p())
 v.econ(a,d);v.costs(b,a,[0,5,0,0,0,0]);eq(r['gold_before'],b.c()['gold'],'reported Gold before');eq(r['gold_after'],a.c()['gold'],'reported Gold');eq(r['judgment_before'],b.abilities()[1],'reported Judgment before');eq(r['judgment_after'],a.abilities()[1],'reported Judgment after');require(all(r['checks'].values()),'producer recorded a failing trade check');eq(r['arms_after'],a.c()['arms'],'reported Arms');eq(r['orders_after'],a.w(0x7b36),'reported orders')
 if i<11:eq(r['stock_before'],b.c()['food' if i==10 else 'good'+str(i)],'reported stock before');eq(r['stock_after'],a.c()['food' if i==10 else 'good'+str(i)],'reported stock')
 return {'quote_tenths':q,'gold_delta':a.c()['gold']-b.c()['gold'],'quantity_effect':1,'weapon_inventory_not_asserted':i>=11}

def tax_case(v,q):
 prefix='tax100_correct' if q==100 else 'tax_'+str(q);b=v.snap(prefix+'_before');a=v.snap(prefix+'_result');d=b.c()
 if q not in range(1,101):v.unchanged(b,a);v.costs(b,a,[0]*6,0);return {'input':q,'scope':'state unchanged at out-of-range input; screen text not inferred'}
 old=d['tax'];stats=b.abilities();possible=[]
 if q<old:
  possible=[(q,min(999,d['morale']+(old-q)*(2+r))) for r in range(3)]
 else:
  gap=stats[0]//5-q;responses=[100] if gap>50 else range(40,61) if gap>0 else range(10,21)
  possible=[(min(100,old+(q-old)*r//100),max(0,d['morale']-(q-old)*(2+t))) for r in responses for t in range(3)]
 actual=(a.c()['tax'],a.c()['morale']);require(actual in possible,f'Tax outcome {actual} outside source RNG domain {sorted(set(possible))}')
 d['tax'],d['morale']=actual;v.econ(a,d);v.costs(b,a,[5,0,0,5,0,0]);return {'requested_tax':q,'actual_tax':actual[0],'rng_scope':'exact source-permitted outcome set; intervening RNG call state not traced'}

def draft_case(v):
 b=v.snap('draft8_before');a=v.snap('draft8_result');d=b.c();q=8;p=b.p();eligible=d['residents']*d['trained']//100;require(q<=min(eligible,999-p),'draft admissibility')
 for k in ['arms','skill','morale']:d[k]=(d[k]*p+q)//(p+q)
 d['troops']+=q;d['residents']-=q;poss={max(0,(eligible-q)*100//d['residents']-r) for r in range(3)};require(a.c()['trained'] in poss,'draft trained% RNG domain');d['trained']=a.c()['trained'];v.econ(a,d);v.costs(b,a,[20,0,0,10,0,10]);eq(a.p()+d['residents'],p+b.c()['residents'],'total resident conservation');return {'q':q,'eligible_before':eligible,'after_trained_percent':d['trained']}

def give_case(v,kind,q):
 name=f'give_{kind}_{q}';b=v.snap(name+'_before');a=v.snap(name+'_result');d=b.c()
 if q==0:v.unchanged(b,a);v.costs(b,a,[0]*6,0);return {'zero_quantity':'no state charge; numeric-message text not inferred'}
 if kind=='gold':value=q;d['gold']-=q
 else:
  i=10 if kind=='food' else b.b(0x7692);d['food' if i==10 else 'good'+str(i)]-=q;avg=sum(quote(b,m,i,True) for m in range(3))//3;value=min(999,q*avg//10)+1
 d['morale']=min(999,d['morale']+30*value//b.p()+1+(0 if b.human() else 10));v.econ(a,d);v.costs(b,a,[0,0,5,0,0,0]);return {'gift_value':value,'quantity':q}

def train_case(v,name):
 b=v.snap('train_'+name+'_before');a=v.snap('train_'+name+'_result');d=b.c();s=b.abilities();diff=b.w(0x7a88)
 if name.startswith('people'):
  if d['trained']>=50:v.unchanged(b,a);v.costs(b,a,[0]*6,0);return {'cap_noop':'no state cost; screen text not inferred'}
  if d['residents']:
   factor=100-6*min(5,(diff-1)&65535);poss={min(50,d['trained']+((s[0]+s[5])//600+r+2)*factor//100) for r in range(2)};require(a.c()['trained'] in poss,'TrainPeople RNG domain');d['trained']=a.c()['trained']
  v.econ(a,d);v.costs(b,a,[0,0,0,5,0,0]);return {'zero_residents_still_costs_order':b.c()['residents']==0}
 factor=100-4*min(5,(diff-1)&65535);poss={min(999,d['skill']+(999*(15+r+(s[0]+s[3]+s[5])//40)//(b.p()+999))*factor//100) for r in range(11)};require(a.c()['skill'] in poss,'TrainTroops RNG domain');d['skill']=a.c()['skill'];v.econ(a,d);v.costs(b,a,[5,0,0,0,0,0]);return {'skill_after':d['skill'],'at_cap_still_charged':b.c()['skill']==999}

def assign_case(v,name):
 b=v.snap('assign_'+name+'_before');a=v.snap('assign_'+name+'_result');d=b.c()
 if name in ['town5_noop','town67_invalid','artisans16_cancel']:v.unchanged(b,a);v.costs(b,a,[0]*6,0);return {'no_change_scope':'no state charge; cancel/refusal screen not inferred'}
 field='artisans' if name.startswith('artisans') else 'town';q=int(name[len(field):]);pair=min(999,d['troops']+d[field]);old=d['troops'];d[field]=q;d['troops']=pair-q;t=100*d['troops']//b.p()
 if t>65 and d['troops']>old:d['morale']=d['morale']*(100-2*(t-65))//100
 v.econ(a,d);eq(a.p(),b.p(),'Assign total');v.costs(b,a,[5,0,5,0,0,0]);return {'assignment':field,'requested':q,'troop_percent':t}

def spring_model(d):
 d=d.copy();p=sum(d[k] for k in GROUPS);short=False
 while p>2*d['food']:
  short=True;pct=200*d['food']//p
  for k in GROUPS:d[k]=d[k]*pct//100
  p=sum(d[k] for k in GROUPS)
 d['food']=0 if short else d['food']-p//2
 gross=(d['economy']*d['tax']//100)*(12+min(13,d['morale']//9))//25;gold=d['gold']+gross;d['troops']=min(d['troops'],gold);d['gold']=min(999,gold-d['troops']);pct=2 if d['tax']<50 else d['tax']-48;d['morale']-=d['morale']*pct//100
 R=d['residents'];require(R>=20,'Spring model low reserve requires randomized branch');d['trained']=min(50,100*(R*(d['trained']+3)//100+1)//(R*105//100+1));d['residents']=min(999,R+R*5//100+1);p=sum(d[k] for k in GROUPS)
 if p<20:d['troops']+=1
 else:
  gain=min(999,p*103//100)-p;q,r=divmod(gain,5)
  for k in GROUPS:d[k]+=q
  for k in ['troops','masons','town','artisans'][:r]:d[k]+=1
 return d,gross

def season_model(d,season,specialty=0):
 d=d.copy();d['food_history']=min(999,d['food_history']+d['foodworkers']);d['commodity_history']=min(999,d['commodity_history']+d['artisans'])
 for k,staff in [('economy','town'),('defense','masons')]:d[k]=min(999,d[k]-d[k]*4//100+d[staff])
 info={}
 if season==0:d,gross=spring_model(d);info['gross_tax']=gross
 if season==1:d['skill']=d['skill']*93//100
 if season==2:
  d['food']=min(999,d['food']+d['food_history']);k='good'+str(specialty);d[k]=min(999,d[k]+d['commodity_history']);d['food_history']=d['commodity_history']=d['extra_used']=0
 if season==3:d['arms']=d['arms']*97//100
 return d,info

def annual_match(v,name):
 b=v.snap('annual_match_'+name+'_before');a=v.snap('annual_match_'+name+'_spring');d,info=season_model(b.c(),0,b.b(0x7692))
 for k in ['merchant0','merchant1','merchant2']:d[k]=a.c()[k]
 v.econ(a,d);eq((a.w(0x65bb),a.w(0x65bd)),(1176,0),'Spring date');eq(b.p(),103,'matched total');eq(b.c()['troops'],61,'matched payroll population');return info

def negative_trade(v,name):
 b=v.snap('trade_negative_'+name+'_before');a=v.snap('trade_negative_'+name+'_result');v.unchanged(b,a);v.costs(b,a,[0]*6,0)
 if name=='judgment4':eq(b.abilities()[1],4,'one-below Judgment gate')
 if name=='china_absent':eq(b.c()['merchant0'],0,'China unavailable word')
 if name=='hire_no_gold':eq(b.c()['gold'],0,'zero funds')
 if name=='hire_population999':eq(b.p(),999,'population capacity')
 if name=='buy_stock999':eq(b.c()['good0'],999,'stock capacity')
 if name=='sell_gold999':eq(b.c()['gold'],999,'Gold capacity')
 return {'scope':'full economic fields, six abilities, orders unchanged; refusal/cancel/zero UI text not inferred'}

def move_scalar(v,name,i,q,before=None,after=None):
 label='move_'+name;b=v.snap(before or label+'_before');a=v.snap(after or label+'_result');d,e=b.c(),b.c(1);k='gold' if i==-1 else 'food' if i==10 else 'good'+str(i);d[k]-=q;e[k]+=q;v.econ(a,d);v.econ(a,e,1);v.costs(b,a,[5,5,5,0,0,0]);return {'resource':k,'quantity':q,'conserved':True}

def move_people(v,name,j,q):
 b=v.snap('move_'+name+'_before');a=v.snap('move_'+name+'_result');d,e=b.c(),b.c(1);field=['town','masons','foodworkers','artisans','troops'][j];d[field]-=q;e[field]+=q
 if sum(d[k] for k in GROUPS)==0:d['arms']=d['skill']=0
 for k in ['arms','skill','morale']:e[k]=(b.p(1)*e[k]+q*d[k])//(b.p(1)+q)
 v.econ(a,d);v.econ(a,e,1);v.costs(b,a,[10,0,0,10,0,0]);eq(a.p()+a.p(1),b.p()+b.p(1),'two-country population');return {'quantity':q,'source_empty':a.p()==0,'destination_quality':[e[k] for k in ['arms','skill','morale']]}

def hire_case(v,q):
 b=v.snap(f'hire{q}_before');a=v.snap(f'hire{q}_result');d=b.c();p=b.p();quote_=60*(b.w(0x7a8a)+100)//100;require(q<=min(999-p,10*d['gold']//quote_),'Hire capacity')
 d['gold']-=(q*quote_+9)//10;d['troops']+=q
 for k in ['arms','skill']:d[k]=(d[k]*p+q*100)//(p+q)
 poss={(d['morale']*p+q*n)//(p+q) for n in range(50,61)};require(a.c()['morale'] in poss,'Hire morale newcomers50..60');d['morale']=a.c()['morale'];v.econ(a,d);v.costs(b,a,[0,5,0,0,0,0]);return {'quantity':q,'quote_tenths':quote_}

def dagger2(v):
 b=v.snap('dagger2_prompt');a=v.snap('dagger2_result');d=b.c();qt=quote(b,0,11);d['gold']-=(2*qt+9)//10;d['arms']=min(999,d['arms']+9*20*3//b.p());v.econ(a,d);v.costs(b,a,[0,5,0,0,0,0]);return {'quantity':2,'Arms':d['arms']}

def check_breakpoint(js,addr):
 hits=[x.get('lastHit',{}) for x in js['breakpoint']['breakpoints']];require(any(x.get('memoryType')=='nesPrgRom' and x.get('address')==addr and x.get('access')=='exec' for x in hits),f'capture lacks expected native breakpoint {addr:05X}')

def event_case(v,name):
 b=v.snap('event_'+name+'_before');a=v.snap('event_'+name+'_after');i=1 if name.startswith('storm') else 0;d=b.c(i);want=d.copy();proof=v.js('event_'+name+'_verification')
 if name.startswith(('cold','sickness','storm')):
  positive=name.endswith('positive');eq(b.w(0x65b9),27 if i else 14,'event scenario');require(b.zero is not None and b.zero[0x8a]!=0,'event NMI RNG guard')
  seed=0xce if positive else 8 if name.startswith('sickness') else 4;eq(b.zero[0x84:0x8a],seed.to_bytes(6,'big'),'gate seed')
  eq(b.human(i),i==0,'controlled ownership')
  if positive:
   eff=v.snap('event_'+name+'_effect_seed');eq(eff.zero[0x84:0x8a],bytes.fromhex('123456789abc'),'damage seed');require(eff.zero[0x8a]!=0,'effect NMI guard');r=Rng(0x123456789abc);bonus=3*b.w(0x7a88)//2 if b.human(i) else 0
   def reduce(k,pct):want[k]-=want[k]*pct//100
   if name.startswith('storm'):reduce('defense',25+r.draw(6)+bonus);reduce('economy',25+r.draw(6)+bonus)
   elif name.startswith('cold'):want['commodity_history']=want['food_history']=0;reduce('economy',20+r.draw(6)+bonus)
   else:
    reduce('economy',10+r.draw(6)+bonus);reduce('morale',20+r.draw(6)+bonus);pct=15+r.draw(6)+bonus
    for k in GROUPS:reduce(k,pct)
   want['residents']=want['residents']*(80+r.draw(11))//100;want['trained']=want['trained']*(80+r.draw(11))//100
   eq([a.w(0x7b7c+2*j) for j in range(b.w(0x65b9))],[int(j==i) for j in range(b.w(0x65b9))],'single selected country');check_breakpoint(proof,0x4531)
  else:check_breakpoint(proof,0x42b1 if name.startswith('sickness') else 0x42a6)
 elif name.startswith('harvest'):
  k='good'+str(b.b(0x7692));want['food']=min(999,want['food']+want['food_history']);want[k]=min(999,want[k]+want['commodity_history']);eq(want['extra_used'],1,'harvest reset precondition');want['food_history']=want['commodity_history']=want['extra_used']=0;check_breakpoint(proof,0x48b1)
 else:
  p=sum(want[k] for k in GROUPS);short=False
  while p>2*want['food']:
   short=True;pct=200*want['food']//p
   for k in GROUPS:want[k]=want[k]*pct//100
   p=sum(want[k] for k in GROUPS)
  want['food']=0 if short else want['food']-p//2;check_breakpoint(proof,0x4106)
 v.econ(a,want,i)
 # Independent check of result fields; never trust the producing script's checks/pass flags.
 for k,x in proof['target'].items():
  if k in want:eq(x,want[k],'reported event target '+k)
 return {'target_country':i+1,'complete_target_economic_record_matches':True,'native_capture_boundary_verified':True}

def levy_case(v,q):
 b=v.snap(f'levy_{q}_before');a=v.snap(f'levy_{q}_result');d=b.c();E=d['economy'];P=b.p()
 fun=lambda x:x//2+((((E-q)*x*10)&65535)//E)//20
 d['gold']=min(999,d['gold']+q);poss={min(999,d['food']+q*p//100) for p in range(1,101)};require(a.c()['food'] in poss,'levy Food possible draws');d['food']=a.c()['food'];d['morale']=fun(d['morale']);target=fun(P)
 while sum(d[k] for k in GROUPS)>target:
  pct=target*100//sum(d[k] for k in GROUPS)
  for k in GROUPS:d[k]=d[k]*pct//100
 d['economy']=max(0,E-q);d['extra_used']=1
 if sum(d[k] for k in GROUPS)==0:d['arms']=d['skill']=0
 v.econ(a,d);v.costs(b,a,[10,0,0,10,0,0]);return {'quantity':q,'16_bit_multiply_wrap_checked':True,'old_v1_second_attempt_excluded':True}

def levy_repeat(v,q):
 b=v.snap(f'levy_repeat_v2_{q}_before');msg=v.snap(f'levy_repeat_v2_{q}_denial_message');a=v.snap(f'levy_repeat_v2_{q}_after');eq(b.c()['extra_used'],1,'used levy gate');v.unchanged(b,msg);v.unchanged(b,a);v.costs(b,a,[0]*6,0)
 return {'scope':'state and cost nonmutation verified; filename denial_message is not proof of visible denial text','visible_denial_text':'NOT VERIFIED; manually inspected live image shows Tax submenu Which option'}

def theft_case(v,positive):
 name='theft_'+('positive' if positive else 'negative');b=v.snap('event_'+name+'_before');a=v.snap('event_'+name+'_after');d=b.c();r=Rng(6 if positive else 0);eq(b.zero[0x84:0x8a],r.s.to_bytes(6,'big'),'theft seed');require(b.zero[0x8a]!=0,'theft NMIguard');dlevel=b.w(0x7a88);theft=True
 if d['morale']*d['trained']//100+r.draw(201)>16:
  if d['food']//90<b.p()//90+r.draw(176-5*dlevel):theft=False
 if theft and r.draw(100)>70+6*dlevel:theft=False
 if theft:d['food']=max(0,d['food']*(55+r.draw(21))//100-(5+r.draw(11)))
 eq(theft,positive,'theft gate');v.econ(a,d);row=v.record('theft_runtime_results',name);eq(row['active'],0,'theft active country');check_breakpoint(row,0x3de0c if positive else 0x346);return {'theft_triggered':theft,'Food':d['food']}

def cross_transfer(v):return move_scalar(v,'crosscountry',2,1,'crosscountry_before','crosscountry_transferred')
def cross_base(v):
 b=v.snap('crosscountry_transferred');c=v.snap('crosscountry_relocated_commit');a=v.snap('crosscountry_newbase_turn');d,e=b.c(),b.c(1);d['lord'],e['lord']=e['lord'],d['lord'];v.econ(c,d);v.econ(c,e,1);eq(c.w(0x7b36),0,'Base ends orders');eq(c.abilities(0x65bf),[x-(20 if i==4 else 0) for i,x in enumerate(b.abilities(0x65bf))],'Base Body20');eq(a.w(0x7b34),1,'later scheduler country2');eq(a.w(0x7b36),3,'new-base orders');eq(a.w(0x65bd),b.w(0x65bd),'same season');eq(a.c()['lord'],0x65de,'old base delegated pointer');eq(a.c(1)['lord'],0x65bf,'new base Temujin pointer');return {'same_season_destination_turn':True,'controlled_friendly_destination':'owner/status byte interventions recorded by producer'}
def cross_sale(v):
 b=v.snap('crosscountry_sale_before');a=v.snap('crosscountry_sale_after');d=b.c(1);q=quote(b,1,2,True);d['good2']-=1;d['gold']+=q//10;v.econ(a,d,1);v.unchanged(b,a);v.costs(b,a,[0,5,0,0,0,0],officer=0x65bf);eq(a.w(0x7b34),1,'destination active');return {'credit':q//10,'source_inventory_untouched':True}

def named(v):
 b=v.snap('named_later_buy_before');a=v.snap('named_later_buy_result');s=v.snap('named_later_sale_before');t=v.snap('named_later_sale_result');fb=v.snap('named_later_fall_before_treaty' if v.profile=='replay' else 'named_later_fall');fa=v.snap('named_later_fall_player');buy=quote(b,0,2);sell=quote(s,0,2,True);cost=(buy+9)//10;credit=sell//10
 d=b.c();d['good2']+=1;d['gold']-=cost;v.econ(a,d);v.costs(b,a,[0,5,0,0,0,0]);d=s.c();d['good2']-=1;d['gold']+=credit;v.econ(t,d);v.costs(s,t,[0,5,0,0,0,0]);eq((b.w(0x7a8a),cost),(101,7),'named initial Rate and debit');eq(fa.c()['gold']-fb.c()['gold'],10,'separate intervening treaty income');eq(t.c()['gold']-b.c()['gold'],10+credit-cost,'overall delta includes treaty');r=v.js('named_roundtrip_final');eq(r['signed_trade_profit'],credit-cost,'reported signed profit');return {'buy':cost,'sale':credit,'sale_rate':s.w(0x7a8a),'trade_profit':credit-cost,'other_treaty_income':10,'overall_gold_change':10+credit-cost,'item_name_scope':'Jewels identity relies on separate named-screen/source crosswalk; field index2 verified here'}

NATURAL_NONRAND=[k for k in F if not k.startswith('merchant')]+['trained']
def natural_transition(v,bname,aname,season,scenario):
 b=v.snap(bname);a=v.snap(aname);eq(b.w(0x65b9),scenario,'scenario before');eq(a.w(0x65b9),scenario,'scenario after');eq(a.w(0x65bd),season,'new season');eq(a.w(0x65bb),b.w(0x65bb)+(1 if season==0 else 0),'year advance')
 predicted,info=season_model(b.c(),season,b.b(0x7692));got=a.c();diff={k:{'observed':got[k],'no_event_expected':predicted[k]} for k in NATURAL_NONRAND if got[k]!=predicted[k]}
 if season:
  rate=b.w(0x7a8a);require(a.w(0x7a8a) in [max(50,rate-rate*p//100) for p in range(1,10)],'season Rate range/rounding')
 else:require(50<=a.w(0x7a8a)<50+94+7*a.w(0x7a88),'Spring Rate bound')
 for addr,n in [(0x7a8c,11),(0x7ace,17)]:
  for merchant in range(3):row=[a.w(addr+2*(merchant*n+j)) for j in range(n)];require(set(row)<={0,1} and any(row),'global offer mask domain/forced offer')
 if diff:
  # Preserve differences without calling a no-event counterfactual the expected natural result.
  if set(diff)=={'food'}:
   f=predicted['food'];candidates=[{'retained_percent':p,'flat_loss':loss} for p in range(55,76) for loss in range(5,16) if max(0,f*p//100-loss)==got['food']];info['theft_compatible_parameters']=candidates;require(bool(candidates),'Food delta not compatible with known theft effect')
  if season==2 and set(diff)<=set(['economy','morale','troops','town','masons','artisans','foodworkers','residents','trained']):
   p=predicted;ok=all(any(p[k]-p[k]*r//100==got[k] for r in range(lo,hi+1)) for k,lo,hi in [('economy',11,16),('morale',21,26)]) and any(all(p[k]-p[k]*r//100==got[k] for k in GROUPS) for r in range(16,22)) and all(any(p[k]*r//100==got[k] for r in range(80,91)) for k in ['residents','trained']);info['joint_sickness_effect_compatible']=ok;require(ok,'natural health delta not compatible with joint sickness effect')
  require('theft_compatible_parameters' in info or info.get('joint_sickness_effect_compatible'),f'unexplained natural economic differences: {diff}');info['untraced_event_differences']=diff;info['event_attribution']='INFERRED compatibility only; captured values verified, no event writer/gate trace in these broad snapshots'
 return info

def flat_summary(row,s):
 aliases={'fur':'good0','reserve':'residents','eligible_percent':'trained','eligible':'trained','used':'extra_used','farmers':'foodworkers','item_history':'commodity_history'}
 d=s.c()
 for k,x in row.items():
  mapped=aliases.get(k,k)
  if mapped in d and not isinstance(x,(dict,list)):eq(x,d[mapped],'summary '+k)
  elif k=='abilities':eq(x,s.abilities(),'summary abilities')
  elif k=='orders':eq(x,s.w(0x7b36),'summary orders')
  elif k=='rate':eq(x,s.w(0x7a8a),'summary Rate')
  elif k=='year':eq(x,s.w(0x65bb),'summary year')
  elif k=='season':eq(x,s.w(0x65bd),'summary season')

def summary_check(v,group,name):
 if group=='domestic6':
  if name=='tax100':row=v.js('tax100_corrected_results');s=v.snap('tax100_correct_result')
  else:row=v.record('domestic_probe_results',name);s=v.snap('draft8_result' if name=='draft8' else 'tax_'+name[3:]+'_result')
  flat_summary(row,s)
 elif group=='give_train15':flat_summary(v.record('give_train_results',name),v.snap((name if name.startswith('give_') else 'train_'+name)+'_result'))
 elif group=='assign6':
  row=v.record('assign_probe_results',name);s=v.snap('assign_'+name+'_result');eq(row['state'],s.meta,'embedded Assign snapshot');flat_summary(row,s)
 elif group=='matched4':
  row=v.record('matched_annual_results',name);s=v.snap('annual_match_'+name+'_spring');eq(row['result'],s.meta,'embedded annual snapshot');flat_summary(row['fields'],s)
 elif group=='trade_negative8':
  row=v.record('trade_negative_results',name);b=v.snap('trade_negative_'+name+'_before');a=v.snap('trade_negative_'+name+'_result');eq(row['before'],b.meta,'embedded negative before');eq(row['after'],a.meta,'embedded negative after');eq(row['stock'],a.c()['good0'],'negative reported stock')
 elif group=='logistics19':
  row=v.record('logistics_matrix_results' if name in ['gold','food'] else 'logistics_all_results',name);label='move_'+name+('_matrix' if name in ['gold','food'] else '');b=v.snap(label+'_before');a=v.snap(label+'_result');flat_summary(row,a)
  if name=='empty_source':eq(row['source_quality'],[a.c()[k] for k in ['arms','skill','morale']],'empty source summary');eq(row['destination_quality'],[a.c(1)[k] for k in ['arms','skill','morale']],'empty destination summary')
  else:
   k='gold' if name=='gold' else 'food' if name=='food' or name=='item10' else 'good'+name[4:] if name.startswith('item') else ['town','masons','foodworkers','artisans','troops'][int(name[-1])]
   for pref,snap in [('before',b),('after',a)]:eq(row['source_'+pref],snap.c()[k],'move source '+pref);eq(row['destination_'+pref],snap.c(1)[k],'move destination '+pref)
   if 'destination_quality' in row:eq(row['destination_quality'],[a.c(1)[k] for k in ['arms','skill','morale']],'move destination quality summary')
 elif group=='hire_weapon3':
  rows=v.js('hire_arms_results');row=next(x for x in rows if x['q']==(2 if name=='dagger2' else int(name[4:])));s=v.snap(name+'_result');flat_summary(row,s)
  if 'state' in row:eq(row['state'],s.meta,'embedded Hire snapshot')
 elif group in ['mongol_events10','world_storm2']:
  file='events_mongol_results' if group=='mongol_events10' else world_event_file(v,name);row=v.record(file,name);s=v.snap('event_'+name+'_after');d=s.c(1 if name.startswith('storm') else 0)
  for k,x in row['fields'].items():eq(x,d[k],'event aggregate '+k)
  eq(row['passed'],True,'producer event passed flag')
 elif group=='levy_first2':flat_summary(v.record('extra_tax_results',name),v.snap('levy_'+name[4:]+'_result'))
 elif group=='levy_repeat_v2_2':
  q=int(name[6:]);row=next(x for x in v.js('extra_tax_second_use_verified') if x['q']==q);b=v.snap(f'levy_repeat_v2_{q}_before');a=v.snap(f'levy_repeat_v2_{q}_after')
  for pref,snap in [('before',b),('after',a)]:eq(row['gold_'+pref],snap.c()['gold'],'repeat summary Gold');eq(row['orders_'+pref],snap.w(0x7b36),'repeat summary orders');eq(row['abilities_'+pref],snap.abilities(),'repeat summary abilities')
  eq(row['used'],a.c()['extra_used'],'repeat summary used')
 elif group=='theft2':
  row=v.record('theft_runtime_results','theft_'+name)
  for pref in ['before','after']:
   s=v.snap('event_theft_'+name+'_'+pref)
   for k in ['food','gold']:eq(row[k+'_'+pref],s.c()[k],'theft summary '+k)
 elif group=='crosscountry3' and name=='base':
  row=v.js('transfer_relocation_results');s=v.snap('crosscountry_newbase_turn')
  for k,val in {'active_country':s.w(0x7b34),'orders':s.w(0x7b36),'season':s.w(0x65bd),'country1_officer':s.c()['lord'],'country2_officer':s.c(1)['lord'],'temujin_body':s.abilities(0x65bf)[4],'destination_jewels':s.c(1)['good2'],'destination_gold':s.c(1)['gold']}.items():eq(row[k],val,'relocation summary '+k)
 elif group=='crosscountry3' and name=='sale':
  row=v.js('crosscountry_sale_results');b=v.snap('crosscountry_sale_before');s=v.snap('crosscountry_sale_after')
  for k,val in {'active_country':s.w(0x7b34),'orders':s.w(0x7b36),'season':s.w(0x65bd),'rate':s.w(0x7a8a),'source_jewels':s.c()['good2'],'destination_jewels':s.c(1)['good2'],'source_gold':s.c()['gold'],'destination_gold':s.c(1)['gold'],'temujin_before':b.abilities(0x65bf),'temujin_after':s.abilities(0x65bf)}.items():eq(row[k],val,'cross sale summary '+k)
 elif group=='named_later_rate1':
  row=v.js('named_roundtrip_final');bb=v.snap('named_later_buy_before');ba=v.snap('named_later_buy_result');b=v.snap('named_later_sale_before');a=v.snap('named_later_sale_result')
  for k,val in {'buy_rate':bb.w(0x7a8a),'buy_quote_tenths':quote(bb,0,2),'buy_cost':bb.c()['gold']-ba.c()['gold'],'sale_rate':b.w(0x7a8a),'sale_quote_tenths':quote(b,0,2,True),'sale_credit':a.c()['gold']-b.c()['gold'],'gold_before_sale':b.c()['gold'],'gold_after_sale':a.c()['gold'],'stock_before':b.c()['good2'],'stock_after':a.c()['good2'],'orders_before_sale':b.w(0x7b36),'orders_after_sale':a.w(0x7b36),'judgment_before_sale':b.abilities()[1],'judgment_after_sale':a.abilities()[1]}.items():eq(row[k],val,'named summary '+k)
 elif group=='natural_mongol_year2_4':flat_summary(next(x for x in v.js('year2_mongol_results') if x['label']==name),v.snap(name))

def verify_log(v):
 files=sorted(v.root.glob('events*.jsonl'));labels=set();errors=[];n=0;captured={};pending=None;label=None
 for f in files:
  for lineno,line in enumerate(v.read(f.name).splitlines(),1):
   try:r=json.loads(line);n+=1
   except Exception as e:errors.append({'file':f.name,'line':lineno,'error':str(e)});continue
   val=r.get('value',{})
   if r.get('kind')=='request' and isinstance(val,dict):
    pending=val
    if val.get('tool')=='video.export_frame':label=Path(val['path']).stem;labels.add(label);captured[label]={'export_file':f.name,'export_line':lineno}
   elif r.get('kind')=='response' and isinstance(val,dict) and pending:
    if label and pending.get('tool')=='cpu.read_memory' and pending.get('memoryType')=='nesMemory' and pending.get('address')==0x6000 and pending.get('length')==8192 and 'bytes' in val:
     captured[label]['sram_sha256']=sha(bytes(val['bytes']));captured[label]['sram_response_line']=lineno
    if label and pending.get('tool')=='cpu.read_memory' and pending.get('memoryType')=='nesMemory' and pending.get('address')==0 and pending.get('length')==256 and 'bytes' in val:captured[label]['zeropage_sha256']=sha(bytes(val['bytes']))
    pending=None
   if r.get('kind')=='error':errors.append({'file':f.name,'line':lineno,'producer_error':val})
 missing=sorted(v.used-labels);bound=[];missing_memory=[];mismatches=[]
 for label in sorted(v.used & labels):
  cap=captured[label];snap=v.cache.get(label)
  if not snap:continue
  if 'sram_sha256' not in cap:missing_memory.append(label);continue
  if cap['sram_sha256']!=sha(snap.data):mismatches.append({'label':label,'logged_sram_sha256':cap['sram_sha256'],'artifact_sram_sha256':sha(snap.data)})
  elif 'zeropage_sha256' in cap and snap.zero is not None and cap['zeropage_sha256']!=sha(snap.zero):mismatches.append({'label':label,'error':'logged zero-page bytes differ from artifact'})
  else:bound.append(label)
 return {'files':[p.name for p in files],'record_count':n,'logged_capture_labels':len(labels),'used_labels_without_export_record':missing,'used_labels_without_memory_response':missing_memory,'snapshots_bound_to_logged_memory_bytes':bound,'logged_memory_mismatches':mismatches,'producer_errors_and_parse_errors':errors,'status':'COMPLETE_FOR_USED_CAPTURES' if files and not missing and not missing_memory and not mismatches and not any('error' in e for e in errors) else 'INCOMPLETE','interpretation':'The verifier binds logged SRAM read-response bytes to persisted captures where available. Export presence alone is not input/action or screen-message proof; unlogged captures remain persisted-state evidence. Producer errors are preserved separately and do not invalidate fully bound, independently passing unrelated cases; parse errors do invalidate log completeness.'}

# Multi-root audit extension. Each Audit has a separate cache and input manifest;
# no missing primary capture is ever fetched from another evidence directory.
def assert_hit(blob,addresses,access):
 hits=[x.get('lastHit',{}) for x in blob.get('breakpoints',[]) if x.get('hit')]
 require(any(x.get('memoryType')=='nesPrgRom' and x.get('address') in addresses and x.get('access')==access for x in hits),'expected actual PRG breakpoint not hit: '+str(addresses))

def ai_case(v,label):
 row=v.record('ai_completion_results',label);i=row['country_index'];b=v.snap(label+'_before');a=v.snap(label+'_after');d=b.c(i);want=d.copy();r=Rng(int.from_bytes(b.zero[132:138],'big'));eq(r.s,0x123456789abc,'controlled AI seed');require(b.zero[138]!=0,'NMI seed guard');entry=row['entry'];bank=entry['entry_bank'];pc=int(entry['entry_vm'],16);eq(int(row['country_base'],16),BASE+83*i,'AI country base');assert_hit(entry['entry_breakpoints'],[bank*16384+pc-0x8000],'read')
 if label.startswith('ai_labor'):
  draw=r.draw(5);P=b.p(i);E=d['economy'];rowpct=[20,5,30,5] if E<300 else [18,7,30,5] if E<500 else [15,8,30,7]
  if draw==0 and d['gold']>150 and P>150 and b.w(0x65bd)!=3 and b.w(0x7b70)==0:rowpct=[5,1,8,1]
  for k,pct in zip(['town','masons','foodworkers','artisans'],rowpct):want[k]=P*pct//100
  want['troops']=P-sum(want[k] for k in GROUPS if k!='troops');stops=[0x8226]
 elif label.startswith('ai_winter_tax'):
  eq(b.w(0x65bd),3,'natural Winter');eq(b.w(0x7b70),0,'AI flag')
  if b.p(i)>600:want['tax']=50+r.draw(6)
  else:require(35<=d['tax']<=50,'tax no-op precondition')
  stops=[0x82ba,0x82eb]
 elif label.startswith('ai_hire'):
  price=60*(b.w(0x7a8a)+100)//100;P=b.p(i);q=min(999-P,10*d['gold']//price)//2 if d['gold']>2*d['troops'] else 0
  want['troops']+=q;want['gold']=(10*d['gold']-q*price)//10
  if q:
   for k,x in [('arms',100),('skill',100),('morale',50+r.draw(11))]:want[k]=(P*d[k]+q*x)//(P+q)
  stops=[0x8ae3]
 else:
  eq(pc,0x82b6,'AI Food routine');stops=[0x832b]
  # The original routine draws once for each non-player country in its full loop.
  for j in range(b.w(0x65b9)):
   out=b.c(j)
   if not b.human(j):
    gate=r.draw(100)<20*b.w(0x7a88);deficit=out['troops']*150//100-out['food'];surplus=out['gold']-out['troops']//2
    if gate and deficit>0 and surplus>0:q=min(deficit,2*surplus);out['food']=min(999,out['food']+q);out['gold']-=q//2
   v.econ(a,out,j)
   if j==i:want=out
 v.econ(a,want,i);eq(a.zero[132:138],r.s.to_bytes(6,'big'),'exact AI final RNG');assert_hit(row['return_breakpoints'],[bank*16384+x-0x8000 for x in stops],'read')
 for phase,s in [('before',b),('after',a)]:
  for k,x in row[phase].items():eq(x,s.c(i)[k],'AI summary '+phase+' '+k)
 return {'country':i+1,'all_economic_fields':True,'exact_rng':True,'scope':'bounded original routine reached by native Pass; controlled operands, not a complete autonomous turn'}

def natural_theft_trace(v):
 row=v.js('year2_natural_theft_trace');b=v.snap('year2_natural_theft_before');a=v.snap('year2_natural_theft_after');d=b.c();r=Rng(int.from_bytes(b.zero[132:138],'big'));hit=True
 if d['morale']*d['trained']//100+r.draw(201)>16:
  if d['food']//90<b.p()//90+r.draw(176-5*b.w(0x7a88)):hit=False
 if hit and r.draw(100)>70+6*b.w(0x7a88):hit=False
 require(hit,'natural theft gate must trigger');pct=55+r.draw(21);loss=5+r.draw(11);d['food']=max(0,d['food']*pct//100-loss);v.econ(a,d);eq(a.zero[132:138],r.s.to_bytes(6,'big'),'natural theft exact final RNG');assert_hit(row['breakpoints'],[0x3de0c],'exec');eq((b.c()['food'],a.c()['food']),(346,214),'natural theft observed loss');eq((row['before_food'],row['after_food']),(346,214),'trace summary');eq(row['before_seed'],b.zero[132:138].hex(),'trace initial seed');eq(row['after_seed'],a.zero[132:138].hex(),'trace final seed');eq(row['no_ram_interventions'],True,'producer no-RAM statement')
 # Independently inspect the recorded interval, not just the producer's boolean.
 start='year2_natural_theft_start';end='year2_natural_theft_after';v.snap(start);active=False;completed=False;writes=[]
 for p in sorted(v.root.glob('events*.jsonl')):
  for line in v.read(p.name).splitlines():
   x=json.loads(line);z=x.get('value',{})
   if x.get('kind')!='request' or not isinstance(z,dict):continue
   if z.get('tool')=='video.export_frame':
    label=Path(z['path']).stem
    if label==start:active=True
    if label==end and active:completed=True;active=False
   if active and z.get('tool') in ['cpu.write_memory','state.restore']:writes.append(z)
 require(completed,'natural theft interval missing from raw log');eq(writes,[],'no intervening RAM writes or restores')
 return {'classification':'OBSERVED_RUNTIME with exact source-formula/RNG confirmation','Food':[346,214],'retained_percent':pct,'flat_loss':loss,'no_ram_writes_in_logged_interval':True}

def domestic_case(v,report,case):
 name=case['name'];row=next((x for x in report['cases'] if x['case']['name']==name),None)
 if row is None:raise FileNotFoundError('Domestic case not yet captured: '+name)
 eq(row['status'],'PASS_VERIFIED_NATIVE','producer completion status');caps={}
 for c in row['captures']:
  s=v.snap(c['snapshot']['label']);eq(c['snapshot'],s.meta,'embedded snapshot');flat_summary(c['ledger'],s);eq(c['ledger']['country_hex'],s.data[BASE-0x6000:BASE-0x6000+83].hex(),'embedded83-byte record');caps[c['phase']]=s
 b=caps['before'];a=list(caps.values())[-1];d=b.c();kind=case.get('kind');group=case['group'];q=case.get('q')
 def same(x,y):v.unchanged(x,y);eq(y.data[BASE-0x6000:BASE-0x6000+83],x.data[BASE-0x6000:BASE-0x6000+83],'whole83-byte unchanged');v.costs(x,y,[0]*6,0)
 def gift(x,y,mode,qty):
  out=x.c();idx=10 if mode==1 else x.b(0x7692);key='gold' if mode==0 else 'food' if idx==10 else 'good'+str(idx);require(1<=qty<=out[key],'gift stock boundary');value=qty if mode==0 else min(999,qty*(sum(quote(x,m,idx,True) for m in range(3))//3)//10)+1;out[key]-=qty;out['morale']=min(999,out['morale']+30*value//x.p()+1);v.econ(y,out);v.costs(x,y,[0,0,5,0,0,0])
 if group=='labor':
  same(b,caps['staged_or_rejected'])
  if kind=='over':same(b,a)
  else:
   field=case['field'];old=d['troops'];pair=d[field]+old;d[field]=q;d['troops']=pair-q;pct=d['troops']*100//b.p()
   if d['troops']>old and pct>65:d['morale']=d['morale']*(100-2*(pct-65))//100
   v.econ(a,d);v.costs(b,a,[5,0,5,0,0,0]);eq(a.p(),b.p(),'labor total')
 elif group in ['specialty','gift_bounds']:
  if kind in ['positive','exact','cap']:gift(b,a,case['mode'],q)
  else:same(b,a)
 elif group=='draft':
  if kind=='chain':
   t=[s for phase,s in caps.items() if phase.startswith('trained_')][-1];d['trained']=50;v.econ(t,d);v.costs(b,t,[0,0,0,5,0,0]);b=t;d=b.c()
  if kind in ['invalid','refusal']:same(b,a)
  else:
   P=b.p();eligible=d['residents']*d['trained']//100;require(1<=q<=min(eligible,999-P),'Draft boundary')
   for k in ['arms','skill','morale']:d[k]=(P*d[k]+q)//(P+q)
   d['troops']+=q;d['residents']-=q;require(a.c()['trained'] in {max(0,100*(eligible-q)//d['residents']-r) for r in range(3)},'Draft RNG domain');d['trained']=a.c()['trained'];v.econ(a,d);v.costs(b,a,[20,0,0,10,0,10]);eq(a.p()+d['residents'],P+b.c()['residents'],'Draft resident conservation')
 elif group=='morale_carryover':
  start=caps['matched_before'];last=start
  for j in range(case['gifts']):
   now=[s for phase,s in caps.items() if phase.startswith('gift'+str(j+1)+'_after')][-1];gift(last,now,0,1);last=now
  before=caps['before_summer_pass'];eq(before.c(),last.c(),'gift carryover before Pass');pred,info=season_model(before.c(),1,before.b(0x7692))
  for k in ['merchant0','merchant1','merchant2']:pred[k]=a.c()[k]
  v.econ(a,pred);eq((a.w(0x65bb),a.w(0x65bd),a.w(0x7b36)),(1175,1,3),'Summer player turn')
 return {'source_formula_checks':'independent captured-byte arithmetic','producer_status':row['status'],'screen_text_asserted':False}

def run_domestic(v):
 files=sorted(v.root.glob('domestic_completion_*_results.json'))
 if not files:v.test('domestic_boundary48','manifest',lambda:(_ for _ in ()).throw(FileNotFoundError('Domestic48 report absent')));return
 reports=[(p,v.js(p.name)) for p in files];latest={}
 for p,report in reports:
  for case in report['case_manifest']:latest[case['name']]=(p,report,case)
 v.inventory.append({'file':'domestic_completion manifests','expected_distinct_cases':48,'observed_distinct_cases':len(latest),'status':'PASS' if len(latest)==48 else 'FAIL'})
 # Re-run each superseded attempt for transparent history; keep it out of the
 # current48-case total and current strict-log used-label set.
 for p,report in reports:
  for case in report['case_manifest']:
   if latest[case['name']][0]!=p:
    oldused=v.used.copy();v.test('superseded_domestic_attempt',p.stem+':'+case['name'],lambda case=case,report=report:domestic_case(v,report,case));v.prior_attempts.append(v.cases.pop());v.used=oldused
 for name,(p,report,case) in latest.items():v.test('domestic_boundary48',name,lambda case=case,report=report:domestic_case(v,report,case))

def year2_endpoint(v):
 b=v.snap('year2_spring1177_treaty');a=v.snap('year2_spring1177_player');eq((a.w(0x65bb),a.w(0x65bd),a.w(0x7b36)),(1177,0,3),'Year2 new player turn');d=b.c();d['gold']+=10;v.econ(a,d);eq((b.c()['gold'],a.c()['gold'],a.c()['food']),(444,454,161),'Year2 separate treaty credit and final ledger');flat_summary(v.js('year2_final_result'),a);return {'scope':'fresh recorded Spring1177 treaty-to-player endpoint only; preceding seasonal path belongs to historical capture root','separate_intervening_Gold_credit':10,'note':'Refusal is not assumed to prevent the original transfer helper; this endpoint proves +10 state change, not its native writer address'}

def run_natural(v,profile):
 if profile=='replay':world=['world_year_repeat_start','world_year_repeat_summer1206','world_year_repeat_fall1206','world_year_repeat_winter1206','world_year_repeat_spring1207']
 else:world=['world_first_turn_step2','world_summer_step1','world_fall_step1','world_winter_step1','world_spring1207_step1']
 for i in range(4):
  b=world[i];a=world[i+1]
  if i==2 and profile!='replay':b='world_fall_player'
  v.test('natural_world4',a,lambda b=b,a=a,i=i:natural_transition(v,('world_year_repeat_fall_player' if profile=='replay' and i==2 and v.snap(b).w(0x7b36)!=3 else b),a,(i+1)%4,27),scope='natural seasonal ledger; untraced event causes remain inferred')
 if profile=='replay':v.test('year2_fresh_endpoint1','Spring1177',lambda:year2_endpoint(v));return
 year2=['year2_spring1176_before','year2_summer1176','year2_fall1176','year2_winter1176','year2_spring1177']
 for i in range(4):
  b,a=year2[i:i+2];v.test('natural_mongol_year2_4',a,lambda b=b,a=a,i=i:natural_transition(v,b,a,(i+1)%4,14),scope='historical natural seasonal ledger; cause trace cross-referenced separately')
 first=['baseline','summer_step3','fall1175_ready','winter_step2','spring1176_step1']
 for i in range(4):
  b=first[i] if i!=2 else 'fall1175_player';a=first[i+1];v.test('natural_mongol_year1_4',a,lambda b=b,a=a,i=i:natural_transition(v,b,a,(i+1)%4,14))


def world_event_file(v,name):
 rows=v.js('events_world_results')
 return 'events_world_results' if any(x.get('case')==name for x in rows) else 'events_world_negative_results'

def world_event_inventory(v):
 try:
  rows=v.js('events_world_results');names=[x['case'] for x in rows];require(names in [['storm_positive'],['storm_positive','storm_negative']],'World event summary must contain positive alone or positive/negative exactly once')
  v.inventory.append({'file':'events_world_results','expected_records':len(names),'status':'PASS','format':'combined positive/negative' if len(names)==2 else 'legacy split'})
  if len(names)==1:v.count('events_world_negative_results',1)
 except Exception as e:v.inventory.append({'file':'events_world_results','status':'MISSING' if isinstance(e,FileNotFoundError) else 'FAIL','error':str(e)})

def run_core(v):
 world_event_inventory(v)
 for file,n in [('trade_matrix_results',84),('domestic_probe_results',6),('give_train_results',15),('assign_probe_results',6),('matched_annual_results',4),('trade_negative_results',8),('logistics_matrix_results',2),('logistics_all_results',17),('hire_arms_results',3),('events_mongol_results',10),('extra_tax_results',2),('extra_tax_second_use_verified',2),('theft_runtime_results',2)]:v.count(file,n)
 for m in range(3):
  for mode,n in [('buy',17),('sell',11)]:
   for i in range(n):v.test('trade84',f'm{m}_{mode}_{i:02d}',lambda m=m,mode=mode,i=i:trade_case(v,m,mode,i))
 for q in [0,1,99,100,101]:v.test('domestic6','tax'+str(q),lambda q=q:tax_case(v,q))
 v.test('domestic6','draft8',lambda:draft_case(v))
 for kind in ['gold','food','native_item']:
  for q in [0,1,8]:v.test('give_train15',f'give_{kind}_{q}',lambda kind=kind,q=q:give_case(v,kind,q))
 for name in ['people_baseline','people49','people50','people_zero_residents','troops_baseline','troops999']:v.test('give_train15',name,lambda name=name:train_case(v,name))
 for name in ['artisans16','town0','town66','town67_invalid','town5_noop','artisans16_cancel']:v.test('assign6',name,lambda name=name:assign_case(v,name))
 for name in ['control','town10','masons10','artisans10']:v.test('matched4',name,lambda name=name:annual_match(v,name))
 for name in ['judgment4','china_absent','hire_no_gold','hire_population999','buy_stock999','sell_gold999','buy_cancel','buy_zero']:v.test('trade_negative8',name,lambda name=name:negative_trade(v,name))
 for name,i,q in [('gold',-1,10),('food',10,8)]:v.test('logistics19',name,lambda name=name,i=i,q=q:move_scalar(v,name+'_matrix',i,q))
 for i in range(11):v.test('logistics19','item'+str(i),lambda i=i:move_scalar(v,'item'+str(i),i,1))
 for j in range(5):v.test('logistics19','people'+str(j),lambda j=j:move_people(v,'people'+str(j),j,8 if j==4 else 1))
 v.test('logistics19','empty_source',lambda:move_people(v,'empty_source',4,8))
 for q in [1,36]:v.test('hire_weapon3','hire'+str(q),lambda q=q:hire_case(v,q))
 v.test('hire_weapon3','dagger2',lambda:dagger2(v))
 for name in ['cold_positive','cold_negative','sickness_positive','sickness_negative','spring_food_10','spring_food_51','spring_food_52','harvest_0','harvest_1','harvest_2']:v.test('mongol_events10',name,lambda name=name:event_case(v,name))
 for name in ['storm_positive','storm_negative']:v.test('world_storm2',name,lambda name=name:event_case(v,name))
 for q in [1,9]:v.test('levy_first2','levy'+str(q),lambda q=q:levy_case(v,q));v.test('levy_repeat_v2_2','repeat'+str(q),lambda q=q:levy_repeat(v,q))
 for positive in [True,False]:v.test('theft2','positive' if positive else 'negative',lambda positive=positive:theft_case(v,positive))
 v.test('crosscountry3','transfer',lambda:cross_transfer(v));v.test('crosscountry3','base',lambda:cross_base(v));v.test('crosscountry3','sale',lambda:cross_sale(v));v.test('named_later_rate1','Jewels',lambda:named(v))

def corruption_controls(v):
 controls=[]
 try:
  s=v.snap('matrix_m0_buy_i00_result');bad=bytearray(s.data);bad[0xe0d]^=1
  try:validate_bytes('deliberate_one_byte_corruption',s.meta,bytes(bad),s.zero);raise RuntimeError('Corruption was not rejected')
  except AssertionError as e:require('SHA256' in str(e),'corruption rejected for wrong reason');controls.append({'test':'one_captured_byte_changed_original_digest','status':'PASS','detected':str(e),'mutation_sram_address':'6E0D','original_files_modified':False})
 except Exception as e:controls.append({'test':'one_captured_byte_changed_original_digest','status':'FAIL','error':str(e)})
 try:
  key='matrix_m0_buy_i00_result';original=v.snap(key);bad=bytearray(original.data);bad[0xe0d]^=1;meta=json.loads(json.dumps(original.meta));newgold=int.from_bytes(bad[0xe0d:0xe0f],'little');meta['gold']=newgold;meta['country_words']['0x6e0d']=newgold;meta['sram_sha256']=sha(bad)
  forged=validate_bytes('deliberate_forged_digest',meta,bytes(bad),original.zero);v.cache[key]=forged
  try:
   trade_case(v,0,'buy',0);raise RuntimeError('Semantic corruption was not rejected')
  except AssertionError as e:require('complete economic fields' in str(e),'semantic corruption rejected for unexpected reason');controls.append({'test':'changed_Gold_with_recomputed_digest_and_JSON_metadata','status':'PASS','detected_by':'independent full-economic-state trade formula','original_files_modified':False})
  finally:v.cache[key]=original
 except Exception as e:controls.append({'test':'changed_Gold_with_recomputed_digest_and_JSON_metadata','status':'FAIL','error':str(e)})
 return controls

def levy_longterm(v):
 ledger=v.js('levy_year_completed_ledger')
 for row in ledger:
  s=v.snap(row['label']);flat_summary(row,s);eq(row['tax_used'],s.c()['extra_used'],'levy ledger used flag')
 def effect(bn,an):
  b=v.snap(bn);a=v.snap(an);d=b.c();E=d['economy'];f=lambda x:x//2+((((E-1)*x*10)&65535)//E)//20;target=f(b.p());d['morale']=f(d['morale']);d['gold']+=1;d['economy']-=1;d['extra_used']=1
  while sum(d[k] for k in GROUPS)>target:
   pct=target*100//sum(d[k] for k in GROUPS)
   for k in GROUPS:d[k]=d[k]*pct//100
  require(a.c()['food'] in [d['food'],min(999,d['food']+1)],'levy1Food range');d['food']=a.c()['food'];v.econ(a,d);v.costs(b,a,[10,0,0,10,0,0])
 effect('levy_year_start','levy_year_first_levy');effect('levy_year_fall1175','levy_year_autumn_reuse')
 infos=[]
 for bn,an,season in [('levy_year_first_levy','levy_year_summer1175',1),('levy_year_summer1175','levy_year_fall1175',2),('levy_year_autumn_reuse','levy_year_winter1175_player',3),('levy_year_winter1175_player','levy_year_spring1176_player',0)]:
  b=v.snap(bn);a=v.snap(an);d,info=season_model(b.c(),season,b.b(0x7692))
  if season==3:d['gold']+=10;info['separate_intervening_Gold_credit']=10
  for k in ['merchant0','merchant1','merchant2']:d[k]=a.c()[k]
  v.econ(a,d);eq(a.w(0x65bd),season,'actual seasonal label');infos.append(info)
 b=v.snap('levy_year_spring1176_player');a=v.snap('levy_year_spring_repeat_denied');v.unchanged(b,a);v.costs(b,a,[0]*6,0);eq(b.c()['extra_used'],1,'Spring does not reset autumn reuse')
 return {'scope':'one native levy/year/reuse trajectory; deterministic ledger and carried used flag','season_infos':infos,'intervening_Gold10_cause':'separate event; this broad ledger does not trace its writer'}

def levy_denial_trace(v):
 row=v.js('levy_denial_breakpoint_results');entry=row['entry'];eq(entry['message_ptr'],0x9dc0,'used-levy denial message pointer');eq(entry['used'],1,'used levy flag');assert_hit(entry['breakpoint'],[0x3eb5d],'read');b=v.snap('levy_deny_bp_before');a=v.snap('levy_deny_bp_after');v.unchanged(b,a);v.costs(b,a,[0]*6,0);v.snap('levy_denial_actual_message');msg=v.snap('levy_denial_message_frame16');eq(sha(v.read('levy_denial_message_frame16.png')),'4d14327f1996c2446db5394743820efcf9d93f8e5f2da40643192aca553fe90b','independently visually reviewed denial image hash');v.unchanged(b,msg);v.costs(b,msg,[0]*6,0)
 return {'message_pointer':'9DC0','actual_original_message_call_observed':True,'screen_content_scope':'Exact PNG independently viewed 2026-11-04; hash bound here. No OCR or inferred no-op message claim.','visible_message':'Conditions in this country make it impossible to collect extra tax.'}

def root_report(v,strict=False):
 log=verify_log(v);prior_log=None
 if v.prior_attempts:
  saved=v.used;v.used={name for row in v.prior_attempts for name in row['snapshots']};prior_log=verify_log(v);v.used=saved
 counts=dict(collections.Counter(x['status'] for x in v.cases));groups={g:dict(collections.Counter(x['status'] for x in v.cases if x['group']==g)) for g in dict.fromkeys(x['group'] for x in v.cases)}
 failed=any(x['status']!='PASS' for x in v.cases+v.integrity+v.inventory) or bool(log['logged_memory_mismatches']) or (strict and log['status']!='COMPLETE_FOR_USED_CAPTURES')
 return {'evidence_root':str(v.root),'strict_log_required':strict,'case_counts':counts,'groups':groups,'snapshot_integrity_counts':dict(collections.Counter(x['status'] for x in v.integrity)),'case_results':v.cases,'snapshot_integrity':v.integrity,'summary_inventories':v.inventory,'superseded_attempts_preserved':v.prior_attempts,'superseded_attempt_log_audit':prior_log,'raw_event_log_audit':log,'input_manifest':v.inputs,'failed':failed}

class Tee:
 def __init__(self,stream,path):self.stream=stream;self.file=path.open('w')
 def write(self,s):self.stream.write(s);self.file.write(s);self.file.flush()
 def flush(self):self.stream.flush();self.file.flush()

def main():
 parser=argparse.ArgumentParser(description='Offline native captured-byte verification; no emulator calls')
 parser.add_argument('--evidence-root',type=Path,default=Path(__file__).resolve().parents[1]/'runtime/live');parser.add_argument('--historic-root',type=Path);parser.add_argument('--boundary-root',type=Path);parser.add_argument('--output-dir',type=Path);parser.add_argument('--require-complete-log',action='store_true');parser.add_argument('--world-profile',choices=['legacy','replay']);args=parser.parse_args()
 root=args.evidence_root.resolve();stamp=datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%S.%fZ');out=args.output_dir or Path(__file__).resolve().parent/('run_'+root.name+'_'+stamp);out.mkdir(parents=True,exist_ok=False);sys.stdout=Tee(sys.stdout,out/'stdout.log');sys.stderr=Tee(sys.stderr,out/'stderr.log')
 v=Audit(root,out);v.profile=args.world_profile or ('replay' if args.historic_root or root.name=='final_replay' else 'legacy');v.all_integrity();v.used=set();run_core(v);run_natural(v,v.profile);controls=corruption_controls(v);sections={'primary':root_report(v,args.require_complete_log)}
 if args.historic_root:
  h=Audit(args.historic_root.resolve(),out);h.all_integrity();h.used=set();run_natural(h,'legacy');sections['historical']=root_report(h,False)
 if args.boundary_root:
  b=Audit(args.boundary_root.resolve(),out);b.all_integrity();b.used=set();b.count('ai_completion_results',10)
  for label in ['ai_labor_e299','ai_labor_e300','ai_labor_e500','ai_winter_tax_p600','ai_winter_tax_p601','ai_food_ai_gold100','ai_food_human_gold100','ai_food_ai_gold30','ai_hire_gold200','ai_hire_gold201']:b.test('AI10',label,lambda label=label:ai_case(b,label))
  b.test('natural_theft_trace1','year2Food346to214',lambda:natural_theft_trace(b));b.test('levy_longterm1','annual_reuse',lambda:levy_longterm(b));b.test('levy_denial_trace1','message_pointer',lambda:levy_denial_trace(b));run_domestic(b);sections['boundary']=root_report(b,args.require_complete_log)
  if any(x['group']=='natural_theft_trace1' and x['status']=='PASS' for x in b.cases):
   context=h if args.historic_root else v
   for row in context.cases:
    if row['id']=='year2_fall1176' and row['status']=='PASS':
     try:
      eq(context.snap('year2_fall1176').c(),b.snap('year2_natural_theft_after').c(),'historical Fall and separately traced theft complete country fields');row['details']['event_attribution']='OBSERVED_RUNTIME in separate authentic natural replay, exact RNG and economic-record match';row['details']['trace_evidence_root']=str(b.root);row['details']['trace_file']='year2_natural_theft_trace.json'
     except Exception as e:row.update(status='FAIL',error=str(e))
   sections['historical' if args.historic_root else 'primary']=root_report(context,False if args.historic_root else args.require_complete_log)
 failure=any(x['failed'] for x in sections.values()) or any(x['status']!='PASS' for x in controls);counts=collections.Counter();integrity=collections.Counter()
 for r in sections.values():counts.update(r['case_counts']);integrity.update(r['snapshot_integrity_counts'])
 loglimits=any(r['raw_event_log_audit']['status']!='COMPLETE_FOR_USED_CAPTURES' for r in sections.values());status='STATE_VERIFICATION_FAILED_OR_MISSING' if failure else 'STATE_VERIFICATION_PASSED_HISTORICAL_LOG_LIMITATIONS_REMAIN' if loglimits else 'STATE_VERIFICATION_PASSED'
 receipt={'created_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'invocation':sys.argv,'working_directory':str(Path.cwd()),'environment':{'python_executable':sys.executable,'python_version':sys.version,'platform':platform.platform()},'verification_scope':'OFFLINE captured-state integrity and independent source-formula checks. No native emulator/bridge/Windows calls. PNG presence/dimensions/hash are not screen-content proof.','root_separation':'Primary and boundary strict logs bind only their own artifacts. Historical snapshots never substitute for absent fresh cases. Historical raw-log omissions are reported separately and not required complete.','sections':sections,'case_counts':dict(counts),'snapshot_integrity_counts':dict(integrity),'corruption_negative_controls':controls,'excluded_execution_evidence':EXCLUSIONS,'verifier_sha256':sha(Path(__file__).read_bytes()),'exit_code':int(failure),'delivery_status':status,'visual_limitations':['No-op equality proves captured state nonmutation, not menu refusal text.','The legacy levy_repeat_v2 denial_message images show Tax submenu; their filenames alone do not establish a displayed denial.'],'natural_event_attribution':'When boundary natural_theft_trace1 passes, the separate reproduced natural Year2 Food346→214 trace is OBSERVED_RUNTIME and exactly matches RNG and theft arithmetic; historical broad snapshots alone remain compatibility evidence.'}
 (out/'receipt.json').write_text(json.dumps(receipt,indent=2)+'\n');(out/'receipt.txt').write_text(status+'\nCases '+str(dict(counts))+'\nIntegrity '+str(dict(integrity))+'\n'+''.join(k+': '+str(r['case_counts'])+'; log '+r['raw_event_log_audit']['status']+'\n' for k,r in sections.items()))
 print(json.dumps({'receipt':str(out/'receipt.json'),'status':status,'cases':dict(counts),'integrity':dict(integrity),'exit':int(failure)},indent=2));return int(failure)
if __name__=='__main__':raise SystemExit(main())
