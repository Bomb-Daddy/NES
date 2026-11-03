"""Original_KOEI_Windows_Game branch only: bounded v0 test persona adapter, NOT production AI."""
import collections, csv, hashlib, json, os, platform, sys, time
from pathlib import Path
from og.core import scheduler, world2
from og.core.bus import Bus
from og.core.fixed import CoreError
from og.core.save import dump_save, load_save
from og.core.world import new_world, add_province, add_officer, validate_world, world_hash

OUT=Path(__file__).resolve().parents[1]/'out'/'personas'
OUT.mkdir(parents=True,exist_ok=True)
CONFIG=Path('/mnt/data/KOEI_12_Player_Playtesting_Profiles_v1.json')
PROFILES=json.loads(CONFIG.read_text())['profiles']
SCORE_KIND={'cultivate':'infrastructure','farmers':'food','town':'trade','artisans':'industry',
 'metalworkers':'industry','resources':'industry','flood':'infrastructure','land':'infrastructure',
 'recruit':'military','distribute_food':'public_support','distribute_goods':'public_support'}
INTENTS={
'P01':['recruit','cultivate','farmers','distribute_food'],
'P02':['farmers','flood','cultivate','distribute_food'],
'P03':['recruit','recruit','farmers','cultivate'],
'P04':['farmers','cultivate','land','flood'],
'P05':['distribute_food','distribute_goods','farmers','cultivate'],
'P06':['town','town','farmers','cultivate'],
'P07':['metalworkers','resources','metalworkers','farmers'],
'P08':['artisans','artisans','town','farmers'],
'P09':['resources','farmers','town','cultivate'], # no naval, no shipping, no transport commands
'P10':['cultivate','cultivate','farmers','recruit'],
'P11':['farmers','cultivate','distribute_food'],
'P12':['recruit','distribute_food','town','cultivate']}
# Shortfall is recorded, never simulated as success. No naval requirements or action attempts.
UNAVAILABLE={'P01':'advisor,battle','P02':'advisor,defense_ai','P03':'battle,weapon_equipping',
'P04':'food_upkeep,advisor','P05':'advisor','P06':'market_trading','P07':'arms_metalworking',
'P08':'regional_goods_trade','P09':'land_logistics','P10':'officer_training,diplomacy',
'P11':'governor_delegation,advisor','P12':'market_trading,battle'}
W={'farmers':300,'town':120,'artisans':60,'metalworkers':30,'resources':30,'unassigned':90,'land':5,'flood':5}

