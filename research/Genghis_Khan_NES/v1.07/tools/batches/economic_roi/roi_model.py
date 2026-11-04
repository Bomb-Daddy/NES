#!/usr/bin/env python3
"""Conditional economic-policy ledger, independently checked against original6502.
Not a new Mesen run, natural RNG predictor, or a port implementation.
"""
import sys,os,json,hashlib,csv,copy
from pathlib import Path
sys.dont_write_bytecode=True
ROOT=Path(__file__).resolve().parent;BASE=ROOT.parents[1]
assert os.environ.get('GK_ROM'), 'Set GK_ROM explicitly'
sys.path.insert(0,str(BASE/'batches/seasonal_source'))
from source_cpu import Cpu
from inspect_source import prg
B=0x6dcd
FIELDS={'arms':0x14,'skill':0x16,'troops':0x18,'economy':0x1a,'town':0x1c,'defense':0x1e,'masons':0x20,'fur':0x22,'food':0x36,'commodity_history':0x38,'artisans':0x3a,'food_history':0x3c,'foodworkers':0x3e,'gold':0x40,'morale':0x42,'tax':0x44,'extra_used':0x46,'residents':0x49}
POOL=['troops','town','masons','artisans','foodworkers']
NAMES=['baseline','summer_step3','fall1175_ready','winter_step2','spring1176_step1'];RATES=[101,99,91,85,110]
RAW=(Path(os.environ['GK_ROI_EVIDENCE'])/'baseline.sram').read_bytes()
word=lambda d,a:int.from_bytes(d[a-0x6000:a-0x6000+2],'little')
COSTS={i:list(RAW[0x1e18+6*i:0x1e1e+6*i]) for i in range(32)}
PRICE=[list(RAW[0x1de5+17*m:0x1de5+17*(m+1)]) for m in range(3)]
PLANS=[('no_change',None,0,0),('reassign_artisans','artisans',0,0),('reassign_food','foodworkers',0,0),('reassign_town','town',0,0),('hire_artisans','artisans',10,0),('hire_food','foodworkers',10,0),('hire_town','town',10,0),('buy_food_summer',None,0,20)]

def state(data=RAW):
 return {**{k:word(data,B+off) for k,off in FIELDS.items()},'eligible':data[B+0x48-0x6000],'stats':[word(data,0x65cd+2*i) for i in range(6)]}
def population(s):return sum(s[k] for k in POOL)
def quote(rate,m,i,sell=False):
 p=PRICE[m][i]*(rate+100)//100;return max(1,p*90//100) if sell else p

def charge(s,code,ledger,season,label):
 cost=COSTS[code];assert all(v>=q for v,q in zip(s['stats'],cost))
 s['stats']=[v-q for v,q in zip(s['stats'],cost)]
 ledger.append({'season':season,'command':label,'code':code,'orders':1,'ability_cost':cost})
def rng(seed):
 n=(-((seed|1)*0xe0900e0187c5))&((1<<48)-1);return n,(n>>32)&0x7fff

