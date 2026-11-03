"""Experimental counterfactual AI: state-dependent, documented-profile-weighted goals.

No changes to the historical game economics, data, underlying legal command engine,
or persistent save schema. Single legal command/faction/month, as in baseline v1.10.
This is deliberately a QA candidate, NOT approved default release AI.
"""
from collections import Counter
from og.core.fixed import CoreError
from .historical_ai import STYLES, PROFILES, _plausibly_legal
from .historical_s1 import list_recruitable

STYLE_TO_PROFILE=dict(zip(STYLES,PROFILES))
CATEGORY={'allocate_workers':'industry','distribute_food':'public_support',
          'distribute_goods':'public_support','recruit':'military','train':'officers','market':'trade',
          'appoint_governor':'officers','Ally':'officers','recruit_officer':'officers',
          'plan_attack':'military','attack':'military','transport':'infrastructure'}

def _recent(s, faction, maxn=6):
    out=[]
    for entry in reversed(s['log']):
        x=entry.get('id','')
        if x.startswith('balance-candidate:') and (':'+faction+':') in x and entry.get('kind') not in ('month','battle_auto'):
            out.append(entry['kind'])
            if len(out)>=maxn:break
    return out

def candidates(c,faction,style):
    s=c.state
    officers=sorted((oid for oid,f in s['officer_faction'].items() if f==faction),
                    key=lambda oid:(-s['core']['officers'][oid]['bp'],-s['core']['officers'][oid]['politics'],oid))
    out=[]
    for oid in officers[:7]:
        o=s['core']['officers'][oid];pid=o['province_id']
        p=s['core']['provinces'][pid]
        out.extend([(oid,'allocate_workers',{'province_id':pid,'source':'town','target':'land','count':2}),
                    (oid,'allocate_workers',{'province_id':pid,'source':'town','target':'farmers','count':2}),
                    (oid,'allocate_workers',{'province_id':pid,'source':'town','target':'metalworkers','count':2}),
                    (oid,'allocate_workers',{'province_id':pid,'source':'town','target':'resources','count':2}),
                    (oid,'distribute_food',{'province_id':pid,'units':min(10,p['food_units'])}),
                    (oid,'distribute_goods',{'province_id':pid,'units':min(5,p['specialty_units'])}),
                    (oid,'recruit',{'province_id':pid,'count':3}),
                    (oid,'train',{'trait':'politics','effort':1}),
                    (oid,'train',{'trait':'war','effort':1}),
                    (oid,'market',{'commodity':'furs','side':'sell','units':min(20,s['extras'][pid]['furs'])}),
                    (oid,'market',{'commodity':'metal','side':'buy','units':min(10,s['market_stocks'][pid]['metal'])}),
                    (oid,'appoint_governor',{'officer':oid,'focus':'farms' if style!='industrialist' else 'industry'})])
        for free in list_recruitable(s,pid)[:2]:
            out.append((oid,'recruit_officer',{'officer_id':free}))
        for d in s['neighbors'][pid]:
            owner=s['owners'][d]
            if owner not in (faction,None):
                army=s['armies'].get('army:'+oid,{})
                n=min(60,army.get('stations',{}).get(pid,0))
                if n>=20 and p['food_units']>=max(2,(n+29)//30):
                    out.append((oid,'attack',{'target':d,'troops':n}))
                if not any(w['actor']==faction and w['target']==d and w['expires']>=s['month'] for w in s['war_plans']):
                    out.append((oid,'plan_attack',{'target':d}))
            elif owner==faction:
                target=s['core']['provinces'][d]
                if p['food_units']>target['food_units']+50:
                    out.append((oid,'transport',{'target':d,'commodity':'food','units':min(10,p['food_units']-target['food_units'])}))
    # Actual diplomacy candidate; do not claim marriage/naval implementation.
    if style in ('court','benevolent','logistician','cautious','curious'):
        for oid in officers[:2]:
            for f,info in sorted(s['factions'].items()):
                if f!=faction and info['alive']:
                    out.append((oid,'Ally',{'target':f}))
                    if len(out)>300:break
    return out

def score(s,faction,style,oid,kind,params,recent,month):
    profile=STYLE_TO_PROFILE[style]
    weight=profile['priority_weights'][CATEGORY[kind]]
    o=s['core']['officers'][oid];pid=o['province_id'];p=s['core']['provinces'][pid]
    civilians=max(1,sum(p['workers'].values()))
    food=p['food_units'];troops=p['troops'];support=p['support_ticks']/50000
    margin=0.0
    if kind=='allocate_workers':
        bucket=params['target'];stock=p['workers'][bucket]
        desire={'farmers':0.40,'metalworkers':0.07,'resources':0.09,'land':0.05}[bucket]*civilians
        if bucket=='land':
            margin=8 if p['cultivation']<(1000 if s.get('balance_policy') else 100) else 0
        elif bucket=='farmers':
            margin=max(0,min(14,(desire-stock)/max(1,civilians)*75))
        elif bucket=='metalworkers':
            arm=s['extras'][pid]['arms']
            margin=max(0,min(14,(desire-stock)/max(1,civilians)*90))*(1 if arm<troops else 0.08)
        elif bucket=='resources':
            metal=s['extras'][pid]['metal']
            margin=max(0,min(12,(desire-stock)/max(1,civilians)*85))*(1 if metal<max(20,p['workers']['metalworkers']*2) else 0.05)
        weight=max(weight,profile['priority_weights']['food'] if bucket=='farmers' else 0)
    elif kind in ('distribute_food','distribute_goods'):
        threshold={'benevolent':78,'cautious':62,'aggressive':35,'farmer':45,'industrialist':44,'court':55}.get(style,50)
        margin=max(0,(threshold-support)/max(1,threshold))*22
        if food <max(60,troops//2) and kind=='distribute_food':margin=0
        if kind=='distribute_goods':margin*=1.12
    elif kind=='recruit':
        hostile=sum(s['owners'][d] not in (None,faction) for d in s['neighbors'][pid])
        enemy=max([s['core']['provinces'][d]['troops'] for d in s['neighbors'][pid] if s['owners'][d] not in (None,faction)] or [0])
        target=max(85, int(enemy*(0.75 if style=='aggressive' else 0.5)))
        margin=(10 if style=='aggressive' else 7)*max(0,min(1.2,(target-troops)/max(1,target)))
        if hostile==0 and style!='aggressive':margin*=0.10
        if food<max(50,troops//2) or p['treasury_gold_cents']<troops*100:margin*=0.15
    elif kind=='train':
        margin=max(0,1-o[params['trait']]/100)*4
        if style=='court':margin*=2
        if params['trait']=='war' and style=='aggressive':margin*=1.4
    elif kind=='market':
        commodity=params['commodity'];side=params['side']
        if side=='sell':
            margin=8 if s['extras'][pid][commodity]>=15 else 2
            if style=='trader':margin*=1.4
        else:
            margin=4 if s['extras'][pid]['metal']<10 and s['extras'][pid]['arms']<troops else 0
    elif kind=='recruit_officer':
        target=s['officer_catalog'][params['officer_id']]
        margin=8+2*(target['politics']+target['war']+target['leadership'])/300
        if style=='court':margin*=1.6
        if len([1 for f in s['officer_faction'].values() if f==faction])>25:margin*=0.3
    elif kind=='appoint_governor':
        if p['workers'].get('town',0)>=30:margin=9 if style in ('delegator','court','farmer','industrialist') else 4
    elif kind=='Ally':
        margin=8 if style=='court' else 2.2
        if s['owners'][next(iter(s['core']['provinces']))]==faction:margin+=0.01
    elif kind=='transport':
        dst=s['core']['provinces'][params['target']]
        margin=6 if food-dst['food_units']>100 else 1.8
    elif kind=='plan_attack':
        margin=1.2 if style=='rules_lawyer' else 0.15
        if kind in recent[:3]:margin=0.01
    elif kind=='attack':
        d=s['core']['provinces'][params['target']]
        defender=max(1,d['troops'])
        ratio=params['troops']/defender
        risk={'aggressive':0.6,'rules_lawyer':0.9,'cautious':1.6,'benevolent':1.6}.get(style,1.1)
        margin=(12 if style=='aggressive' else 6)*min(1.8,ratio) if ratio>=risk else 0.01
        if food<30:margin=0
    if margin<=0:return -100
    # Profile preferences + actual marginal needs, not an unconditional list-order policy.
    value=margin*(0.35+weight/23)
    if kind in recent[:4]:
        hits=recent[:4].count(kind)
        # Novice and obsessive profile intentionally still repeat more.
        penalty=0.30 if style in ('curious','rules_lawyer') else 0.53
        value*=penalty**min(3,hits)
    if style=='delegator' and kind=='appoint_governor':value*=2.1
    if style=='rules_lawyer' and kind=='market':value*=1.6
    if style=='logistician' and kind=='transport':value*=1.65
    return value

def take_turn(c,faction,style,month=None,prefix='balance-candidate'):
    if style not in STYLES:raise ValueError(style)
    s=c.state;month=s['month'] if month is None else month
    if not s['factions'][faction]['alive']:return {'kind':'inactive','faction':faction}
    # Preserve partial-attention behavior rather than forcing optimal expert play.
    if style=='delegator' and month%3==2:return {'kind':'delegated_idle','faction':faction,'attempted':0}
    recent=_recent(s,faction)
    opts=[]
    for idx,(oid,kind,p) in enumerate(candidates(c,faction,style)):
        if p.get('units',1)<1:continue
        if kind=='distribute_goods' and s['core']['provinces'][s['core']['officers'][oid]['province_id']]['specialty_units']<=0:continue
        if not _plausibly_legal(s,oid,kind,p,faction):continue
        val=score(s,faction,style,oid,kind,p,recent,month)
        if val>0:opts.append((-val,idx,oid,kind,p))
    opts.sort()
    # A failed legal preview must never mutate the original state; engine validates.
    attempted=0
    limit=STYLE_TO_PROFILE[style]['competence']['candidate_limit']
    for neg,idx,oid,kind,p in opts[:max(2,limit)]:
        request=f'{prefix}:{month}:{faction}:{style}:{idx}'
        attempted+=1
        try:
            q=c.preview(kind,oid,p,request)
            result=c.execute(kind,oid,p,request,quote_id=q['quote_id'],expected_revision=q['revision'])
            if c.state['battle']:
                req=request+':auto';t=c.preview('battle_auto',oid,{},req)
                c.execute('battle_auto',oid,{},req,quote_id=t['quote_id'],expected_revision=t['revision'])
            return {'kind':kind,'faction':faction,'actor':oid,'attempted':attempted,'result':result}
        except CoreError:
            continue
    return {'kind':'idle','faction':faction,'attempted':attempted}
