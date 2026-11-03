"""Q10 player bribe: battle-pool spending, named army side transfer, cold load and rollback."""
import pytest
from third_branch.integrated import new_campaign, validate, save_bytes, load_bytes
from third_branch import officer_battle_adapter as ba
from third_branch import army_cohorts as ac
from og.core.fixed import CoreError


def make_battle():
    c=new_campaign(210)
    ac.add(c.state,'B1','p5',50)
    ac.refresh(c.state)
    c.do('attack','A3',{'target':'p5','troops':150,'gold':99},'q10:start')
    return c


def test_failed_bribe_spends_gold_and_actor_turn_without_changing_army():
    c=make_battle();b=c.state['battle'];old=b['gold_a_cents']
    before=ac.strength(c.state,'B2','p5')
    result=c.do('battle_bribe','A3',{'officer_id':'A3','target_officer_id':'B2','gold':1},'q10:refuse')
    assert result['success'] is False
    assert c.state['battle']['gold_a_cents']==old-100
    assert ac.strength(c.state,'B2','p5')==before
    assert ba.find(c.state['battle'],'A','A3')['acted']
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_return_to_original_lord_switches_named_army_without_troop_creation():
    c=make_battle();b=c.state['battle']
    target=ba.find(b,'D','B2')
    target['original_faction']='A'  # source-documented automatic return path
    total_before=sum(ac.army_total(a) for a in c.state['armies'].values())
    strength=target['troops']
    result=c.do('battle_bribe','A3',{'officer_id':'A3','target_officer_id':'B2','gold':1},'q10:return')
    assert result['success'] is True
    b=c.state['battle'];target=ba.find(b,'A','B2')
    assert target and target['slot']==2 and target['switched']
    assert c.state['officer_faction']['B2']=='A'
    assert c.state['officer_loyalty']['B2']==100
    assert c.state['armies']['army:B2']['engaged']==strength
    assert c.state['armies']['army:B2']['stations'].get('p5',0)==0
    assert sum(ac.army_total(a) for a in c.state['armies'].values())==total_before
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_commander_bribe_is_rejected_with_atomic_rollback():
    c=make_battle();before=c.fingerprint
    with pytest.raises(CoreError):
        c.do('battle_bribe','A3',{'officer_id':'A3','target_officer_id':'B1','gold':20},'q10:commander')
    assert c.fingerprint==before


def test_daily_low_loyalty_switch_is_once_only_and_conserves_armies():
    from unittest.mock import patch
    c=make_battle();s=c.state;s['officer_loyalty']['B2']=0
    initial=sum(ac.army_total(a) for a in s['armies'].values())
    from itertools import cycle
    # Preview and commit each run on a distinct deep-copied world.
    # Keep the Q10 scripted draws independent of subsequent Q08 weather
    # draws; the former shared cycle accidentally relied on an old RNG count.
    rolls_by_world={}
    def rng(state,n):
        key=id(state)
        if key not in rolls_by_world:
            rolls_by_world[key]=(state,cycle((0,99,0,0,0)))
        return next(rolls_by_world[key][1])%n
    with patch.object(ba,'_auto_ai',return_value=None),patch.object(ba,'randrange',side_effect=rng):
        c.do('battle_end_turn','A3',{},'q10:day1')
    b=c.state['battle']
    assert ba.find(b,'A','B2') is not None
    assert c.state['officer_loyalty']['B2']==90
    assert sum(ac.army_total(a) for a in c.state['armies'].values())==initial
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_bribe_reassigns_distant_garrison_to_real_same_faction_officer():
    c=make_battle();s=c.state
    ac.add(s,'B2','p4',15)
    ac.refresh(s)
    assert validate(s)
    b=s['battle'];ba.find(b,'D','B2')['original_faction']='A'
    before=ac.strength(s,'B1','p4')
    total=sum(ac.army_total(a) for a in s['armies'].values())
    out=c.do('battle_bribe','A3',{'officer_id':'A3','target_officer_id':'B2','gold':1},'q10:remote')
    assert out['success']
    assert ac.strength(c.state,'B1','p4')==before+15
    assert ac.strength(c.state,'B2','p4')==0
    assert sum(ac.army_total(a) for a in c.state['armies'].values())==total
    assert validate(c.state)


def test_bribe_remote_unstaffed_garrison_rolls_back_all_mutations():
    c=make_battle();s=c.state
    ac.add(s,'B2','p4',15)
    # B1's real post moves to p5, so nobody can inherit p4's 15 soldiers.
    s['core']['officers']['B1']['province_id']='p5'
    ac.refresh(s)
    assert validate(s)
    ba.find(s['battle'],'D','B2')['original_faction']='A'
    before=c.fingerprint
    with pytest.raises(CoreError) as ex:
        c.do('battle_bribe','A3',{'officer_id':'A3','target_officer_id':'B2','gold':1},'q10:remote_fail')
    assert ex.value.code=='REMOTE_ARMY_REASSIGNMENT_REQUIRED'
    assert c.fingerprint==before


def test_bribed_officer_survives_victory_at_contested_province_after_cold_load():
    c=make_battle();s=c.state
    ba.find(s['battle'],'D','B2')['original_faction']='A'
    c.do('battle_bribe','A3',{'officer_id':'A3','target_officer_id':'B2','gold':1},'q10:win_bribe')
    c=load_bytes(save_bytes(c))
    c.state['battle']['rice_d']=0
    out=c.do('battle_end_turn','A3',{},'q10:win_settle')
    assert out['resolved'] and c.state['battle'] is None
    assert c.state['owners']['p5']=='A'
    assert c.state['officer_faction']['B2']=='A'
    assert c.state['core']['officers']['B2']['province_id']=='p5'
    assert ac.strength(c.state,'B2','p5')>0
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
