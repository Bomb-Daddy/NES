"""Twelve differentiated, legal-command AI player policies for historical Scenario 1.

Policies are explicit, reversible balance candidates. Nothing here mutates world
state directly; all actions go through Campaign.preview/execute/validation.
"""
from __future__ import annotations
from collections import Counter
from og.core.fixed import CoreError
from .historical_s1 import list_recruitable

STYLES=('curious','cautious','aggressive','farmer','benevolent','trader',
        'industrialist','artisan','logistician','court','delegator','rules_lawyer')
PROFILE_IDS=tuple(f'P{i:02}' for i in range(1,13))

def profile_data():
    import json
    from pathlib import Path
    path=Path(__file__).resolve().parents[1]/'config'/'KOEI_12_Han_AI_Profiles_v3_2.json'
    records=json.loads(path.read_text(encoding='utf-8'))['profiles']
    if len(records)!=12 or [p['id'] for p in records]!=list(PROFILE_IDS):
        raise ValueError('Expected exactly 12 approved player profiles')
    return records

PROFILES=profile_data()

def _options(c,faction,style,month):
    s=c.state
    officers=sorted((k for k,v in s['officer_faction'].items() if v==faction),
                    key=lambda k:(-s['core']['officers'][k]['bp'],-s['core']['officers'][k]['politics'],k))
    border=[];friendly=[]
    for oid in officers:
        p=s['core']['officers'][oid]['province_id']
        for dest in s['neighbors'][p]:
            if s['owners'][dest] not in (faction,None):border.append((oid,p,dest))
            elif s['owners'][dest]==faction:friendly.append((oid,p,dest))
    foes=sorted(f for f,z in s['factions'].items() if f!=faction and z['alive'])
    opts=[]
    for oid in officers[:5]:
        p=s['core']['officers'][oid]['province_id'];v=s['core']['provinces'][p]
        home=[
            ('allocate_workers',{'province_id':p,'source':'town','target':'farmers','count':2}),
            ('allocate_workers',{'province_id':p,'source':'town','target':'metalworkers','count':2}),
            ('allocate_workers',{'province_id':p,'source':'town','target':'resources','count':2}),
            ('distribute_food',{'province_id':p,'units':1}),
            ('recruit',{'province_id':p,'count':3}),
            ('train',{'trait':'intelligence','effort':1}),
            ('train',{'trait':'war','effort':1}),
            ('market',{'commodity':'furs','side':'sell','units':1}),
            ('market',{'commodity':'metal','side':'buy','units':2}),
            ('appoint_governor',{'officer':oid,'focus':'farms'}),
        ]
        style_order={
            'farmer':[0,1,4,8], 'industrialist':[2,3,9,0], 'trader':[8,9,4,0],
            'artisan':[2,8,0,3], 'benevolent':[4,0,1], 'cautious':[4,0,1,5],
            'curious': [0,8,5,6,4] if month%5==0 else ([8,5,6,0,4] if month%2==0 else [0,4,8,6,5]),
            'court':[6,10,7,0], 'rules_lawyer':[8,9,5,2,0],
            'recruiter':[5,0,7], 'scholar':[6,7,0], 'defender':[7,5,0],
            'logistician':[4,1,0], 'delegator':[10,1,0],
            'aggressive':[7,5,0], 'diplomatic':[4,6,0],
            'opportunist':[5,7,0], 'balanced':[0,8,4,5]}
        for i in style_order[style]:opts.append((oid,*home[i]))
    if style in ('court','curious') or (style=='trader' and month%6==0):
        for oid in officers[:2]:
            for f in foes[:4]:opts.insert(0,(oid,'Ally',{'target':f}))
    if style=='rules_lawyer' and border:
        for oid,p,dest in sorted(border):
            if s['core']['provinces'][p]['troops']>=40:
                opts.insert(0,(oid,'plan_attack',{'target':dest}))
                break
    if style=='aggressive' and border:
        for oid,p,dest in sorted(border):
            pstate=s['core']['provinces'][p]
            a=s.get('armies',{}).get('army:'+oid,{})
            n=min(60,a.get('stations',{}).get(p,0))
            if n>=20 and pstate['food_units']>30:
                opts.insert(0,(oid,'attack',{'target':dest,'troops':n}))
                break
    if style=='logistician' and friendly:
        oid,p,d=sorted(friendly)[0]
        opts.insert(0,(oid,'transport',{'target':d,'commodity':'food','units':2}))
    if style in ('rules_lawyer','court','curious'):
        for oid in officers[:4]:
            pid=s['core']['officers'][oid]['province_id']
            for free in list_recruitable(s,pid)[:1]:
                opts.insert(0,(oid,'recruit_officer',{'officer_id':free}))
    return opts

