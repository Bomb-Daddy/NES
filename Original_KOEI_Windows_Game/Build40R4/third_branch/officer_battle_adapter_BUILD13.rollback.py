"""Original-game officer-level ROTK II unified battle adapter.

Uses exactly the Q01/Q02/Q03/Q05/Q08/Q13 primitive functions in
third_branch.rotk2_battle_core, with campaign army ledgers from army_cohorts.
The remaining Q09-Q11/Q14/Q18 advanced menus remain a Classic source priority,
not implied to be player-facing here until their UI and persistent routing pass.
"""
from __future__ import annotations
from og.core.rng import randrange
from og.core.fixed import CoreError
from . import rotk2_battle_core as core
from . import army_cohorts as ac


def fail(code):
    raise CoreError(code,code)


def _unit(s,oid,side,slot,men,q,r,commander=False):
    o=s['core']['officers'][oid]
    return {'id':oid,'army_id':ac.army_id(oid),'side':side,'slot':slot,'commander':commander,
            'q':q,'r':r,'troops':men,'troops100':men*100,'war':o['war'],
            'intel':o['intelligence'],'morale':s['officer_loyalty'].get(oid,70),
            'training':s.get('unit_training',{}).get(oid,50),'mp':core.refill(s.get('unit_training',{}).get(oid,50)),
            'troop_ratio':100,'acted':False,'fled':False,'arrive_day':1}


def initialize(s,b):
    """Materialize real named officers with stationed/engaged army strength."""
    units=[]
    for idx,entry in enumerate(b['participants']):
        oid=entry['id'].removeprefix('army:')
        if oid not in s['core']['officers']:fail('MISSING_BATTLE_OFFICER')
        q,r=[(0,6),(0,5),(0,7),(1,6),(1,5),(1,7),(2,6),(2,5),(2,7)][idx%9]
        units.append(_unit(s,oid,'A',2 if idx==0 else 3,entry['troops'],q,r,idx==0 or idx==1))
    defender=[]
    for oid,o in sorted(s['core']['officers'].items()):
        men=s['armies'].get(ac.army_id(oid),{}).get('stations',{}).get(b['dst'],0)
        if men and s['officer_faction'][oid]==b['defender']:
            defender.append((oid,men))
    if sum(x[1] for x in defender)!=b['d']['troops']:
        fail('UNACCOUNTED_DEFENDER_TROOPS')
    for idx,(oid,men) in enumerate(defender):
        q,r=[(10,6),(10,5),(10,7),(9,6),(9,5),(9,7),(11,5),(11,6),(11,7)][idx%9]
        units.append(_unit(s,oid,'D',0,men,q,r,idx==0))
    b['officer_units']=units
    b['selected_officer']=units[0]['id'] if units else None
    b['combat_rule']='ROTK2_UNIFIED_V1_4_OFFICER_LEDGER'
    return units


def find(b,side,oid=None):
    units=[u for u in b.get('officer_units',()) if u['side']==side and u['troops100']>=100 and not u['fled']]
    if oid is None:
        return next((u for u in units if u['commander']),units[0] if units else None)
    return next((u for u in units if u['id']==oid),None)


def _all(b,side):
    return [u for u in b['officer_units'] if u['side']==side and u['troops100']>=100 and not u['fled']]


def _sync(s,b):
    # Reconcile EXACT named officer strengths against armies. Never invent troops.
    for entry in b['participants']:
        u=next((u for u in b['officer_units'] if u['army_id']==entry['id'] and u['side']=='A'),None)
        survivors=core.projected_soldiers(u) if u is not None else 0
        entry['troops']=survivors
        army=s['armies'][entry['id']];army['engaged']=survivors
    for u in b['officer_units']:
        if u['side']!='D':continue
        army=s['armies'].get(u['army_id'])
        if army is None:continue
        n=core.projected_soldiers(u)
        if n:army['stations'][b['dst']]=n
        else:army['stations'].pop(b['dst'],None)
    b['a']['troops']=sum(p['troops'] for p in b['participants'])
    b['d']['troops']=sum(core.projected_soldiers(u) for u in b['officer_units'] if u['side']=='D')
    a=find(b,'A');d=find(b,'D')
    if a is not None:
        for k in ('q','r','morale','mp','war','intel','training'):
            b['a'][k]=a[k]
    if d is not None:
        for k in ('q','r','morale','mp','war','intel','training'):
            b['d'][k]=d[k]
    b['a']['troops100']=sum(u['troops100'] for u in b['officer_units'] if u['side']=='A')
    b['d']['troops100']=sum(u['troops100'] for u in b['officer_units'] if u['side']=='D')
    b['acted']=bool(a is not None and a['acted'])
    ac.refresh(s)


def _result(s,b):
    if not _all(b,'A') or b['rice_a']<=0:
        return 'DEFENDER'
    if not _all(b,'D') or b['rice_d']<=0:
        return 'ATTACKER'
    return None


def _adjacent(b,u):
    return sorted((v for v in _all(b,'D' if u['side']=='A' else 'A')
                   if core.distance((u['q'],u['r']),(v['q'],v['r']))==1),
                  key=lambda v:(v['troops100'],v['id']))


def _exchange(s,b,u,v):
    rand=lambda n:randrange(s['core'],n)
    aa,dd=core.exchange(u,v,b['terrain'][u['r']][u['q']],b['terrain'][v['r']][v['q']],rand,
                        att_ai=u['side']=='D',dfn_ai=v['side']=='D')
    u['troops']=core.projected_soldiers(u)
    v['troops']=core.projected_soldiers(v)
    return {'officer':u['id'],'target':v['id'],'actor_loss100':aa,'target_loss100':dd}


