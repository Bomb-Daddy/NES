"""Persistent army ledgers for the original game. No unassigned military reserves.

An army belongs to exactly one commander; stationed detachments may occupy different
friendly provinces but remain part of that one assigned army. A battle field
`engaged` is an army's committed attacker troops (not an unassigned cohort).
The provincial `troops` number is a derived/indexed total of `stations` only.

The economy still tracks eligibility via civilian worker buckets; all recruitment
must move people from that population directly into an identified army.
"""
from __future__ import annotations
from og.core.fixed import CoreError


def fail(code):
    raise CoreError(code, code)


def army_id(commander):
    return 'army:'+commander


def bootstrap(s):
    """One-time migration of test scenario's old numeric troops to known officers.

    This is not a reserve: every old troop is assigned immediately to an officer.
    Migration rejects any province with soldiers and no legal commanding officer.
    """
    s['armies']={}
    for pid,p in sorted(s['core']['provinces'].items()):
        if not p['troops']:continue
        owned=s['owners'][pid]
        commanders=sorted(o for o,v in s['core']['officers'].items()
                          if v['province_id']==pid and s['officer_faction'][o]==owned)
        if not commanders:fail('ARMY_REQUIRES_COMMANDER')
        officer=commanders[0]
        s['armies'][army_id(officer)]={'commander':officer,'stations':{pid:p['troops']},'engaged':0}
    if s.get('battle'):hydrate_legacy_battle(s)
    validate_armies(s)


def ensure(s, commander):
    if commander not in s['core']['officers']:fail('UNKNOWN_COMMANDER')
    aid=army_id(commander)
    if aid not in s['armies']:
        s['armies'][aid]={'commander':commander,'stations':{},'engaged':0}
    return s['armies'][aid]


def strength(s,commander,pid):
    return s['armies'].get(army_id(commander),{}).get('stations',{}).get(pid,0)


# ROTK II historical cap is PER GENERAL across all stations and an active
# engagement, not per province. Synthetic 7/41-province legacy fixtures retain
# their separate test-only army sizing and cannot prove original-game parity.
ROTK2_MAX_TROOP_GROUPS = 100


def army_total(army):
    return sum(army['stations'].values()) + army['engaged']


def capacity_available(s,commander,number):
    if s.get('source_label') != 'ORIGINAL_GAME_HISTORICAL_S1_PROPOSED':
        return True
    a=s['armies'].get(army_id(commander))
    return (army_total(a) if a else 0)+number <= ROTK2_MAX_TROOP_GROUPS


def check_capacity(s,commander,number):
    if not capacity_available(s,commander,number):fail('GENERAL_TROOP_CAP')


def plan_remote_capture(s,commander,defender,besieged):
    """Pure, cap-aware plan for a defeated officer's distant garrisons.

    Returns whole-detachment (province, groups, replacement officer) tuples,
    or None when there is no legal replacement. No soldiers are created,
    destroyed, moved or reassigned by planning. The current interim policy
    prefers a physically present friendly commander; original-ROM prisoner
    disposition parity is still unverified.
    """
    army=s['armies'].get(army_id(commander))
    if army is None:return []
    shadow={oid:army_total(a) for oid,a in
            ((a['commander'],a) for a in s['armies'].values())}
    plan=[]
    for pid,n in sorted(army['stations'].items()):
        if pid==besieged or not n:continue
        if s['owners'].get(pid)!=defender:return None
        peers=sorted((oid for oid,o in s['core']['officers'].items()
                      if oid!=commander and s['officer_faction'].get(oid)==defender
                      and s['owners'].get(o['province_id'])==defender
                      and o['province_id']!=besieged),
                     key=lambda oid:(s['core']['officers'][oid]['province_id']!=pid,oid))
        target=next((oid for oid in peers
                     if s.get('source_label')!='ORIGINAL_GAME_HISTORICAL_S1_PROPOSED'
                     or shadow.get(oid,0)+n<=ROTK2_MAX_TROOP_GROUPS),None)
        if target is None:return None
        plan.append((pid,n,target))
        shadow[target]=shadow.get(target,0)+n
    return plan


