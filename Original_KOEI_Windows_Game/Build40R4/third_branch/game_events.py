"""Versioned original-branch event mechanics (not claims of NES mechanics parity).

A deterministic campaign event occurs only on monthly simulation or an actual
courier/diplomatic command.  It commits an observable change to genuine saved
state, with rollback under Campaign's atomic save/validation boundary.
"""
from __future__ import annotations
import hashlib

POLICY = 'OG_EVENTS_V1_PROVISIONAL'
ENVIRONMENTAL = ('locust_plague', 'typhoon', 'flood', 'epidemic',
                 'riot', 'rebellion', 'falling_star')


def roll(seed, *parts):
    data = '|'.join(map(str, (seed,)+parts)).encode('utf8')
    return int.from_bytes(hashlib.sha256(data).digest()[:8], 'big')


def append_record(s, kind, province, effects, *, text='', actor=None):
    """All presented events are real committed records, never UI-only guesses."""
    event_id='event:'+str(s['month'])+':'+str(len(s.get('event_history',[])))+':'+kind
    entry={'id':event_id,'kind':kind,'actor':actor,'month':s['month'],
           'province_id':province,'result':{'effects':effects,'text':text},'event_policy':POLICY}
    s.setdefault('event_history',[]).append(entry)
    s.setdefault('event_feed',[]).append(entry)
    s['revision']+=1
    s['processed'].append(event_id)
    s['log'].append(entry)
    return entry


