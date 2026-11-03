"""Deterministic public-command AI for the third game; synthetic strategy.
No direct state mutation. Uses ordinary Campaign.preview/execute, full quote and validation.
"""
import hashlib,json
from og.core.fixed import CoreError


def _roll(*args):
    return int(hashlib.sha256(json.dumps(args,sort_keys=True).encode()).hexdigest()[:8],16)%100


def take_turn(campaign,faction,style='balanced',prefix='AI',index=0):
    """Actually issues legal commands; no score based on future RNG outcomes."""
    s=campaign.state
    if not s['factions'][faction]['alive']:return {'kind':'inactive'}
    ids=[id for id,v in sorted(s['officer_faction'].items()) if v==faction]
    if not ids:return {'kind':'no_officer'}
    if style=='opportunistic':
        border_officers=[i for i in ids if any(s['owners'][n]!=faction for n in s['neighbors'][s['core']['officers'][i]['province_id']])]
        officer=max(border_officers or ids,key=lambda i:s['core']['officers'][i]['bp'])
    else:officer=max(ids,key=lambda i:s['core']['officers'][i]['bp'])
    home=s['core']['officers'][officer]['province_id']
    opponents=[x for x in 'ABC' if x!=faction and s['factions'][x]['alive']]
    border=[x for x in s['neighbors'][home] if s['owners'][x]!=faction and s['owners'][x] is not None]
    candidates=[]
    if opponents:
        to=opponents[_roll(faction,s['month'],'target')%len(opponents)]
        if style=='diplomatic' or _roll(style,s['month'],faction)<(25 if style=='cautious' else 45):
            candidates.append(('Ally',{'target':to}))
        candidates.append(('Give',{'target':to,'gold_cents':300}))
    if border and style=='opportunistic':
        to=sorted(border)[0]
        p=s['core']['provinces'][home]
        if p['troops']>80 and p['food_units']>25:
            candidates.insert(0,('attack',{'target':to,'troops':min(180,p['troops'])}))
    if style=='cautious' and s['core']['provinces'][home]['workers'].get('town',0)>=2:
        candidates.insert(0,('allocate_workers',{'province_id':home,'source':'town','target':'land','count':2}))
    if style=='logistics':
        safe=[k for k,v in s['owners'].items() if v==faction and any(s['owners'][n]==faction for n in s['neighbors'][k])]
        if safe:
            officer=next((id for id in ids if s['core']['officers'][id]['province_id'] in safe),officer)
            home=s['core']['officers'][officer]['province_id']
            friendly=[n for n in s['neighbors'][home] if s['owners'][n]==faction]
            if friendly:candidates.insert(0,('transport',{'target':sorted(friendly)[0],'commodity':'food','units':10}))
    if style=='delegation':candidates.insert(0,('appoint_governor',{'officer':officer,'focus':'farms'}))
    if style=='diplomatic' and opponents:
        to=opponents[_roll(faction,s['month'],'target')%len(opponents)]
        candidates.insert(0,('Ally',{'target':to}))
    if style=='balanced':
        if s['month']%4==0 and opponents:candidates.insert(0,('Give',{'target':opponents[0],'gold_cents':300}))
        else:candidates.insert(0,('market',{'side':'sell','commodity':'furs','units':1}))
    candidates.extend([('recruit',{'province_id':home,'count':10}),
                       ('train',{'effort':1}),('market',{'commodity':'metal','side':'buy','units':2})])
    for slot,(kind,params) in enumerate(candidates):
        request_id=f'{prefix}:{style}:{faction}:m{s["month"]}:{index}:{slot}'
        try:
            q=campaign.preview(kind,officer,params,request_id)
            campaign.execute(kind,officer,params,request_id,quote_id=q['quote_id'],expected_revision=q['revision'])
            if campaign.state['battle']:
                bq=campaign.preview('battle_auto',officer,{},request_id+':auto')
                campaign.execute('battle_auto',officer,{},request_id+':auto',quote_id=bq['quote_id'])
            return {'kind':kind,'officer':officer,'params':params,'faction':faction}
        except CoreError:
            continue
    return {'kind':'idle','faction':faction}


def profile_campaigns(profile_path,months=12,seed=7):
    from pathlib import Path
    from .integrated import new_campaign,save_bytes,load_bytes
    raw=json.loads(Path(profile_path).read_text())
    out=[]
    for profile in raw['profiles']:
        c=new_campaign(seed)
        me=profile['id'];actions=[]
        for m in range(1,months+1):
            # Style selection from actual profile traits; use same real third-game interface.
            desc=(profile['persona']+' '+profile['goal']).lower()
            style=('opportunistic' if 'conquer' in desc or 'aggress' in desc or me in ('P03','P09')
                   else 'logistics' if me=='P09' else 'delegation' if me=='P11' else 'cautious' if me in ('P02','P04','P05') else 'diplomatic' if me=='P10' else 'balanced')
            # Some profiles spend their month on decisions; P11 often delegates and idles.
            if not(me=='P11' and m%3==1):actions.append(take_turn(c,'A',style,me,0))
            for foe in ('B','C'):actions.append(take_turn(c,foe,'balanced',me+'-opponent',0))
            c.advance_month()
            if m==6:c=load_bytes(save_bytes(c))
        out.append({'profile':me,'months':months,'actions':len(actions),
                    'battle_results':len(c.state['history']),'diplomatic_orders':len([x for x in c.state['log'] if x['kind'] in ('Ally','Give','Marry','Cancel','Joint','Threat')]),
                    'provinces':sum(v=='A' for v in c.state['owners'].values()),
                    'gold_cents':sum(p['treasury_gold_cents'] for pid,p in c.state['core']['provinces'].items() if c.state['owners'][pid]=='A'),
                    'hash':c.fingerprint})
    return out
