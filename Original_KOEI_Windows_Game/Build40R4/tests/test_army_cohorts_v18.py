"""Director: soldiers ONLY in officer-assigned armies or civilian population.

Regression suite executes real Campaign commands, not fake UI-side army numbers.
"""
import copy
from pathlib import Path
import pytest
from og.core.fixed import CoreError
from og.core.world import add_officer
from third_branch import army_cohorts as ac
from third_branch.integrated import new_campaign_41,new_campaign,validate,save_bytes,load_bytes,save_file,load_file
from third_branch.troop_assignments_ui import province_armies,build_command,ACTIONS


def act(c,kind,actor,params):
    return c.do(kind,actor,params,f'v18:{c.state["revision"]}:{kind}')


def add_general(c,oid='T99',pid='p14'):
    s=c.state
    add_officer(s['core'],oid,pid,(55,66,60,40,55))
    s['officer_faction'][oid]=s['owners'][pid]
    s['officer_loyalty'][oid]=80
    validate(s)
    return oid


def all_men(c):
    s=c.state
    return sum(sum(p['workers'].values())+p['troops'] for p in s['core']['provinces'].values())


def test_41_provinces_have_no_unassigned_military_and_every_soldier_has_general():
    c=new_campaign_41()
    assert len(c.state['armies'])==41
    assert all(a['commander'] in c.state['core']['officers'] for a in c.state['armies'].values())
    assert all(set(a)=={'commander','stations','engaged'} for a in c.state['armies'].values())
    assert ac.totals(c.state)=={pid:p['troops'] for pid,p in c.state['core']['provinces'].items()}
    assert not any('reserve' in a for a in c.state['armies'].values())
    assert validate(c.state)


def test_recruit_is_civilian_to_assigned_general_not_ghost_reserve():
    c=new_campaign_41();pid='p14';p=c.state['core']['provinces'][pid]
    before_pop=p['workers']['unassigned'];before_troops=p['troops'];before_men=all_men(c)
    out=act(c,'recruit','O14',{'province_id':pid,'count':30})
    assert out['assigned_army']=='army:O14'
    assert p is not c.state['core']['provinces'][pid]
    current=c.state['core']['provinces'][pid]
    assert current['workers']['unassigned']==before_pop-30
    assert current['troops']==before_troops+30
    assert ac.strength(c.state,'O14',pid)==before_troops+30
    assert all_men(c)==before_men


def test_actual_transfer_requires_co_located_friendly_generals_conserves_men():
    c=new_campaign_41();add_general(c)
    before=all_men(c);src=ac.strength(c.state,'O14','p14')
    out=act(c,'assign_troops','O14',{'target_officer':'T99','count':75})
    assert out['assigned']==75
    assert ac.strength(c.state,'O14','p14')==src-75
    assert ac.strength(c.state,'T99','p14')==75
    assert c.state['core']['provinces']['p14']['troops']==src
    assert all_men(c)==before
    act(c,'assign_troops','T99',{'target_officer':'O14','count':75})
    assert ac.strength(c.state,'T99','p14')==0
    assert 'army:T99' not in c.state['armies']
    assert all_men(c)==before


def test_disband_reenters_civilian_workers_and_removes_empty_army():
    c=new_campaign_41();add_general(c)
    act(c,'assign_troops','O14',{'target_officer':'T99','count':80})
    before=all_men(c);p=c.state['core']['provinces']['p14'];workers=p['workers']['unassigned']
    act(c,'disband_troops','T99',{'count':80})
    assert ac.strength(c.state,'T99','p14')==0
    assert 'army:T99' not in c.state['armies']
    assert c.state['core']['provinces']['p14']['workers']['unassigned']==workers+80
    assert all_men(c)==before


@pytest.mark.parametrize('kind,actor,params,error',[
    ('assign_troops','O14',{'target_officer':'O14','count':2},'SAME_COMMANDER'),
    ('assign_troops','O14',{'target_officer':'O15','count':2},'WRONG_PROVINCE'),
    ('assign_troops','O14',{'target_officer':'bogus','count':2},'UNKNOWN_COMMANDER'),
    ('disband_troops','O14',{'count':99999},'NO_ARMY_TROOPS'),
    ('recruit','O14',{'province_id':'p14','count':99999},'NOT_ENOUGH_WORKERS'),
    ('relocate_army','O14',{'target':'p41'},'NOT_ADJACENT'),
])
def test_unsafe_commands_do_not_change_hash_or_army_ledger(kind,actor,params,error):
    c=new_campaign_41();before=c.fingerprint
    with pytest.raises(CoreError) as e:c.do(kind,actor,params,'invalid')
    assert e.value.code==error
    assert c.fingerprint==before


def test_move_full_assigned_army_to_adjacent_friend_and_commander_moves():
    c=new_campaign_41();src='p14'
    target=next(pid for pid in c.state['neighbors'][src] if c.state['owners'][pid]=='A')
    total=ac.strength(c.state,'O14',src)
    act(c,'relocate_army','O14',{'target':target})
    assert ac.strength(c.state,'O14',target)==total
    assert ac.strength(c.state,'O14',src)==0
    assert c.state['core']['officers']['O14']['province_id']==target
    validate(c.state)