def add(s,commander,pid,number):
    if type(number) is not int or number<=0:fail('BAD_TROOPS')
    if s['officer_faction'][commander]!=s['owners'][pid]:fail('WRONG_OWNER')
    check_capacity(s,commander,number)
    army=ensure(s,commander)
    army['stations'][pid]=army['stations'].get(pid,0)+number
    return army['stations'][pid]


def take(s,commander,pid,number):
    if type(number) is not int or number<=0:fail('BAD_TROOPS')
    army=s['armies'].get(army_id(commander))
    if army is None or army['stations'].get(pid,0)<number:fail('NO_ARMY_TROOPS')
    left=army['stations'][pid]-number
    if left:army['stations'][pid]=left
    else:del army['stations'][pid]
    cleanup(s)
    return left


def cleanup(s):
    for aid,army in list(s['armies'].items()):
        army['stations']={pid:n for pid,n in army['stations'].items() if n}
        if not army['stations'] and not army.get('engaged',0):del s['armies'][aid]


def totals(s):
    out={pid:0 for pid in s['core']['provinces']}
    for army in s['armies'].values():
        for pid,n in army['stations'].items():out[pid]+=n
    return out


def reconcile_unit_weapons(s):
    """Lose equipped weapons when soldiers die; never invent equipment.

    Equipping and proportional handling are enhanced adapters pending full NES
    source testing; original +14 bytes must not be treated as per-soldier objects.
    """
    equipped=s.get('unit_armed_men')
    if equipped is None:return
    for oid,have in equipped.items():
        a=s['armies'].get(army_id(oid))
        men=100*army_total(a) if a else 0
        if have>men:
            equipped[oid]=men
            s['unit_arms_casualty_losses']=s.get('unit_arms_casualty_losses',0)+(have-men)


def refresh(s):
    reconcile_unit_weapons(s)
    for pid,n in totals(s).items():s['core']['provinces'][pid]['troops']=n

def lose_at(s,pid,n):
    """Apply casualties or starvation in deterministic army-ID order."""
    if type(n) is not int or n<0:fail('BAD_TROOPS')
    if totals(s)[pid]<n:fail('NO_ARMY_TROOPS')
    left=n
    for aid,a in sorted(s['armies'].items()):
        amount=min(left,a['stations'].get(pid,0))
        if amount:
            v=a['stations'][pid]-amount
            if v:a['stations'][pid]=v
            else:del a['stations'][pid]
            left-=amount
        if not left:break
    cleanup(s)
    refresh(s)


