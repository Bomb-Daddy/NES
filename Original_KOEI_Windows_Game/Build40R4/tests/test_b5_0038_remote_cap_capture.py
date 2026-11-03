"""B5-0038: historical cap-aware remote-garrison capture planning.

This tests Enhanced campaign ledger safety, not claimed original ROM prisoner AI.
"""
from third_branch.historical_s1 import new_campaign_historical_s1
from third_branch import army_cohorts as ac
from third_branch.integrated import validate,save_bytes,load_bytes


def _historical_remote():
    c=new_campaign_historical_s1(3801,balance_policy='OG_1000_DIRECTOR_V2')
    s=c.state
    # Original historical R6 owns p30/p32/p33. Move (not create) 80 of
    # nes:006's 100 groups to the friendly distant p30 garrison.
    army=s['armies']['army:nes:006']
    army['stations']={'p33':20,'p30':80}
    ac.refresh(s)
    assert validate(s)
    return c


def test_cap_aware_planner_does_not_overfill_first_local_commander():
    c=_historical_remote();s=c.state
    before=c.fingerprint
    plan=ac.plan_remote_capture(s,'nes:006','R6','p33')
    assert plan==[('p30',80,'nes:131')]
    assert c.fingerprint==before
    assert load_bytes(save_bytes(c)).fingerprint==before
    assert sum(n for pid,n,off in plan)==80
    assert ac.army_total(s['armies']['army:nes:018'])+80>100
    assert ac.army_total(s['armies']['army:nes:131'])+80<=100


def test_no_remaining_capacity_means_escape_not_illegal_capture():
    c=_historical_remote();s=c.state
    # Fill every other R6 commander to the cap using stationed troops for
    # this planner-only fixture; never write to original save or claim a
    # production recruitment transaction.
    for oid in sorted(s['core']['officers']):
        if oid=='nes:006' or s['officer_faction'][oid]!='R6':continue
        army=ac.ensure(s,oid)
        have=ac.army_total(army)
        if have<100:
            pid=s['core']['officers'][oid]['province_id']
            army['stations'][pid]=army['stations'].get(pid,0)+(100-have)
    ac.refresh(s)
    assert validate(s)
    before=c.fingerprint
    assert ac.plan_remote_capture(s,'nes:006','R6','p33') is None
    assert c.fingerprint==before


def test_plan_never_assigns_remote_troops_to_enemy_or_besieged_officer():
    c=_historical_remote();s=c.state
    plan=ac.plan_remote_capture(s,'nes:006','R6','p33')
    assert all(s['officer_faction'][off]=='R6' for _,_,off in plan)
    assert all(s['core']['officers'][off]['province_id']!='p33' for _,_,off in plan)
    assert all(s['owners'][pid]=='R6' for pid,_,_ in plan)
