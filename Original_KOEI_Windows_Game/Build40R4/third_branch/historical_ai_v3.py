"""Han-named v3.2 deterministic player proxies for the ORIGINAL third-branch engine.

A deliberately bounded, testable decision policy (not an LLM). Does not alter
rules, inspect future RNG, mutate saves, or create unapproved naval mechanics.
Only real Campaign.preview/execute can authorize a state transition.

Profile strategy persists without a second mutable state: legal committed journal
records and outstanding public war plans reconstruct recent intent after cold load.
Each faction-month currently commits at most ONE nonbattle command because the
v1.10 game loop calls take_turn once. Attention budgets are a future integration
gate, not silently simulated as additional free actions.
"""
from __future__ import annotations
from collections import Counter
from dataclasses import dataclass
import hashlib
import json
from pathlib import Path
from og.core.fixed import CoreError
from . import army_cohorts as ac
from .historical_ai import STYLES

CONF = Path(__file__).resolve().parents[1] / 'config' / 'KOEI_12_Han_AI_Profiles_v3_2.json'
PROFILE_DATA = json.loads(CONF.read_text(encoding='utf-8'))
PROFILES = {p['id']:p for p in PROFILE_DATA['profiles']}
STYLE_TO_ID = {style:f'P{i+1:02}' for i,style in enumerate(STYLES)}

@dataclass(frozen=True)
class Choice:
    actor: str
    kind: str
    params: dict
    domain: str
    value: float
    reason: str


def _hashed(*args):
    return int(hashlib.sha256(json.dumps(args,sort_keys=True).encode()).hexdigest()[:12],16)


def _recent(s, faction, window=12):
    # Only public, previously committed game actions; no future or hidden state.
    return [x for x in s['log'] if x.get('month',0)>=s['month']-window
            and x.get('actor') in s['officer_faction']
            and s['officer_faction'][x['actor']]==faction and x.get('kind')!='month']


def _border(s, faction):
    out=[]
    for pid,owner in sorted(s['owners'].items()):
        if owner!=faction:continue
        for target in sorted(s['neighbors'][pid]):
            enemy=s['owners'][target]
            if enemy not in (faction,None) and '|'.join(sorted((faction,enemy))) not in s['alliances']:
                out.append((pid,target))
    return out


def _plan_target(s,faction,style,history):
    targets={target for _,target in _border(s,faction)}
    # Existing legal plan retains priority until its target ceases to be hostile.
    for q in reversed(s['war_plans']):
        if q['actor']==faction and q['target'] in targets:
            return q['target']
    # Reconstruct recent committed war intent on cold restart (no extra save fields).
    for q in reversed(history):
        if q['kind'] in ('plan_attack','attack'):
            target=q.get('result',{}).get('scheduled') or q.get('result',{}).get('target')
            if target in targets:return target
    if not targets:return None
    # Choose weakest visible frontier garrison, stable with same public state.
    return min(targets,key=lambda t:(s['core']['provinces'][t]['troops'],int(t[1:])))