def transfer(s,source,target,pid,n):
    if source==target:fail('SAME_COMMANDER')
    if s['core']['officers'][source]['province_id']!=pid or s['core']['officers'][target]['province_id']!=pid:fail('WRONG_PROVINCE')
    if s['officer_faction'][source]!=s['officer_faction'][target] or s['officer_faction'][source]!=s['owners'][pid]:fail('WRONG_OWNER')
    check_capacity(s,target,n)
    units=s.get('unit_armed_men')
    before_total=army_total(s['armies'][army_id(source)])
    transferable=min(units.get(source,0),units.get(source,0)*n//before_total) if units else 0
    take(s,source,pid,n)
    add(s,target,pid,n)
    if units is not None:
        units[source]-=transferable
        units[target]=units.get(target,0)+transferable
    refresh(s)


def validate_armies(s):
    armies=s.get('armies')
    if not isinstance(armies,dict):fail('MISSING_ARMY_LEDGER')
    for aid,a in armies.items():
        if not isinstance(a,dict) or set(a)!={'commander','stations','engaged'}:fail('BAD_ARMY_SHAPE')
        oid=a['commander']
        if aid!=army_id(oid) or oid not in s['core']['officers'] or s['officer_faction'].get(oid) not in s['factions']:fail('BAD_COMMANDER')
        if not isinstance(a['stations'],dict):fail('EMPTY_ARMY')
        if not a['stations'] and not a['engaged'] and not (s.get('battle') and any(p['id']==aid for p in s['battle'].get('participants',[]))):fail('EMPTY_ARMY')
        if type(a['engaged']) is not int or a['engaged']<0:fail('BAD_ENGAGED')
        if s.get('source_label')=='ORIGINAL_GAME_HISTORICAL_S1_PROPOSED' and army_total(a)>ROTK2_MAX_TROOP_GROUPS:fail('GENERAL_TROOP_CAP')
        for pid,n in a['stations'].items():
            if pid not in s['owners'] or type(n) is not int or n<1:fail('BAD_STATION')
            if s['owners'][pid]!=s['officer_faction'][oid]:fail('ENEMY_ARMY_STATION')
    if any(s['core']['provinces'][pid]['troops']!=n for pid,n in totals(s).items()):fail('GHOST_TROOPS')
    battle=s.get('battle')
    if not battle:
        if any(a['engaged'] for a in armies.values()):fail('ORPHAN_ENGAGEMENT')
    else:
        if 'participants' not in battle:fail('MISSING_BATTLE_ARMIES')
        names={p['id'] for p in battle['participants']}
        if len(names)!=len(battle['participants']):fail('DUPLICATE_BATTLE_ARMY')
        for p in battle['participants']:
            if p['id'] not in armies or armies[p['id']]['engaged']!=p['troops']:fail('BATTLE_ARMY_MISMATCH')
        if sum(p['troops'] for p in battle['participants'])!=battle['a']['troops']:fail('BATTLE_STRENGTH_MISMATCH')
        # Defender military remains stationed at the besieged province and loses
        # headcount as tactical combat reduces its strength.
        if totals(s)[battle['dst']]!=battle['d']['troops']:fail('DEFENDER_STRENGTH_MISMATCH')
    return True


def reduce_attackers(s,b,new_strength):
    """Distribute attacker combat losses among participating army IDs deterministically."""
    participants=b['participants']
    old=sum(p['troops'] for p in participants)
    if not 0<=new_strength<=old:fail('BAD_BATTLE')
    survivors=new_strength
    for i,p in enumerate(participants):
        n=survivors if i==len(participants)-1 else new_strength*p['troops']//max(1,old)
        p['troops']=n;survivors-=n
        s['armies'][p['id']]['engaged']=n
    b['a']['troops']=new_strength
    reconcile_unit_weapons(s)


def reduce_defenders(s,b,new_strength):
    now=s['core']['provinces'][b['dst']]['troops']
    if not 0<=new_strength<=now:fail('BAD_BATTLE')
    lose_at(s,b['dst'],now-new_strength)
    b['d']['troops']=new_strength


def hydrate_legacy_battle(s):
    """Upgrade a v1.7 mid-battle save with no separate army IDs.

    Finds the *recorded* attack command rather than inventing a commander. The
    old v1.7 tactical defender number could diverge from its province total;
    that delta is casualties and must be removed from the assigned armies.
    """
    b=s['battle']
    if 'participants' in b:return
    attacks=[event for event in s['log'] if event.get('kind')=='attack'
             and event.get('result',{}).get('from')==b['src']
             and event.get('result',{}).get('to')==b['dst']]
    if not attacks:fail('MIGRATION_REQUIRES_ATTACK_JOURNAL')
    actor=attacks[-1]['actor']
    if actor not in s['core']['officers'] or s['officer_faction'][actor]!=b['attacker']:
        fail('MIGRATION_UNKNOWN_COMMANDER')
    initial=b['committed_troops']+b['ally_support']
    total=b['a']['troops']
    if not 0<=total<=initial:fail('BAD_BATTLE')
    ally_home=b.get('ally_home')
    ally=None
    if ally_home and b['ally_support']:
        commanders=sorted(oid for oid,o in s['core']['officers'].items()
                          if o['province_id']==ally_home and s['officer_faction'][oid]==s['owners'][ally_home])
        if not commanders:fail('MIGRATION_REQUIRES_ALLY_COMMANDER')
        ally=commanders[0]
    ally_survivors=total*b['ally_support']//max(1,initial) if ally else 0
    main_survivors=total-ally_survivors
    participants=[{'id':army_id(actor),'troops':main_survivors,'home':b['src']}]
    ensure(s,actor)['engaged']=main_survivors
    if ally:
        participants.append({'id':army_id(ally),'troops':ally_survivors,'home':ally_home})
        ensure(s,ally)['engaged']=ally_survivors
    b['participants']=participants
    b['initial_defender']=s['core']['provinces'][b['dst']]['troops']
    after=b['d']['troops']
    if not 0<=after<=b['initial_defender']:fail('BAD_BATTLE')
    lose_at(s,b['dst'],b['initial_defender']-after)
