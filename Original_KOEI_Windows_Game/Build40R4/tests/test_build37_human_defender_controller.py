"""Q04/Q09/Q10/Q11: human defender can issue real named-officer orders.

Original battle sides remain A/D; these tests cover controller/campaign routing,
not original-emulator parity or physical Xbox acceptance.
"""
from types import SimpleNamespace
from unittest.mock import Mock
import pytest
from og.core.fixed import CoreError
from og.core.world import add_officer
from third_branch.integrated import new_campaign,save_bytes,load_bytes,validate
from third_branch import army_cohorts as ac,officer_battle_adapter as ba
from third_branch.tactical_view import TacticalWindow


def started(seed=3701):
    c=new_campaign(seed)
    ac.add(c.state,'B1','p5',50);ac.refresh(c.state)
    c.state['human_faction']='B'
    # A surviving friendly neighbor for Q09, absent in the stock 7-town fixture.
    c.state['owners']['p7']='B';c.state['officer_faction']['C2']='B'
    c.do('attack','A3',{'target':'p5','troops':150,'gold':10},'b37:start')
    assert c.state['battle']['duel_pending'] is None
    return c


def test_defender_wait_and_cpu_attacker_turn_cold_replay():
    c=started();before=save_bytes(c)
    out=c.do('battle_wait','B2',{'officer_id':'B2'},'b37:wait')
    assert out['officer']=='B2' and ba.find(c.state['battle'],'D','B2')['acted']
    with pytest.raises(CoreError):
        c.do('battle_wait','A3',{'officer_id':'A3'},'b37:impostor')
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    replay=load_bytes(before)
    assert replay.do('battle_wait','B2',{'officer_id':'B2'},'b37:wait')==out
    assert replay.fingerprint==c.fingerprint
    day=c.do('battle_end_turn','B2',{},'b37:end')
    assert day['day']==2
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_defender_attack_changes_attacker_ledger_and_rollback():
    c=started(3702);b=c.state['battle']
    d=ba.find(b,'D','B2');a=ba.find(b,'A','A3')
    d['q'],d['r']=9,6;a['q'],a['r']=8,6
    old=a['troops100'];before=save_bytes(c)
    with pytest.raises(CoreError):
        c.do('battle_attack','B2',{'officer_id':'A3','target_officer_id':'B2'},'b37:cheat')
    assert save_bytes(c)==before
    out=c.do('battle_attack','B2',{'officer_id':'B2','target_officer_id':'A3','mode':'normal'},'b37:attack')
    assert out['officer']=='B2' and out['target']=='A3'
    assert c.state['battle'] is None or ba.find(c.state['battle'],'A','A3')['troops100']<=old
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_defender_reinforcement_arrives_from_actual_home():
    c=started(3703);s=c.state
    add_officer(s['core'],'B3','p7',(55,65,70,60,70))
    s['officer_faction']['B3']='B';s['officer_loyalty']['B3']=75
    ac.add(s,'B3','p7',25);ac.refresh(s)
    total=sum(ac.army_total(a) for a in s['armies'].values())
    out=c.do('battle_reinforce','B2',{'source':'p7','officer_ids':['B3'],'rice':0,'gold':0,'arrival_day':2},'b37:reinforce')
    assert out['queued']==['B3']
    u=next(u for u in c.state['battle']['officer_units'] if u['id']=='B3')
    assert u['side']=='D' and u['slot']==1 and u['reserve']
    assert sum(ac.army_total(a) for a in c.state['armies'].values())==total
    assert validate(c.state)
    c.do('battle_end_turn','B2',{},'b37:arrival')
    u=next(u for u in c.state['battle']['officer_units'] if u['id']=='B3')
    assert not u['reserve'] and u['arrive_day']==2
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_defender_whole_withdraw_and_invalid_destination():
    c=started(3704);before=save_bytes(c)
    with pytest.raises(CoreError):
        c.do('battle_withdraw','B2',{'destination':'p3','whole_army':True},'b37:bad')
    assert save_bytes(c)==before
    out=c.do('battle_withdraw','B2',{'destination':'p7','whole_army':True},'b37:whole')
    assert out['resolved'] and c.state['battle'] is None
    assert c.state['owners']['p5']=='A'
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_defender_controller_lists_own_units_and_enemy_targets():
    c=started(3705)
    w=TacticalWindow.__new__(TacticalWindow)
    w.parent=SimpleNamespace(c=c)
    w.selected_officer='A3';w.selected_target=None
    w.withdraw_index=0
    w.message=SimpleNamespace(set=Mock());w.redraw=Mock()
    assert w._player_side()=='D'
    assert w._officer() in ('B1','B2')
    w.next_officer();assert w.selected_officer in ('B1','B2')
    w.next_target();assert w.selected_target=='A3'
    assert w._withdraw_destination()=='p7'