def _plausibly_legal(s,oid,kind,p,f):
    """A cheap necessary-condition prefilter. Engine remains the only rules authority;
    Campaign.preview/execute still checks all legal preconditions before every commit.
    """
    o=s['core']['officers'][oid]; pid=o['province_id'];v=s['core']['provinces'][pid]
    if s['owners'][pid]!=f:return False
    if kind=='allocate_workers':return v['workers'][p['source']]>=p['count'] and o['bp']>=1
    if kind=='distribute_food':return v['food_units']>=p['units'] and v['support_ticks']<5000000 and o['bp']>=1
    if kind=='recruit':
        from .recruitment_labor import available
        workers=v['workers']
        n=available(workers) if s.get('workforce_policy') else workers['unassigned']
        return n>=p['count'] and o['bp']>=1
    if kind=='train':return o[p['trait']]<100 and o['bp']>=5*p.get('effort',1)
    if kind=='market':
        good=p['commodity'];n=p['units']
        if o['bp']<4:return False
        if p['side']=='sell':return s['extras'][pid].get(good,0)>=n or v.get({'food':'food_units','specialty':'specialty_units'}.get(good,''),0)>=n
        return s['market_stocks'][pid][good]>=n and v['treasury_gold_cents']>=n
    if kind=='appoint_governor':
        return o['bp']>=6 and s['governors'].get(pid,{}).get('officer')!=oid
    if kind=='Ally':
        t=p['target']; alliance='|'.join(sorted((f,t)))
        last=s['diplomacy_last'].get(f+'>'+t+':Ally',0)
        return o['bp']>=8 and alliance not in s['alliances'] and (not last or s['month']-last>=3)
    if kind=='Give':return o['bp']>=5 and v['treasury_gold_cents']>=p['gold_cents']
    if kind=='plan_attack':return o['bp']>=2 and s['owners'][p['target']] not in (f,None)
    if kind=='attack':
        n=p['troops'];a=s['armies'].get('army:'+oid,{})
        return o['bp']>=12 and not s['battle'] and a.get('stations',{}).get(pid,0)>=n and v['food_units']>=max(1,(n+29)//30)
    if kind=='transport':return o['bp']>=4 and v['food_units']>=p['units'] and s['core']['provinces'][p['target']]['food_units']+p['units']<=s['core']['provinces'][p['target']]['warehouse_capacity_units']
    if kind=='recruit_officer':return o['bp']>=8 and p['officer_id'] in list_recruitable(s,pid)
    return True

def take_turn_legacy(c,faction,style,month=None,prefix='historical-ai'):
    if style not in STYLES:raise ValueError('Unknown AI style')
    s=c.state
    month=s['month'] if month is None else month
    if not s['factions'][faction]['alive']:return {'kind':'inactive','faction':faction}
    if style=='delegator' and month%3==2:return {'kind':'delegated_idle','faction':faction,'attempted':0}
    attempted=0
    for i,(oid,kind,p) in enumerate(_options(c,faction,style,month)):
        if not _plausibly_legal(c.state,oid,kind,p,faction):continue
        request=f'{prefix}:{month}:{faction}:{style}:{i}'
        attempted+=1
        try:
            quote=c.preview(kind,oid,p,request)
            result=c.execute(kind,oid,p,request,quote_id=quote['quote_id'],expected_revision=quote['revision'])
            # Never leave unresolved battles blocking monthly persistence.
            if c.state['battle']:
                req=request+':auto'
                q=c.preview('battle_auto',oid,{},req)
                c.execute('battle_auto',oid,{},req,quote_id=q['quote_id'],expected_revision=q['revision'])
            return {'kind':kind,'faction':faction,'actor':oid,'attempted':attempted,'result':result}
        except CoreError:
            continue
    return {'kind':'idle','faction':faction,'attempted':attempted}


def take_turn(c,faction,style,month=None,prefix='historical-ai'):
    """Current 12-profile Han AI v3, using the genuine public command gateway."""
    from .historical_ai_v3 import take_turn as v3_turn
    return v3_turn(c,faction,style,month=month,prefix=prefix)