def hire(s,q,seed=0x123456789abc):
 n=population(s);_,r=rng(seed);incoming=50+r%11
 for f,v in [('arms',100),('skill',100),('morale',incoming)]:s[f]=min(999,(s[f]*n+v*q)//(n+q))
 cost=(q*(60*201//100)+9)//10;assert cost<=s['gold'] and n+q<=999;s['gold']-=cost;s['troops']+=q
 return {'quantity':q,'incoming_morale':incoming,'gold_cost':cost}
def assign(s,field,q):
 assert s['troops']>=q;s['troops']-=q;s[field]+=q

def common(s):
 for history,field in [('food_history','foodworkers'),('commodity_history','artisans')]:s[history]=min(999,s[history]+s[field])
 for field,workers in [('economy','town'),('defense','masons')]:s[field]=min(999,s[field]-s[field]*4//100+s[workers])
def autumn(s):
 s['food']=min(999,s['food']+s['food_history']);s['fur']=min(999,s['fur']+s['commodity_history']);s['food_history']=s['commodity_history']=s['extra_used']=0

def spring(s):
 short=False
 while population(s)>2*s['food']:
  short=True;p=200*s['food']//population(s)
  for f in POOL:s[f]=s[f]*p//100
 n=population(s);used=s['food'] if short else n//2;s['food']=0 if short else s['food']-used
 gross=(s['economy']*s['tax']//100)*(12+min(13,s['morale']//9))//25
 available=s['gold']+gross;paid=min(s['troops'],available);s['troops']=paid;s['gold']=min(999,available-paid)
 pct=2 if s['tax']<50 else s['tax']-48;s['morale']-=s['morale']*pct//100
 # These seeded baseline comparisons all have reserve>=20; no unseen low-reserve RNG assumption.
 assert s['residents']>=20
 a=s['residents']*(s['eligible']+3)//100+1;b=s['residents']*105//100+1;s['eligible']=min(50,100*a//b)
 n=min(999,population(s));g=1 if n<20 else min(999,n*103//100)-n
 if n<20:s['troops']+=1
 else:
  q,r=divmod(g,5)
  for f in POOL:s[f]+=q
  for f in ['troops','masons','town','artisans'][:r]:s[f]+=1
 s['residents']=min(999,s['residents']+s['residents']*5//100+1)
 return {'food_used':used,'food_shortage':short,'gross_tax':gross,'troop_payroll':paid,'population_growth':g}

def simulate(plan,overrides=None,common_gold10=True,liquidate=False):
 name,field,hireq,buyq=plan;s=state();s.update(overrides or {});ledger=[];observations=[];transactions=[]
 if hireq:
  transactions.append({'season':'Spring1175','action':'hire',**hire(s,hireq)});charge(s,9,ledger,'Spring1175','Hire')
 if field:assign(s,field,10);charge(s,2,ledger,'Spring1175','Assign Labor')
 observations.append(copy.deepcopy(s));harvest=None;spring_cost=None
 for season in range(1,5):
  common(s)
  if season==1:
   s['skill']=s['skill']*93//100
   if buyq:
    p=quote(RATES[1],0,10);cost=(p*buyq+9)//10;assert s['food']+buyq<=999 and cost<=s['gold'];s['gold']-=cost;s['food']+=buyq
    charge(s,9,ledger,'Summer1175','Buy Food');transactions.append({'season':'Summer1175','action':'buy_food','quantity':buyq,'quote_tenths':p,'gold_cost':cost})
  if season==2:
   before=dict(s);autumn(s);harvest={'food':s['food']-before['food'],'fur':s['fur']-before['fur']}
   # Optional controlled monetization of only marginal first-harvest Fur.
   if liquidate and field=='artisans':
    baseline=simulate(PLANS[0],overrides,common_gold10,False);marginal=max(0,s['fur']-baseline['seasons'][2]['fur'])
    p=quote(RATES[2],0,0,True);q=min(marginal,s['fur'],10*(999-s['gold'])//p)
    if q:
     credit=q*p//10;s['fur']-=q;s['gold']=min(999,s['gold']+credit);charge(s,9,ledger,'Autumn1175','Sell marginal Fur');transactions.append({'season':'Autumn1175','action':'sell_fur','quantity':q,'quote_tenths':p,'gold_credit':credit})
  if season==3:
   # Recorded +10 occurs after Autumn harvest/diplomacy, before Winter snapshot.
   if common_gold10:s['gold']+=10
   s['arms']=s['arms']*97//100
  if season==4:spring_cost=spring(s)
  observations.append(copy.deepcopy(s))
 return {'name':name,'assumptions':{'common_observed_gold10':common_gold10,'liquidate_extra_fur':liquidate},'seasons':observations,'harvest':harvest,'spring':spring_cost,'commands':ledger,'transactions':transactions,'orders_total':len(ledger),'ability_cost_total':[sum(x['ability_cost'][i] for x in ledger) for i in range(6)]}

# Independent original-instruction reproduction using the seasonal worker's CPU interpreter.
def original(plan,liquidate=False):
 name,field,hireq,buyq=plan;c=Cpu(1);c.m[0x6000:0x8000]=RAW;c.setword(0x65b9,1);c.m[0x84:0x8a]=(0x123456789abc).to_bytes(6,'big')
 def exe(bank,start,stop,args=(),locals_=None):
  c.m[0x8000:0xc000]=prg[bank*16384:(bank+1)*16384];c.pc=0xefc9;c.setup(start,args)
  for p,v in (locals_ or {}).items():c.setword(0x7200+p,v)
  c.run(stop,maxsteps=10000000,trace=False)
 def charge_source(code):exe(3,0x8247,0x827a,[code])
 def snapshot():return state(bytes(c.m[0x6000:0x8000]))
 if hireq:exe(3,0x8d84,0x8d9a,[B],{-6:hireq,-4:120});charge_source(9)
 if field:
  ptr=B+FIELDS[field];pair=c.word(B+0x18)+c.word(ptr);new=c.word(ptr)+10
  exe(2,0x9f61,0x9f6c,[B],{-12:pair,-10:new,-14:ptr});charge_source(2)
 out=[snapshot()]
 for season in range(1,5):
  exe(1,0x9034,0x907a,locals_={-12:B})
  if season==1:
   exe(1,0x83ef,0x840e)
   if buyq:
    exe(3,0x8cfb,0x8d2d,[B],{-2:10,-4:buyq,-6:quote(99,0,10),-10:B+0x36});charge_source(9)
  if season==2:
   exe(1,0x8e55,0x8e9c)
   if liquidate and field=='artisans':
    marginal=max(0,c.word(B+0x22)-91);p=quote(RATES[2],0,0,True)
    exe(3,0x8bf1,0x8c06,[B],{-12:B+0x22,-8:p,-10:B+0x40})
    q=min(marginal,c.word(0x71fa))
    if q:exe(3,0x8c20,0x8c33,[B],{-4:q,-12:B+0x22,-8:p,-10:B+0x40});charge_source(9)
  if season==3:
   c.setword(B+0x40,c.word(B+0x40)+10);exe(1,0x83ca,0x83e9)
  if season==4:
   exe(1,0x809e,0x8105,[B]);exe(1,0x810b,0x8168,[B]);exe(1,0x898e,0x89c0);exe(1,0x8008,0x8063);exe(1,0x8a28,0x8ad7)
  out.append(snapshot())
 return out,c.count

def build_outputs():
 inputs={};observed={}
 for n in NAMES:
  p=BASE/'batches/runtime/live'/f'{n}.sram';b=p.read_bytes();inputs[n]={'path':str(p.relative_to(BASE)),'sha256':hashlib.sha256(b).hexdigest()};observed[n]={'fields':state(b),'year':word(b,0x65bb),'season':word(b,0x65bd),'rate':word(b,0x7a8a),'orders':word(b,0x7b36)}
 results=[simulate(p) for p in PLANS];liquidated=[simulate(p,liquidate=True) for p in PLANS if p[1]=='artisans']
 out={'evidence_class':'MODELED_SOURCE_CONDITIONED_POLICY_SCENARIOS','inputs':inputs,'observed_runtime_captures':observed,'policies':results,'conditional_extra_fur_sales':liquidated}
 (ROOT/'roi_results.json').write_text(json.dumps(out,indent=2)+'\n')
 with (ROOT/'roi_comparison.csv').open('w',newline='') as f:
  w=csv.writer(f);w.writerow(['scenario','gold_spring1176','gold_delta','food_spring1176','food_delta','fur_spring1176','fur_delta','economy','troops','total_drafted','food_history','commodity_history','orders_used','leadership_cost','judgment_cost','planning_cost','charm_cost','body_cost','battle_cost','gross_tax','spring_payroll','spring_food_used'])
  base=results[0]['seasons'][-1]
  for r in results+liquidated:
   s=r['seasons'][-1];w.writerow([r['name']+('_sell_extra' if r['assumptions']['liquidate_extra_fur'] else ''),s['gold'],s['gold']-base['gold'],s['food'],s['food']-base['food'],s['fur'],s['fur']-base['fur'],s['economy'],s['troops'],population(s),s['food_history'],s['commodity_history'],r['orders_total'],*r['ability_cost_total'],r['spring']['gross_tax'],r['spring']['troop_payroll'],r['spring']['food_used']])
 return out
if __name__=='__main__':build_outputs();print((ROOT/'roi_comparison.csv').read_text())
