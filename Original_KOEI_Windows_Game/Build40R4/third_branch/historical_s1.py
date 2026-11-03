"""Original-game Scenario 1: historical NES-derived identities, provinces and family groups.

One-scenario launch. Historical source data is a ROM-derived prior workbook, not a
fresh NES runtime test. Stats Politics/Leadership are reversible original-game
proposals. This module does not import or mutate the NES Classic game engine.
"""
from __future__ import annotations
import json
from pathlib import Path
from og.core.world import new_world, add_province, add_officer
from og.core.world2 import migrate_v1_to_v2
from .graph41 import serialize_neighbors
from . import army_cohorts as armies
from . import scenario1_workforce as civilian

DATA = Path(__file__).with_name('data') / 'historical_s1.json'
S1_LABEL = 'ORIGINAL_GAME_HISTORICAL_S1_PROPOSED'
TRAITS = ('politics','war','leadership','intelligence','charisma')

# Historical genealogical edges are an independently-curated design layer; the
# NES family mask itself says *clan group only*, not parentage. Never treat
# mask=0 as proof two officers are unrelated.
FAMILY_RELATIONS = (
 ('Cao Cao','Cao Ang','father_child'),('Cao Cao','Cao Zhang','father_child'),
 ('Cao Cao','Cao Pi','father_child'),('Cao Cao','Cao Zhi','father_child'),
 ('Cao Pi','Cao Rui','father_child'),
 ('Sun Jian','Sun Ce','father_child'),('Sun Jian','Sun Quan','father_child'),
 ('Sun Jian','Sun Yi','father_child'),('Sun Quan','Sun Deng','father_child'),
 ('Sun Quan','Sun He','father_child'),('Sun Quan','Sun Liang','father_child'),
 ('Liu Bei','Liu Chan','father_child'),('Liu Bei','Liu Feng','adoptive_parent_child'),
 ('Liu Yan (1)','Liu Zhang','father_child'),
 ('Yuan Shao','Yuan Xi','father_child'),('Yuan Shao','Yuan Tan','father_child'),
 ('Yuan Shao','Yuan Shang','father_child'),
 ('Ma Teng','Ma Chao','father_child'),('Ma Teng','Ma Tie','father_child'),
 ('Ma Teng','Ma Xiu','father_child'),
 ('Liu Biao','Liu Qi','father_child'),('Liu Biao','Liu Zong','father_child'),
 ('Meng Huo','Meng You','siblings'),
)

def source():
    d=json.loads(DATA.read_text(encoding='utf-8'))
    if d['scenario']!=1 or len(d['officers'])!=352 or len(d['owners'])!=41:
        raise ValueError('Invalid original scenario data')
    if sorted(o['nes_id'] for o in d['officers'])!=list(range(352)):
        raise ValueError('Missing or duplicate NES officer identity')
    if len(d['rulers'])!=15 or len(d['resources'])!=41:
        raise ValueError('Scenario 1 must contain 15 real rulers and 41 provinces')
    return d

def oid(nes_id):
    return 'nes:'+str(nes_id).zfill(3)

def _relationships(offs):
    names={x['name']:x['nes_id'] for x in offs}
    assert len(names)==352
    out=[]
    for a,b,kind in FAMILY_RELATIONS:
        if a not in names or b not in names:raise ValueError(f'Missing curated name {a}/{b}')
        x,y=names[a],names[b]
        if x==y:raise ValueError('Self relationship')
        out.append({'from_id':oid(x),'to_id':oid(y),'type':kind,
                    'evidence':'CURATED_HISTORICAL_PROPOSAL_NOT_VERIFIED_FROM_ROM_MASK'})
    return out

