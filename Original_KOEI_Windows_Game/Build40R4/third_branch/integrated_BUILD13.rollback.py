"""Third branch original KOEI-style game: integrated, deterministic TEST PROTOTYPE.
Own engine adapter over original-game economic command Bus; diplomacy follows the
CAO-DIPLO-0003 design *architecture*, not NES code or recovered battle algorithms.
All new costs, probabilities and scenarios are SYNTHETIC-NOT-APPROVED.
"""
from __future__ import annotations
import copy, hashlib, json, os
from pathlib import Path
from og.core.world import new_world, add_province, add_officer, validate_world, canonical_bytes, world_hash
from og.core.world2 import migrate_v1_to_v2, calendar_of
from og.core.bus import Bus
from og.core import scheduler
from og.core.rng import randrange
from og.core.fixed import CoreError
from .approved_distribution import HANDLERS as APPROVED_DISTRIBUTION
from . import army_cohorts as ac
from . import officer_relations as relations
from . import rotk2_battle_core as combat
from . import officer_battle_adapter as officercombat

DOMESTIC = set(('allocate_workers','distribute_food','distribute_goods','recruit'))
DIPLO = ('Ally','Joint','Marry','Give','Cancel','Threat')
PRICE_CENTS = {'food':25,'specialty':80,'metal':180,'arms':240,'furs':120}
# All new numbers are reversible synthetic balance values, not historical KOEI constants.
BP_COST = {'market':4,'train':5,'train_troops':5,'equip_arms':4,'attack':12,'Ally':8,'Joint':9,'Marry':10,
           'Give':5,'Cancel':3,'Threat':14,'battle_move':0,'battle_attack':0,'battle_end_turn':0,'battle_auto':0, 'plan_attack':2,'transport':4,'appoint_governor':6,
           'assign_troops':0,'disband_troops':0,'relocate_army':0,'recruit_officer':8,**relations.BP}
NEIGHBORS = {'p1':['p2','p3'],'p2':['p1','p4'],'p3':['p1','p4','p5'],
             'p4':['p2','p3','p6'],'p5':['p3','p6','p7'],'p6':['p4','p5','p7'],'p7':['p5','p6']}
DEFAULT_OWNERS = {'p1':'A','p2':'A','p3':'A','p4':'B','p5':'B','p6':'C','p7':'C'}
COMMANDS = DOMESTIC | set(DIPLO) | {'market','train','train_troops','equip_arms','attack','battle_move','battle_attack','battle_end_turn','battle_auto','plan_attack','transport','appoint_governor','assign_troops','disband_troops','relocate_army','recruit_officer'} | relations.PERSONNEL_COMMANDS


def reject(code, message=''):
    raise CoreError(code,message or code)


def integer(x,lo=None,hi=None):
    if type(x) is not int or (lo is not None and x<lo) or (hi is not None and x>hi):
        reject('BAD_PARAMS','integer out of range')
    return x


def _pair(a,b):return '|'.join(sorted((a,b)))


def _key(a,b):return a+'>'+b


def hash_state(state):return hashlib.sha256(canonical_bytes(state)).hexdigest()


def new_campaign(seed=7):
    integer(seed,0,2**64-1)
    w=new_world(seed)
    for i in range(1,8):add_province(w,f'p{i}',36+i*3)
    officers={'A1':('p1',(80,60,65,75,85)), 'A2':('p2',(55,77,68,45,56)),
              'A3':('p3',(60,52,58,70,50)), 'B1':('p4',(63,70,74,62,58)),
              'B2':('p5',(56,68,55,60,46)), 'C1':('p6',(66,52,55,66,70)),
              'C2':('p7',(48,61,58,45,65))}
    for oid,(pid,stats) in officers.items():add_officer(w,oid,pid,stats)
    w=migrate_v1_to_v2(w)
    for i in range(1,8):
        p=w['provinces'][f'p{i}']
        p['workers']={'farmers':150,'town':65,'artisans':30,'metalworkers':18,
                      'resources':22,'unassigned':100,'land':4,'flood':3}
        p.update(wealth_milli=34000,support_ticks=2300000,tax_rate_percent=30,
                 treasury_gold_cents=16000,food_units=450,warehouse_capacity_units=1000,
                 specialty_units=25,troops=240 if i!=4 else 300,flood_control=20)
    validate_world(w)
    c={'version':1,'revision':0,'core':w,'owners':copy.deepcopy(DEFAULT_OWNERS),
       'neighbors':copy.deepcopy(NEIGHBORS),'factions':{'A':{'name':'Han','trust':65,'daughter':True,'alive':True},
        'B':{'name':'Wei','trust':55,'daughter':True,'alive':True},'C':{'name':'Shu','trust':60,'daughter':True,'alive':True}},
       'officer_faction':{oid:oid[0] for oid in officers},
       'officer_loyalty':{oid:65 for oid in officers},
       'market_stocks':{f'p{i}':{'food':2000,'specialty':200,'metal':1000,'arms':500,'furs':300} for i in range(1,8)},
       'extras':{f'p{i}':{'metal':40,'arms':40,'furs':10,'prisoners':0} for i in range(1,8)},
       'hostility':{_key(a,b):40 for a in 'ABC' for b in 'ABC' if a!=b},
       'alliances':[],'marriages':[],'promises':[], 'diplomacy_last':{},
       'battle':None,'war_plans':[],'governors':{},'month':1,'history':[],'advisor_snapshots':[],'processed':[],'log':[],
       'source_label':'ORIGINAL_GAME_SYNTHETIC_INTEGRATED_TEST'}
    ac.bootstrap(c)
    validate(c)
    return Campaign(c)



def new_campaign_41(seed=7):
    """Runnable, historically unapproved THREE-FACTION test scenario with 41 real provinces.

    Uses same command ledger and economic policy as the existing integrated game.
    Unlike the old seven-province fixture, all 41 province records are live:
    officers, owners, economy, adjacency, monthly processing and save/load.
    Faction placements and balances remain PROVISIONAL, NOT an approved scenario.
    """
    from .graph41 import serialize_neighbors
    integer(seed,0,2**64-1)
    neighbors=serialize_neighbors()
    w=new_world(seed)
    for i in range(1,42):add_province(w,f'p{i}',36+i%40)
    def faction(i):
        return 'A' if i <= 14 else ('B' if i <= 27 else 'C')
    for i in range(1,42):
        stats=(50+(i%11),55+(i%9),55+(i%13),50+(i%15),52+(i%12))
        add_officer(w,f'O{i:02d}',f'p{i}',stats)
    w=migrate_v1_to_v2(w)
    for i in range(1,42):
        p=w['provinces'][f'p{i}']
        p['workers']={'farmers':150,'town':65,'artisans':30,'metalworkers':18,
                      'resources':22,'unassigned':100,'land':4,'flood':3}
        p.update(wealth_milli=34000,support_ticks=2300000,tax_rate_percent=30,
                 treasury_gold_cents=16000,food_units=450,warehouse_capacity_units=1000,
                 specialty_units=25,troops=240,flood_control=20)
    owners={f'p{i}':faction(i) for i in range(1,42)}
    c={'version':1,'revision':0,'core':w,'owners':owners,
       'neighbors':neighbors,
       'factions':{'A':{'name':'Han','trust':65,'daughter':True,'alive':True},
                   'B':{'name':'Wei','trust':55,'daughter':True,'alive':True},
                   'C':{'name':'Shu','trust':60,'daughter':True,'alive':True}},
       'officer_faction':{f'O{i:02d}':faction(i) for i in range(1,42)},
       'officer_loyalty':{f'O{i:02d}':65 for i in range(1,42)},
       'market_stocks':{f'p{i}':{'food':2000,'specialty':200,'metal':1000,
                               'arms':500,'furs':300} for i in range(1,42)},
       'extras':{f'p{i}':{'metal':40,'arms':40,'furs':10,'prisoners':0}
                 for i in range(1,42)},
       'hostility':{_key(a,b):40 for a in 'ABC' for b in 'ABC' if a!=b},
       'alliances':[],'marriages':[],'promises':[], 'diplomacy_last':{},
       'battle':None,'war_plans':[],'governors':{},'month':1,'history':[],
       'advisor_snapshots':[],'processed':[],'log':[],
       'scenario_id':'prototype_41_province_3_factions_NOT_FINAL',
       'source_label':'ORIGINAL_GAME_SYNTHETIC_INTEGRATED_TEST'}
    ac.bootstrap(c)
    validate(c)
    return Campaign(c)

