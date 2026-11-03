"""Q03 original side-0 extra list food: queued defenders consume before entry.

A real Q09 defender reserve is put in its own original slot 1. The source
battle food tick includes that second defender list, but not the attacking
slot 3; the latter is a negative control. This is port behavior, not a claim
of frame-exact emulator replay.
"""
from third_branch.integrated import new_campaign,load_bytes,save_bytes,validate
from third_branch import officer_battle_adapter as ba
from third_branch import rotk2_battle_core as core
from third_branch import army_cohorts as ac
from og.core.world import add_officer


def test_defender_reserve_is_counted_before_arrival_and_no_double_count_after():
    c=new_campaign(3803);s=c.state
    s['owners']['p6']='B';s['officer_faction']['C1']='B'
    add_officer(s['core'],'A4','p3',(55,65,70,60,70))
    s['officer_faction']['A4']='A';s['officer_loyalty']['A4']=72
    ac.add(s,'A4','p3',45);ac.refresh(s)
    assert validate(s)
    c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'q03:attack')
    s=c.state;b=s['battle']
    ba._queue_reinforcements(s,b,{'source':'p6','officer_ids':['C1'],'rice':0,'gold':0,'arrival_day':3},side='D')
    c.do('battle_reinforce','A3',{'source':'p3','officer_ids':['A4'],'rice':0,'gold':0,'arrival_day':3},'q03:attackerqueue')
    b=c.state['battle']
    active_d=sum(u['troops100'] for u in ba._all(b,'D'))
    active_a=sum(u['troops100'] for u in ba._all(b,'A'))
    reserve_d=sum(u['troops100'] for u in b['officer_units'] if u['side']=='D' and u.get('reserve'))
    reserve_a=sum(u['troops100'] for u in b['officer_units'] if u['side']=='A' and u.get('reserve'))
    assert reserve_d>0 and reserve_a>0
    before_d=b['rice_d'];before_a=b['rice_a']
    c.do('battle_end_turn','A3',{},'q03:tick')
    b=c.state['battle'];assert b is not None
    assert b['rice_d']==core.rice_tick(before_d,active_d,reserve_d)
    assert b['rice_a']==core.rice_tick(before_a,active_a)
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