def build(seed):
    w=new_world(seed)
    for pid,n in [('p1',45),('p2',28)]:add_province(w,pid,n)
    add_officer(w,'o1','p1',(75,55,35,65,80),bp=10)
    add_officer(w,'o2','p1',(45,70,70,35,40),bp=10)
    add_officer(w,'o3','p2',(45,45,45,45,45),bp=10)
    w=world2.migrate_v1_to_v2(w)
    for pid in ('p1','p2'):
        p=w['provinces'][pid];p['workers']={k:v if pid=='p1' else v//2 for k,v in W.items()}
        p.update(wealth_milli=40000,support_ticks=2000000,tax_rate_percent=30,
                 warehouse_capacity_units=250,treasury_gold_cents=2500,food_units=300,flood_control=20)
    validate_world(w);return Bus(w)

def envelope(b,pid,month,index,kind,params,actor='o1'):
    return {'request_id':f'{pid}-{month}-{index}','actor_officer_id':actor,
            'expected_world_revision':b.data['revision'],'kind':kind,'params':params}

def proposals(b):
    p=b.data['provinces']['p1'];w=p['workers'];out=[]
    for bucket in ('farmers','town','artisans','metalworkers','resources','land','flood'):
        if w['unassigned']>=1:out.append((bucket,'allocate_workers',dict(province_id='p1',source='unassigned',target=bucket,count=min(10,w['unassigned']))))
    if p['cultivation']<100:out.append(('cultivate','cultivate',{'province_id':'p1'}))
    if w['unassigned']>=1:out.append(('recruit','recruit',{'province_id':'p1','count':min(8,w['unassigned'])}))
    if p['food_units']>=1:out.append(('distribute_food','distribute_food',{'province_id':'p1','units':min(20,p['food_units'])}))
    if p['specialty_units']>=1:out.append(('distribute_goods','distribute_goods',{'province_id':'p1','units':min(5,p['specialty_units'])}))
    return out

def adversarial(b,month,o):
    cases=[('recruit',{'province_id':'p1','count':999999}),
        ('distribute_food',{'province_id':'p1','units':999999}),
        ('allocate_workers',{'province_id':'p1','source':'farmers','target':'farmers','count':1}),
        ('recruit',{'province_id':'p1','count':0}),('cultivate',{'province_id':'p2'})]
    kind,params=cases[(month-1)%len(cases)]
    c=envelope(b,'P12_probe',month,0,kind,params)
    before=world_hash(b.data)
    try:b.commit(c);o['invalid_accepted']+=1
    except CoreError as e:o['invalid_rejected']+=1;o['rejection_codes'][e.code]+=1
    if world_hash(b.data)!=before:o['unexpected_mutation']+=1

def state(b,profile,seed,month,o):
    p=b.data['provinces']['p1'];return dict(profile=profile,seed=seed,month=month,
        hash=world_hash(b.data),cultivation=p['cultivation'],food=p['food_units'],
        specialty=p['specialty_units'],gold_cents=p['treasury_gold_cents'],support_ticks=p['support_ticks'],
        workers=sum(p['workers'].values()),farmers=p['workers']['farmers'],artisans=p['workers']['artisans'],
        metalworkers=p['workers']['metalworkers'],town=p['workers']['town'],resources=p['workers']['resources'],
        troops=p['troops'],flood_control=p['flood_control'],bp=b.data['officers']['o1']['bp']+b.data['officers']['o2']['bp'],
        accepted=o['accepted'],invalid_rejected=o['invalid_rejected'],idle_months=o['idle_months'],
        flood=sum(x['kind']=='Flood' for x in b.data['event_ledger']),
        disease=sum(x['kind']=='Disease' for x in b.data['event_ledger']))

def run(profile,seed,months=120,reload=False):
    pid=profile['id'];b=build(seed);o=collections.Counter();o['rejection_codes']=collections.Counter();actions=[];snapshots=[]
    for m in range(1,months+1):
        if pid=='P12':adversarial(b,m,o)
        # An explicit limited-attention policy, not a research-calibrated human model.
        limit=0 if (pid=='P11' and m%3!=1) or (b.data['officers']['o1']['bp']==0 and b.data['officers']['o2']['bp']==0) else min(profile['attention_actions_per_month'],1 if pid in ('P01','P02','P05','P11') else 2)
        made=0
        for j in range(limit):
            choices=proposals(b)
            ordered=INTENTS[pid];selector=(m//3)%len(ordered) if pid=='P01' else 0
            scores=[]
            for tag,kind,params in choices:
                if kind not in b._handlers:continue
                actor='o1' if b.data['officers']['o1']['bp']>=2 else 'o2'
                c=envelope(b,pid,m,j,kind,params,actor)
                prev_hash=world_hash(b.data)
                try:b.preview(c)
                except CoreError:pass
                else:
                    rank=ordered.index(tag) if tag in ordered else 20
                    score=(rank-selector)%len(ordered) if rank<20 and pid=='P01' else rank
                    score+=(-profile['priority_weights'].get(SCORE_KIND[tag],0)/1000)
                    scores.append((score,tag,kind,params,actor))
                if world_hash(b.data)!=prev_hash:o['preview_mutation']+=1
            if not scores:break
            _,tag,kind,params,actor=min(scores,key=lambda x:(x[0],x[1]))
            c=envelope(b,pid,m,j,kind,params,actor);before=world_hash(b.data)
            try:
                quote=b.preview(c);b.commit(c,quote['quote_id']);made+=1;o['accepted']+=1
                actions.append(dict(month=m,kind=kind,tag=tag,params=params,actor=actor,hash=world_hash(b.data)))
            except CoreError as e:
                o['rejected']+=1;o['rejection_codes'][e.code]+=1
                if world_hash(b.data)!=before:o['unexpected_mutation']+=1
        if not made:o['idle_months']+=1
        scheduler.run_month(b);validate_world(b.data)
        if reload and m in (6,12,24,60,99):
            h=world_hash(b.data);data=dump_save(b.data);b=Bus(load_save(data))
            if h!=world_hash(b.data) or dump_save(b.data)!=data:o['reload_mismatch']+=1
        if m in (12,24,60,120):snapshots.append(state(b,pid,seed,m,o))
    return b,snapshots,o,actions

def main():
    st=time.monotonic();rows=[];runs=[];failures=[];trace={}
    print('PROFILE_SHA256',hashlib.sha256(CONFIG.read_bytes()).hexdigest(),'PYTHON',sys.version.split()[0],flush=True)
    print('SOURCE original game only; synthetic fixture; NO NAVAL; actor decisions via production preview/commit',flush=True)
    for seed in (7,8,41):
        for p in PROFILES:
            b,shots,o,actions=run(p,seed,120)
            rows.extend(shots)
            rec={'profile':p['id'],'name':p['name'],'seed':seed,'hash':world_hash(b.data),
                 'accepted':o['accepted'],'rejected':o['rejected'],'invalid_rejected':o['invalid_rejected'],
                 'invalid_accepted':o['invalid_accepted'],'preview_mutation':o['preview_mutation'],
                 'unexpected_mutation':o['unexpected_mutation'],'idle_months':o['idle_months'],
                 'rejection_codes':dict(o['rejection_codes']),'unavailable':UNAVAILABLE[p['id']].split(',')}
            if seed==7:
                trace[p['id']]=actions
                br,_,orr,_=run(p,seed,120,reload=True)
                rec['cold_load_equal']=world_hash(br.data)==world_hash(b.data) and orr['accepted']==o['accepted']
                if not rec['cold_load_equal']:failures.append({'profile':p['id'],'failure':'REPLAY_MISMATCH'})
            for key in ('invalid_accepted','preview_mutation','unexpected_mutation'):
                if rec[key]:failures.append({'profile':p['id'],'seed':seed,'failure':key,'count':rec[key]})
            runs.append(rec)
            print('DONE',p['id'],seed,'accepted',o['accepted'],'invalid_rejected',o['invalid_rejected'],
                  'food',shots[-1]['food'],'gold',shots[-1]['gold_cents'],'BP',shots[-1]['bp'],flush=True)
    # Passive baseline for each seed.
    baseline=[]
    for seed in (7,8,41):
        b=build(seed)
        for m in range(1,121):scheduler.run_month(b)
        baseline.append(state(b,'PASSIVE',seed,120,collections.Counter()))
    with (OUT/'milestones.csv').open('w',newline='') as f:
        writer=csv.DictWriter(f,fieldnames=rows[0]);writer.writeheader();writer.writerows(rows)
    (OUT/'report.json').write_text(json.dumps(dict(runs=runs,baselines=baseline,issues=failures,
        total_months=36*120+12*120+3*120,elapsed_seconds=round(time.monotonic()-st,2)),indent=2))
    (OUT/'trace_seed7.json').write_text(json.dumps(trace,indent=2))
    print('DONE ALL, campaigns=36, replay_campaigns=12, passive_baselines=3, issues=',len(failures),
          'seconds=',round(time.monotonic()-st,2),flush=True)
    sys.exit(0 if not failures else 2)
if __name__=='__main__':main()