def validate(s):
    if not isinstance(s,dict) or s.get('version')!=1 or s.get('source_label') not in ('ORIGINAL_GAME_SYNTHETIC_INTEGRATED_TEST','ORIGINAL_GAME_HISTORICAL_S1_PROPOSED'):reject('BAD_SHAPE')
    validate_world(s['core'])
    if 'human_faction' in s and s['human_faction'] not in s['factions']:reject('BAD_HUMAN_FACTION')
    if s.get('source_label')=='ORIGINAL_GAME_HISTORICAL_S1_PROPOSED':
        catalog=s.get('officer_catalog',{})
        statuses=s.get('officer_availability',{})
        if len(catalog)!=352 or set(catalog)!=set(statuses):reject('BAD_OFFICER_CATALOG')
        if any(not isinstance(v,dict) or 'rom_family_mask_candidate' not in v for v in catalog.values()):reject('BAD_FAMILY_REGISTRY')
        if len(s.get('ruler_slots',{}))!=15:reject('BAD_RULER_SLOTS')
        for e in s.get('family_relations',[]):
            if e['from_id'] not in catalog or e['to_id'] not in catalog:reject('BAD_FAMILY_LINK')
    if s.get('balance_policy')=='OG_1000_DIRECTOR_V2':
        dep=s.get('metal_deposits')
        if not isinstance(dep,dict) or set(dep)!=set(s['owners']):reject('BAD_METAL_GEOGRAPHY')
        if any(v is not None and type(v) is not bool for v in dep.values()):reject('BAD_METAL_GEOGRAPHY')
        if s.get('metal_deposit_provenance') != 'UNRESOLVED_41_PROVINCE_SITE_DATA':reject('MISSING_METAL_SOURCE')
    if s.get('source_label')=='ORIGINAL_GAME_HISTORICAL_S1_PROPOSED':
        training=s.get('unit_training');armed=s.get('unit_armed_men')
        if not isinstance(training,dict) or not isinstance(armed,dict):reject('MISSING_UNIT_SKILL_ARMS')
        if set(training)!=set(s['core']['officers']) or set(armed)!=set(training):reject('BAD_UNIT_REGISTRY')
        for oid in training:
            integer(training[oid],0,100)
            integer(armed[oid],0)
            # Empty units may have ROM field +14 sentinel=100 but must not
            # create actual weapons; reference raw remains in source data.
            stationed=s['armies'].get(ac.army_id(oid),{'stations':{},'engaged':0})
            soldier_capacity=(sum(stationed['stations'].values())+stationed['engaged'])*100
            if armed[oid]>soldier_capacity:reject('ARMED_SOLDIERS_EXCEED_TROOPS')
    if s.get('workforce_policy'):
        from .scenario1_workforce import validate as validate_workforce
        validate_workforce(s)
    ac.validate_armies(s)
    relations.validate(s)
    if s['month']!=scheduler.cursor(s['core'])[0]:reject('CALENDAR_MISMATCH')
    if s['revision']!=len(s['log']) or len(s['processed'])!=len(s['log']):reject('JOURNAL_MISMATCH')
    if len(set(s['processed']))!=len(s['processed']):reject('DUPLICATE_ID')
    if set(s['owners'])!=set(s['core']['provinces']) or set(s['extras'])!=set(s['owners']):reject('PROVINCE_MISMATCH')
    for pid,owner in s['owners'].items():
        if owner is not None and (owner not in s['factions'] or not s['factions'][owner]['alive']):reject('BAD_OWNER')
        for val in s['extras'][pid].values():integer(val,0)
        for other in s['neighbors'][pid]:
            if pid not in s['neighbors'].get(other,[]):reject('ASYMMETRIC_GRAPH')
    for o,f in s['officer_faction'].items():
        if o not in s['core']['officers'] or f not in s['factions']:reject('BAD_OFFICER')
        if s['owners'][s['core']['officers'][o]['province_id']]!=f:reject('OFFICER_OWNER_MISMATCH',o)
        integer(s['officer_loyalty'][o],0,100)
    for k,v in s['hostility'].items():integer(v,0,100)
    if set(s['market_stocks'])!=set(s['owners']):reject('MARKET_SHAPE')
    for stock in s['market_stocks'].values():
        if set(stock)!=set(PRICE_CENTS):reject('MARKET_SHAPE')
        for n in stock.values():integer(n,0)
    for province,entry in s['governors'].items():
        if (province not in s['owners'] or not isinstance(entry,dict)
                or entry.get('officer') not in s['core']['officers']):reject('BAD_GOVERNOR')
        if entry.get('focus') not in ('farms','industry','defense','personnel'):
            reject('BAD_GOVERNOR_FOCUS')
        if s['core']['officers'][entry['officer']]['province_id']!=province:reject('BAD_GOVERNOR_POSTING')
        if s['officer_faction'][entry['officer']]!=s['owners'][province]:reject('BAD_GOVERNOR_LOYALTY')
    for pair in s['alliances']:
        a,b=pair.split('|');
        if a==b or not s['factions'][a]['alive'] or not s['factions'][b]['alive']:reject('BAD_ALLIANCE')
    for promise in s['promises']:
        if _pair(promise['actor'],promise['ally']) not in s['alliances']:reject('ORPHAN_PROMISE')
        if s['owners'][promise['target']] in (promise['actor'],promise['ally']):reject('FRIENDLY_PROMISE')
        if promise['expires']<s['month']:reject('EXPIRED_PROMISE')
    if s['battle']:
        b=s['battle']
        if b['src'] not in s['owners'] or b['dst'] not in s['owners']:reject('BAD_BATTLE')
        for u in ('a','d'):
            if b[u]['troops']<0 or b[u]['morale']<0:reject('BAD_BATTLE')
    return True