def _blocked(b,actor):
    return {(u['q'],u['r']) for u in b['officer_units'] if u is not actor and u['troops100']>=100 and not u['fled']}


def _move(s,b,u,to):
    if u['acted']:fail('ALREADY_ACTED')
    options=core.reachable(b['terrain'],(u['q'],u['r']),u['mp'],_blocked(b,u),b['burning'])
    if to not in options:fail('ILLEGAL_MOVE')
    u['q'],u['r']=to;u['mp']-=options[to]
    return {'officer':u['id'],'moved':list(to),'mobility_left':u['mp']}


def _auto_ai(s,b,side):
    for u in _all(b,side):
        if u['acted']:continue
        targets=_all(b,'D' if side=='A' else 'A')
        if not targets:break
        foes=_adjacent(b,u)
        if not foes:
            target=min(targets,key=lambda v:(core.distance((u['q'],u['r']),(v['q'],v['r'])),v['id']))
            options=core.reachable(b['terrain'],(u['q'],u['r']),u['mp'],_blocked(b,u),b['burning'])
            closer=[(pos,cost) for pos,cost in options.items() if core.distance(pos,(target['q'],target['r']))<core.distance((u['q'],u['r']),(target['q'],target['r']))]
            if closer:
                pos,_=min(closer,key=lambda z:(core.distance(z[0],(target['q'],target['r'])),z[1],z[0]))
                _move(s,b,u,pos)
        foes=_adjacent(b,u)
        if foes:
            _exchange(s,b,u,foes[0]);u['acted']=True


def execute(s,b,kind,p,settle):
    """Execute real named officer commands and commit campaign ledger each move."""
    rand=lambda n:randrange(s['core'],n)
    if kind=='battle_auto':
        for i in range(31):
            if not s.get('battle'):break
            execute(s,b,'battle_end_turn',{'_auto_player':True},settle)
        return {'auto':True,'resolved':s['battle'] is None}
    if kind=='battle_move':
        oid=p.get('officer_id',b.get('selected_officer'))
        u=find(b,'A',oid)
        if u is None:fail('NO_SUCH_BATTLE_OFFICER')
        q=p.get('q');r=p.get('r')
        if type(q) is not int or type(r) is not int or not 0<=q<13 or not 0<=r<12:fail('BAD_PARAMS')
        result=_move(s,b,u,(q,r))
        _sync(s,b)
        return result
    if kind=='battle_attack':
        oid=p.get('officer_id',b.get('selected_officer'))
        u=find(b,'A',oid)
        if u is None:fail('NO_SUCH_BATTLE_OFFICER')
        if u['acted']:fail('ALREADY_ACTED')
        mode=p.get('mode','normal')
        if mode not in ('normal','charge','simult','fire'):fail('BAD_ATTACK_MODE')
        target=find(b,'D',p.get('target_officer_id'))
        if target is None or core.distance((u['q'],u['r']),(target['q'],target['r']))!=1:
            fail('NOT_ADJACENT')
        if mode=='fire':
            pos=(target['q'],target['r'])
            fire=core.ignite(b['terrain'][pos[1]][pos[0]],u['intel'],target['intel'],True,b['weather'],rand)
            if fire:
                if list(pos) not in b['burning']:b['burning'].append(list(pos))
                target['troops100']=max(0,target['troops100']-rand(max(1,target['troops100']*5//100)))
                target['troops']=core.projected_soldiers(target)
            losses=[]
        else:
            losses=[]
            if mode=='simult':
                for _ in range(2):
                    if u['troops100']<100:break
                    destinations=list(core.neighbors(u['q'],u['r']))
                    for cell in destinations:
                        foe=next((v for v in _all(b,'D') if (v['q'],v['r'])==cell),None)
                        if foe:
                            losses.append(_exchange(s,b,u,foe))
                            if u['troops100']<100:break
            else:
                for _ in range(rand(10)+1 if mode=='charge' else 1):
                    if u['troops100']<100 or target['troops100']<100:break
                    losses.append(_exchange(s,b,u,target))
                if mode=='charge' and target['troops100']<100 and u['troops100']>=100:
                    u['q'],u['r']=target['q'],target['r']
            fire=False
        u['acted']=True
        _sync(s,b)
        outcome=_result(s,b)
        if outcome:settle(s,outcome)
        return {'mode':mode,'officer':oid,'target':target['id'],'losses':losses,
                'ignited':fire,'resolved':s['battle'] is None}
    if kind!='battle_end_turn':fail('UNKNOWN_KIND')
    _auto_ai(s,b,'D')
    if p.get('_auto_player'): _auto_ai(s,b,'A')
    _sync(s,b)
    outcome=_result(s,b)
    if outcome:settle(s,outcome);return {'resolved':True}
    b['rice_a']=core.rice_tick(b['rice_a'],sum(u['troops100'] for u in _all(b,'A')))
    b['rice_d']=core.rice_tick(b['rice_d'],sum(u['troops100'] for u in _all(b,'D')))
    b['day']+=1
    b['burning']=[] if b['weather']==3 or b['wind']==6 else b['burning']
    if b['day']%3==1:b['wind']=rand(7)
    if rand(100)<40:b['weather']=rand(4)
    for unit in b['officer_units']:
        unit['acted']=False
        unit['mp']=core.refill(unit['training'],unit['mp'])
    _sync(s,b)
    outcome=_result(s,b)
    if b['day']>30:outcome='DEFENDER'
    if outcome:settle(s,outcome)
    return {'day':b['day'],'resolved':s['battle'] is None}
