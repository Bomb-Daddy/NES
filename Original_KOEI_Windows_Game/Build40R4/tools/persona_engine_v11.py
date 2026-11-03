"""KOEI original-game playtest agent v1.1 (Zhuge Liang).

Runs against the *existing* OG world-v2 Bus, not the NES game. This is a
headless, player-visible-own-state PROXY, not a GUI agent or production AI.
Only Bus.preview/commit changes live game world. Test fixtures are synthetic.
Policy/skill coefficients are provisional -- never Director-approved game rules.
"""
import copy
import hashlib
import json
import sys
from pathlib import Path

from og.core import scheduler, world2
from og.core.bus import Bus
from og.core.fixed import CoreError
from og.core.save import dump_save, load_save
from og.core.world import world_hash, validate_world, new_world, add_province, add_officer

# Isolated deterministic test-fixture builder derived from the previously published
# persona-campaign harness. NOT an approved game scenario, and never a player cheat.
START_WORKERS={'farmers':300,'town':120,'artisans':60,'metalworkers':30,
               'resources':30,'unassigned':90,'land':5,'flood':5}

def build(seed):
    w=new_world(seed)
    for pid,n in [('p1',45),('p2',28)]:add_province(w,pid,n)
    add_officer(w,'o1','p1',(75,55,35,65,80))
    add_officer(w,'o2','p1',(45,70,70,35,40))
    add_officer(w,'o3','p2',(45,45,45,45,45))
    w=world2.migrate_v1_to_v2(w)
    for pid in ('p1','p2'):
        p=w['provinces'][pid]
        p['workers']={k:v if pid=='p1' else v//2 for k,v in START_WORKERS.items()}
        p.update(wealth_milli=40000,support_ticks=2000000,tax_rate_percent=30,
                 warehouse_capacity_units=250,treasury_gold_cents=2500,
                 food_units=300,flood_control=20)
    validate_world(w)
    return Bus(w)

def envelope(b,pid,month,index,kind,params,actor='o1'):
    return {'request_id':f'{pid}-{month}-{index}','actor_officer_id':actor,
            'expected_world_revision':b.data['revision'],'kind':kind,'params':params}

CATEGORIES = ('food','infrastructure','public_support','officers','trade','industry','military','naval')
PROFILE_KEYS = {'id','name','persona','skill','risk','horizon_months','micro','advisor','inspection',
    'attention_actions_per_month','source_influences','goal','behaviors','blind_spots','challenge',
    'failure_signatures','priority_weights','competence','personality','schema_note'}
SKILL_KEYS = {'tier','candidate_limit','mechanic_knowledge_pct','opportunity_cost_pct',
 'downside_stress_pct','evaluation_noise_bps','adaptation_pct','cross_domain_skill_floor',
 'domain_mastery_pct','planning_checks','signature_plan','specialty_domains',
 'strategic_blindspot','behavioral_rules','adaptation_trigger'}
ROOT_KEYS={'schemaVersion','title','date','status','scope','source_basis','priority_categories',
           'policy_contract','test_protocol','profiles','parent_source','provenance','competence_contract'}
BANNED = {'__class__','__dict__','__import__'}
PROFILE_FILENAME='KOEI_12_Player_Playtesting_Profiles_v1_1.json'
CONFIG_DEFAULT=Path(__file__).resolve().parents[1]/'config'/PROFILE_FILENAME
POLICY_VERSION='OG-v1.1-selector-1'
# These commands are IMPLEMENTED in the input source ZIP; not an assertion about a later native Windows build.
AVAILABLE={'cultivate','allocate_workers','distribute_food','distribute_goods','recruit'}
UNAVAILABLE={'market_trading','combat','ships','land_transport','metal_to_arms','officer_training',
             'diplomacy','governor_delegation','advisor','food_upkeep'}
DOMAIN={'farmers':'food','town':'trade','artisans':'industry','metalworkers':'industry',
 'resources':'industry','land':'infrastructure','flood':'infrastructure','cultivate':'infrastructure',
 'recruit':'military','distribute_food':'public_support','distribute_goods':'public_support'}


def _no_dupe(pairs):
    d={}
    for k,v in pairs:
        if k in d: raise ValueError('duplicate JSON key: '+k)
        d[k]=v
    return d


def _reject_constant(s): raise ValueError('nonfinite constant: '+s)


def load_profiles(path=CONFIG_DEFAULT):
    raw=Path(path).read_bytes()
    if len(raw)>250_000: raise ValueError('oversized config')
    data=json.loads(raw.decode('utf-8'),object_pairs_hook=_no_dupe,parse_constant=_reject_constant)
    if not isinstance(data,dict) or set(data)!=ROOT_KEYS or data['schemaVersion']!='1.1':
        raise ValueError('unknown schema or root fields')
    if data['priority_categories']!=list(CATEGORIES): raise ValueError('priority category schema')
    p=data['profiles']
    if not isinstance(p,list) or len(p)!=12 or [x.get('id') for x in p]!=[f'P{i:02d}' for i in range(1,13)]:
        raise ValueError('profile list/ids')
    for x in p:
        if set(x)!=PROFILE_KEYS:raise ValueError('profile keys '+str(x.get('id')))
        w=x['priority_weights'];c=x['competence']
        if not isinstance(w,dict) or set(w)!=set(CATEGORIES) or not all(type(v) is int and 0<=v<=100 for v in w.values()) or sum(w.values())!=100:
            raise ValueError('priority weights')
        if not isinstance(c,dict) or set(c)!=SKILL_KEYS:raise ValueError('competence keys')
        for field in ('candidate_limit','mechanic_knowledge_pct','opportunity_cost_pct','downside_stress_pct','adaptation_pct','cross_domain_skill_floor','planning_checks'):
            if type(c[field]) is not int or not (1<=c[field]<=100 if field in ('candidate_limit','planning_checks') else 0<=c[field]<=100):
                raise ValueError('competence '+field)
        if type(c['evaluation_noise_bps']) is not int or not 0<=c['evaluation_noise_bps']<=10000:raise ValueError('noise')
        if not isinstance(c['domain_mastery_pct'],dict) or any(k not in CATEGORIES or type(v) is not int or not 0<=v<=100 for k,v in c['domain_mastery_pct'].items()):
            raise ValueError('domain mastery')
        if not isinstance(x['name'],str) or any(k in x['name'] for k in BANNED):raise ValueError('name')
        if type(x['attention_actions_per_month']) is not int or not 1<=x['attention_actions_per_month']<=20:raise ValueError('attention')
        if not isinstance(c['behavioral_rules'],list) or len(c['behavioral_rules'])!=3 or not all(isinstance(s,str) for s in c['behavioral_rules']):raise ValueError('behavior rules')
    return p,hashlib.sha256(raw).hexdigest()


def stable_unit(*parts):
    h=hashlib.sha256(json.dumps(parts,separators=(',',':'),ensure_ascii=True).encode()).digest()
    return int.from_bytes(h[:8],'big')/float(1<<64)


def visible_own_state(bus):
    """Own-province/officer abstraction; UI binding is NOT established in this source."""
    own=copy.deepcopy(bus.data['provinces']['p1'])
    officers={k:copy.deepcopy(v) for k,v in bus.data['officers'].items() if v['province_id']=='p1'}
    return {'province':own,'officers':officers}  # excludes p2, all future RNG and journal


def legal_choices(bus,ctx,profile,decision_id):
    """Finite, ordinary commands only; all legality checked by production Bus.preview.
    Candidate amount variants permit experienced personas to evaluate marginal scale.
    """
    p=ctx['province'];w=p['workers'];choices=[]
    for tag in ('farmers','town','artisans','metalworkers','resources','land','flood'):
        for qty in (5,15,40):
            if w['unassigned']>=qty:
                choices.append((tag,'allocate_workers',{'province_id':'p1','source':'unassigned','target':tag,'count':qty}))
    if p['cultivation']<100:choices.append(('cultivate','cultivate',{'province_id':'p1'}))
    for qty in (5,12,35):
        if w['unassigned']>=qty:choices.append(('recruit','recruit',{'province_id':'p1','count':qty}))
        if p['food_units']>=qty:choices.append(('distribute_food','distribute_food',{'province_id':'p1','units':qty}))
        if p['specialty_units']>=qty:choices.append(('distribute_goods','distribute_goods',{'province_id':'p1','units':qty}))
    choices.sort(key=lambda x:(stable_unit(profile['id'],decision_id,x[0],x[2].get('count',x[2].get('units',1))),x[0]))
    accepted=[]; rejected=[]
    for tag,kind,params in choices:
        if kind not in AVAILABLE or kind not in bus._handlers:continue
        # No secret privilege: actor exists in own state and has enough BP according to validator.
        for actor in sorted(ctx['officers'],key=lambda a:(-ctx['officers'][a]['bp'],a)):
            c=envelope(bus,profile['id']+'-v11',decision_id, len(accepted)+len(rejected),kind,params,actor)
            before=world_hash(bus.data)
            try:preview=bus.preview(c)
            except CoreError as e:rejected.append({'tag':tag,'reason':e.code});continue
            finally:
                if world_hash(bus.data)!=before:raise AssertionError('Preview mutated game world!')
            accepted.append({'tag':tag,'kind':kind,'params':params,'actor':actor,'quote':preview,'category':DOMAIN[tag]})
            break
    return accepted,rejected


def quality(candidate,ctx,profile):
    """Visible-state diagnostic value: independent of personality or skill; synthetic, not gameplay economy.
    Measured on one constrained decision opportunity; does not imply game victory.
    """
    p=ctx['province'];w=p['workers'];tag=candidate['tag'];a=candidate['params']
    qty=a.get('count',a.get('units',1));cat=candidate['category']
    # Approximate durable output, immediacy and known opportunity costs from own-visible info.
    if tag=='farmers':base=0.58*min(qty,30)/30+ (0.18 if p['food_units']<130 else 0)
    elif tag=='land':base=0.43*min(qty,30)/30+(0.13 if p['cultivation']<50 else 0)
    elif tag=='flood':base=0.43*min(qty,30)/30+(0.19 if p['flood_control']<30 else 0)
    elif tag=='cultivate':base=0.37*(1-p['cultivation']/100)
    elif tag=='town':base=0.35*min(qty,30)/30+(0.08 if p['treasury_gold_cents']<3000 else 0)
    elif tag=='artisans':base=0.34*min(qty,30)/30
    elif tag=='metalworkers':base=0.03 # phase 4 metal-to-arms UNAVAILABLE: no pretend equipment gain
    elif tag=='resources':base=0.035 # no implemented extraction in this snapshot
    elif tag=='recruit':base=0.58*min(qty,30)/30
    elif tag=='distribute_food':base=0.45*min(qty,30)/30 + (0.13 if p['support_ticks']<1200000 else 0)
    elif tag=='distribute_goods':base=0.47*min(qty,30)/30 + (0.13 if p['support_ticks']<1200000 else 0)
    else:base=0
    # All values derived from own visible proxies / fixed publicly documented fixture estimates.
    labor_cost=(qty/150 if a.get('source')=='unassigned' else 0)
    troop_support_cost=0.16*(qty/35) if tag=='recruit' else 0
    stock_cost=0.15*qty/max(20,p['food_units']) if tag=='distribute_food' else (0.12*qty/max(20,p['specialty_units']) if tag=='distribute_goods' else 0)
    downside=(0.12 if (tag=='recruit' and p['food_units']<150) else 0)
    true_net=base-labor_cost-troop_support_cost-stock_cost-downside
    weighted=true_net * (profile['priority_weights'][cat]/100)
    # Importance of strategic category & visible resource risk. No hidden AI/state access.
    return {'base':base,'cost':labor_cost+troop_support_cost+stock_cost,'downside':downside,
            'weighted':weighted,'net':true_net,'category':cat}


def judge(c,ctx,profile,seed,decision_id,competence=None):
    sk=competence or profile['competence'];q=quality(c,ctx,profile);cat=q['category']
    # Weaker players consistently undercount opportunity costs and downsides.
    oc=sk['opportunity_cost_pct']/100
    ds=sk['downside_stress_pct']/100
    actual=q['base']-q['cost']*oc-q['downside']*ds
    domain=sk['domain_mastery_pct'].get(cat,sk['cross_domain_skill_floor'])/100
    knowledge=sk['mechanic_knowledge_pct']/100
    recognized=actual*(0.32+0.68*knowledge)*(0.35+0.65*domain)
    noise=(stable_unit('noise',seed,decision_id,profile['id'],c['tag'],str(c['params']))-0.5)*2*(sk['evaluation_noise_bps']/10000)
    # No free outcome bonuses. Preferences and bias influence only selection.
    score=recognized*(profile['priority_weights'][cat]/100)+noise
    if profile['id']=='P03' and c['tag']=='recruit':score+=0.025
    if profile['id']=='P05' and c['tag'].startswith('distribute'):score+=0.018
    if profile['id']=='P01' and decision_id%2==0 and c['tag']=='recruit':score+=0.016
    if profile['id']=='P11' and c['tag'] in ('farmers','flood'):score+=0.012
    if profile['id']=='P12' and c['tag']=='recruit':score+=0.006
    return score


def choose(bus,profile,seed,decision_id,competence=None):
    ctx=visible_own_state(bus)
    legal,rejected=legal_choices(bus,ctx,profile,decision_id)
    if not legal:return None,{'legal':0,'rejected':rejected,'regret':0,'reason':'NO_LEGAL_ACTION'}
    sk=competence or profile['competence']
    # Candidate access is fixed by personality+seed, widening as skill increases. No RNG touches the engine.
    considered=legal[:min(sk['candidate_limit'],len(legal))]
    ranked=sorted(((judge(c,ctx,profile,seed,decision_id,sk),c) for c in considered),key=lambda z:(-z[0],z[1]['tag'],str(z[1]['params'])))
    chosen=ranked[0][1]
    qbest=max(quality(c,ctx,profile)['weighted'] for c in legal)
    qpick=quality(chosen,ctx,profile)['weighted']
    return chosen,{'legal':len(legal),'considered':len(considered),'rejected':rejected,
       'regret':round(max(0,qbest-qpick),7),'reason':'SELECTED',
       'first_alternative':ranked[1][1]['tag'] if len(ranked)>1 else None,
       'true_best_visible':round(qbest,7),'chosen_visible':round(qpick,7)}


def run_campaign(profile,seed,months=12,cold_reload=False):
    b=build(seed);log=[];decisions=[];accepted=0;idle=0;probe_rejections=0;probe_accepted=0
    for month in range(1,months+1):
        limit=profile['attention_actions_per_month']
        if profile['id']=='P11':limit=1 if month%3==1 else 0
        limit=min(limit, 2 if profile['id']!='P11' else 1)
        made=0
        for k in range(limit):
            decision_id=month*100+k
            if profile['id']=='P12' and k==0:
                probe=envelope(b,'P12-invalid',month,k,'recruit',{'province_id':'p1','count':-1})
                before=world_hash(b.data)
                try:b.preview(probe);probe_accepted+=1
                except CoreError:probe_rejections+=1
                assert world_hash(b.data)==before
            choice,meta=choose(b,profile,seed,decision_id)
            if choice is None:break
            c=envelope(b,profile['id']+'-run',month,k,choice['kind'],choice['params'],choice['actor'])
            prev=world_hash(b.data)
            quote=b.preview(c)
            assert world_hash(b.data)==prev
            result=b.commit(c,quote['quote_id'])
            accepted+=1;made+=1
            rec={'month':month,'tag':choice['tag'],'kind':choice['kind'],'actor':choice['actor'],
                 'params':choice['params'],'hash_after':world_hash(b.data),'revision':result['world_revision'],**meta}
            decisions.append(rec)
        if not made:idle+=1
        scheduler.run_month(b)
        validate_world(b.data)
        log.append({'month':month,'hash':world_hash(b.data),'revision':b.data['revision']})
        if cold_reload and month in (6,12):
            saved=dump_save(b.data)
            b=Bus(load_save(saved))
            assert dump_save(b.data)==saved
    p=b.data['provinces']['p1']
    return {'profile':profile['id'],'seed':seed,'months':months,'accepted':accepted,
           'idle_months':idle,'food':p['food_units'],'support_ticks':p['support_ticks'],
           'troops':p['troops'],'workers':dict(p['workers']),
           'remaining_bp':sum(v['bp'] for v in b.data['officers'].values() if v['province_id']=='p1'),
           'final_hash':world_hash(b.data),'calendar':log,'decisions':decisions,
           'mean_decision_regret':round(sum(x['regret'] for x in decisions)/max(1,len(decisions)),6),
           'probe_rejected':probe_rejections,'probe_accepted':probe_accepted}


def decision_probes(profiles,seeds=(7,8,41)):
    """Paired same-personality skill-change counterfactual on independent fresh fixtures.
    Uses a visible-state benchmark (non-game strategic model), not claimed win-rate.
    """
    results=[]
    for p in profiles:
        for seed in seeds:
            for variant in ('baseline','food_shortage','support_crisis','labor_scarcity'):
                b=build(seed)
                prov=b.data['provinces']['p1'] # TEST FIXTURE CONSTRUCTION, not player observation
                if variant=='food_shortage':prov['food_units']=30
                if variant=='support_crisis':prov['support_ticks']=250000
                if variant=='labor_scarcity':prov['workers']['unassigned']=15
                validate_world(b.data)
                full=copy.deepcopy(p['competence']);low=copy.deepcopy(full)
                low.update(candidate_limit=3,mechanic_knowledge_pct=35,opportunity_cost_pct=15,
                           downside_stress_pct=10,evaluation_noise_bps=1800,cross_domain_skill_floor=20)
                low['domain_mastery_pct']={x:20 for x in CATEGORIES}
                a,ma=choose(b,p,seed,10,full)
                z,mz=choose(b,p,seed,10,low)
                results.append({'profile':p['id'],'seed':seed,'fixture':variant,
                                'skilled':a['tag'] if a else None,'weak':z['tag'] if z else None,
                                'skilled_regret':ma['regret'],'weak_regret':mz['regret'],
                                'skilled_legal':ma['legal'],'weak_legal':mz['legal'],
                                'chosen_differ':(a or {}).get('tag')!=(z or {}).get('tag')})
    return results


def run_all(config=CONFIG_DEFAULT,out=None):
    profiles,checksum=load_profiles(config)
    rows=[]
    for seed in (7,8,41):
        for p in profiles:
            a=run_campaign(p,seed,12)
            if seed==7:
                b=run_campaign(p,seed,12,cold_reload=True)
                assert a['final_hash']==b['final_hash'] and a['calendar']==b['calendar']
                a['cold_load_match']=True
            else:a['cold_load_match']=None
            rows.append(a)
    probes=decision_probes(profiles)
    skilled=sum(p['skilled_regret'] for p in probes)
    weak=sum(p['weak_regret'] for p in probes)
    distinct={p['id']:set(x['tag'] for r in rows if r['profile']==p['id'] and r['seed']==7 for x in r['decisions']) for p in profiles}
    report={'policy_version':POLICY_VERSION,'config_sha256':checksum,'engine':'Original KOEI Windows game Python core v2 copied from 2026-10-29 QA evidence ZIP',
      'warnings':['Fixture values SYNTHETIC NOT APPROVED','Decision regret synthetic diagnostic, not actual campaign outcome',
                  'Headless own-state visibility proxy: NOT verified against player UI',
                  'BP regenerates monthly per Politics (30%, maximum 200); inherited action costs remain provisional',
                  'Missing modules UNAVAILABLE, especially combat/trade/arms/ships/officer training/advisor',
                  'Do not treat legacy v1 source ZIP as canonical latest implementation'],
      'unavailable':sorted(UNAVAILABLE),'campaigns':rows,'skill_probes':probes,
      'summary':{'campaign_count':len(rows),'replayed':12,'paired_probe_count':len(probes),
        'total_skill_regret':round(skilled,6),'total_weak_regret':round(weak,6),
        'skilled_lower_regret_count':sum(x['skilled_regret']<x['weak_regret'] for x in probes),
        'equal_regret_count':sum(x['skilled_regret']==x['weak_regret'] for x in probes),
        'skilled_higher_regret_count':sum(x['skilled_regret']>x['weak_regret'] for x in probes),
        'changed_choice_count':sum(x['chosen_differ'] for x in probes),
        'action_tags_seed7':{k:sorted(v) for k,v in distinct.items()},
        'profile_first_actions_seed7':{p['id']:[x['tag'] for r in rows if r['profile']==p['id'] and r['seed']==7 for x in r['decisions'][:3]][:3] for p in profiles}}}
    if out:Path(out).write_text(json.dumps(report,indent=2)+'\n')
    return report


if __name__=='__main__':
    out=Path(sys.argv[1]) if len(sys.argv)>1 else Path('out/personas_v11_runtime.json')
    r=run_all(out=out)
    print(json.dumps({'summary':r['summary'],'config_sha256':r['config_sha256'],'output':str(out)},indent=2))