class Campaign:
    def __init__(self, state):
        validate(state)
        self.state=copy.deepcopy(state)
    def clone(self):return Campaign(self.state)
    @property
    def fingerprint(self):return hash_state(self.state)
    def _quote(self,cmd):return hashlib.sha256(canonical_bytes({'revision':self.state['revision'],'cmd':cmd})).hexdigest()
    def _envelope(self,kind,actor,params,request_id):
        if type(kind)!=str or kind not in COMMANDS:reject('UNKNOWN_KIND',str(kind))
        if actor not in self.state['core']['officers']:reject('UNKNOWN_ACTOR')
        if type(request_id)!=str or not request_id:reject('BAD_ID')
        if request_id in self.state['processed']:reject('DUPLICATE_ID')
        if type(params)!=dict:reject('BAD_PARAMS')
        return {'kind':kind,'actor':actor,'params':params,'id':request_id}
    def preview(self,kind,actor,params,request_id):
        cmd=self._envelope(kind,actor,params,request_id)
        staged=copy.deepcopy(self.state)
        result=self._step(staged,cmd)
        if staged.get('workforce_policy'):
            from .scenario1_workforce import sync
            sync(staged,'preview:'+kind)
        validate(staged)
        return {'quote_id':self._quote(cmd),'result':result,'revision':self.state['revision']}
    def execute(self,kind,actor,params,request_id,quote_id=None,expected_revision=None):
        cmd=self._envelope(kind,actor,params,request_id)
        if expected_revision is not None and expected_revision!=self.state['revision']:reject('STALE_REVISION')
        if quote_id is not None and quote_id!=self._quote(cmd):reject('STALE_QUOTE')
        staged=copy.deepcopy(self.state)
        result=self._step(staged,cmd)
        staged['revision']+=1
        staged['processed'].append(request_id)
        staged['log'].append({'id':request_id,'kind':kind,'actor':actor,'month':staged['month'],'result':result})
        if staged.get('event_policy')=='OG_EVENTS_V1_PROVISIONAL':
            from .game_events import process_command
            pid=staged['core']['officers'][actor]['province_id']
            process_command(staged,cmd,result,pid)
        if staged.get('workforce_policy'):
            from .scenario1_workforce import sync
            sync(staged,'command:'+kind)
        validate(staged)
        self.state=staged
        return result
    def do(self,kind,actor,params,request_id):
        q=self.preview(kind,actor,params,request_id)
        return self.execute(kind,actor,params,request_id,q['quote_id'],q['revision'])
    def _actor(self,s,actor,require_owner=True):
        o=s['core']['officers'][actor];pid=o['province_id'];f=s['officer_faction'][actor]
        if require_owner and s['owners'][pid]!=f:reject('WRONG_OWNER')
        return o,pid,f
    def _bp(self,o,kind):
        cost=BP_COST[kind]
        if o['bp']<cost:reject('NO_BP',kind)
        o['bp']-=cost
    def _step(self,s,c):
        kind,actor,p=c['kind'],c['actor'],c['params']
        o,pid,f=self._actor(s,actor)
        if kind in relations.PERSONNEL_COMMANDS:
            return relations.perform(s,actor,kind,p)
        if kind=='recruit_officer':
            from .historical_s1 import recruit_officer
            if s.get('source_label')!='ORIGINAL_GAME_HISTORICAL_S1_PROPOSED':reject('BAD_SCENARIO')
            return recruit_officer(s,actor,pid,f,p)
        if kind=='assign_troops':
            target=p.get('target_officer');n=integer(p.get('count'),1)
            if target not in s['core']['officers']:reject('UNKNOWN_COMMANDER')
            ac.transfer(s,actor,target,pid,n)
            return {'assigned':n,'source':actor,'destination':target,'province':pid,'troops_after':s['core']['provinces'][pid]['troops']}
        if kind=='disband_troops':
            n=integer(p.get('count'),1)
            ac.take(s,actor,pid,n)
            if s.get('workforce_policy'):
                from .recruitment_labor import restore
                restore(s['core']['provinces'][pid]['workers'],n)
            else:
                s['core']['provinces'][pid]['workers']['unassigned']+=n
            ac.refresh(s)
            return {'disbanded':n,'population_returned':n,'commander':actor,'province':pid}
        if kind=='relocate_army':
            dst=p.get('target')
            if dst not in s['owners'] or dst not in s['neighbors'][pid]:reject('NOT_ADJACENT')
            owner=s['owners'][dst]
            if owner not in (f,None):reject('NOT_FRIENDLY_OR_VACANT')
            a=s['armies'].get(ac.army_id(actor))
            if not a or set(a['stations'])!={pid} or a['engaged']:reject('ARMY_NOT_ALL_AT_HOME')
            if s['governors'].get(pid,{}).get('officer')==actor:reject('GOVERNOR_CANNOT_MOVE')
            n=a['stations'][pid]
            if not n:reject('NO_ARMY_TROOPS')
            rations=0
            # Neutral acquisition is the consequence of REAL Move Army, not a
            # separate Occupy command. Source garrison remains defensible.
            if owner is None:
                neighbors=s['neighbors'][pid]
                hostiles=[s['core']['provinces'][q]['troops'] for q in neighbors
                          if s['owners'][q] not in (None,f)]
                reserve=max(15, (max(hostiles)*4+4)//5 + 5*len(hostiles)) if hostiles else max(15,(n+3)//4)
                if s['core']['provinces'][pid]['troops']-n < reserve:
                    reject('SOURCE_GARRISON_UNSAFE')
                rations=max(1,(n+59)//60)
                if s['core']['provinces'][pid]['food_units']<rations:reject('NO_FOOD')
                # Same action cost as the prior acquisition transaction,
                # charged once from the moving general before any mutation.
                if o['bp']<8:reject('NO_BP')
                o['bp']-=8
                s['core']['provinces'][pid]['food_units']-=rations
            del a['stations'][pid]
            a['stations'][dst]=n
            o['province_id']=dst
            if owner is None:s['owners'][dst]=f
            relations.drop_binding(s,actor) if s.get('appointed_advisors',{}).get(pid)==actor else None
            ac.refresh(s)
            return {'command':'relocate_army','army':ac.army_id(actor),'troops':n,
                    'from':pid,'to':dst,'took_vacant':owner is None,'rations':rations}
        if kind in DOMESTIC:
            if 'province_id' in p and s['owners'].get(p['province_id'])!=f:reject('WRONG_OWNER')
            if kind=='allocate_workers' and s.get('workforce_policy') and (
                    p.get('source')=='unassigned' or p.get('target')=='unassigned'):
                reject('RETIRED_WORKER_CATEGORY')
            handler_map=APPROVED_DISTRIBUTION
            if kind=='recruit' and s.get('workforce_policy'):
                from .recruitment_labor import recruit_from_jobs
                # Ensure commander capacity BEFORE changing the core labor ledger.
                ac.check_capacity(s,actor,p['count'] if type(p.get('count')) is int else 0)
                handler_map={**APPROVED_DISTRIBUTION,'recruit':recruit_from_jobs}
            b=Bus(s['core'],handlers=handler_map)
            inner={'request_id':'integrated:'+c['id'],'actor_officer_id':actor,
                   'expected_world_revision':b.data['revision'],'kind':kind,'params':p}
            result=b.commit(inner)['result'];s['core']=b.data
            if kind=='recruit':
                n=p['count']
                # Core population recruitment and army posting must happen in
                # one clone, never create an independent reserve.
                ac.add(s,actor,pid,n)
                ac.refresh(s)
                result['assigned_army']=ac.army_id(actor)
            return result
        if kind in DIPLO:return self._diplomacy(s,o,pid,f,kind,p)
        if kind=='market':return self._market(s,o,pid,p)
        if kind=='transport':return self._transport(s,o,pid,f,p)
        if kind=='appoint_governor':
            # Only the ruler may appoint, including a governor in another
            # friendly province. Actor's local province is NOT the target.
            if actor!=relations.ruler(s,f):reject('REQUIRES_RULER')
            name=p.get('officer');focus=p.get('focus')
            if name not in s['core']['officers'] or s['officer_faction'].get(name)!=f:
                reject('BAD_GOVERNOR')
            target_pid=s['core']['officers'][name]['province_id']
            if s['owners'].get(target_pid)!=f:reject('BAD_GOVERNOR')
            if 'province_id' in p and p['province_id']!=target_pid:reject('BAD_GOVERNOR')
            if focus not in ('farms','industry','defense','personnel'):reject('BAD_FOCUS')
            self._bp(o,kind)
            s['governors'][target_pid]={'officer':name,'focus':focus}
            return {'governor':name,'province':target_pid,'focus':focus}
        if kind=='train_troops':
            # OBSERVED NES/Mesen original: all 21-byte local officer +18 skill
            # fields rise by group sum floor(2*WAR/13), capped at 100.
            # Does not change officer WAR, general trait XP or unit equipment.
            if s.get('source_label') != 'ORIGINAL_GAME_HISTORICAL_S1_PROPOSED':
                reject('NEEDS_HISTORICAL_UNIT_SCHEMA')
            trainers=p.get('trainers',[actor])
            if (not isinstance(trainers,list) or not 1<=len(trainers)<=5
                    or len(set(trainers))!=len(trainers) or actor not in trainers):
                reject('BAD_TRAINERS')
            for trainer in trainers:
                if trainer not in s['core']['officers'] or s['officer_faction'].get(trainer)!=f or s['core']['officers'][trainer]['province_id']!=pid:
                    reject('BAD_TRAINER')
                if s['core']['officers'][trainer]['bp']<BP_COST['train_troops']:
                    reject('NO_BP')
            gain=sum(2*s['core']['officers'][trainer]['war']//13 for trainer in trainers)
            affected=[]
            for oid,off in sorted(s['core']['officers'].items()):
                if off['province_id'] != pid or s['officer_faction'].get(oid)!=f:
                    continue
                skill=s['unit_training'].get(oid)
                if skill is None:reject('MISSING_TRAINING')
                before=skill
                s['unit_training'][oid]=min(100,skill+gain)
                affected.append({'general':oid,'before':before,'after':s['unit_training'][oid]})
            if not affected:reject('NO_GENERALS')
            for trainer in trainers:
                s['core']['officers'][trainer]['bp']-=BP_COST['train_troops']
            return {'province':pid,'trainers':trainers,'skill_gain_nominal':gain,
                    'units':affected,'mechanic':'NES_MESEN_OBSERVED_2WAR_DIV13'}
        if kind=='equip_arms':
            # This explicit assignment is an ENHANCED commodity bridge, NOT a
            # claim that original NES Buy Weapons UI uses these source units.
            if s.get('source_label') != 'ORIGINAL_GAME_HISTORICAL_S1_PROPOSED':
                reject('NEEDS_HISTORICAL_UNIT_SCHEMA')
            target=p.get('general',actor)
            if target not in s['core']['officers'] or s['core']['officers'][target]['province_id']!=pid or s['officer_faction'].get(target)!=f:
                reject('BAD_COMMANDER')
            batches=integer(p.get('units'),1,100)
            soldiers=sum(s['armies'].get(ac.army_id(target),{}).get('stations',{}).values())*100
            existing=s['unit_armed_men'].get(target,0)
            if not soldiers:reject('NO_SOLDIERS')
            if existing+100*batches>soldiers:reject('ARMS_OVER_CAPACITY')
            if s['extras'][pid]['arms']<batches:reject('NO_STOCK')
            self._bp(o,'equip_arms')
            s['extras'][pid]['arms']-=batches
            s['unit_armed_men'][target]=existing+100*batches
            return {'general':target,'weapons_equipped':batches*100,'arms_pct':100*s['unit_armed_men'][target]//soldiers,'stock_after':s['extras'][pid]['arms'],'mechanic':'ENHANCED_STOCK_TO_NES_UNIT_BRIDGE_PROVISIONAL'}
        if kind=='train':
            effort=integer(p.get('effort',1),1,10);trait=p.get('trait','war')
            if trait not in ('politics','war','leadership','intelligence','charisma'):reject('BAD_TRAIT')
            if o[trait]>=100:reject('AT_CAP')
            if o['bp']<BP_COST[kind]*effort:reject('NO_BP')
            o['bp']-=BP_COST[kind]*effort
            o[trait]=min(100,o[trait]+effort);o['xp']=min(1000000,o['xp']+effort*10)
            return {'trait':trait,'new_stat':o[trait],'xp':o['xp'],'bp':o['bp']}
        if kind=='plan_attack':
            target=p.get('target')
            if target not in s['neighbors'][pid] or s['owners'][target] in (f,None):reject('INVALID_WAR_OBJECTIVE')
            self._bp(o,'plan_attack')
            s['war_plans']=[z for z in s['war_plans'] if z['actor']!=f]
            s['war_plans'].append({'actor':f,'src':pid,'target':target,'expires':s['month']+1})
            return {'scheduled':target,'src':pid}
        if kind=='attack':return self._attack(s,o,pid,f,p)
        if kind in ('battle_move','battle_attack','battle_end_turn','battle_auto'):
            return self._battle(s,actor,kind,p)
        reject('UNKNOWN_KIND')
    def _transport(self,s,o,pid,f,p):
        target=p.get('target');good=p.get('commodity');n=integer(p.get('units'),1,1000000)
        if target not in s['neighbors'][pid] or s['owners'][target]!=f:reject('NOT_FRIENDLY_ADJACENT')
        allowed={'gold':'treasury_gold_cents','food':'food_units','specialty':'specialty_units','troops':'troops'}
        extra=('metal','arms','furs')
        if good in allowed:
            src=s['core']['provinces'][pid];dst=s['core']['provinces'][target];key=allowed[good]
        elif good in extra:
            src=s['extras'][pid];dst=s['extras'][target];key=good
        else:reject('BAD_COMMODITY')
        if src[key]<n:reject('NO_STOCK')
        if key=='food_units' and dst[key]+n>dst['warehouse_capacity_units']:reject('WAREHOUSE_FULL')
        if good=='troops':
            recipients=sorted(oid for oid,off in s['core']['officers'].items()
                              if off['province_id']==target and s['officer_faction'][oid]==f)
            if not recipients:reject('ARMY_REQUIRES_COMMANDER')
            if ac.strength(s,o and next(oid for oid,v in s['core']['officers'].items() if v is o),pid)<n:reject('NO_ARMY_TROOPS')
        lost=0
        if s.get('event_policy')=='OG_EVENTS_V1_PROVISIONAL' and good in ('gold','food'):
            from .game_events import roll
            hostile=any(s['owners'].get(q) not in (f,None) for q in s['neighbors'][pid]+s['neighbors'][target])
            if hostile and roll(s['core']['rng_state'],s['month'],pid,target,good,n,'courier')%100 < 8:
                lost=max(1,n//5)
        self._bp(o,'transport');src[key]-=n;dst[key]+=n-lost
        if good=='troops':
            commander=next(oid for oid,v in s['core']['officers'].items() if v is o)
            ac.take(s,commander,pid,n);ac.add(s,recipients[0],target,n);ac.refresh(s)
        return {'source':pid,'target':target,'commodity':good,'units':n,
                'intercepted':bool(lost),'lost_units':lost,'delivered_units':n-lost,
                'source_remaining':src[key]}
    def _market(self,s,o,pid,p):
        commodity=p.get('commodity');side=p.get('side');n=integer(p.get('units'),1,5000)
        if commodity not in PRICE_CENTS or side not in ('buy','sell'):reject('BAD_PARAMS')
        q=s['core']['provinces'][pid];x=s['extras'][pid]
        place={'food':'food_units','specialty':'specialty_units'}.get(commodity)
        store=q if place else x;item=place or commodity
        season=(s['month']-1)%12//3
        multiplier=(100,110,95,120)[season] # SYNTHETIC seasonal market changes
        mstock=s['market_stocks'][pid][commodity]
        target_stock={'food':2000,'specialty':200,'metal':1000,'arms':500,'furs':300}[commodity]
        scarcity=max(75,min(130,100+(target_stock-mstock)//max(1,target_stock//20)))
        base=max(1,PRICE_CENTS[commodity]*multiplier*scarcity//10000)
        # 15% buy/sell spread prevents unlimited instant circular arbitrage.
        # Incremental volume tier and CHA adjustments, max benefit 10%.
        benefit=min(10,1+n//500 + o['charisma']//25)
        unit=max(1,base*115*(100-benefit)//10000) if side=='buy' else max(1,base*85*(100+benefit)//10000)
        total=unit*n
        if side=='buy':
            if mstock<n and not s.get('balance_policy')=='OG_1000_DIRECTOR_V2':reject('NO_MARKET_STOCK')
            if q['treasury_gold_cents']<total:reject('NO_GOLD')
            if item=='food_units' and q[item]+n>q['warehouse_capacity_units']:reject('WAREHOUSE_FULL')
            self._bp(o,'market');q['treasury_gold_cents']-=total;store[item]+=n
            s['market_stocks'][pid][commodity]=max(0,mstock-n)  # price-demand indicator, not finite inventory
        else:
            if store[item]<n:reject('NO_STOCK')
            self._bp(o,'market');store[item]-=n;q['treasury_gold_cents']+=total
            s['market_stocks'][pid][commodity]+=n
        return {'commodity':commodity,'side':side,'units':n,'unit_price_cents':unit,
                'total_cents':total,'concession_pct':benefit,'gold_cents':q['treasury_gold_cents'],
                'scarcity_pct':scarcity,'market_stock_after':s['market_stocks'][pid][commodity]}
    def _diplomacy(self,s,o,pid,f,kind,p):
        target=p.get('target')
        if target not in s['factions'] or target==f or not s['factions'][target]['alive']:reject('BAD_TARGET')
        # envoy must govern from an owned province and use actual 200-point officer BP.
        alliance=_pair(f,target);ally=alliance in s['alliances']
        last=s['diplomacy_last'].get(f+'>'+target+':'+kind,0)
        cooldown={'Ally':3,'Joint':1,'Marry':3,'Give':1,'Cancel':0,'Threat':6}[kind]
        if last and s['month']-last<cooldown:reject('COOLDOWN')
        if kind=='Ally' and ally:reject('ALREADY_ALLIED')
        if kind=='Cancel' and not ally:reject('NOT_ALLIED')
        if kind=='Threat' and ally:reject('ALLIED_TARGET')
        if kind=='Marry' and (not s['factions'][f]['daughter'] or alliance in s['marriages']):reject('NO_DAUGHTER')
        gold=0
        if kind=='Give':
            gold=integer(p.get('gold_cents'),1)
            if s['core']['provinces'][pid]['treasury_gold_cents']<gold:reject('NO_GOLD')
        objective=None
        if kind=='Joint':
            if not ally:reject('NOT_ALLIED')
            objective=p.get('objective')
            if objective not in s['owners'] or s['owners'][objective] in (f,target,None):reject('BAD_OBJECTIVE')
            if not any(objective in s['neighbors'][own] for own,v in s['owners'].items() if v in (f,target)):
                reject('UNREACHABLE_OBJECTIVE')
            integer(p.get('promised_troops'),1,10000)
            if not any(z['actor']==f and z['target']==objective and z['expires']>=s['month'] for z in s['war_plans']):reject('NO_SCHEDULED_ATTACK')
        self._bp(o,kind)
        # CANDIDATE odds: command-specific, based on directed hostility / charisma / power.
        hostility=s['hostility'][_key(target,f)];cha=o['charisma'];trust=s['factions'][f]['trust']
        if kind=='Ally':chance=max(3,min(97,63-hostility//2+(cha-50)//5+(trust-50)//4))
        elif kind=='Joint':chance=max(3,min(97,65+(cha-50)//5+(trust-50)//4))
        elif kind=='Marry':chance=max(3,min(97,61-hostility//3+(cha-50)//5))
        elif kind=='Give':chance=max(3,min(97,70-hostility//3+min(20,gold//250)+(cha-50)//5))
        elif kind=='Threat':
            own=sum(q['troops'] for k,q in s['core']['provinces'].items() if s['owners'][k]==f)
            enemy=sum(q['troops'] for k,q in s['core']['provinces'].items() if s['owners'][k]==target)
            if not any(target==s['owners'][n] for k,v in s['owners'].items() if v==f for n in s['neighbors'][k]):reject('NO_BORDER')
            chance=max(3,min(28,3+12*own//max(1,enemy)-hostility//10))
        else:chance=100
        accepted=randrange(s['core'],100)<chance if kind!='Cancel' else True
        intercepted=False
        if s.get('event_policy')=='OG_EVENTS_V1_PROVISIONAL' and kind in ('Ally','Joint'):
            # An intercepted actual dispatched diplomatic letter cannot be accepted.
            from .game_events import roll
            intercepted=(roll(s['core']['rng_state'],s['month'],pid,target,kind,'intercept')%100 < max(3,hostility//10))
            if intercepted:accepted=False
        s['diplomacy_last'][f+'>'+target+':'+kind]=s['month']
        if accepted:
            if kind=='Ally':
                s['alliances'].append(alliance);s['alliances'].sort()
                for a,b in ((f,target),(target,f)):s['hostility'][_key(a,b)]=max(0,s['hostility'][_key(a,b)]-20)
            if kind=='Cancel':
                s['alliances'].remove(alliance)
                s['promises']=[v for v in s['promises'] if _pair(v['actor'],v['ally'])!=alliance]
                s['hostility'][_key(target,f)]=min(100,s['hostility'][_key(target,f)]+15)
            if kind=='Give':
                s['core']['provinces'][pid]['treasury_gold_cents']-=gold
                home=sorted(k for k,v in s['owners'].items() if v==target)[0]
                s['core']['provinces'][home]['treasury_gold_cents']+=gold
                s['hostility'][_key(target,f)]=max(0,s['hostility'][_key(target,f)]-min(25,gold//100))
            if kind=='Marry':
                s['factions'][f]['daughter']=False;s['marriages'].append(alliance);s['marriages'].sort()
                s['hostility'][_key(target,f)]=max(0,hostility-20)
            if kind=='Joint':
                s['promises']=[v for v in s['promises'] if not(v['actor']==f and v['ally']==target)]
                s['promises'].append({'actor':f,'ally':target,'target':objective,'expires':s['month']+1,
                                      'troops':p['promised_troops']})
            if kind=='Threat':
                # Clear all relations with capitulated ruler. Move all stationed officers to new ownership.
                for own,v in list(s['owners'].items()):
                    if v==target:s['owners'][own]=f
                for oid,v in list(s['officer_faction'].items()):
                    if v==target:
                        s['officer_faction'][oid]=f;s['officer_loyalty'][oid]=max(20,s['officer_loyalty'][oid]//2)
                        relations.drop_binding(s,oid)
                s['factions'][target]['alive']=False
                s['alliances']=[z for z in s['alliances'] if target not in z.split('|')]
                s['promises']=[v for v in s['promises'] if target not in (v['actor'],v['ally'])]
                s['diplomacy_last']={k:v for k,v in s['diplomacy_last'].items() if target not in k.split(':')[0].split('>')}
        elif kind=='Threat':s['hostility'][_key(target,f)]=min(100,hostility+10)
        return {'command':kind,'accepted':accepted,'intercepted':intercepted,'chance_pct':chance,'target':target,
                'officer_bp':o['bp'],'joint_objective':objective}
    def _attack(self,s,o,pid,f,p):
        if s['battle']:reject('BATTLE_ACTIVE')
        dst=p.get('target');n=integer(p.get('troops'),1,100000)
        if dst not in s['neighbors'][pid]:reject('NOT_ADJACENT')
        foe=s['owners'][dst]
        if foe==f or foe is None:reject('BAD_TARGET')
        treaty=_pair(f,foe)
        if treaty in s['alliances']:reject('ATTACK_ALLY')
        src=s['core']['provinces'][pid];defender=s['core']['provinces'][dst]
        if src['troops']<n:reject('NO_TROOPS')
        if ac.strength(s,pid and next(oid for oid,off in s['core']['officers'].items() if off is o),pid)<n:reject('NO_ARMY_TROOPS')
        ration=n  # Q19: a month of provisions; restored unused at battle settlement
        if src['food_units']<ration:reject('NO_FOOD')
        gold=integer(p.get('gold',0),0,30000)
        if src['treasury_gold_cents']<gold*100:reject('NO_GOLD')
        defending_gold=defender['treasury_gold_cents']
        defending_food=defender['food_units']
        self._bp(o,'attack');src['food_units']-=ration
        src['treasury_gold_cents']-=gold*100
        defender['food_units']=0
        defender['treasury_gold_cents']=0
        commander=next(oid for oid,off in s['core']['officers'].items() if off is o)
        ac.take(s,commander,pid,n)
        ac.ensure(s,commander)['engaged']+=n
        participants=[{'id':ac.army_id(commander),'troops':n,'home':pid}]
        ac.refresh(s)
        s['war_plans']=[z for z in s['war_plans'] if not(z['actor']==f and z['target']==dst)]
        support=0
        for promise in s['promises']:
            if promise['actor']==f and promise['target']==dst and promise['expires']>=s['month']:
                # A promise is NOT automatically army arrival: supply actual ally/adjacency + consent roll.
                ally=promise['ally']
                if _pair(f,ally) in s['alliances'] and randrange(s['core'],100)<s['factions'][ally]['trust']:
                    home=next((k for k,v in sorted(s['owners'].items()) if v==ally and dst in s['neighbors'][k] and s['core']['provinces'][k]['troops']>=promise['troops']),None)
                    if home:
                        recipient=next((oid for oid,off in sorted(s['core']['officers'].items())
                            if off['province_id']==home and s['officer_faction'][oid]==ally
                            and ac.strength(s,oid,home)>=promise['troops']),None)
                        if recipient is not None:
                            support=promise['troops']
                            ac.take(s,recipient,home,support)
                            ac.ensure(s,recipient)['engaged']+=support
                            participants.append({'id':ac.army_id(recipient),'troops':support,'home':home})
                            ac.refresh(s)
                            s['battle_support_home']=home
                break
        s['promises']=[z for z in s['promises'] if not(z['actor']==f and z['target']==dst)]
        if len(s['owners'])==41:
            from .battle_assets import battle_grid, ASSET_VERSION
            battlefield_terrain=battle_grid(int(dst[1:]))
            map_source=ASSET_VERSION
        else:
            battlefield_terrain=self._terrain(dst)
            map_source='legacy-seven-province-synthetic'
        defender_officers=sorted(oid for oid,off in s['core']['officers'].items()
                                 if off['province_id']==dst and s['officer_faction'][oid]==foe)
        defender_officer=max(defender_officers,key=lambda oid:s['core']['officers'][oid]['war']) if defender_officers else None
        att_training=s.get('unit_training',{}).get(commander,50)
        def_training=s.get('unit_training',{}).get(defender_officer,50)
        s['battle']={'src':pid,'dst':dst,'attacker':f,'defender':foe,'day':1,
            'combat_rule':'ROTK2_UNIFIED_V1_4_AGGREGATE_ADAPTER',
            'rice_a':ration*100,'rice_d':defending_food*100,
            'gold_a_cents':gold*100,'gold_d_cents':defending_gold,
            'weather':randrange(s['core'],4),'wind':randrange(s['core'],7),'burning':[],
            'map_source':map_source, 'terrain':battlefield_terrain, 'a':{'q':0,'r':6,'troops':n+support,'troops100':(n+support)*100,
                  'morale':75,'war':o['war'],'intel':o['intelligence'],
                  'training':att_training,'mp':combat.refill(att_training)},
            'd':{'q':10,'r':6,'troops':defender['troops'],'troops100':defender['troops']*100,
                  'morale':70,'war':s['core']['officers'][defender_officer]['war'] if defender_officer else 40,
                  'intel':s['core']['officers'][defender_officer]['intelligence'] if defender_officer else 40,
                  'training':def_training,'mp':combat.refill(def_training)},
            'initial_defender':defender['troops'],
            'committed_troops':n,'ally_support':support,'ally_home':s.pop('battle_support_home',None),
            'spent_rations':ration, 'result':None,'acted':False, 'participants':participants}
        officercombat.initialize(s,s['battle'])
        return {'battle_started':True,'from':pid,'to':dst,'attackers':n,'allied_support':support,
                'defenders':defender['troops'],'map':map_source}
    @staticmethod
    def _terrain(dst):
        # Explicitly provisional, deterministic 13x12 tactical grid, not imported ROM geography.
        rows=[list('.'*13) for _ in range(12)]
        for r in range(2,10):
            rows[r][5]='H' if r%2 else 'F'
            if r in (3,8):rows[r][7]='W'
        rows[6][10]='C'
        return [''.join(r) for r in rows]
    @staticmethod
    def _distance(a,b):return combat.distance((a['q'],a['r']),(b['q'],b['r']))
    def _battle(self,s,actor,kind,p):
        b=s['battle']
        if not b:reject('NO_BATTLE')
        _,_,f=self._actor(s,actor)
        if f!=b['attacker']:reject('NOT_BATTLE_OWNER')
        if b.get('officer_units'):
            return officercombat.execute(s,b,kind,p,self._settle)
        if kind=='battle_auto':
            for i in range(31):  # Q03: 30 calendar days; not 20
                if not s['battle']:break
                self._battle(s,actor,'battle_end_turn',{'_auto_player':True})
            return {'auto':True,'resolved':s['battle'] is None,
                    'outcome':s['history'][-1] if s['history'] else None}
        a,d=b['a'],b['d']
        rand=lambda n:randrange(s['core'],n)
        def _apply_exchange(att,dfn,active_is_attacker=True,charge=False):
            # Q05 simultaneous troop computation. Only commit integers to the
            # existing persistent officer-army ledger; hundredths remain saved.
            first=att is a
            aa,dd=combat.exchange(att,dfn,
                       b['terrain'][att['r']][att['q']],b['terrain'][dfn['r']][dfn['q']],
                       rand,att_ai=not first,dfn_ai=first,charge=charge)
            ac.reduce_attackers(s,b,combat.projected_soldiers(a))
            ac.reduce_defenders(s,b,combat.projected_soldiers(d))
            return {'attacker_loss100':aa,'defender_loss100':dd}
        def _ai_advance(unit,other):
            if unit['troops']<=0 or self._distance(unit,other)<=1:return
            steps=combat.reachable(b['terrain'],(unit['q'],unit['r']),unit['mp'],
                  blocked={(other['q'],other['r'])},burning=b['burning'])
            target=(other['q'],other['r'])
            closer=[(cell,cost) for cell,cost in steps.items()
                    if combat.distance(cell,target)<self._distance(unit,other)]
            if closer:
                (q,r),cost=min(closer,key=lambda v:(combat.distance(v[0],target),v[1],v[0]))
                unit['q'],unit['r']=q,r;unit['mp']-=cost
        def _test_outcome():
            if a['troops']<=0 or b['rice_a']<=0:self._settle(s,'DEFENDER');return True
            if d['troops']<=0 or b['rice_d']<=0:self._settle(s,'ATTACKER');return True
            return False
        if kind=='battle_move':
            if b['acted']:reject('ALREADY_ACTED')
            q=integer(p.get('q'),0,12);r=integer(p.get('r'),0,11)
            paths=combat.reachable(b['terrain'],(a['q'],a['r']),a['mp'],
                    blocked={(d['q'],d['r'])} if d['troops'] else (),burning=b['burning'])
            if (q,r) not in paths:reject('ILLEGAL_MOVE')
            a['q'],a['r']=q,r;a['mp']-=paths[(q,r)]
            # Moving does not consume attack eligibility (Q03).
            return {'moved':[q,r],'mobility_left':a['mp']}
        if kind=='battle_attack':
            if b['acted']:reject('ALREADY_ACTED')
            mode=p.get('mode','normal')
            if mode not in ('normal','charge','simult','fire'):reject('BAD_ATTACK_MODE')
            if self._distance(a,d)!=1:reject('NOT_ADJACENT')
            if mode=='fire':
                tile=b['terrain'][d['r']][d['q']]
                caught=combat.ignite(tile,a['intel'],d['intel'],True,b['weather'],rand)
                if caught:
                    cell=[d['q'],d['r']]
                    if cell not in b['burning']:b['burning'].append(cell)
                    if d['troops']:
                        loss=rand(max(1,d['troops100']*5//100))
                        d['troops100']=max(0,d['troops100']-loss)
                        ac.reduce_defenders(s,b,combat.projected_soldiers(d))
                b['acted']=True
                _test_outcome()
                return {'mode':'fire','ignited':caught,'resolved':s['battle'] is None}
            times=rand(10)+1 if mode=='charge' else 1
            losses=[]
            for _ in range(times):
                if a['troops']<=0 or d['troops']<=0:break
                losses.append(_apply_exchange(a,d,charge=mode=='charge'))
            if mode=='charge' and a['troops']>0:
                if d['troops']<=0:
                    a['q'],a['r']=d['q'],d['r']
                else:
                    # Q05: surviving charge can break through one hex in the
                    # original attack direction, provided the hex is free.
                    old=(a['q'],a['r']);target=(d['q'],d['r'])
                    offsets=lambda x:(((-1,1),(0,1),(1,1),(-1,0),(0,-1),(1,0)) if x&1 else
                                      ((-1,0),(0,1),(1,0),(-1,-1),(0,-1),(1,-1)))
                    direction=next((i for i,(dx,dy) in enumerate(offsets(old[0]))
                                  if (old[0]+dx,old[1]+dy)==target),None)
                    if direction is not None:
                        dx,dy=offsets(target[0])[direction];behind=(target[0]+dx,target[1]+dy)
                        if (0<=behind[0]<13 and 0<=behind[1]<12
                            and b['terrain'][behind[1]][behind[0]]!='M'
                            and list(behind) not in b['burning']
                            and rand(100)<a['war']):
                            a['q'],a['r']=behind
            b['acted']=True
            _test_outcome()
            return {'mode':mode,'exchanges':len(losses),'losses':losses,
                    'resolved':s['battle'] is None}
        if kind!='battle_end_turn':reject('UNKNOWN_KIND')
        # Original is defender slots 0/1 before invader slots 2/3. This
        # current two-aggregate-army adapter cannot expose all four turns;
        # the defender acts first in each automatic day tick.
        if d['troops']>0 and a['troops']>0:
            _ai_advance(d,a)
            if self._distance(a,d)==1:_apply_exchange(d,a)
        if _test_outcome():return {'resolved':True}
        if a['troops']>0 and d['troops']>0 and not b['acted'] and p.get('_auto_player'):
            _ai_advance(a,d)
            if self._distance(a,d)==1:_apply_exchange(a,d)
        if _test_outcome():return {'resolved':True}
        # Side-wide rice pools, stored as hundredths, Q03. Defender extra
        # reserve list is unavailable in this aggregate adapter: count 0.
        b['rice_a']=combat.rice_tick(b['rice_a'],a['troops100'])
        b['rice_d']=combat.rice_tick(b['rice_d'],d['troops100'])
        # Q08: wind selects two/three eligible hex directions; each fire burns
        # one day. Rain and calm wind clear the flames. Unit damage is posted
        # through the ordinary persistent army ledger, never silently erased.
        if b['weather']==3 or b['wind']==6:
            b['burning']=[]
        else:
            winds=((4,5),(3,4,5),(3,4),(1,2),(0,1,2),(0,1))
            newly=set()
            old={tuple(x) for x in b['burning']}
            for x,y in sorted(old):
                # Directional positions are computed without clipping; the
                # iterator's skipped edge cells must not renumber direction IDs.
                q_odd=bool(x&1)
                offsets=(((-1,1),(0,1),(1,1),(-1,0),(0,-1),(1,0)) if q_odd else
                         ((-1,0),(0,1),(1,0),(-1,-1),(0,-1),(1,-1)))
                for direction in winds[b['wind']]:
                    dx,dy=offsets[direction];nx,ny=x+dx,y+dy
                    if not 0<=nx<13 or not 0<=ny<12 or (nx,ny) in old or (nx,ny) in newly:continue
                    tile=b['terrain'][ny][nx]
                    if tile=='C':continue
                    occupant=next((u for u in (a,d) if (u['q'],u['r'])==(nx,ny) and u['troops']>0),None)
                    if combat.ignite(tile,0,occupant['intel'] if occupant else 0,bool(occupant),b['weather'],rand):
                        newly.add((nx,ny))
            for unit,is_defender in ((a,False),(d,True)):
                if (unit['q'],unit['r']) in old and unit['troops100']>0:
                    unit['troops100']=max(0,unit['troops100']-rand(max(1,unit['troops100']*30//100)))
                    if is_defender:ac.reduce_defenders(s,b,combat.projected_soldiers(unit))
                    else:
                        ac.reduce_attackers(s,b,combat.projected_soldiers(unit))
                        # Slot-2 commander (invading group): Q08 burns 30%
                        # of rice reserves. Gold is not a battlefield pool in
                        # this current aggregate state schema (known gap).
                        b['rice_a']=b['rice_a']*70//100
            b['burning']=[list(c) for c in sorted(newly)]
        b['day']+=1;b['acted']=False
        a['mp']=combat.refill(a['training'],a['mp'])
        d['mp']=combat.refill(d['training'],d['mp'])
        if b['day']%3==1:b['wind']=rand(7)
        if rand(100)<40:b['weather']=rand(4)
        if _test_outcome():return {'resolved':True}
        if b['day']>30:self._settle(s,'DEFENDER') # Q13: invader loses timeout
        return {'day':b['day'],'attackers':a['troops'],'defenders':d['troops'],
                'rice_a':b['rice_a'],'rice_d':b['rice_d'],
                'resolved':s['battle'] is None}
    def _settle(self,s,winner):
        """Settle assigned army survivors, never create an unassigned military pool."""
        b=s['battle'];src=s['core']['provinces'][b['src']];dst=s['core']['provinces'][b['dst']]
        # Unused attacker rations are returned, and defender's actual battle
        # consumption is posted once to its store (not each command preview).
        if b.get('combat_rule') in ('ROTK2_UNIFIED_V1_4_AGGREGATE_ADAPTER',
                                    'ROTK2_UNIFIED_V1_4_OFFICER_LEDGER'):
            # Q09 source: entire remaining attacker + defender battlefield pools
            # go to the contested province regardless of outcome; retreaters
            # carry no supplies. Source province was debited at battle launch.
            dst['food_units']=min(30000,(b['rice_a']+b['rice_d'])//100)
            dst['treasury_gold_cents']=min(3000000,max(0,b.get('gold_a_cents',0))+
                                                      max(0,b.get('gold_d_cents',0)))
        previous=b['start_defender'] if 'start_defender' in b else b.get('initial_defender',dst['troops'])
        # On capture, survivors of each defending army retreat with their own
        # general to a still-friendly neighbor, or are captured as prisoners.
        retreat=None
        if winner=='ATTACKER':
            s['owners'][b['dst']]=b['attacker']
            dst['support_ticks']=max(0,dst['support_ticks']-200000)
            retreat=next((pid for pid in s['neighbors'][b['dst']] if s['owners'][pid]==b['defender']),None)
            for aid,army in list(s['armies'].items()):
                count=army['stations'].pop(b['dst'],0)
                if not count:continue
                if retreat:
                    army['stations'][retreat]=army['stations'].get(retreat,0)+count
                else:s['extras'][b['dst']]['prisoners']+=count
            s['governors'].pop(b['dst'],None)
            s.get('appointed_advisors',{}).pop(b['dst'],None)
            # A commander may have *other* garrisons outside this battlefield.
            # Capturing him without reassigning those troops makes a foreign army
            # appear inside friendly territory (ENEMY_ARMY_STATION) and stalls
            # battle_auto. This is an interim campaign-ledger safety policy,
            # NOT a claim of verified original ROTK II prisoner/retreat logic.
            # First try a real officer from the defending side to take each
            # off-field detachment. If none exists, the commander can withdraw
            # to his own surviving distant garrison instead of being captured.
            dispositions=[]
            for oid,off in sorted(s['core']['officers'].items()):
                if off['province_id']!=b['dst'] or s['officer_faction'][oid]!=b['defender']:
                    continue
                if retreat:
                    off['province_id']=retreat
                    dispositions.append({'officer':oid,'status':'retreated','province':retreat})
                    continue
                army=s['armies'].get(ac.army_id(oid))
                stations=sorted(army['stations'].items()) if army else []
                can_capture=True
                reassign=[]
                for remote_pid,n in stations:
                    if s['owners'].get(remote_pid)!=b['defender']:
                        reject('ENEMY_ARMY_STATION')
                    # Prefer an officer physically at the remote garrison;
                    # otherwise another surviving officer of the original side.
                    peers=sorted(other for other,v in s['core']['officers'].items()
                                 if other!=oid and s['officer_faction'].get(other)==b['defender']
                                 and s['owners'].get(v['province_id'])==b['defender']
                                 and v['province_id']!=b['dst'])
                    if not peers:
                        can_capture=False
                        break
                    target=min(peers,key=lambda other:(
                        s['core']['officers'][other]['province_id']!=remote_pid,other))
                    reassign.append((remote_pid,n,target))
                if not can_capture:
                    # No legal replacement commander: retain the original
                    # faction/army and withdraw to the staffed distant post.
                    # Do not conjure an officer or delete living soldiers.
                    home=stations[0][0]
                    off['province_id']=home
                    dispositions.append({'officer':oid,'status':'escaped_to_distant_garrison','province':home})
                    continue
                for remote_pid,n,target in reassign:
                    ac.ensure(s,target)['stations'][remote_pid]=(
                        ac.ensure(s,target)['stations'].get(remote_pid,0)+n)
                    del army['stations'][remote_pid]
                    dispositions.append({'officer':oid,'status':'remote_troops_reassigned',
                                         'province':remote_pid,'troops':n,'new_commander':target})
                s['officer_faction'][oid]=b['attacker']
                s['officer_loyalty'][oid]=25
                relations.drop_binding(s,oid)
                dispositions.append({'officer':oid,'status':'captured_then_joined_winner',
                                     'province':b['dst']})
            if not any(o==b['defender'] for o in s['owners'].values()):
                s['factions'][b['defender']]['alive']=False
                s['alliances']=[z for z in s['alliances'] if b['defender'] not in z.split('|')]
                s['promises']=[v for v in s['promises'] if b['defender'] not in (v['actor'],v['ally'])]
        # Each attacking/allied contingent has an actual army with a named commander.
        for entry in b['participants']:
            army=s['armies'][entry['id']]
            number=entry['troops']
            army['engaged']=0
            if number:
                where=(entry['home'] if entry['home']!=b['src'] else
                       (b['dst'] if winner=='ATTACKER' else b['src']))
                army['stations'][where]=army['stations'].get(where,0)+number
        ally_survivors=sum(p['troops'] for p in b['participants'] if p['home']!=b['src'])
        ac.cleanup(s);ac.refresh(s)
        h={'month':s['month'],'src':b['src'],'dst':b['dst'],'winner':winner,
           'attacker_survivors':b['a']['troops'],'defender_survivors':b['d']['troops'],
           'start_attacker':b['committed_troops']+b['ally_support'],
           'start_defender':b.get('initial_defender',previous),
           'defender_captured_prisoners':b['d']['troops'] if winner=='ATTACKER' and not retreat else 0,
           'allied_survivors':ally_survivors}
        if winner=='ATTACKER':
            h['defender_officer_dispositions']=dispositions
        s['history'].append(h)
        s['battle']=None
    def advance_month(self):
        if self.state['battle']:reject('BATTLE_ACTIVE')
        staged=copy.deepcopy(self.state)
        old=staged['month']
        troop_before={pid:p['troops'] for pid,p in staged['core']['provinces'].items()}
        b=Bus(staged['core'])
        if staged.get('balance_policy') in ('OG_1000_SLOW_GROWTH_V1_CANDIDATE','OG_1000_DIRECTOR_V2'):
            from .balance_policy_1000 import handlers
            scheduler.run_month(b,handlers=handlers(staged['owners']))
        else:scheduler.run_month(b)
        staged['core']=b.data
        # The core's March wage policy sends unpaid soldiers back to civilian
        # population. Reconcile the assigned army ledger without creating a
        # separate military reserve or double-returning personnel.
        for pid,old_total in troop_before.items():
            delta=old_total-staged['core']['provinces'][pid]['troops']
            if delta<0:reject('UNBACKED_SOLDIER_CREATION')
            if delta:ac.lose_at(staged,pid,delta)
        # Governorships issue autonomous work orders through this one atomic monthly transaction.
        for pid,g in sorted(staged['governors'].items()):
            o=staged['core']['officers'][g['officer']];p=staged['core']['provinces'][pid]
            if g['focus']=='personnel':
                # Synthetic delegated Personnel Affairs. Spend ACTUAL governor BP
                # and province gold; no free loyalty or arbitrary troop changes.
                f=staged['owners'][pid]
                eligible=sorted((oid for oid,v in staged['officer_faction'].items()
                          if v==f and oid!=relations.ruler(staged,f)
                          and oid not in staged.get('sworn_brothers',{}).get(f,[])
                          and staged['core']['officers'][oid]['province_id']==pid
                          and staged['officer_loyalty'][oid]<85),
                          key=lambda oid:(staged['officer_loyalty'][oid],oid))
                if eligible and o['bp']>=3 and p['treasury_gold_cents']>=1000:
                    target=eligible[0];before=staged['officer_loyalty'][target]
                    o['bp']-=3;p['treasury_gold_cents']-=1000
                    staged['officer_loyalty'][target]=min(100,before+3)
                    staged.setdefault('personnel_history',[]).append({
                        'month':old,'province':pid,'governor':g['officer'],
                        'target':target,'gold_cents':1000,'bp':3,
                        'before':before,'after':staged['officer_loyalty'][target],
                        'policy':relations.POLICY})
                continue
            if o['bp']<3:continue
            focus={'farms':'farmers','industry':'metalworkers','defense':None}[g['focus']]
            if staged.get('workforce_policy'):
                from .recruitment_labor import draw, available
                # A governor reassigns existing real jobs, never an empty retired
                # phantom labor pool. Do not consume the target job itself.
                if available(p['workers'],exclude=(focus,) if focus else ())<10:continue
            elif p['workers']['unassigned']<10:continue
            if focus is None and not ac.capacity_available(staged,g['officer'],10):continue
            if staged.get('workforce_policy'):
                draw(p['workers'],10,exclude=(focus,) if focus else ())
            else:p['workers']['unassigned']-=10
            o['bp']-=3
            if focus:p['workers'][focus]+=10
            else:
                ac.add(staged,g['officer'],pid,10)
                ac.refresh(staged)
                p['support_ticks']=max(0,p['support_ticks']-5000)
        if staged.get('balance_policy') in ('OG_1000_SLOW_GROWTH_V1_CANDIDATE','OG_1000_DIRECTOR_V2'):
            from .balance_policy_1000 import post_month
            post_month(staged) if staged.get('balance_policy')=='OG_1000_SLOW_GROWTH_V1_CANDIDATE' else __import__('third_branch.balance_policy_1000',fromlist=['post_month_director_v2']).post_month_director_v2(staged)
        else:
            # Legacy fixture logic is preserved for archived replay.
            for pid in sorted(staged['owners']):
                p=staged['core']['provinces'][pid];x=staged['extras'][pid]
                x['metal']+=p['workers']['resources']//10
                forging=min(x['metal'],p['workers']['metalworkers']//10)
                x['metal']-=forging;x['arms']+=forging
                consumption=(p['troops']+59)//60
                used=min(consumption,p['food_units']);p['food_units']-=used
                if used<consumption:
                    ac.lose_at(staged,pid,min(p['troops'],(consumption-used)*2))
        staged['month']=scheduler.cursor(staged['core'])[0]
        if staged.get('source_label')=='ORIGINAL_GAME_HISTORICAL_S1_PROPOSED':
            from .historical_s1 import process_arrivals
            process_arrivals(staged)
        staged['advisor_snapshots'].append({'month':old, 'owners':copy.deepcopy(staged['owners']),
            'provinces':{pid:{'gold':q['treasury_gold_cents'],'food':q['food_units'],
                'support':q['support_ticks'],'troops':q['troops']} for pid,q in sorted(staged['core']['provinces'].items())}})
        staged['promises']=[v for v in staged['promises'] if v['expires']>=staged['month']]
        staged['war_plans']=[v for v in staged['war_plans'] if v['expires']>=staged['month']]
        staged['revision']+=1
        rid='month:'+str(old)
        staged['processed'].append(rid)
        staged['log'].append({'id':rid,'kind':'month','actor':None,'month':old,'result':{'next_month':staged['month']}})
        if staged.get('event_policy')=='OG_EVENTS_V1_PROVISIONAL':
            from .game_events import process_month
            process_month(staged,old)
        if staged.get('workforce_policy'):
            from .scenario1_workforce import sync
            sync(staged,'month:'+str(old))
        validate(staged)
        self.state=staged
        return {'month_completed':old,'next_month':self.state['month'],'state_hash':self.fingerprint}
    def advice(self,owner='A'):
        if owner not in self.state['factions']:reject('BAD_TARGET')
        s=self.state
        rows=[]
        for pid,p in s['core']['provinces'].items():
            if s['owners'][pid]!=owner:continue
            x=s['extras'][pid]
            warnings=[]
            if p['food_units']<p['troops']//10:warnings.append('Food reserves low')
            if p['support_ticks']<1500000:warnings.append('Public support low')
            if x['arms']<p['troops']//4:warnings.append('Arms production lagging')
            if any(s['owners'][n]!=owner for n in s['neighbors'][pid]):warnings.append('Border garrison at risk')
            rows.append({'province':pid,'troops':p['troops'],'food':p['food_units'],
                        'gold_cents':p['treasury_gold_cents'],'metal':x['metal'],'arms':x['arms'],
                        'support_ticks':p['support_ticks'],'warnings':warnings})
        prev=s['advisor_snapshots'][-6] if len(s['advisor_snapshots'])>=6 else None
        six_month=[]
        if prev:
            for r in rows:
                pid=r['province'];earlier=prev['provinces'][pid]
                six_month.append({'province':pid,'gold_change_cents':r['gold_cents']-earlier['gold'],
                    'food_change':r['food']-earlier['food'],
                    'support_change_ticks':r['support_ticks']-earlier['support'],
                    'troop_change':r['troops']-earlier['troops']})
        return {'ruler':owner,'month':s['month'],'advice':rows,
                'six_month_review':six_month,'observed_months':len(s['advisor_snapshots']),
                'year_or_season':'PROVISIONAL'}


def save_bytes(c):
    validate(c.state)
    d=c.state
    return canonical_bytes({'sha256':hash_state(d),'state':d,'format':'OG-INTEGRATED-TEST-1'})


def load_bytes(raw):
    def reject_dupes(pairs):
        d={}
        for k,v in pairs:
            if k in d:reject('DUP_KEY')
            d[k]=v
        return d
    try:doc=json.loads(raw.decode('utf-8'),object_pairs_hook=reject_dupes,
                       parse_constant=lambda v:reject('BAD_CONST'),parse_float=lambda v:reject('FLOAT'))
    except (UnicodeDecodeError,ValueError) as e:reject('BAD_SAVE',str(e))
    if set(doc)!=set(('format','sha256','state')) or doc['format']!='OG-INTEGRATED-TEST-1':reject('BAD_SAVE')
    if hash_state(doc['state'])!=doc['sha256']:reject('HASH_MISMATCH')
    state=doc['state']
    # Older v1.7 saves have only aggregate province troop numbers. Each soldier
    # is assigned to an existing legal officer on upgrade, never to a reserve.
    if 'armies' not in state:
        ac.bootstrap(state)
    return Campaign(state)


def save_file(c,path):
    path=Path(path);temp=path.with_suffix(path.suffix+'.tmp')
    with temp.open('wb') as f:f.write(save_bytes(c));f.flush();os.fsync(f.fileno())
    os.replace(temp,path)


def load_file(path):return load_bytes(Path(path).read_bytes())