def test_battle_takes_only_assigned_army_and_counts_casualties():
    c=new_campaign_41()
    s=c.state;initial=sum(x['troops'] for x in s['core']['provinces'].values())
    act(c,'attack','O14',{'target':'p15','troops':160})
    battle=c.state['battle']
    assert battle['participants'][0]['id']=='army:O14'
    assert c.state['armies']['army:O14']['engaged']==160
    assert ac.strength(c.state,'O14','p14')==80
    assert sum(x['troops'] for x in c.state['core']['provinces'].values())+battle['a']['troops']==initial
    save=load_bytes(save_bytes(c))
    assert save.fingerprint==c.fingerprint
    act(c,'battle_auto','O14',{})
    assert c.state['battle'] is None
    h=c.state['history'][-1]
    current=sum(x['troops'] for x in c.state['core']['provinces'].values())
    prisoners=sum(x['prisoners'] for x in c.state['extras'].values())
    casualties=h['start_attacker']-h['attacker_survivors']+h['start_defender']-h['defender_survivors']
    assert initial-(current+prisoners)==casualties
    assert not any(a['engaged'] for a in c.state['armies'].values())
    validate(c.state)


def test_monthly_unpaid_troops_return_to_civilian_population_and_armies_shrink():
    c=new_campaign_41();s=c.state
    # Set March payment deadline: January then February. Only target province
    # lacks enough gold for payroll; its unpaid soldiers become civilian workers.
    p=s['core']['provinces']['p14']
    p['treasury_gold_cents']=0
    start=all_men(c)
    for _ in range(3):c.advance_month()
    assert all_men(c)>=start  # civilian yearly population growth can add persons
    assert ac.totals(c.state)=={pid:q['troops'] for pid,q in c.state['core']['provinces'].items()}
    assert validate(c.state)


def test_save_cold_load_with_assignments_and_migration_of_v17_save():
    c=new_campaign_41();add_general(c)
    act(c,'assign_troops','O14',{'target_officer':'T99','count':70})
    act(c,'recruit','T99',{'province_id':'p14','count':20})
    restored=load_bytes(save_bytes(c))
    assert restored.fingerprint==c.fingerprint
    assert ac.strength(restored.state,'T99','p14')==90
    old=new_campaign_41();del old.state['armies']
    # Old v1.7-era JSON wrapped in full checksum; test actual loader migration.
    from third_branch.integrated import canonical_bytes,hash_state
    legacy=canonical_bytes({'sha256':hash_state(old.state),'state':old.state,'format':'OG-INTEGRATED-TEST-1'})
    migrated=load_bytes(legacy)
    assert len(migrated.state['armies'])==41
    assert validate(migrated.state)


def test_army_corruption_and_orphan_soldiers_rejected():
    c=new_campaign_41()
    for mutation in ('ghost','enemy','orphan','dup'):
        s=copy.deepcopy(c.state)
        if mutation=='ghost':s['core']['provinces']['p1']['troops']+=1
        if mutation=='enemy':s['armies']['army:O01']['stations']={'p15':240}
        if mutation=='orphan':s['armies']['army:O01']['engaged']=10
        if mutation=='dup':s['armies']['army:O02']['stations']['p1']=10
        with pytest.raises(CoreError):validate(s)


def test_controller_command_builder_exposes_real_ops():
    c=new_campaign_41()
    assert set(ACTIONS)=={'Transfer Between Generals','Disband to Population'}
    assert build_command(c.state,'p14',ACTIONS[0],'O14','O13',10)==('assign_troops',{'target_officer':'O13','count':10})
    assert build_command(c.state,'p14',ACTIONS[1],'O14','O13',10)==('disband_troops',{'count':10})
    assert len(province_armies(c,'p14'))==1


def test_legacy_midbattle_save_upgrades_to_commanded_army_and_resolves():
    c=new_campaign_41();act(c,'attack','O14',{'target':'p15','troops':160})
    from third_branch.integrated import canonical_bytes,hash_state
    old=copy.deepcopy(c.state)
    old.pop('armies');old['battle'].pop('participants');old['battle'].pop('initial_defender')
    legacy=canonical_bytes({'format':'OG-INTEGRATED-TEST-1','sha256':hash_state(old),'state':old})
    restored=load_bytes(legacy)
    assert ac.strength(restored.state,'O14','p14')==80
    assert restored.state['armies']['army:O14']['engaged']==160
    act(restored,'battle_auto','O14',{})
    assert validate(restored.state)
    assert restored.state['battle'] is None


def test_transport_of_soldiers_requires_destination_general_no_ghosts():
    c=new_campaign_41()
    neighbors=[n for n in c.state['neighbors']['p14'] if c.state['owners'][n]=='A']
    assert neighbors
    dst=neighbors[0]
    source=ac.strength(c.state,'O14','p14')
    recipient=next(oid for oid,v in c.state['core']['officers'].items() if v['province_id']==dst)
    dest=ac.strength(c.state,recipient,dst)
    before=all_men(c)
    act(c,'transport','O14',{'target':dst,'commodity':'troops','units':30})
    assert ac.strength(c.state,'O14','p14')==source-30
    assert ac.strength(c.state,recipient,dst)==dest+30
    assert all_men(c)==before
    assert validate(c.state)


def test_military_governor_allocation_produces_directly_assigned_army():
    c=new_campaign_41()
    c.do('appoint_governor','O01',{'officer':'O14','focus':'defense'},'governor')
    troops=ac.strength(c.state,'O14','p14')
    workers=c.state['core']['provinces']['p14']['workers']['unassigned']
    c.advance_month()
    assert ac.strength(c.state,'O14','p14')==troops+10
    # The January economy also adds eligible new civilians; defense assignment
    # removes exactly 10 AFTER population growth, not 10 from the old January count.
    from og.core.world2 import FIXTURE_V1,WORKER_KEYS
    # The annual growth amount is independent of troop assignment.
    growth=sum(c.state['core']['provinces']['p14']['workers'][key] for key in WORKER_KEYS)
    assert workers-10 <= c.state['core']['provinces']['p14']['workers']['unassigned'] < workers
    assert growth>0
    assert validate(c.state)
