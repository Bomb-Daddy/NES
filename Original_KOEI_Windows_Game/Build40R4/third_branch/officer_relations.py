"""Additive KOEI-style personnel actions for the isolated Fork Four experiment.

Mechanic categories mirror ROTK II NES Reward (gold, horse, writings) and
Bandit Kings NES brotherhood, feast, and rest. Coefficients, BP costs,
compatibility and availability are NOT reconstructed ROM rules.

Fork Four directly appoints governors and controls owned provinces; Bandit
Kings brother-only prefecture permissions are deliberately NOT part of the design.
"""
from __future__ import annotations
from og.core.fixed import CoreError

POLICY='KOEI_OFFICER_RELATIONS_EXPERIMENTAL_V1_NON_PARITY'
PERSONNEL_COMMANDS={'reward_governor','reward_officer','buy_horse','appoint_advisor','swear_brotherhood','feast','rest'}
BP={'reward_governor':6,'reward_officer':6,'buy_horse':4,'appoint_advisor':0,
    'swear_brotherhood':8,'feast':0,'rest':0}

def fail(code):
    raise CoreError(code,code)

def ruler(s,f):
    slots=s.get('ruler_slots',{})
    if f in slots:return slots[f]
    # Synthetic fixture has no historical roster; convention is for fixture only.
    eligible=sorted(k for k,v in s['officer_faction'].items() if v==f)
    if not eligible:fail('NO_RULER')
    return f+'1' if f+'1' in eligible else eligible[0]

def governor(s,pid):
    return s.get('governors',{}).get(pid,{}).get('officer')

def authorized(s,actor,pid,f):
    return actor in (ruler(s,f),governor(s,pid))

def _local(s,target,pid,f):
    if target not in s['core']['officers']:fail('UNKNOWN_OFFICER')
    if s['officer_faction'].get(target)!=f:fail('FOREIGN_OFFICER')
    if s['core']['officers'][target]['province_id']!=pid:fail('NOT_LOCAL')

def _pay_bp(o,kind):
    n=BP[kind]
    if o['bp']<n:fail('NO_BP')
    o['bp']-=n

def _silver(s,pid,n):
    if s['core']['provinces'][pid]['treasury_gold_cents']<n:fail('NO_GOLD')
    s['core']['provinces'][pid]['treasury_gold_cents']-=n

def drop_binding(s,oid):
    """Remove vows and advisor posts when an officer is captured or changes faction."""
    for members in s.get('sworn_brothers',{}).values():
        if oid in members:members.remove(oid)
    for pid,v in list(s.get('appointed_advisors',{}).items()):
        if v==oid:del s['appointed_advisors'][pid]

def validate(s):
    from .integrated import integer
    for f,group in s.get('sworn_brothers',{}).items():
        if f not in s['factions'] or not isinstance(group,list) or len(group)!=len(set(group)) or len(group)>9:fail('BAD_BROTHERHOOD')
        for oid in group:
            if oid not in s['core']['officers'] or s['officer_faction'].get(oid)!=f or s['officer_loyalty'].get(oid)!=100:fail('BAD_BROTHERHOOD')
            if oid==ruler(s,f):fail('BAD_BROTHERHOOD')
    for pid,oid in s.get('appointed_advisors',{}).items():
        if pid not in s['owners'] or oid not in s['core']['officers']:fail('BAD_ADVISOR_POSTING')
        if s['officer_faction'].get(oid)!=s['owners'][pid] or s['core']['officers'][oid]['province_id']!=pid or s['core']['officers'][oid]['intelligence']<=80:fail('BAD_ADVISOR_POSTING')
    for pid,month in s.get('personnel_last_recovery_month',{}).items():
        if pid not in s['owners']:fail('BAD_RECOVERY_DATE')
        integer(month,1)
        if month>s['month']:fail('BAD_RECOVERY_DATE')
    for pid,n in s.get('reward_horses',{}).items():
        if pid not in s['owners']:fail('BAD_HORSE_INVENTORY')
        integer(n,0)
    return True

