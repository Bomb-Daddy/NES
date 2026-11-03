"""Original-game officer-level ROTK II unified battle adapter.

Uses exactly the Q01/Q02/Q03/Q05/Q08/Q13 primitive functions in
third_branch.rotk2_battle_core, with campaign army ledgers from army_cohorts.
The remaining Q09-Q11/Q14/Q18 advanced menus remain a Classic source priority,
not implied to be player-facing here until their UI and persistent routing pass.
"""
from __future__ import annotations
from collections import deque
from og.core.rng import randrange
from og.core.fixed import CoreError
from . import rotk2_battle_core as core
from . import army_cohorts as ac
from . import rom_battle_map


# Q08 source-backed NES/SNES Markov acceptance thresholds.  The original
# English NES code's table at file offset 0x1DCAC is byte-identical to the
# user's SNES ROM table at 0x3B5DB.  Rows = current weather, columns =
# proposed weather.  The 40% change-attempt gate is applied by the day loop.
WEATHER_MARKOV = ((6, 9, 10, 0),
                  (1, 3, 6, 10),
                  (0, 1, 2, 10),
                  (5, 7, 8, 10))


def weather_transition(current: int, proposed: int, threshold_roll: int) -> int:
    """Accept a proposed weather type only under its ROM-backed threshold.

    This is the original source's transition predicate; deterministic PRNG
    draw timing remains PORT behavior, not an NES hardware-RNG claim.
    """
    if not (0 <= current < 4 and 0 <= proposed < 4 and 0 <= threshold_roll < 10):
        raise ValueError('INVALID_WEATHER_TRANSITION')
    return proposed if proposed != current and threshold_roll < WEATHER_MARKOV[current][proposed] else current

def ruler_release_destination(s, b, officer_id):
    """Q14 source-backed ruler refuge: an adjacent owned province is required.

    The previous fallback to any remote garrison made an impossible Release
    button available when the defeated ruler had no neighboring safe home.
    Other officers retain the legacy release fallback pending source decoding.
    """
    defender=b['defender']
    if s['factions'][defender].get('ruler_officer')!=officer_id:
        return None
    return next((pid for pid in s['neighbors'][b['dst']]
                 if pid!=b['dst'] and s['owners'].get(pid)==defender),None)