def _source_reserve(s, pid, faction, departing):
    """Threat-sensitive candidate garrison; a policy guard, NOT a new game law.

    Uses currently visible adjacent hostiles, no opponent future knowledge.
    When no hostile border exists, retains a sustainable fraction of the source
    army against unexpected contacts. The committed army must stay legal.
    """
    nearby=[s['core']['provinces'][q]['troops'] for q in s['neighbors'][pid]
            if s['owners'][q] not in (None, faction)]
    if nearby:
        return max(15, (max(nearby)*4+4)//5 + 5*len(nearby))
    return max(15, (departing+3)//4)


def _source_safe(s, pid, faction, departing, commander=None):
    left=s['core']['provinces'][pid]['troops']-departing
    if commander is None:  # Retain historical policy helper for independent tests.
        return left >= _source_reserve(s,pid,faction,departing)
    from .defensive_move import source_safe
    return source_safe(s,pid,faction,departing,commander)


def _candidates(c,faction,style,profile,history):
    s=c.state;month=s["month"];out=[];target=_plan_target(s,faction,style,history)
    officers=sorted((oid for oid,v in s['officer_faction'].items() if v==faction),
        key=lambda k:(-s['core']['officers'][k]['bp'],-s['core']['officers'][k]['politics'],k))
    if not officers:return out
    recent_counts=Counter(x['kind'] for x in history)
    for oid in officers[:6]:
        o=s['core']['officers'][oid];pid=o['province_id'];p=s['core']['provinces'][pid];x=s['extras'][pid]
        food=p['food_units'];troops=p['troops'];gold=p['treasury_gold_cents'];w=p['workers']
        frontline=any(s['owners'][dst] not in (faction,None) for dst in s['neighbors'][pid])
        food_floor=max(30, (troops+59)//60*9)
        food_buffer=food-food_floor
        support=p['support_ticks']/50000
        def add(kind,params,domain,value,reason):
            out.append(Choice(oid,kind,params,domain,round(value,3),reason))
        # Conserve real garrison troops before occupying a neutral neighbor.
        # Early S1 places most armies entirely with one commander; splitting a
        # portion to a second present officer is a real player-legal action.
        vacant=sorted(dst for dst in s['neighbors'][pid] if s['owners'][dst] is None)
        stationed=ac.strength(s,oid,pid)
        own_army=s['armies'].get(ac.army_id(oid),{})
        can_depart=(stationed>0 and set(own_army.get('stations',{}))=={pid}
                    and not own_army.get('engaged')
                    and s['governors'].get(pid,{}).get('officer')!=oid)
        if vacant and can_depart and o['bp']>=8 and food>=max(1,(stationed+59)//60):
            if _source_safe(s,pid,faction,stationed,oid):
                for dst in vacant:
                    add('relocate_army',{'target':dst},'military',79,
                        'legal neutral occupation; defensive source garrison retained')
            elif stationed>=30:
                keep=_source_reserve(s,pid,faction,stationed)
                # Split only if real remaining mobile troops can still occupy.
                if keep+15<=stationed and p['troops']==stationed:
                    recipient=next((other for other in sorted(s['core']['officers'])
                                    if other!=oid and s['officer_faction'][other]==faction
                                    and s['core']['officers'][other]['province_id']==pid
                                    and ac.strength(s,other,pid)==0),None)
                    if recipient:
                        add('assign_troops',{'target_officer':recipient,'count':keep},
                            'military',83,'transfer real defenders before neutral occupation')
        # Land improvement is passive output of REAL land crews. Where the
        # player has spare town workers, the AI may reassign to land crews;
        # there is no instant Cultivate officer command.
        if o['bp']>=1 and p['cultivation']<(1000 if s.get('balance_policy') else 100) and w['town']>=2 and w['land']<max(2,w['farmers']//10):
            add('allocate_workers',{'province_id':pid,'source':'town','target':'land','count':2},
                'infrastructure',20+(20 if food_buffer<20 else 0),'support future monthly land improvement')
        if w['unassigned']>=5 and o['bp']>=1:
            if food_buffer<100 or style in ('farmer','cautious','benevolent'):
                add('allocate_workers',{'province_id':pid,'source':'unassigned','target':'farmers','count':5},'food',17+max(0,100-food_buffer)/12,'prepare harvest before shortage')
            if x['metal']<80 or style=='industrialist':
                add('allocate_workers',{'province_id':pid,'source':'unassigned','target':'resources','count':5},'industry',17+max(0,80-x['metal'])/10,'relieve metal input bottleneck')
            if x['arms']<max(30,troops//2) or style=='industrialist':
                add('allocate_workers',{'province_id':pid,'source':'unassigned','target':'metalworkers','count':5},'industry',16+max(0,troops//2-x['arms'])/8,'relieve arms output bottleneck')
            if style=='artisan' and w['artisans']<w['farmers']//2:
                add('allocate_workers',{'province_id':pid,'source':'unassigned','target':'artisans','count':5},'industry',22,'diversified artisan capacity')
        if o['bp']>=1 and support<95 and food>food_floor+10:
            units=min(100,max(10,min(food-food_floor,30)))
            add('distribute_food',{'province_id':pid,'units':units},'public_support',14+max(0,62-support)*0.6,'support threshold, not one-unit spam')
        from .recruitment_labor import available
        real_civilians=available(w) if s.get('workforce_policy') else w['unassigned']
        capacity=(100-ac.army_total(own_army)) if s.get('source_label')=='ORIGINAL_GAME_HISTORICAL_S1_PROPOSED' and own_army else 100
        # When neutral provinces remain, raise a REAL defense reserve or a
        # mobile army rather than trying to recruit from nonexistent workers.
        if (o['bp']>=1 and real_civilians>=10 and capacity>0 and food>=food_floor+8
                and (frontline or vacant or style in ('aggressive','cautious'))):
            n=min(20,real_civilians//3,capacity)
            if n>=1:
                add('recruit',{'province_id':pid,'count':n},'military',
                    27+(25 if vacant else 0)+(16 if frontline else 0)+max(0,60-troops)/3,
                    'conserved real civilian labor into commanded border troops')
        if o['bp']>=5:
            trait=('war' if style in ('aggressive','logistician','rules_lawyer') else 'politics' if style in ('court','delegator') else 'charisma' if style in ('trader','benevolent') else 'intelligence')
            if o[trait]<95 and (style in ('court','curious','rules_lawyer') or (style=='delegator' and not frontline)):
                add('train',{'trait':trait,'effort':1},'officers',10+max(0,70-o[trait])/5,'officer development, not unconditional max-stat growth')
        if o['bp']>=6 and style in ('delegator','court','farmer','industrialist'):
            current=s['governors'].get(pid,{});focus='industry' if style=='industrialist' else 'farms'
            if not current or current.get('focus')!=focus:
                add('appoint_governor',{'officer':oid,'focus':focus},'officers',32 if style=='delegator' else 16,'persistent delegated policy')
        if o['bp']>=4:
            # Sell only discretionary stock above a strategic reserve.
            for commodity,qty,floor in [('furs',x['furs'],0),('specialty',p['specialty_units'],10),('arms',x['arms'],max(20,troops//2)),('metal',x['metal'],40),('food',food,food_floor+80)]:
                excess=qty-floor
                if excess>0:
                    n=min(100,excess)
                    if n>=5:
                        add('market',{'commodity':commodity,'side':'sell','units':n},'trade',11+min(17,excess/15)+(10 if commodity in ('specialty','furs') and style in ('trader','artisan') else 0),'sell true discretionary inventory; retain reserves')
            if food<food_floor and s['market_stocks'][pid]['food']>=20 and gold>200:
                add('market',{'commodity':'food','side':'buy','units':min(50,max(10,food_floor-food))},'food',44,'emergency food purchase')
            if x['metal']<15 and s['market_stocks'][pid]['metal']>=10 and gold>1500:
                add('market',{'commodity':'metal','side':'buy','units':10},'industry',25,'critical input buy')
        if o['bp']>=4:
            for dst in sorted(s['neighbors'][pid]):
                if s['owners'][dst]!=faction:continue
                d=s['core']['provinces'][dst]
                destination_front=any(s['owners'][n] not in (faction,None) for n in s['neighbors'][dst])
                deficit=(d['food_units']<max(50,d['troops']//10) or
                         style=='logistician' and destination_front and d['food_units']*4<food*3)
                if food>food_floor+65 and deficit:
                    proposed=min(60,food-food_floor-25,max(1,(food-d['food_units'])//4))
                    if proposed>=10 and d['food_units']+proposed<=d['warehouse_capacity_units']:
                        add('transport',{'target':dst,'commodity':'food','units':proposed},
                            'infrastructure',40+(28 if style=='logistician' else 0),
                            'concentrate valid supply reserves in threatened/undersupplied province')
                if x['arms']>max(40,troops//2)+20 and s['extras'][dst]['arms']<d['troops']//3:
                    add('transport',{'target':dst,'commodity':'arms','units':min(30,x['arms']-max(40,troops//2))},'military',31+(20 if style=='logistician' else 0),'supply frontier equipment')
        if target and target in s['neighbors'][pid] and o['bp']>=2:
            strength=ac.strength(s,oid,pid);defender=s['core']['provinces'][target]['troops']
            if strength>=20 and food>=max(30,(strength+29)//30):
                power=max(0, strength-defender)
                # This source previously made almost every personality attack on
                # the opening turn. Restrict offensive initiative to genuine
                # military priorities; others fight only rare clear openings.
                military_priority=profile['priority_weights']['military']
                recent_attacks=sum(x['kind']=='attack' for x in history)
                qualifies=(style=='aggressive' or
                           style=='curious' and month%6==0 and strength>=2*max(1,defender) or
                           style=='rules_lawyer' and month%12==0 or
                           style in ('cautious','logistician','court','delegator','industrialist')
                           and month%12==0 and strength>=3*max(1,defender))
                # Rebuild temporarily after two unsuccessful recent campaigns.
                recent_wins=sum(x.get('winner')=='ATTACKER' for x in s['history'][-8:] if x.get('month',0)>=month-5 and x.get('src')==pid)
                if recent_attacks>=2 and not recent_wins and month%3:qualifies=False
                if qualifies and strength>=max(30,int(defender*1.15)) and o['bp']>=12:
                    add('attack',{'target':target,'troops':min(strength, max(30,min(150,strength)))},'military',51+min(35,power/6)+(18 if style=='aggressive' else 0),'execute feasible frontier offensive')
                elif style=='aggressive':
                    # The engine does NOT require plan_attack before attack. Do
                    # not pay BP every month for a plan that cannot execute.
                    # Instead consolidate stationed armies or recruit defenders.
                    colleagues=sorted((x for x,v in s['officer_faction'].items()
                                       if v==faction and x!=oid
                                       and s['core']['officers'][x]['province_id']==pid))
                    for donor in colleagues:
                        available=ac.strength(s,donor,pid)
                        if available>=10 and strength+available>=min(250,max(30,int(defender*1.15))):
                            add('assign_troops',{'target_officer':oid,'count':available},
                                'military',64,'consolidate a real field army for planned offensive')
                            break
        if o['bp']>=8 and style in ('court','cautious','benevolent'):
            enemies=[f for f in sorted(s['factions']) if f!=faction and s['factions'][f]['alive']
                     and '|'.join(sorted((faction,f))) not in s['alliances']]
            if enemies:
                foe=min(enemies,key=lambda f:s['hostility'].get(faction+'>'+f,50))
                add('Ally',{'target':foe},'officers',20+(9 if style=='court' else 0),'reduce diplomatic exposure')
    return out


def rank_choices(c, faction, style, month=None, *, with_audit=False):
    """Nonmutating public-candidate ranking; useful for exact QA counterfactuals."""
    if style not in STYLE_TO_ID:raise ValueError('Unknown v3 AI style')
    p=PROFILES[STYLE_TO_ID[style]];s=c.state
    if not s['factions'][faction]['alive']:return ([],{}) if with_audit else []
    month=s['month'] if month is None else month
    recent=_recent(s,faction,min(24,p['policy_v3']['goal_memory_months']))
    counts=Counter(x['kind'] for x in recent[-24:]);scores=[]
    for choice in _candidates(c,faction,style,p,recent):
        weight=p['priority_weights'].get(choice.domain,0)
        if not weight:continue
        # Specialty still matters, but does not bypass financial/legal constraints.
        base=choice.value*(0.7+weight/35)
        if style=='aggressive' and choice.kind=='attack':base+=46
        if style=='trader' and choice.kind=='market':base+=14
        if style=='logistician' and choice.kind=='transport':base+=35
        if style=='rules_lawyer' and choice.kind in ('market','train','recruit'):base+=12
        if style=='delegator' and choice.kind=='appoint_governor':base+=35
        # Experience improves use of visible opportunity costs and downside checks,
        # not resources, action legality or omniscience. A beginner largely ignores
        # displaced work; an expert calculates it from his own province ledger.
        comp=p['competence'];off=s['core']['officers'][choice.actor]
        home=s['core']['provinces'][off['province_id']]
        floor=max(30,(home['troops']+59)//60*9)
        opp=0.0;downside=0.0
        if choice.kind=='market':
            good=choice.params['commodity'];n=choice.params['units']
            if choice.params['side']=='sell' and good=='food':
                opp=20*min(1,n/max(1,home['food_units']-floor))
                downside=12 if home['food_units']-n<floor+30 else 0
            if choice.params['side']=='buy':opp=5
        if choice.kind=='train':opp=9 if any(s['owners'][dst] not in (faction,None) for dst in s['neighbors'][off['province_id']]) else 3
        if choice.kind=='recruit':
            real_available=sum(home['workers'][k] for k in ('farmers','town','artisans','metalworkers','resources','land','flood'))
            opp=4 if real_available>40 else 16
            downside=12 if home['food_units']<floor+20 else 0
        if choice.kind=='attack':
            target=choice.params['target'];enemy=s['core']['provinces'][target]['troops']
            strength=ac.strength(s,choice.actor,off['province_id'])
            downside=35*min(1,enemy/max(1,strength))
            opp=7 if home['food_units']<floor+40 else 2
        base-=opp*comp['opportunity_cost_pct']/100
        # Risk appetite modifies subjective downside; a high-risk conqueror
        # can rationally choose a risky battle while a survivor refuses it.
        risk_scale={'very_low':1.4,'low':1.2,'medium_low':1.1,'medium':1.0,
                    'medium_high':0.85,'very_high':0.55,'variable':1.0}
        base-=downside*comp['downside_stress_pct']/100*risk_scale.get(p['risk'],1)
        # Planning horizon: patient specialists value durable improvements more;
        # short-horizon rulers still see immediate action value.
        if choice.kind in ('allocate_workers','appoint_governor'):
            base+=min(9,p['horizon_months']/12)*comp['mechanic_knowledge_pct']/100
        # Visible prior-month records supply bounded adaptation. No direct access
        # to unseen opponent intentions or future random draws.
        if choice.kind in ('market','train','distribute_food') and counts[choice.kind]>=4:
            base-=min(12,counts[choice.kind]-3)*comp['adaptation_pct']/100
        # Reduce old one-command loops; specialists can repeat when genuinely profitable.
        repetition=counts[choice.kind]
        base-=min(25,max(0,repetition-2)*3.8)
        if style=='cautious' and choice.kind=='attack':base-=55
        if style=='aggressive' and choice.kind=='market':base-=26
        # Noise deterministically limits novice competence; NEVER controls legality.
        noise=p['competence']['evaluation_noise_bps']/10000.0
        base+=(_hashed(month,faction,p['id'],choice.actor,choice.kind,choice.params)%2001-1000)/1000 * noise*12
        scores.append((round(base,3),choice))
    scores.sort(key=lambda t:(-t[0],t[1].kind,t[1].actor,json.dumps(t[1].params,sort_keys=True)))
    # Fork Four: separately switchable, read-only advisor interpretation layer.
    # Only AI calls rank_choices for autonomous execution; the human gets zero
    # automatic advisor influence regardless of consultation content.
    from .fork4_advisor import evaluate_advisor
    config=s.get('fork4_advisor_config', {'enabled': True, 'weight_bps': 2000})
    influence=0 if faction==s.get('human_faction') else (int(config.get('weight_bps',2000)) if config.get('enabled',True) else 0)
    limited=scores[:max(1,int(p['competence']['candidate_limit']))]
    ranked,audit=evaluate_advisor(s,faction,limited,weight_bps=influence)
    return (ranked,audit) if with_audit else ranked


def _rules_lawyer_probes(c, actor, faction, month):
    # Negative-control previews only; never create out-of-range committed states.
    s=c.state;pid=s['core']['officers'][actor]['province_id'];before=c.fingerprint
    probes=[]
    for i,(kind,p) in enumerate([('distribute_food',{'province_id':pid,'units':101}),
                                ('market',{'commodity':'furs','side':'sell','units':0}),
                                ('recruit',{'province_id':pid,'count':-1})]):
        try:
            c.preview(kind,actor,p,f'ai3:probe:{month}:{faction}:{i}')
            probes.append({'kind':kind,'input':p,'result':'ACCEPTED_PREVIEW_REVIEW_REQUIRED'})
        except CoreError as e:
            probes.append({'kind':kind,'input':p,'result':'REJECTED','code':str(e)[:80]})
    if c.fingerprint!=before:raise RuntimeError('AI preview illegally mutated source campaign')
    return probes


def take_turn(c,faction,style,month=None,prefix='historical-ai'):
    if style not in STYLE_TO_ID:raise ValueError('Unknown v3 AI style')
    s=c.state;month=s['month'] if month is None else month
    profile=PROFILES[STYLE_TO_ID[style]]
    if not s['factions'][faction]['alive']:
        return {'kind':'inactive','faction':faction,'ai_name':profile['name']}
    recent=_recent(s,faction)
    if style=='delegator' and month%3==2:
        return {'kind':'delegated_idle','faction':faction,'ai_name':profile['name'],'attempted':0}
    ranked,advisor_meta=rank_choices(c,faction,style,month,with_audit=True)
    probes=[]
    if style=='rules_lawyer' and ranked:probes=_rules_lawyer_probes(c,ranked[0][1].actor,faction,month)
    attempted=0
    for index,(score,choice) in enumerate(ranked):
        request=f'{prefix}:ai3:{month}:{faction}:{profile["id"]}:{index}'
        attempted+=1
        try:
            q=c.preview(choice.kind,choice.actor,choice.params,request)
            result=c.execute(choice.kind,choice.actor,choice.params,request,
                             quote_id=q['quote_id'],expected_revision=q['revision'])
            if c.state['battle']:
                bq=c.preview('battle_auto',choice.actor,{},request+':auto')
                c.execute('battle_auto',choice.actor,{},request+':auto',
                          quote_id=bq['quote_id'],expected_revision=bq['revision'])
            return {'kind':choice.kind,'faction':faction,'actor':choice.actor,
                    'ai_name':profile['name'],'profile_id':profile['id'],
                    'attempted':attempted,'score':score,'reason':choice.reason,
                    'params':choice.params,'result':result,'probe_results':probes, 'fork4_advisor':advisor_meta}
        except CoreError:continue
    return {'kind':'idle','faction':faction,'ai_name':profile['name'],
            'profile_id':profile['id'],'attempted':attempted,'probe_results':probes,'fork4_advisor':advisor_meta}
