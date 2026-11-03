"""Q09 executable officer-level campaign ledger tests, not ROM acceptance."""
import pytest
from og.core.world import add_officer
from og.core.fixed import CoreError
from third_branch.integrated import new_campaign, save_bytes, load_bytes, validate
from third_branch import army_cohorts as ac
from third_branch import officer_battle_adapter as ba


def started():
    c=new_campaign(1717)
    s=c.state
    add_officer(s['core'],'A4','p3',(55,65,70,60,70))
    s['officer_faction']['A4']='A';s['officer_loyalty']['A4']=72
    ac.add(s,'A4','p3',45);ac.refresh(s)
    assert validate(s)
    c.do('attack','A3',{'target':'p5','troops':120,'gold':10},'q09:start')
    return c


def totals(s):
    return sum(ac.army_total(a) for a in s['armies'].values())


def test_queue_arrive_and_cold_load_conserves_named_officer_army():
    c=started();s=c.state
    before=totals(s);before_source=ac.strength(s,'A4','p3')
    out=c.do('battle_reinforce','A3',{'source':'p3','officer_ids':['A4'],
            'rice':20,'gold':2,'arrival_day':2},'q09:reinforce')
    assert out['queued']==['A4']
    s=c.state;b=s['battle'];u=next(u for u in b['officer_units'] if u['id']=='A4')
    assert u['reserve'] and u['slot']==3 and u['troops']==before_source
    assert ac.strength(s,'A4','p3')==0 and s['armies']['army:A4']['engaged']==before_source
    assert totals(s)==before
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    c.do('battle_end_turn','A3',{},'q09:arrival')
    b=c.state['battle'];u=next(u for u in b['officer_units'] if u['id']=='A4')
    assert not u['reserve'] and u['arrive_day']==2
    assert (u['q'],u['r']) not in {(-1,-1),(0,6)}
    assert totals(c.state)<=before and validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_bad_reinforcement_source_and_overspend_rollback():
    c=started();before=c.fingerprint
    for i,params in enumerate((
        {'source':'p1','officer_ids':['A4'],'rice':1,'gold':0,'arrival_day':2},
        {'source':'p3','officer_ids':['A3'],'rice':1,'gold':0,'arrival_day':2},
        {'source':'p3','officer_ids':['A4'],'rice':99999,'gold':0,'arrival_day':2},
        {'source':'p3','officer_ids':['A4','A4'],'rice':1,'gold':0,'arrival_day':2},
        {'source':'p3','officer_ids':['A4'],'rice':1,'gold':0,'arrival_day':1},
    )):
        with pytest.raises(CoreError) as err:c.do('battle_reinforce','A3',params,f'q09:reject:{i}')
        assert err.value.code!='UNKNOWN_KIND'
        assert c.fingerprint==before


def test_withdraw_one_officer_to_friendly_neighbor_and_persist():
    c=started()
    c.do('battle_reinforce','A3',{'source':'p3','officer_ids':['A4'],
        'rice':0,'gold':0,'arrival_day':2},'q09:q')
    c.do('battle_end_turn','A3',{},'q09:arrive')
    b=c.state['battle'];u=next(u for u in b['officer_units'] if u['id']=='A4')
    remaining=u['troops']
    c.do('battle_withdraw','A3',{'officer_id':'A4','destination':'p3'},'q09:withdraw')
    u=next(u for u in c.state['battle']['officer_units'] if u['id']=='A4')
    assert u['fled'] and u['retreat_to']=='p3'
    assert c.state['battle'] is not None
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    c.state['battle']['rice_a']=0
    c.do('battle_end_turn','A3',{},'q09:finish')
    assert c.state['battle'] is None
    assert ac.strength(c.state,'A4','p3')==remaining
    assert c.state['core']['officers']['A4']['province_id']=='p3'
    assert validate(c.state)