def apply_environmental(s, kind, pid):
    """State change is the underlying trigger; image/sound merely describes it."""
    if kind not in ENVIRONMENTAL or pid not in s['owners'] or s['owners'][pid] is None:
        raise ValueError('Environmental events require a valid governed province')
    p=s['core']['provinces'][pid]
    before={key:p[key] for key in ('food_units','treasury_gold_cents','support_ticks','flood_control')}
    if kind=='locust_plague':
        p['food_units']=max(0,p['food_units']-max(1,p['food_units']//10))
        p['support_ticks']=max(0,p['support_ticks']-20000)
    elif kind=='typhoon':
        p['food_units']=max(0,p['food_units']-max(1,p['food_units']//14))
        p['flood_control']=max(0,p['flood_control']-10)
        p['support_ticks']=max(0,p['support_ticks']-20000)
    elif kind=='flood':
        p['food_units']=max(0,p['food_units']-max(1,p['food_units']//8))
        p['flood_control']=max(0,p['flood_control']-20)
        p['support_ticks']=max(0,p['support_ticks']-30000)
    elif kind=='epidemic':
        # Civilian workforce loses some available labor. No government-food charge.
        unassigned=p['workers']['unassigned']
        p['workers']['unassigned']=max(0,unassigned-max(1,unassigned//50))
        p['support_ticks']=max(0,p['support_ticks']-25000)
    elif kind=='riot':
        p['support_ticks']=max(0,p['support_ticks']-30000)
        p['treasury_gold_cents']=max(0,p['treasury_gold_cents']-min(1200,p['treasury_gold_cents']//50))
    elif kind=='rebellion':
        # Unrest reduces support and disrupts revenue; it does not conjure a faction.
        p['support_ticks']=max(0,p['support_ticks']-60000)
        p['treasury_gold_cents']=max(0,p['treasury_gold_cents']-min(2500,p['treasury_gold_cents']//30))
    elif kind=='falling_star':
        p['support_ticks']=max(0,p['support_ticks']-5000)
    effects={key:p[key]-before[key] for key in before if p[key]!=before[key]}
    if kind=='epidemic':effects['unassigned_workers_lost']=unassigned-p['workers']['unassigned']
    # Do not assert an outcome that did not occur (e.g. zero-stock food loss).
    return append_record(s,kind,pid,effects,text=kind.replace('_',' ').capitalize())


def eligible(s, kind, pid, month):
    p=s['core']['provinces'][pid]
    season=((month-1)//3)%4
    if kind=='typhoon':return season in (1,2) and int(pid[1:])>=18
    if kind=='flood':return season in (1,2) and p['flood_control']<900
    if kind=='locust_plague':return season in (1,2) and p['workers']['farmers']>0
    if kind=='riot':return p['support_ticks']<3000000
    if kind=='rebellion':return p['support_ticks']<2250000
    return True


def apply_ruler_death(s, faction):
    """Actual ruler demise with a replacement officer and army continuity.

    Never substitutes faction elimination for death. A ruler cannot die under
    this provisional random-mortality system without a living successor.
    """
    from . import army_cohorts as ac
    if faction not in s['factions'] or not s['factions'][faction]['alive']:
        raise ValueError('No living ruler for faction')
    old=s['ruler_slots'][faction]
    if old not in s['core']['officers']:
        raise ValueError('Ruler not active')
    heirs=[k for k,f in s['officer_faction'].items() if f==faction and k!=old and k in s['core']['officers']]
    if not heirs:
        raise ValueError('Cannot remove last active officer')
    heirs.sort(key=lambda k:(-(s['core']['officers'][k]['leadership']+
                              s['core']['officers'][k]['politics']+s['officer_loyalty'][k]//2),k))
    heir=heirs[0]
    pid=s['core']['officers'][old]['province_id']
    oldname=s['officer_catalog'][old]['name'];newname=s['officer_catalog'][heir]['name']
    # Preserve troops on ruler death WITHOUT violating ROTK II's 100-group
    # individual unit limit. Deterministic capacity-aware redistribution among
    # existing living officers only; no created garrison or new officer.
    oldarmy=s['armies'].get(ac.army_id(old))
    candidates=[heir]+[oid for oid in sorted(heirs) if oid!=heir]
    needed=ac.army_total(oldarmy) if oldarmy else 0
    if oldarmy and oldarmy['engaged']:
        raise ValueError('Cannot process ruler succession inside active battle')
    total_capacity=sum(max(0,ac.ROTK2_MAX_TROOP_GROUPS-(ac.army_total(s['armies'][ac.army_id(oid)]) if ac.army_id(oid) in s['armies'] else 0)) for oid in candidates)
    if oldarmy and total_capacity<needed:
        raise ValueError('No lawful existing-commanders for deceased ruler army')
    old_weapons=s.get('unit_armed_men',{}).get(old,0)
    assigned=[]
    if oldarmy:
        s['armies'].pop(ac.army_id(old))
        for station,station_total in sorted(oldarmy['stations'].items()):
            left=station_total
            for oid in candidates:
                if not left:break
                present=s['armies'].get(ac.army_id(oid))
                free=ac.ROTK2_MAX_TROOP_GROUPS-(ac.army_total(present) if present else 0)
                n=min(left,free)
                if n:
                    ac.add(s,oid,station,n)
                    assigned.append((oid,station,n))
                    left-=n
            if left:raise ValueError('Ruler army reassignment capacity changed')
        # Inherits raw material weapons proportionally (a provisional Enhanced
        # reconciliation policy; original NES transfer rounding remains open).
        available=old_weapons
        for i,(recipient,station,n) in enumerate(assigned):
            weapons=available if i==len(assigned)-1 else old_weapons*n//max(1,needed)
            weapons=min(available,weapons)
            s.setdefault('unit_armed_men',{})[recipient]=s['unit_armed_men'].get(recipient,0)+weapons
            available-=weapons
    s.get('unit_training',{}).pop(old,None)
    s.get('unit_armed_men',{}).pop(old,None)
    for p,g in list(s['governors'].items()):
        if g['officer']==old:s['governors'].pop(p)
    s['core']['officers'].pop(old)
    s['officer_faction'].pop(old)
    s['officer_loyalty'].pop(old)
    from .officer_relations import drop_binding
    drop_binding(s,old)
    drop_binding(s,heir)  # A newly promoted ruler cannot remain a sworn subordinate.
    s['officer_availability'][old]='Deceased'
    s['ruler_slots'][faction]=heir
    s['factions'][faction]['name']=newname
    ac.cleanup(s);ac.refresh(s)
    return append_record(s,'ruler_died',pid,
        {'deceased_officer':old,'successor_officer':heir,'troops_unchanged':True},
        text=oldname+' has died. '+newname+' succeeds as ruler.')


def process_month(s, month):
    """No more than one province hazard per month; deterministic, bounded.

    This is an explicit provisional scenario tuning, not reverse-engineered NES
    probabilities. It is optional for older saves via the policy gate.
    """
    if s.get('source_label')!='ORIGINAL_GAME_HISTORICAL_S1_PROPOSED':return []
    # Full history persisted; latest month's feed is replaced each turn.
    s['event_feed']=[]
    seed=s['core'].get('rng_state',7)
    # Rare mortality is independent of the ordinary provincial disaster gate.
    # No birth-year assumptions, deterministic and always requires succession.
    for faction in sorted(s['factions']):
        if (s['factions'][faction]['alive'] and
            roll(seed,month,faction,'ruler_mortality')%20000 < 7):
            try:return [apply_ruler_death(s,faction)]
            except ValueError:pass
    if roll(seed,month,'event_gate')%100 >= 36:return []
    live=sorted(pid for pid,owner in s['owners'].items() if owner is not None)
    if not live:return []
    pid=live[roll(seed,month,'province')%len(live)]
    order=('locust_plague','typhoon','flood','epidemic','riot','rebellion','falling_star')
    for j in range(len(order)):
        kind=order[(roll(seed,month,'kind')+j)%len(order)]
        if eligible(s,kind,pid,month):
            return [apply_environmental(s,kind,pid)]
    return []


def process_command(s, cmd, result, pid):
    """Courier pictures display only when actual eligible commands reach an outcome."""
    if s.get('source_label')!='ORIGINAL_GAME_HISTORICAL_S1_PROPOSED':return []
    kind=cmd['kind']
    if kind in ('Ally','Joint'):
        if result.get('intercepted'):
            return [append_record(s,'messenger_caught_secret_letter',pid,
                    {'diplomatic_contact_blocked':True},text='Secret letter intercepted',actor=cmd['actor'])]
        if result.get('accepted'):
            return [append_record(s,'secret_letter_delivered',pid,
                    {'diplomatic_contact_accepted':True},text='Secret letter delivered',actor=cmd['actor'])]
    if kind=='transport' and result.get('intercepted') and result.get('commodity') in ('gold','food'):
        return [append_record(s,'messenger_caught_gold_rice',pid,
                 {'lost_units':result.get('lost_units',0),'commodity':result['commodity']},
                 text='Supply courier intercepted',actor=cmd['actor'])]
    return []