def perform(s,actor,kind,params):
    if kind not in PERSONNEL_COMMANDS:fail('UNKNOWN_PERSONNEL_COMMAND')
    o=s['core']['officers'][actor];pid=o['province_id'];f=s['officer_faction'][actor]
    if s['owners'][pid]!=f:fail('WRONG_OWNER')
    if not authorized(s,actor,pid,f):fail('REQUIRES_RULER_OR_GOVERNOR')
    sworn=s.setdefault('sworn_brothers',{})
    if kind=='swear_brotherhood':
        if actor!=ruler(s,f):fail('REQUIRES_RULER')
        target=params.get('target_officer');_local(s,target,pid,f)
        if target==actor:fail('SELF_BROTHERHOOD')
        if s['officer_loyalty'][target]<95:fail('INSUFFICIENT_LOYALTY')
        if target in sworn.get(f,[]):fail('ALREADY_SWORN')
        maximum=5 if s.get('bandit_difficulty')==5 else 9
        if len(sworn.get(f,[]))>=maximum:fail('BROTHERHOOD_FULL')
        _pay_bp(o,kind)
        sworn.setdefault(f,[]).append(target);sworn[f].sort()
        s['officer_loyalty'][target]=100
        return {'command':kind,'officer':target,'ruler':actor,'loyalty':100,
                'province_control':'UNCHANGED_DIRECT_APPOINTMENT','policy':POLICY}
    if kind=='appoint_advisor':
        if actor!=ruler(s,f):fail('REQUIRES_RULER')
        target=params.get('target_officer');_local(s,target,pid,f)
        if s['core']['officers'][target]['intelligence']<=80:fail('ADVISOR_INTELLIGENCE_TOO_LOW')
        _pay_bp(o,kind)
        s.setdefault('appointed_advisors',{})[pid]=target
        return {'command':kind,'advisor':target,'province':pid,'policy':POLICY}
    if kind=='buy_horse':
        units=params.get('units')
        if type(units) is not int or not 1<=units<=100:fail('BAD_QUANTITY')
        # ROTK II manual: 100 gold per horse. This is a temporary unified-market
        # specialty purchase: future quotation must use common market engine.
        cost=units*100*100
        if s['core']['provinces'][pid]['treasury_gold_cents']<cost:fail('NO_GOLD')
        _pay_bp(o,kind);_silver(s,pid,cost)
        horses=s.setdefault('reward_horses',{})
        horses[pid]=horses.get(pid,0)+units
        return {'command':kind,'province':pid,'horses_bought':units,'cost_cents':cost,
                'horses_after':horses[pid],'policy':POLICY}
    if kind in ('reward_officer','reward_governor'):
        if kind=='reward_governor':
            if actor!=ruler(s,f):fail('REQUIRES_RULER')
            target=params.get('target_governor')
            # Appointment, ownership and station are checked at execution time,
            # not merely when the list is displayed to the player.
            if target not in s['core']['officers']:fail('UNKNOWN_GOVERNOR')
            posts=[loc for loc,g in s.get('governors',{}).items()
                   if g.get('officer')==target and s['owners'].get(loc)==f]
            if not posts:fail('NOT_APPOINTED_GOVERNOR')
            if not any(s['core']['officers'][target]['province_id']==loc for loc in posts):fail('BAD_GOVERNOR_POSTING')
            if s['officer_faction'].get(target)!=f:fail('FOREIGN_GOVERNOR')
        else:
            target=params.get('target_officer');_local(s,target,pid,f)
        if target==actor:fail('SELF_REWARD')
        mode=params.get('method');val=params.get('gold',1)
        if mode not in (('gold','horse') if kind=='reward_governor' else ('gold','horse','writings')):fail('BAD_REWARD_METHOD')
        if mode=='gold':
            if type(val)is not int or not 1<=val<=100:fail('BAD_QUANTITY')
            cost=val*100
            if s['core']['provinces'][pid]['treasury_gold_cents']<cost:fail('NO_GOLD')
        elif mode=='horse':
            if s.get('reward_horses',{}).get(pid,0)<1:fail('NO_HORSE')
        else:
            mentor=s.get('appointed_advisors',{}).get(pid)
            if mentor is None:fail('NO_ADVISOR')
            if s['core']['officers'][mentor]['intelligence']-s['core']['officers'][target]['intelligence']<2:fail('INTELLIGENCE_GAP_TOO_SMALL')
        _pay_bp(o,kind)
        if mode=='writings':
            v=s['core']['officers'][target]
            v['intelligence']+=1
            return {'command':kind,'method':mode,'target_officer':target,
                    'intelligence_after':v['intelligence'],'loyalty_after':s['officer_loyalty'][target],
                    'policy':POLICY}
        if mode=='gold':_silver(s,pid,cost)
        else:s['reward_horses'][pid]-=1
        # Placeholder EFFECT, not an extracted ROTK II probability/formula.
        # Charm and military skill modulate benefit, gift has diminishing return.
        bonus=(max(1,val//5)+o['charisma']//25 if mode=='gold' else 10+s['core']['officers'][target]['war']//25)
        before=s['officer_loyalty'][target]
        s['officer_loyalty'][target]=min(100,before+bonus)
        return {'command':kind,'method':mode,'target_officer':target,
                'loyalty_before':before,'loyalty_after':s['officer_loyalty'][target],
                'source_province':pid, 'governor_province':s['core']['officers'][target]['province_id'] if kind=='reward_governor' else pid,
                'cost_cents':cost if mode=='gold' else 0,'policy':POLICY}
    if kind in ('feast','rest'):
        if s.get('personnel_last_recovery_month',{}).get(pid)==s['month']:
            fail('RECOVERY_ALREADY_USED')
        people=sorted(k for k,v in s['officer_faction'].items()
                      if v==f and s['core']['officers'][k]['province_id']==pid)
        if not people:fail('NO_ATTENDEES')
        cost=(10*len(people)*100 if kind=='feast' else 0)
        if cost and s['core']['provinces'][pid]['treasury_gold_cents']<cost:fail('NO_GOLD')
        _pay_bp(o,kind)
        if cost:_silver(s,pid,cost)
        s.setdefault('personnel_last_recovery_month',{})[pid]=s['month']
        recovered={};loyalty={}
        for oid in people:
            x=s['core']['officers'][oid]
            before_bp=x['bp'];x['bp']=min(200,x['bp']+(5 if kind=='feast' else 10))
            recovered[oid]=x['bp']-before_bp
            if kind=='feast' and oid!=ruler(s,f) and oid not in sworn.get(f,[]):
                before=s['officer_loyalty'][oid]
                s['officer_loyalty'][oid]=min(100,before+2)
                loyalty[oid]=s['officer_loyalty'][oid]-before
        return {'command':kind,'province':pid,'attendees':people,'cost_cents':cost,
                'body_points_recovered':recovered,'loyalty_gains':loyalty,'policy':POLICY}
    fail('UNKNOWN_PERSONNEL_COMMAND')