def cpu_ruler_disposition(s, b, winner, defeated):
    """Q14 SNES sengo ruler decision when the original source bytes are known.

    q14_source_relation on the winning faction is the original R byte keyed by
    defeated faction; q14_source_stat6 on the defeated faction is its +6 byte;
    q14_source_guard on the winner is g_E006. These are independent source
    fields, NOT provisional diplomacy/trust scores. Missing bytes preserve the
    old explicitly provisional release policy; no parity is claimed for it.
    """
    if ruler_release_destination(s, b, s['factions'][defeated].get('ruler_officer')) is None:
        return 'behead'
    victor=s['factions'][winner];loser=s['factions'][defeated]
    relations=victor.get('q14_source_relation',{})
    if not isinstance(relations,dict):fail('INVALID_Q14_SOURCE_FIELDS')
    r=relations.get(defeated)
    stat6=loser.get('q14_source_stat6')
    guard=victor.get('q14_source_guard')
    if r is None or stat6 is None or guard is None:
        return 'release'  # preexisting provisional fallback, not original parity
    if not (type(r) is int and 0<=r<=255 and type(stat6) is int and 0<=stat6<=255
            and type(guard) is int and 0<=guard<=65535):
        fail('INVALID_Q14_SOURCE_FIELDS')
    if guard>=200:return 'behead'
    chance=max(0,min(100,(100-r+stat6)//2))
    if randrange(s['core'],100)<chance:
        relations[defeated]=max(0,r-50)
        return 'release'
    return 'behead'


def prisoner_options(s, b, officer_id):
    """The actual allowed Q14 choices, shared by controller and engine."""
    ruler=s['factions'][b['defender']].get('ruler_officer')
    if officer_id!=ruler:
        return ('recruit','release','behead')
    return (('release','behead') if
            ruler_release_destination(s,b,officer_id) is not None
            else ('behead',))


def fail(code):
    raise CoreError(code,code)


def _unit(s,oid,side,slot,men,q,r,commander=False):
    o=s['core']['officers'][oid]
    return {'id':oid,'army_id':ac.army_id(oid),'side':side,'slot':slot,'commander':commander,
            'q':q,'r':r,'troops':men,'troops100':men*100,'war':o['war'],
            'intel':o['intelligence'],'morale':s['officer_loyalty'].get(oid,70),
            'training':s.get('unit_training',{}).get(oid,50),'mp':core.refill(s.get('unit_training',{}).get(oid,50)),
            'troop_ratio':100,'acted':False,'fled':False,'arrive_day':1,
            'original_faction':s['officer_faction'][oid],'switched':False,
            'duty':o.get('duty',60),'protected':False}


def initialize(s,b):
    """Materialize named officer armies and deploy in original Q04 regions.

    The 41-map path uses exact ROM terrain/region/neighbor bytes. CPU-controlled
    tactical units draw a free zone cell using the persisted PORT RNG; the
    original SNES beam-timing RNG/candidate ordering remains unverified.
    Seven-province legacy fixtures retain their historical synthetic positions.
    """
    units=[]
    original=b.get('terrain_source')==rom_battle_map.MAP_SOURCE
    if original:
        target=int(b['dst'][1:]);source=int(b['src'][1:])
        if b['terrain']!=rom_battle_map.province(target)['terrain']:
            fail('Q04_TERRAIN_MISMATCH')
        if source not in rom_battle_map.province(target)['neighbors']:
            fail('Q04_NONADJACENT_ORIGIN')
        occupied=set()
        cpu_random_used=False
        human=s.get('human_faction')
        tactical_human=human in (b['attacker'],b['defender'])
        def choose(side,faction):
            nonlocal cpu_random_used
            zone=source if side=='A' else target
            cells=rom_battle_map.legal_cells(target,zone,occupied=occupied,burning=b['burning'])
            if side=='D':
                castle=tuple(rom_battle_map.province(target)['castle'])
                cells.sort(key=lambda cell:(cell!=castle,core.distance(cell,castle),cell[1],cell[0]))
            else:
                cells.sort(key=lambda cell:(b['terrain'][cell[1]][cell[0]]!='.',cell[1],cell[0]))
            if not cells:fail('Q04_DEPLOYMENT_ZONE_FULL')
            # The SNES AI selects a random free candidate in the correct zone.
            # Do not consume tactical-placement RNG for offscreen CPU-vs-CPU.
            if tactical_human and faction!=human:
                cell=cells[randrange(s['core'],len(cells))]
                cpu_random_used=True
            else:
                cell=cells[0]  # Human interactive placement still unimplemented.
            occupied.add(cell)
            return cell
    for idx,entry in enumerate(b['participants']):
        oid=entry['id'].removeprefix('army:')
        if oid not in s['core']['officers']:fail('MISSING_BATTLE_OFFICER')
        q,r=choose('A',s['officer_faction'][oid]) if original else [(0,6),(0,5),(0,7),(1,6),(1,5),(1,7),(2,6),(2,5),(2,7)][idx%9]
        units.append(_unit(s,oid,'A',2,entry['troops'],q,r,idx==0))
    defender=[]
    for oid,o in sorted(s['core']['officers'].items()):
        men=s['armies'].get(ac.army_id(oid),{}).get('stations',{}).get(b['dst'],0)
        if men and s['officer_faction'][oid]==b['defender']:
            defender.append((oid,men))
    if sum(x[1] for x in defender)!=b['d']['troops']:
        fail('UNACCOUNTED_DEFENDER_TROOPS')
    for idx,(oid,men) in enumerate(defender):
        q,r=choose('D',s['officer_faction'][oid]) if original else [(10,6),(10,5),(10,7),(9,6),(9,5),(9,7),(11,5),(11,6),(11,7)][idx%9]
        units.append(_unit(s,oid,'D',0,men,q,r,idx==0))
    b['officer_units']=units
    b['selected_officer']=units[0]['id'] if units else None
    # Q11 CPU challenge occurs at battle opening, before ordinary orders.
    # Legacy campaigns with no explicit human faction treat attacker as human.
    human=s.get('human_faction') or b['attacker']
    b['duel_pending']=None
    if human in (b['attacker'],b['defender']):
        ai_side='D' if human==b['attacker'] else 'A'
        challenger=choose_ai_duel_challenger(b,ai_side)
        if challenger is not None:
            b['duel_pending']={'side':ai_side,'officer_id':challenger['id']}
    b['combat_rule']='ROTK2_UNIFIED_V1_4_OFFICER_LEDGER'
    if original:
        for side,key in (('A','a'),('D','d')):
            first=next((u for u in units if u['side']==side),None)
            if first:
                b[key]['q'],b[key]['r']=first['q'],first['r']
        b['deployment_source']=('NES_ROM_Q04_REGIONS_CPU_RANDOM_PORT' if cpu_random_used
                                else 'NES_ROM_Q04_REGIONS_DETERMINISTIC_PORT')
    return units


def find(b,side,oid=None):
    units=[u for u in b.get('officer_units',()) if u['side']==side and u['troops100']>=100 and not u['fled'] and not u.get('reserve')]
    if oid is None:
        return next((u for u in units if u['commander']),units[0] if units else None)
    return next((u for u in units if u['id']==oid),None)


def _all(b,side):
    return [u for u in b['officer_units'] if u['side']==side and u['troops100']>=100 and not u['fled'] and not u.get('reserve')]


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
        if u.get('reserve'):
            # Defender reserves are in transit: no phantom defenders at dst.
            army['engaged']=n
        elif n:army['stations'][b['dst']]=n
        else:army['stations'].pop(b['dst'],None)
    b['a']['troops']=sum(p['troops'] for p in b['participants'])
    b['d']['troops']=sum(core.projected_soldiers(u) for u in b['officer_units']
                       if u['side']=='D' and not u.get('reserve') and not u['fled'])
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
    # A defender can lose its last station in the middle of battle.
    # Remove zero-strength army records before campaign validation/cold save.
    ac.cleanup(s)
    # Preserve a zero-strength participant until once-only settlement.
    for entry in b['participants']:
        if entry['id'] not in s['armies']:
            ac.ensure(s,entry['id'].removeprefix('army:'))
    ac.refresh(s)


def _pending(b,side):
    return any(u['side']==side and u.get('reserve') and not u.get('fled')
               and u['troops100']>=100 for u in b['officer_units'])

def _result(s,b):
    if b['rice_a']<=0 or (not _all(b,'A') and not _pending(b,'A')):
        return 'DEFENDER'
    if b['rice_d']<=0 or (not _all(b,'D') and not _pending(b,'D')):
        return 'ATTACKER'
    return None


def _adjacent(b,u):
    return sorted((v for v in _all(b,'D' if u['side']=='A' else 'A')
                   if core.distance((u['q'],u['r']),(v['q'],v['r']))==1),
                  key=lambda v:(v['troops100'],v['id']))


def _human_controls(s,b,u):
    """Q05/Q07: control belongs to the faction, never to a fixed side label.

    Legacy single-player fixtures without an explicit human faction treat the
    attacking side as player-controlled. A third-party human faction makes
    both sides CPU-controlled in the tactical adapter.
    """
    human=s.get('human_faction',b.get('attacker'))
    faction=b.get('attacker') if u['side']=='A' else b.get('defender')
    return human is not None and human==faction


def _difficulty(s,b):
    level=b.get('difficulty',s.get('battle_difficulty',2))
    if type(level) is not int or not 1<=level<=5:fail('BAD_BATTLE_DIFFICULTY')
    return level


def _exchange(s,b,u,v):
    rand=lambda n:randrange(s['core'],n)
    aa,dd=core.exchange(u,v,b['terrain'][u['r']][u['q']],b['terrain'][v['r']][v['q']],rand,
                        difficulty=_difficulty(s,b),
                        att_ai=not _human_controls(s,b,u),
                        dfn_ai=not _human_controls(s,b,v))
    u['troops']=core.projected_soldiers(u)
    v['troops']=core.projected_soldiers(v)
    return {'officer':u['id'],'target':v['id'],'actor_loss100':aa,'target_loss100':dd}


def _blocked(b,actor):
    return {(u['q'],u['r']) for u in b['officer_units'] if u is not actor and u['troops100']>=100 and not u['fled']}


# Q07: automatic three-direction forest reaction; not a selectable Ambush command.
_AMBUSH_ARC=((0,1,3),(0,1,2),(1,2,5),(0,3,4),(3,4,5),(2,4,5))
_WIND_SPREAD=((4,5),(3,4,5),(3,4),(1,2),(0,1,2),(0,1),())


def _step_hex(q,r,direction):
    if direction not in range(6):fail('BAD_DIRECTION')
    opts=(((-1,1),(0,1),(1,1),(-1,0),(0,-1),(1,0)) if q&1 else
          ((-1,0),(0,1),(1,0),(-1,-1),(0,-1),(1,-1)))
    dx,dy=opts[direction]
    x,y=q+dx,r+dy
    return (x,y) if 0<=x<13 and 0<=y<12 else None


def _forest_reaction(s,b,mover,from_cell,*,directions=None,exclude=None):
    direction=next((d for d in range(6) if _step_hex(*from_cell,d)==(mover['q'],mover['r'])),None)
    if (directions is None and direction is None) or mover['intel']>89 or mover['troops100']<100:return 0
    rand=lambda n:randrange(s['core'],n)
    damage=0
    for d in (_AMBUSH_ARC[direction] if directions is None else directions):
        pos=_step_hex(mover['q'],mover['r'],d)
        if pos is None or b['terrain'][pos[1]][pos[0]]!='F':continue
        enemy=next((u for u in _all(b,'D' if mover['side']=='A' else 'A')
                    if u is not exclude and (u['q'],u['r'])==pos),None)
        if enemy is None:continue
        power=lambda u:3*(u['war']+(20 if u.get('lu_bu',False) else 0))+u['morale']+u.get('troop_ratio',100)
        d_difficulty=(_difficulty(s,b) if not _human_controls(s,b,mover)
                      and _human_controls(s,b,enemy) else 1)
        numerator=(power(mover)-power(enemy)-min(enemy['troops100'],rand(300)))*10
        divisor=core.DEF_DIV[b['terrain'][mover['r']][mover['q']]]*d_difficulty
        quotient=abs(numerator)//divisor
        quotient=quotient if numerator>=0 else -quotient
        single=rand(30)+1 if quotient>=0 else -quotient
        loss=min(mover['troops100'],3*min(mover['troops100'],single)*mover.get('troop_ratio',100)//100)
        mover['troops100']-=loss;damage+=loss
        if mover['troops100']<100:break
    mover['troops']=core.projected_soldiers(mover)
    if damage:b.setdefault('combat_events',[]).append({'kind':'forest_ambush','officer':mover['id'],'loss100':damage})
    return damage


def _spread_fire(s,b):
    old={tuple(p) for p in b['burning']}
    if not old or b['weather']==3 or b['wind']==6:
        b['burning']=[]
        return
    rand=lambda n:randrange(s['core'],n)
    fresh=set()
    for cell in sorted(old):
        for d in _WIND_SPREAD[b['wind']]:
            loc=_step_hex(*cell,d)
            if loc is None or loc in old or loc in fresh:continue
            tile=b['terrain'][loc[1]][loc[0]]
            if tile=='C':continue
            target=next((u for u in b['officer_units']
                         if u['troops100']>=100 and not u['fled'] and (u['q'],u['r'])==loc),None)
            if core.ignite(tile,0,target['intel'] if target else 0,target is not None,b['weather'],rand):
                fresh.add(loc)
    b['burning']=[list(p) for p in sorted(fresh)]
    for loc in sorted(fresh):
        victim=next((u for u in b['officer_units']
                     if u['troops100']>=100 and not u['fled'] and (u['q'],u['r'])==loc),None)
        if victim is None:continue
        if victim['side']=='A' and victim['commander']:
            b['rice_a']=b['rice_a']*70//100
            b['gold_a_cents']=b.get('gold_a_cents',0)*70//100
        damage=rand(max(1,victim['troops100']*30//100))
        victim['troops100']=max(0,victim['troops100']-damage)
        victim['troops']=core.projected_soldiers(victim)
        b.setdefault('combat_events',[]).append({'kind':'fire_spread','officer':victim['id'],'loss100':damage})


def _move(s,b,u,to):
    if u['acted']:fail('ALREADY_ACTED')
    options=core.reachable(b['terrain'],(u['q'],u['r']),u['mp'],_blocked(b,u),b['burning'])
    if to not in options:fail('ILLEGAL_MOVE')
    # Q07 reacts after EVERY traversed hex, not just the final selected hex.
    # The original unit can perish en route; never teleport a dead unit onward.
    route=core.shortest_route(b['terrain'],(u['q'],u['r']),to,u['mp'],
                              _blocked(b,u),b['burning'])
    for cell in route:
        previous=(u['q'],u['r'])
        u['mp']-=core.COST[b['terrain'][cell[1]][cell[0]]]
        u['q'],u['r']=cell
        _forest_reaction(s,b,u,previous)
        if u['troops100']<100:
            break
    return {'officer':u['id'],'moved':[u['q'],u['r']],'mobility_left':u['mp']}



def _reinforcement_spot(b,side,home=None):
    """Q09 arrival uses the source province's Q04 region on verified maps."""
    if b.get('terrain_source')==rom_battle_map.MAP_SOURCE:
        if home is None:return None
        target=int(b['dst'][1:]);source=int(home[1:])
        cells=rom_battle_map.legal_cells(target,source,occupied=_blocked(b,None),burning=b['burning'])
        return cells[0] if cells else None
    # Legacy seven-province fixture, explicitly not a ROM deployment oracle.
    active=_all(b,side)
    anchors=sorted(((u['q'],u['r']) for u in active),key=lambda z:(z[0],z[1]))
    if not anchors:anchors=[(0,6) if side=='A' else (11,6)]
    blocked=_blocked(b,None)
    seen=set(anchors);queue=deque(anchors)
    while queue:
        pos=queue.popleft()
        for nxt in core.neighbors(*pos):
            if nxt in seen:continue
            seen.add(nxt);queue.append(nxt)
            q,r=nxt
            if nxt not in blocked and list(nxt) not in b['burning'] and b['terrain'][r][q] not in ('M','C'):
                return nxt
    return None


def _queue_reinforcements(s,b,p,*,side='A'):
    """Q09: actual named officer station -> in-transit reserve -> field.

    Source and supplies are debited atomically by Campaign.execute's staged
    transaction. An allied-faction volunteer requires separate allegiance
    handling; do not silently relabel an allied army as a native faction.
    """
    if side not in ('A','D'):fail('BAD_REINFORCEMENT_SIDE')
    faction=b['attacker'] if side=='A' else b['defender']
    pool='a' if side=='A' else 'd'
    source=p.get('source');ids=p.get('officer_ids');day=p.get('arrival_day')
    rice=p.get('rice',0);gold=p.get('gold',0)
    if source not in s['neighbors'].get(b['dst'],[]) or s['owners'].get(source)!=faction:
        fail('BAD_REINFORCEMENT_SOURCE')
    if type(day) is not int or not b['day']<day<=30:fail('BAD_ARRIVAL_DAY')
    if (not isinstance(ids,list) or not ids or len(ids)>8 or
        len(set(map(str,ids)))!=len(ids) or any(type(oid) is not str for oid in ids)):
        fail('BAD_REINFORCEMENT_OFFICERS')
    if any(type(v) is not int or v<0 for v in (rice,gold)):fail('BAD_REINFORCEMENT_SUPPLIES')
    province=s['core']['provinces'][source]
    if province['food_units']<rice or province['treasury_gold_cents']<gold*100:
        fail('INSUFFICIENT_REINFORCEMENT_SUPPLIES')
    if b['rice_'+pool]+rice*100>3000000 or b['gold_'+pool+'_cents']+gold*100>3000000:
        fail('REINFORCEMENT_POOL_OVERFLOW')
    slot=3 if side=='A' else 1
    existing_slot=[u for u in b['officer_units'] if u['side']==side and u['slot']==slot]
    if len(existing_slot)+len(ids)>8:
        fail('REINFORCEMENT_SLOT_FULL')
    troops=[]
    for oid in ids:
        if (oid not in s['core']['officers'] or
            s['officer_faction'].get(oid)!=faction or
            s['core']['officers'][oid]['province_id']!=source or
            any(u['id']==oid for u in b['officer_units'])):
            fail('BAD_REINFORCEMENT_OFFICER')
        n=ac.strength(s,oid,source)
        if n<1:fail('NO_REINFORCEMENT_TROOPS')
        troops.append(n)
    for index,(oid,n) in enumerate(zip(ids,troops)):
        ac.take(s,oid,source,n)
        ac.ensure(s,oid)['engaged']+=n
        unit=_unit(s,oid,side,slot,n,-1,-1)
        # Q09: the first member is the head of this reinforcement list.
        # Later arrivals append to that list; never create a second head.
        unit.update(commander=not existing_slot and index==0,
                    reserve=True,arrive_day=day,home=source,mp=0)
        b['officer_units'].append(unit)
        if side=='A':b['participants'].append({'id':ac.army_id(oid),'troops':n,'home':source})
    province['food_units']-=rice;province['treasury_gold_cents']-=gold*100
    b['rice_'+pool]+=rice*100;b['gold_'+pool+'_cents']+=gold*100
    b.setdefault('combat_events',[]).append({'kind':'reinforcement_queued','side':side,
        'source':source,'officers':ids,'day':day,'rice':rice,'gold':gold})
    _sync(s,b)
    return {'queued':ids,'source':source,'arrival_day':day,'rice':rice,'gold':gold}


def _arrive_reinforcements(s,b):
    arrived=[]
    for u in b['officer_units']:
        if (not u.get('reserve') or u.get('fled') or u['arrive_day']>b['day']
                or u['troops100']<100):continue
        spot=_reinforcement_spot(b,u['side'],u.get('home'))
        if spot is None:continue  # do not overwrite an occupied hex
        u['q'],u['r']=spot;u['reserve']=False
        u['mp']=core.refill(u['training']);u['acted']=False
        if u['side']=='D':
            army=s['armies'][u['army_id']]
            army['engaged']-=core.projected_soldiers(u)
            army['stations'][b['dst']]=army['stations'].get(b['dst'],0)+core.projected_soldiers(u)
        arrived.append(u['id'])
        b.setdefault('combat_events',[]).append({'kind':'reinforcement_arrived',
            'officer':u['id'],'side':u['side'],'cell':list(spot),'day':b['day']})
    return arrived


def _player_side(s,b):
    """The player can command either original main list; old saves default A."""
    return 'D' if s.get('human_faction')==b['defender'] else 'A'


def _withdraw(s,b,p,*,side='A'):
    """Q09: officer or whole-army withdrawal with explicit destination ledger."""
    dest=p.get('destination');whole=p.get('whole_army',False)
    if type(whole) is not bool:fail('BAD_WITHDRAW_SCOPE')
    if dest not in s['neighbors'].get(b['dst'],[]):fail('BAD_WITHDRAW_DESTINATION')
    faction=b['attacker'] if side=='A' else b['defender']
    if s['owners'].get(dest) not in (None,faction):fail('ENEMY_WITHDRAW_DESTINATION')
    if whole:
        units=[u for u in _all(b,side) if s['officer_faction'][u['id']]==faction]
        if not units or not any(u['commander'] for u in units):fail('COMMANDER_REQUIRED')
        if any(u['side']==side and s['officer_faction'][u['id']]!=faction and
               u['troops100']>=100 for u in b['officer_units']):fail('ALLIED_WITHDRAW_UNRESOLVED')
        # Unarrived reserves must also return to their actual home stations.
        units += [u for u in b['officer_units'] if u['side']==side and u.get('reserve')
                  and s['officer_faction'][u['id']]==faction]
    else:
        oid=p.get('officer_id',b.get('selected_officer'))
        unit=find(b,side,oid)
        if unit is None:fail('NO_SUCH_BATTLE_OFFICER')
        if s['officer_faction'][unit['id']]!=faction:fail('NOT_OWN_OFFICER')
        units=[unit]
    for u in units:
        u['fled']=True;u['acted']=True;u['mp']=0
        u['retreat_to']=u.get('home',dest) if u.get('reserve') else dest
    b.setdefault('combat_events',[]).append({'kind':'withdraw','officers':[u['id'] for u in units],
                                               'destination':dest,'whole_army':whole})
    _sync(s,b)
    outcome=('DEFENDER' if side=='A' else 'ATTACKER') if whole else _result(s,b)
    return {'withdrawn':[u['id'] for u in units],'destination':dest,
            'outcome':outcome}

def q19_defender_help_threshold(invader_troops,defender_troops,defender_rice):
    """SNES excsub2:C3B0 integer/strict CPU defender request thresholds."""
    values=(invader_troops,defender_troops,defender_rice)
    if any(type(v) is not int or v<0 for v in values):fail('Q19_INVALID_HELP_INPUT')
    return invader_troops//5 > defender_troops//4 and defender_troops < defender_rice//2


def q19_request_cpu_defender_help(s,b):
    """Source-backed request, Q09 real ledger debit; source choice/delay PORT."""
    if any(ev.get('kind')=='q19_cpu_defender_help_request'
           for ev in b.get('combat_events',())):
        return False  # one opening request; prevents duplicated Q09 debit
    if s.get('human_faction')!=b['attacker'] or b['defender']==s.get('human_faction'):
        return False
    attackers=sum(u['troops100']//100 for u in _all(b,'A'))
    defenders=sum(u['troops100']//100 for u in _all(b,'D'))
    if not q19_defender_help_threshold(attackers,defenders,b['rice_d']//100):
        return False
    b.setdefault('combat_events',[]).append({'kind':'q19_cpu_defender_help_request',
        'invaders':attackers,'defenders':defenders,'day':b['day']})
    if len([u for u in b['officer_units'] if u['slot']==1])>=8 or b['day']>=30:
        b['combat_events'].append({'kind':'q19_help_unfulfilled','reason':'slot_or_day'})
        return True
    for source in sorted(s['neighbors'].get(b['dst'],())):
        if s['owners'].get(source)!=b['defender']:continue
        eligible=[oid for oid,o in sorted(s['core']['officers'].items())
                  if o['province_id']==source and s['officer_faction'].get(oid)==b['defender']
                  and ac.strength(s,oid,source)>0
                  and all(u['id']!=oid for u in b['officer_units'])]
        if eligible:
            _queue_reinforcements(s,b,{'source':source,'officer_ids':[eligible[0]],
                'rice':0,'gold':0,'arrival_day':min(30,b['day']+2)},side='D')
            b['combat_events'].append({'kind':'q19_help_queued_port',
                'officer':eligible[0],'source':source})
            return True
    b['combat_events'].append({'kind':'q19_help_unfulfilled','reason':'no_eligible_officer'})
    return True

def _auto_ai(s,b,side):
    for u in _all(b,side):
        if u['acted']:continue
        targets=_all(b,'D' if side=='A' else 'A')
        # Q17 ai_select_target: evacuate an occupied burning hex before
        # attacking an adjacent enemy. The SNES free-cell candidate ranking
        # remains unknown; deterministic cost/distance is PORT-only policy.
        if [u['q'],u['r']] in b['burning']:
            safe=core.reachable(b['terrain'],(u['q'],u['r']),u['mp'],
                                _blocked(b,u),b['burning'])
            if safe:
                escape=min(safe,key=lambda cell:(
                    safe[cell],
                    -min((core.distance(cell,(v['q'],v['r'])) for v in targets),default=0),
                    cell))
                _move(s,b,u,escape)
                if u['troops100']<100:
                    continue
        # Q17: a commander with <3000 scaled troops retreats or holds.
        # Holding is the conservative source-permitted branch until the
        # exact SNES retreat candidate ranking is implemented. This must
        # happen after emergency fire evacuation and before target selection.
        if u.get('commander') and u['troops100']<3000:
            u['acted']=True
            b.setdefault('combat_events',[]).append({
                'kind':'q17_commander_hold','officer':u['id'],
                'troops100':u['troops100']})
            continue
        if not targets:continue  # Q17: do not skip later burning CPU officers
        foes=_adjacent(b,u)
        if not foes:
            target=min(targets,key=lambda v:(core.distance((u['q'],u['r']),(v['q'],v['r'])),v['id']))
            options=core.reachable(b['terrain'],(u['q'],u['r']),u['mp'],_blocked(b,u),b['burning'])
            closer=[(pos,cost) for pos,cost in options.items() if core.distance(pos,(target['q'],target['r']))<core.distance((u['q'],u['r']),(target['q'],target['r']))]
            if closer:
                pos,_=min(closer,key=lambda z:(core.distance(z[0],(target['q'],target['r'])),z[1],z[0]))
                _move(s,b,u,pos)
                if u['troops100']<100:
                    continue
        foes=_adjacent(b,u)
        if foes:
            _exchange(s,b,u,foes[0]);u['acted']=True



def _switch_side(s,b,u,new_side,loyalty):
    """Q10: transfer one named army between the field lists, never duplicate it.

    Existing distant stations are handed to real same-faction officers before
    a switch. If there is no valid officer, fail closed and let the campaign's
    transaction rollback rather than leaving enemy armies in friendly towns.
    """
    old_side=u['side']
    if old_side==new_side or u['commander'] or u.get('switched'):
        fail('ILLEGAL_SWITCH')
    old_faction=s['officer_faction'][u['id']]
    new_faction=b['attacker'] if new_side=='A' else b['defender']
    if old_faction==new_faction:fail('ILLEGAL_SWITCH')
    aid=u['army_id'];army=s['armies'][aid]
    field=core.projected_soldiers(u)
    # Validate the entire transfer before any mutation, including off-field
    # detachments that the new faction cannot command after switching.
    source=b['dst'] if old_side=='D' else None
    if old_side=='D' and army['stations'].get(source,0)!=field:
        fail('BATTLE_ARMY_MISMATCH')
    if old_side=='A' and army['engaged']!=field:
        fail('BATTLE_ARMY_MISMATCH')
    transfers=[]
    for pid,n in sorted(army['stations'].items()):
        if old_side=='D' and pid==source:continue
        peers=[oid for oid,off in s['core']['officers'].items()
               if oid!=u['id'] and s['officer_faction'].get(oid)==old_faction
               and off['province_id']==pid and s['owners'].get(pid)==old_faction]
        if not peers:fail('REMOTE_ARMY_REASSIGNMENT_REQUIRED')
        transfers.append((pid,n,min(peers)))
    for pid,n,peer in transfers:
        receiver=ac.ensure(s,peer)
        ac.check_capacity(s,peer,n)
        receiver['stations'][pid]=receiver['stations'].get(pid,0)+n
        del army['stations'][pid]
    if old_side=='D':
        del army['stations'][source]
        army['engaged']=field
        b['participants'].append({'id':aid,'troops':field,'home':b['src']})
        s['core']['officers'][u['id']]['province_id']=b['src']
    else:
        b['participants']=[p for p in b['participants'] if p['id']!=aid]
        army['engaged']=0
        army['stations'][b['dst']]=army['stations'].get(b['dst'],0)+field
        s['core']['officers'][u['id']]['province_id']=b['dst']
    s['officer_faction'][u['id']]=new_faction
    s['officer_loyalty'][u['id']]=loyalty
    from . import officer_relations as relations
    relations.drop_binding(s,u['id'])
    u['side']=new_side;u['slot']=2 if new_side=='A' else 0
    u['commander']=False;u['switched']=True;u['acted']=True;u['mp']=0
    u['morale']=loyalty
    b.setdefault('combat_events',[]).append({'kind':'switch_sides','officer':u['id'],
                                              'from':old_side,'to':new_side})
    _sync(s,b)


def _bribe(s,b,p,*,side='A'):
    """Q10: gold is spent on failure; cannot bribe a list head or ally."""
    oid=p.get('officer_id',b.get('selected_officer'))
    actor=find(b,side,oid)
    if actor is None:fail('NO_SUCH_BATTLE_OFFICER')
    if actor['acted']:fail('ALREADY_ACTED')
    enemy='D' if side=='A' else 'A'
    target=find(b,enemy,p.get('target_officer_id'))
    if target is None or target['commander'] or target.get('switched'):
        fail('ILLEGAL_BRIBE_TARGET')
    gold=p.get('gold')
    if type(gold) is not int or not 1<=gold<=99:fail('ILLEGAL_BRIBE_AMOUNT')
    pool='gold_a_cents' if side=='A' else 'gold_d_cents'
    if b[pool]<gold*100:fail('NOT_ENOUGH_BATTLE_GOLD')
    # Q10 duty is not fully sourced for original-game officer catalog yet;
    # the existing 60 fallback remains explicitly provisional.
    loyalty=s['officer_loyalty'][target['id']]
    threshold=target.get('duty',60)//3+8+(2*loyalty)//3+(7 if loyalty==100 else 0)
    returning=target.get('original_faction')==(b['attacker'] if side=='A' else b['defender'])
    rand=lambda n:randrange(s['core'],n)
    success=returning or target.get('protected',False) or rand(gold)>threshold
    # For an actual switch, check remote army ownership before spending gold.
    # Transaction rollback protects the entire command on a failed invariant.
    if success:
        _switch_side(s,b,target,side,100 if returning else 80+rand(20))
    b[pool]-=gold*100
    actor['acted']=True;actor['mp']=0
    b.setdefault('combat_events',[]).append({'kind':'bribe','officer':oid,
        'target':target['id'],'gold':gold,'success':success})
    _sync(s,b)
    return {'officer':oid,'target':target['id'],'gold_spent':gold,'success':success}



def _spontaneous_switches(s,b):
    """Q10: non-head loyalty <=49 units get one daily side-change trial."""
    rand=lambda n:randrange(s['core'],n)
    changed=[]
    for u in list(b['officer_units']):
        # Original Q10 daily scan is limited to primary lists 0/2.
        # Reinforcement slots 1/3, even after arrival, never enter this scan.
        if (u['slot'] not in (0,2) or u.get('reserve') or
            u['troops100']<100 or u['fled'] or u['commander'] or
            u.get('switched') or u.get('protected')):
            continue
        loyalty=s['officer_loyalty'][u['id']]
        if loyalty>49:continue
        if rand(100)+10>=rand(100-loyalty):continue
        other='D' if u['side']=='A' else 'A'
        # Any unstaffed distant station is a currently unsupported ownership
        # transfer; do not corrupt the army ledger to manufacture a switch.
        # This condition must be closed by the complete original rule work.
        try:
            _switch_side(s,b,u,other,90+rand(10))
        except CoreError as exc:
            if exc.code!='REMOTE_ARMY_REASSIGNMENT_REQUIRED':raise
            b.setdefault('combat_events',[]).append({'kind':'switch_blocked_remote',
                                                       'officer':u['id']})
            continue
        changed.append(u['id'])
    return changed



def choose_ai_duel_challenger(b, side):
    """Q11 hsovl5 CPU opening: primary-list officer with highest War >=90.

    No reinforcement candidates, no RNG draw, stable first-in-list tie.
    """
    if side not in ('A','D'):fail('BAD_DUEL_SIDE')
    slot=2 if side=='A' else 0
    candidates=[u for u in b.get('officer_units',()) if u['side']==side and
                u['slot']==slot and u['troops100']>=100 and
                not u.get('fled') and not u.get('reserve') and u['war']>=90]
    return max(candidates,key=lambda u:u['war']) if candidates else None


def choose_ai_duel_acceptor(b, challenger):
    """Q11 hsovl5: original non-head first, commander fallback acceptance.

    Only the opposing primary list participates; reserve slots do not
    introduce new duel candidates.  No RNG is consumed by this scan.
    """
    if challenger.get('reserve') or challenger.get('fled') or challenger['troops100']<100:
        return None
    opposing = 'D' if challenger['side']=='A' else 'A'
    main_slot = 0 if opposing=='D' else 2
    members = [u for u in b['officer_units'] if u['side']==opposing and
               u['slot']==main_slot and u['troops100']>=100 and
               not u.get('reserve') and not u.get('fled')]
    if not members:
        return None
    head = next((u for u in members if u['commander']), members[0])
    war = challenger['war']
    threshold = min(war+20,74)
    for unit in members:
        if unit is not head and unit['war']>threshold:
            return unit
    if head['war']>war+19:
        return head
    return None


def _duel(s,b,p,settle,*,response=False):
    """Q11 once-only duel, including CPU-originated challenge to a human."""
    if b.get('duel_done') or b['day']!=1:fail('DUEL_ALREADY_RESOLVED')
    pending=b.get('duel_pending')
    if response and pending is None:fail('NO_PENDING_CPU_CHALLENGE')
    if not response and pending is not None:fail('PENDING_CPU_CHALLENGE')
    side=pending['side'] if response else _player_side(s,b)
    challenger=find(b,side,pending['officer_id'] if response else
                    p.get('officer_id',b.get('selected_officer')))
    if challenger is None:fail('INVALID_DUEL_OFFICER')
    accepted=p.get('accepted')
    if type(accepted) is not bool:fail('BAD_DUEL_ACCEPTANCE')
    opponent_side='D' if side=='A' else 'A'
    opponent=find(b,opponent_side,p.get('target_officer_id')) if accepted else None
    if challenger['slot']!=(2 if side=='A' else 0):fail('INVALID_DUEL_OFFICER')
    if any(u.get('acted') for u in b['officer_units'] if u['troops100']>=100 and not u.get('reserve')):
        fail('DUEL_NOT_AT_BATTLE_OPENING')
    if not response:
        # Source Q11: the AI chooses its own responder. A human cannot force
        # acceptance, refusal, or a specific CPU officer by editing params.
        expected=choose_ai_duel_acceptor(b,challenger)
        if accepted!=(expected is not None) or (accepted and
                (opponent is None or opponent['id']!=expected['id'])):
            fail('AI_DUEL_ACCEPTANCE_MISMATCH')
    if accepted and (opponent is None or
                     opponent['slot']!=(0 if opponent_side=='D' else 2)):
        fail('INVALID_DUEL_OFFICER')
    rand=lambda n:randrange(s['core'],n)
    b['duel_done']=True
    b['duel_pending']=None
    result='REFUSED'
    if not accepted:
        # Refusal penalizes the *responding* side.  A CPU defender can
        # challenge the human attacker, in which case A—not D—loses troops.
        for u in _all(b,opponent_side):
            loss=u['troops100']*(5+rand(5))//100
            u['troops100']=max(0,u['troops100']-loss)
            u['troops']=core.projected_soldiers(u)
    else:
        aw,dw=challenger['war'],opponent['war']
        ah,dh=aw,dw
        diff=abs(aw-dw)
        ceiling=max(1,20-max(diff//5,1))
        aa=20 if aw<dw else ceiling
        dd=20 if dw<aw else ceiling
        if challenger.get('protected') or opponent.get('protected'):
            aa=max(1,aw//10);dd=max(1,dw//10)
        result='DRAW'
        for _ in range(10):
            ah-=rand(aa);dh-=rand(dd)
            if ah<=0 and dh<=0:result='DRAW';break
            # Label victory by actual army side. A defending CPU officer
            # can be the challenger; that does not make it the attacker.
            if ah<=0:result='DEFENDER' if side=='A' else 'ATTACKER';break
            if dh<=0:result='ATTACKER' if side=='A' else 'DEFENDER';break
        if result in ('ATTACKER','DEFENDER'):
            challenger_win='ATTACKER' if side=='A' else 'DEFENDER'
            winner,loser=(challenger,opponent) if result==challenger_win else (opponent,challenger)
            # Q14 fate/capture still requires a separate prisoner-disposition UI.
            loser['troops100']=0;loser['troops']=0
            if loser['war']>winner['war']:
                new_war=(winner['war']+loser['war']+1)//2
                winner['war']=new_war
                s['core']['officers'][winner['id']]['war']=new_war
    b.setdefault('combat_events',[]).append({'kind':'duel','challenger':challenger['id'],
        'defender':opponent['id'] if opponent else None,'result':result})
    _sync(s,b)
    outcome=_result(s,b)
    if outcome:settle(s,outcome)
    return {'duel_result':result,'resolved':s['battle'] is None}



def _normalize_build26_initial_slots(b):
    """Repair pre-Build27 saved initial attackers without touching real Q09 reserves.

    Build26 placed every nonfirst initial attacker into reinforcement slot 3,
    with its second attacker incorrectly flagged as that slot's commander.
    Genuine Q09 reinforcements have arrival_day >= 2, so the day-one
    non-reserve signature is unambiguous for this specific historical bug.
    """
    for u in b.get('officer_units',()):
        if (u['side']=='A' and u['slot']==3 and not u.get('reserve')
                and u.get('arrive_day',1)==1):
            u['slot']=2
            u['commander']=False


def execute(s,b,kind,p,settle):
    """Execute real named officer commands and commit campaign ledger each move."""
    _normalize_build26_initial_slots(b)
    rand=lambda n:randrange(s['core'],n)
    if b.get('duel_pending') is not None and kind!='battle_duel_response':
        fail('PENDING_CPU_CHALLENGE')
    if kind=='battle_duel_response':
        if not b.get('duel_pending'):fail('NO_PENDING_CPU_CHALLENGE')
        if type(p.get('accepted')) is not bool:fail('BAD_DUEL_ACCEPTANCE')
        if p['accepted'] and type(p.get('target_officer_id')) is not str:
            fail('INVALID_DUEL_OFFICER')
        return _duel(s,b,p,settle,response=True)
    if b.get('pending_outcome'):
        if kind!='battle_prisoner':fail('PRISONER_DECISIONS_REQUIRED')
        if set(p)!={'officer_id','decision'}:fail('BAD_PRISONER_PARAMS')
        oid=p['officer_id'];decision=p['decision']
        if oid not in b.get('prisoner_queue',[]) or oid in b.get('prisoner_decisions',{}):
            fail('INVALID_PRISONER')
        if decision not in ('recruit','release','behead'):fail('BAD_PRISONER_DECISION')
        # The original ruler menu cannot offer a non-adjacent refuge.
        ruler=s['factions'][b['defender']].get('ruler_officer')
        if oid==ruler and decision=='recruit':fail('RULER_CANNOT_BE_RECRUITED')
        if decision=='release' and (
                ruler_release_destination(s,b,oid) is None if oid==ruler
                else not any(pid!=b['dst'] and owner==b['defender']
                             for pid,owner in s['owners'].items())):
            fail('NO_RELEASE_DESTINATION')
        if decision not in prisoner_options(s,b,oid):fail('BAD_PRISONER_DECISION')
        b['prisoner_decisions'][oid]=decision
        pending=[x for x in b['prisoner_queue'] if x not in b['prisoner_decisions']]
        if not pending:settle(s,b['pending_outcome'])
        return {'officer_id':oid,'decision':decision,'pending':pending,
                'resolved':s['battle'] is None}
    if kind=='battle_prisoner':fail('NO_PENDING_PRISONERS')
    if kind=='battle_auto':
        for i in range(31):
            if not s.get('battle') or b.get('pending_outcome'):break
            execute(s,b,'battle_end_turn',{'_auto_player':True},settle)
        return {'auto':True,'resolved':s['battle'] is None}
    player_side=_player_side(s,b)
    enemy_side='D' if player_side=='A' else 'A'
    if kind=='battle_reinforce':
        return _queue_reinforcements(s,b,p,side=player_side)
    if kind=='battle_withdraw':
        result=_withdraw(s,b,p,side=player_side)
        if result['outcome']:settle(s,result['outcome'])
        return dict(result,resolved=s['battle'] is None)
    if kind=='battle_duel':
        return _duel(s,b,p,settle)
    if kind=='battle_bribe':
        result=_bribe(s,b,p,side=player_side)
        outcome=_result(s,b)
        if outcome:settle(s,outcome)
        return result
    if kind=='battle_wait':
        oid=p.get('officer_id',b.get('selected_officer'))
        u=find(b,player_side,oid)
        if u is None:fail('NO_SUCH_BATTLE_OFFICER')
        if u['acted']:fail('ALREADY_ACTED')
        u['mp']=min(6,u['mp']+1)  # Q02 Wait/Rest, one point, cap six
        u['acted']=True
        _sync(s,b)
        return {'officer':oid,'rested':True,'mobility_left':u['mp']}
    if kind=='battle_move_enemy':
        oid=p.get('officer_id',b.get('selected_officer'))
        u=find(b,player_side,oid)
        if u is None:fail('NO_SUCH_BATTLE_OFFICER')
        tid=p.get('target_officer_id')
        target=find(b,enemy_side,tid) if tid is not None else None
        if target is None:fail('NO_SUCH_TARGET')
        old=(u['q'],u['r'])
        if core.distance(old,(target['q'],target['r']))!=1:fail('NOT_ADJACENT')
        if u['mp']<=1:fail('INSUFFICIENT_MOBILITY')
        q=p.get('q');r=p.get('r')
        if type(q) is not int or type(r) is not int or not 0<=q<13 or not 0<=r<12:fail('BAD_PARAMS')
        if core.distance(old,(q,r))!=1:fail('MOVE_ENEMY_ONE_HEX_ONLY')
        result=_move(s,b,u,(q,r))
        # Q06: drag only if actor INT >= enemy INT, enemy not on castle,
        # and random 0..99 is less than 50; otherwise the move still occurs.
        dragged=(u['troops100']>=100 and u['intel']>=target['intel']
                 and b['terrain'][target['r']][target['q']]!='C'
                 and rand(100)<50)
        if dragged:
            target_original=(target['q'],target['r'])
            target['q'],target['r']=old
            if list(old) in b['burning']:
                loss=rand(max(1,target['troops100']*5//100))
                target['troops100']=max(0,target['troops100']-loss)
                target['troops']=core.projected_soldiers(target)
            if target['troops100']>=100:
                # Q07: after Move Enemy, the dragged target receives a five-
                # direction passive forest scan, excluding the reverse path.
                opposite=next((d for d in range(6) if _step_hex(*old,d)==target_original),None)
                _forest_reaction(s,b,target,old,
                    directions=tuple(d for d in range(6) if d!=opposite),exclude=u)
        _sync(s,b)
        outcome=_result(s,b)
        if outcome:settle(s,outcome)
        return dict(result,dragged=dragged,target=tid,resolved=s['battle'] is None)
    if kind=='battle_move':
        oid=p.get('officer_id',b.get('selected_officer'))
        u=find(b,player_side,oid)
        if u is None:fail('NO_SUCH_BATTLE_OFFICER')
        q=p.get('q');r=p.get('r')
        if type(q) is not int or type(r) is not int or not 0<=q<13 or not 0<=r<12:fail('BAD_PARAMS')
        result=_move(s,b,u,(q,r))
        _sync(s,b)
        return result
    if kind=='battle_attack':
        oid=p.get('officer_id',b.get('selected_officer'))
        u=find(b,player_side,oid)
        if u is None:fail('NO_SUCH_BATTLE_OFFICER')
        if u['acted']:fail('ALREADY_ACTED')
        mode=p.get('mode','normal')
        if mode not in ('normal','charge','simult','fire'):fail('BAD_ATTACK_MODE')
        target=find(b,enemy_side,p.get('target_officer_id'))
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
                        foe=next((v for v in _all(b,enemy_side) if (v['q'],v['r'])==cell),None)
                        if foe:
                            losses.append(_exchange(s,b,u,foe))
                            if u['troops100']<100:break
            else:
                for _ in range(rand(10)+1 if mode=='charge' else 1):
                    if u['troops100']<100 or target['troops100']<100:break
                    losses.append(_exchange(s,b,u,target))
                if mode=='charge' and target['troops100']<100 and u['troops100']>=100:
                    previous=(u['q'],u['r'])
                    u['q'],u['r']=target['q'],target['r']
                    # Q07: an advancing charger is exposed to forest reactions
                    # from the entry direction, just like a normal hex step.
                    _forest_reaction(s,b,u,previous)
            fire=False
        u['acted']=True
        _sync(s,b)
        outcome=_result(s,b)
        if outcome:settle(s,outcome)
        return {'mode':mode,'officer':oid,'target':target['id'],'losses':losses,
                'ignited':fire,'resolved':s['battle'] is None}
    if kind!='battle_end_turn':fail('UNKNOWN_KIND')
    # Human defender acts manually; CPU invaders must get their own turn.
    # For CPU-only fixtures the legacy attacker-driven daily tick remains.
    _auto_ai(s,b,enemy_side)
    if p.get('_auto_player'): _auto_ai(s,b,player_side)
    _sync(s,b)
    outcome=_result(s,b)
    if outcome:settle(s,outcome);return {'resolved':True}
    _spontaneous_switches(s,b)
    outcome=_result(s,b)
    if outcome:settle(s,outcome);return {'resolved':True}
    b['rice_a']=core.rice_tick(b['rice_a'],sum(u['troops100'] for u in _all(b,'A')))
    # Q03: defender original slot 1 is the second side-0 list included in
    # the food tick even before entry. Arrived officers are already in _all.
    defender_second_list=sum(u['troops100'] for u in b['officer_units']
        if u['side']=='D' and u['slot']==1 and u.get('reserve')
        and not u.get('fled') and u['troops100']>=100)
    b['rice_d']=core.rice_tick(b['rice_d'],sum(u['troops100'] for u in _all(b,'D')),
                               defender_second_list)
    b['day']+=1
    _arrive_reinforcements(s,b)
    if b['day']%3==1:b['wind']=rand(7)
    if rand(100)<40:
        candidate=rand(4)
        if candidate!=b['weather']:
            b['weather']=weather_transition(b['weather'],candidate,rand(10))
    _spread_fire(s,b)
    for unit in b['officer_units']:
        unit['acted']=False
        unit['mp']=core.refill(unit['training'],unit['mp'])
    _sync(s,b)
    outcome=_result(s,b)
    if b['day']>30:outcome='DEFENDER'
    if outcome:settle(s,outcome)
    return {'day':b['day'],'resolved':s['battle'] is None}