def test_whole_army_withdrawal_ends_battle_and_no_supply_carry():
    c=started();s=c.state
    before=s['core']['provinces']['p3']['food_units']
    out=c.do('battle_withdraw','A3',{'destination':'p3','whole_army':True},'q09:whole')
    assert out['resolved'] and c.state['battle'] is None
    assert c.state['owners']['p5']=='B'
    assert ac.strength(c.state,'A3','p3')>0
    assert c.state['core']['provinces']['p3']['food_units']==before
    assert validate(c.state)


def test_withdraw_invalid_destination_rejects_without_mutation():
    c=started();before=c.fingerprint
    for i,p in enumerate(({'destination':'p4','whole_army':True},
                          {'destination':'p1','whole_army':True},
                          {'destination':'p3','officer_id':'B2'})):
        with pytest.raises(CoreError) as err:c.do('battle_withdraw','A3',p,f'q09:badwithdraw:{i}')
        assert err.value.code!='UNKNOWN_KIND'
        assert c.fingerprint==before


def test_defender_reserved_officer_has_no_phantom_destination_troops_and_returns_if_unarrived():
    c=new_campaign(1729);s=c.state
    # Fixture: a real same-faction neighboring source, without conjured units.
    s['owners']['p6']='B';s['officer_faction']['C1']='B'
    assert validate(s)
    c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'q09:defstart')
    s=c.state;b=s['battle'];before=totals(s)
    n=ac.strength(s,'C1','p6');at_dst=s['core']['provinces']['p5']['troops']
    ba._queue_reinforcements(s,b,{'source':'p6','officer_ids':['C1'],
                              'rice':0,'gold':0,'arrival_day':2},side='D')
    assert ac.strength(s,'C1','p6')==0
    assert s['armies']['army:C1']['engaged']==n
    assert b['d']['troops']==at_dst  # pending reserves are not physically at dst
    assert totals(s)==before and validate(s)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    # Battle ends before day 2; the reserve never arrived, so return to source.
    b['rice_a']=0
    c.do('battle_end_turn','A3',{},'q09:defsettle')
    assert c.state['battle'] is None
    assert ac.strength(c.state,'C1','p6')==n
    assert validate(c.state)


def test_defender_reserve_arrives_and_joins_real_destination_garrison():
    c=new_campaign(1730);s=c.state
    s['owners']['p6']='B';s['officer_faction']['C1']='B'
    assert validate(s)
    c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'q09:defstart2')
    s=c.state;b=s['battle'];before=totals(s)
    n=ac.strength(s,'C1','p6')
    ba._queue_reinforcements(s,b,{'source':'p6','officer_ids':['C1'],
                               'rice':0,'gold':0,'arrival_day':2},side='D')
    c.do('battle_end_turn','A3',{},'q09:defarrive')
    b=c.state['battle']
    assert b is not None
    u=next(u for u in b['officer_units'] if u['id']=='C1')
    assert not u['reserve'] and u['side']=='D'
    assert ac.strength(c.state,'C1','p5')==u['troops']
    assert c.state['armies']['army:C1']['engaged']==0
    assert totals(c.state)<=before and validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_controller_only_reinforce_and_withdraw_dispatch_real_commands():
    from types import SimpleNamespace
    from third_branch.tactical_view import TacticalWindow
    class Message:
        def set(self,value):self.value=value
    c=started();w=TacticalWindow.__new__(TacticalWindow)
    w.parent=SimpleNamespace(c=c);w.selected_officer='A3'
    w.reinforce_index=0;w.reinforce_rice=0;w.reinforce_gold=0
    w.withdraw_index=0;w.withdraw_whole=False
    w.targeting=False;w.message=Message()
    issued=[]
    w._issue=lambda kind,params:issued.append((kind,params))
    w.controller_actions=[('Move',w.move),('Reinforce',w.reinforce),('Withdraw',w.withdraw)]
    w.controller_index=1
    w.on_nes_button('right');w.on_nes_button('a')
    assert issued[-1]==('battle_reinforce',{'source':'p3','officer_ids':['A4'],
        'rice':1,'gold':0,'arrival_day':2})
    w.controller_index=2
    w.on_nes_button('right');w.on_nes_button('a')
    assert issued[-1]==('battle_withdraw',{'destination':'p3','whole_army':True})