def _workers(pop):
    """Scenario1 displayed civilian population / 10, evenly over seven jobs."""
    if type(pop) is not int or pop%10:raise ValueError('ROM civilian population must divide by 10')
    return civilian.even_workers(pop//10)

def new_campaign_historical_s1(seed=7,balance_policy=None):
    if balance_policy not in (None, 'OG_1000_SLOW_GROWTH_V1_CANDIDATE','OG_1000_DIRECTOR_V2'):
        raise ValueError('Unsupported balance policy')
    rebalance = balance_policy is not None
    from .integrated import Campaign, _key, validate
    d=source();w=new_world(seed)
    for i,resource in enumerate(d['resources'],start=1):
        add_province(w,'p'+str(i),max(0,min(100,int(resource[3]))))
    active=[o for o in d['officers'] if o['scenario1_status']=='Active at start']
    if len(active)!=107:raise ValueError('Expected 107 active original officers')
    for o in active:
        pid='p'+str(o['scenario1_start_province'])
        add_officer(w,oid(o['nes_id']),pid,tuple(o[k] for k in TRAITS))
    w=migrate_v1_to_v2(w,start_year=189)
    if rebalance:
        w["balance_policy"] = balance_policy
        for province in w["provinces"].values(): province["cultivation"] *= 10
    factions={f'R{r["slot"]}':{'name':r['name'],'trust':60,'daughter':False,'alive':True,
              'nes_ruler_officer_id':r['officer_id']} for r in d['rulers']}
    owners={f'p{i}':(f'R{slot}' if slot is not None else None)
            for i,slot in enumerate(d['owners'],start=1)}
    source_rows=civilian.baseline()
    for i,resource in enumerate(d['resources'],start=1):
        p=w['provinces']['p'+str(i)]
        gold,rice,pop,land,loyalty,flood=resource
        row=source_rows[i-1]
        if row['rom_displayed_civilian_population_hundreds']!=pop:
            raise ValueError('Historical resource population differs from ROM CSV')
        p['workers']=_workers(pop)
        p.update(wealth_milli=340000 if rebalance else 34000,support_ticks=min(5000000,int(loyalty)*50000),tax_rate_percent=30,
                 treasury_gold_cents=int(gold)*100,food_units=int(rice),
                 warehouse_capacity_units=max(1000,int(rice)+500),specialty_units=25,
                 troops=0,flood_control=int(flood)*(10 if rebalance else 1))
    faction_by_officer={};loyalty={}
    for o in active:
        p='p'+str(o['scenario1_start_province']);f=owners[p]
        if f is None:raise ValueError('Officer in vacant province')
        key=oid(o['nes_id'])
        faction_by_officer[key]=f
        loyalty[key]=100 if o['nes_id']==int(f[1:]) else 65
    for r in d['rulers']:
        if oid(r['officer_id']) not in faction_by_officer:raise ValueError('Ruler absent from source active roster')
    # Director mandate: preserve EACH historical general's starting troops,
    # not a synthetic province garrison or first-officer aggregation.
    # This Jan 189 original-ROM save-state reference is VERIFIED STATIC but
    # provisional as an untouched start: province 19 civilian pop differs
    # 9070 in the save vs 9000 in ROM. Preserve ROM civilian population.
    ref_file = DATA.with_name('nes_scenario1_armies_189jan_provisional.json')
    units = json.loads(ref_file.read_text(encoding='utf-8'))
    entries = units['officers']
    if units['scenario'] != 1 or len(entries) != 107 or units['custom_rulers'] != 0:
        raise ValueError('Starting-army provenance/roster mismatch')
    if {oid(o['nes_id']) for o in entries} != set(faction_by_officer):
        raise ValueError('Starting-army roster differs from original scenario')
    initial_armies = {}
    troops_by_province = {p:0 for p in owners}
    for entry in entries:
        commander=oid(entry['nes_id']);pid='p'+str(entry['province'])
        n=entry['troop_groups_100_men']
        if type(n) is not int or not 0<=n<=100:raise ValueError('ROTK2 unit over 100')
        if pid not in owners or w['officers'][commander]['province_id']!=pid:
            raise ValueError('Starting-army location mismatch')
        if n:
            initial_armies[armies.army_id(commander)]={'commander':commander,
                'stations':{pid:n},'engaged':0}
        troops_by_province[pid]+=n
    for pid,n in troops_by_province.items():w['provinces'][pid]['troops']=n
    catalog={oid(o['nes_id']):o for o in d['officers']}
    status={k:o['scenario1_status'] for k,o in catalog.items()}
    from .historical_ai import PROFILES, STYLES
    s={'version':1,'revision':0,'core':w,'owners':owners,'neighbors':serialize_neighbors(),
       'factions':factions,'officer_faction':faction_by_officer,'officer_loyalty':loyalty,
       'market_stocks':{pid:{'food':2000,'specialty':200,'metal':1000,'arms':500,'furs':300} for pid in owners},
       'extras':{pid:{'metal':40,'arms':40,'furs':10,'prisoners':0} for pid in owners},
       'hostility':{_key(a,b):40 for a in factions for b in factions if a!=b},
       'alliances':[],'marriages':[],'promises':[],'diplomacy_last':{},'battle':None,'war_plans':[],
       'governors':{},'month':1,'history':[],'advisor_snapshots':[],'processed':[],'log':[],
       'scenario_id':'NES_DERIVED_189_S1_ONLY','source_label':S1_LABEL,
       'ai_profiles':{f'R{i}':{'id':p['id'],'name':p['name'],
                     'style':STYLES[i],'risk':p['risk'],
                     'priority_weights':p['priority_weights'],
                     'horizon_months':p['horizon_months'],
                     'competence':p['competence']} for i,p in enumerate(PROFILES)},
       'officer_catalog':catalog,'officer_availability':status,
       'family_relations':_relationships(d['officers']),
       'scenario_evidence':d['provenance'],'ruler_slots':{f'R{r["slot"]}':oid(r['officer_id']) for r in d['rulers']}}
    if rebalance: s['balance_policy'] = balance_policy
    if balance_policy=='OG_1000_DIRECTOR_V2':
        s['metal_deposits']={pid:None for pid in owners}
        s['metal_deposit_provenance']='UNRESOLVED_41_PROVINCE_SITE_DATA'
    # Historical-only accounting. Legacy synthetic fixtures/saves retain their
    # original schema. Army group counts are NEVER subtracted from civilians.
    s['workforce_policy']=civilian.POLICY
    s['civilian_workforce_baseline']={f'p{r["province_id"]}':{
        'rom_displayed_population':r['rom_displayed_civilian_population_hundreds'],
        'initial_assignment_units':r['assignment_units']} for r in source_rows}
    s['civilian_workforce_current']={pid:sum(p['workers'][k] for k in civilian.JOBS)
        for pid,p in w['provinces'].items()}
    s['civilian_workforce_ledger']=[]
    s['event_policy']='OG_EVENTS_V1_PROVISIONAL'
    s['event_feed']=[]
    s['event_history']=[]
    # 21-byte original Mesen records +16 Skill, +14 Arms candidate; empty
    # units' 100 raw Arms is a display sentinel, not 100 weapons created.
    s['unit_training']={oid(e['nes_id']):int(e['training_raw']) for e in entries}
    s['unit_armed_men']={oid(e['nes_id']):(int(e['arms_raw']) if e['troop_groups_100_men'] else 0) for e in entries}
    s['armies']=initial_armies
    armies.validate_armies(s)
    s['starting_army_evidence']={'type':'STATIC_EARLY_UNMODIFIED_189_01_NOT_PROVEN_UNTOUCHED',
        'officer_count':107,'initial_troop_groups':sum(troops_by_province.values()),
        'save_ram_sha256':units['both_saveRAM_sha256'],
        'province19_pop_discrepancy':'saved 9070 vs original ROM 9000; ROM population retained'}
    validate(s)
    return Campaign(s)

def list_recruitable(s,province):
    """Hidden or arrived officers in target province; no hidden officer commands until recruited."""
    if s.get('source_label')!=S1_LABEL:return []
    return sorted((key for key,o in s['officer_catalog'].items()
                   if s['officer_availability'][key] in ('Hidden/searchable at start','Arrived/searchable')
                   and (o['scenario1_start_province'] if s['officer_availability'][key]=='Hidden/searchable at start'
                        else o['scenario1_arrival_province'])==int(province[1:])),key=lambda k:int(k[4:]))

def recruit_officer(s,actor,pid,f,params):
    from .integrated import reject
    from .approved_distribution import HANDLERS
    target=params.get('officer_id')
    if target not in list_recruitable(s,pid):reject('NOT_RECRUITABLE')
    officer=s['core']['officers'][actor]
    if officer['bp']<8:reject('NO_BP')
    data=s['officer_catalog'][target]
    officer['bp']-=8
    # Deterministic successful visit is currently a design-rule candidate;
    # do not represent as historical probability or a battle-tested AI action.
    add_officer(s['core'],target,pid,tuple(data[k] for k in TRAITS))
    s['officer_faction'][target]=f
    ruler=s['ruler_slots'][f]
    group=data['rom_family_mask_candidate']
    family_bonus=10 if group and group==s['officer_catalog'][ruler]['rom_family_mask_candidate'] else 0
    s['officer_loyalty'][target]=min(100,60+family_bonus)
    s['officer_availability'][target]='Serving'
    s.setdefault('unit_training',{})[target]=10  # PROVISIONAL for newly recruited officer
    s.setdefault('unit_armed_men',{})[target]=0
    return {'officer_id':target,'name':data['name'],'province':pid,'ruler':f,
            'loyalty':s['officer_loyalty'][target],'family_group_bonus':family_bonus}

def process_arrivals(s):
    if s.get('source_label')!=S1_LABEL:return []
    from og.core.world2 import calendar_of
    year,_=calendar_of(s['core'],s['month'])
    arrived=[]
    for key,d in sorted(s['officer_catalog'].items()):
        if s['officer_availability'][key]=='Arrives later' and d['scenario1_arrival_year']<=year:
            s['officer_availability'][key]='Arrived/searchable'
            arrived.append(key)
    return arrived


def officer_family(s,officer_id):
    """Return independent NES clan-membership candidate and curated relationship edges."""
    if officer_id not in s.get('officer_catalog',{}):raise KeyError(officer_id)
    o=s['officer_catalog'][officer_id];group=o['rom_family_mask_candidate']
    members=sorted(({'id':key,'name':record['name']} for key,record in s['officer_catalog'].items()
                    if group and record['rom_family_mask_candidate']==group),key=lambda r:r['id'])
    related=[e for e in s['family_relations'] if officer_id in (e['from_id'],e['to_id'])]
    return {'officer_id':officer_id,'name':o['name'],'rom_family_mask_candidate':group,
            'shared_group':members,'curated_relations':related,
            'caveat':'Group mask is not a full genealogy; curated relationships are historical design candidates.'}
