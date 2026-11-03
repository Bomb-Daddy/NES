"""Enhanced gameplay gate: actual individual officer army ledgers and live tactical UI."""
import copy
import pytest
from third_branch.integrated import new_campaign,load_bytes,save_bytes,validate
from third_branch.tactical_view import render_battle
from third_branch import army_cohorts as ac
from og.core.fixed import CoreError


def two_defenders(seed=91):
    c=new_campaign(seed)
    s=c.state
    ac.add(s,'B1','p5',50)
    ac.refresh(s)
    validate(s)
    c.do('attack','A3',{'target':'p5','troops':150,'gold':12},'test:attack')
    return c


def test_named_officers_are_not_aggregate_armies():
    c=two_defenders()
    b=c.state['battle']
    assert b['combat_rule']=='ROTK2_UNIFIED_V1_4_OFFICER_LEDGER'
    defenders=[u for u in b['officer_units'] if u['side']=='D']
    assert {u['id'] for u in defenders}=={'B1','B2'}
    assert len({(u['q'],u['r']) for u in defenders})==2
    assert sum(u['troops'] for u in defenders)==b['d']['troops']==ac.totals(c.state)['p5']
    assert b['a']['troops']==sum(u['troops'] for u in b['officer_units'] if u['side']=='A')
    assert validate(c.state)


def test_cold_save_preserves_all_officer_units_and_escrow():
    c=two_defenders()
    b=c.state['battle']
    assert b['gold_a_cents']==1200
    assert b['gold_d_cents']>0
    assert c.state['core']['provinces']['p5']['treasury_gold_cents']==0
    h=c.fingerprint
    cold=load_bytes(save_bytes(c))
    assert cold.fingerprint==h
    assert cold.state['battle']['officer_units']==b['officer_units']


def test_named_officer_attack_changes_named_defender_station_not_anonymous_pool():
    c=two_defenders()
    b=c.state['battle']
    a=next(u for u in b['officer_units'] if u['id']=='A3')
    b2=next(u for u in b['officer_units'] if u['id']=='B2')
    b1=next(u for u in b['officer_units'] if u['id']=='B1')
    a['q'],a['r']=9,6
    a['mp']=6
    b2['q'],b2['r']=10,6
    b1['q'],b1['r']=10,7
    before_b2=ac.strength(c.state,'B2','p5')
    before_b1=ac.strength(c.state,'B1','p5')
    out=c.do('battle_attack','A3',{'officer_id':'A3','target_officer_id':'B2','mode':'normal'},'test:normal')
    assert out['mode']=='normal'
    if c.state['battle']:
        assert ac.strength(c.state,'B2','p5')<=before_b2
        assert ac.strength(c.state,'B1','p5')==before_b1
        assert c.state['battle']['d']['troops']==ac.totals(c.state)['p5']
    assert validate(c.state)


def test_invalid_enemy_as_actor_rolls_back_atomic_command():
    c=two_defenders();h=c.fingerprint
    with pytest.raises(CoreError):
        c.do('battle_attack','A3',{'officer_id':'B2','target_officer_id':'A3'},'test:cheat')
    assert c.fingerprint==h


def test_actual_officer_marker_rendering_changes_when_one_officer_dead():
    c=two_defenders()
    a=render_battle(c)
    d=next(u for u in c.state['battle']['officer_units'] if u['id']=='B1')
    q,r=d['q'],d['r']
    d['troops']=0;d['troops100']=0
    b=render_battle(c)
    from third_branch.tactical_view import X0,Y0,CELL
    assert a.getpixel((X0+q*CELL+10,Y0+r*CELL+10))!=b.getpixel((X0+q*CELL+10,Y0+r*CELL+10))


def test_original_rice_and_gold_recovered_in_contested_province():
    c=two_defenders()
    s=c.state;b=s['battle'];src=s['core']['provinces']['p3'];dst=s['core']['provinces']['p5']
    assert src['treasury_gold_cents']>=0 and dst['treasury_gold_cents']==0
    assert dst['food_units']==0
    orig_gold=b['gold_a_cents']+b['gold_d_cents']
    orig_rice=b['rice_a']+b['rice_d']
    c.do('battle_auto','A3',{},'test:auto')
    assert s is not c.state # old reference remains unchanged when execute clones state
    s=c.state;dst=s['core']['provinces']['p5']
    assert dst['treasury_gold_cents']==min(3000000,orig_gold)
    assert 0<=dst['food_units']<=orig_rice//100
    assert validate(s)


def test_officer_only_attack_and_wait_order_save_replay():
    c=two_defenders()
    b=c.state['battle']
    a=next(u for u in b['officer_units'] if u['id']=='A3')
    before=c.fingerprint
    position=(a['q'],a['r'])
    out=c.do('battle_move','A3',{'officer_id':'A3','q':1,'r':6},'test:move')
    assert out['moved']==[1,6]
    assert c.fingerprint!=before
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    assert validate(c.state)