def test_defender_duel_refusal_and_unauthorized_bribe_roll_back():
    c=started(3706);b=c.state['battle']
    # No CPU opening challenge is pending; a human defender may challenge.
    before=save_bytes(c)
    with pytest.raises(CoreError):
        c.do('battle_bribe','B2',{'officer_id':'B2','target_officer_id':'A3','gold':1},'b37:commander-bribe')
    assert save_bytes(c)==before
    duel=c.do('battle_duel','B1',{'officer_id':'B1','accepted':False},'b37:duel')
    assert duel['duel_result']=='REFUSED'
    assert c.state['battle']['duel_done']
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_controller_dispatch_corrects_stale_attacker_selection():
    c=started(3707)
    w=TacticalWindow.__new__(TacticalWindow)
    w.parent=SimpleNamespace(c=c)
    w.selected_officer='A3';w.selected_target=None
    w.message=SimpleNamespace(set=Mock());w.redraw=Mock()
    # Use a real staged command via the controller's side-selection logic.
    # Stub the parent UI and persistence so this is not a native Tk test.
    from tempfile import TemporaryDirectory
    with TemporaryDirectory() as folder:
        w.parent.path=__import__('pathlib').Path(folder)/'save.json'
        w.parent.counter=0
        w.parent.audio=SimpleNamespace(effect=Mock())
        w.parent.redraw=Mock()
        w._issue('battle_wait',{})
    assert w.selected_officer in ('B1','B2')
    assert ba.find(w.parent.c.state['battle'],'D',w.selected_officer)['acted']
    assert validate(w.parent.c.state)


def test_human_defender_victory_settles_and_survives_cold_load():
    c=started(3708)
    c.state['battle']['rice_a']=0
    result=c.do('battle_end_turn','B2',{},'b37:defense-wins')
    assert result['resolved'] and c.state['battle'] is None
    assert c.state['owners']['p5']=='B'
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_defender_protected_bribe_switches_real_enemy_army_and_conserves_troops():
    c=started(3709);s=c.state
    for oid in ('A4','A5'):
        add_officer(s['core'],oid,'p3',(55,65,70,60,70))
        s['officer_faction'][oid]='A';s['officer_loyalty'][oid]=80
        ac.add(s,oid,'p3',25)
    ac.refresh(s)
    # Enemy CPU's Q09 reserve setup, not an unauthorized defender order.
    ba._queue_reinforcements(s,s['battle'],{'source':'p3',
        'officer_ids':['A4','A5'],'rice':0,'gold':0,'arrival_day':2},side='A')
    c.do('battle_end_turn','B2',{},'b37:enemy-arrives')
    b=c.state['battle'];target=ba.find(b,'A','A5')
    assert target['slot']==3 and not target['commander']
    target['protected']=True
    before=sum(ac.army_total(a) for a in c.state['armies'].values())
    gold=b['gold_d_cents']
    result=c.do('battle_bribe','B2',{'officer_id':'B2',
        'target_officer_id':'A5','gold':1},'b37:defender-bribe')
    assert result['success'] and c.state['battle']['gold_d_cents']==gold-100
    assert c.state['officer_faction']['A5']=='B'
    assert ba.find(c.state['battle'],'D','A5')['slot']==0
    assert sum(ac.army_total(a) for a in c.state['armies'].values())==before
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
