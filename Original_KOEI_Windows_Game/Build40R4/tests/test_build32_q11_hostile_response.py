"""Q11 hostile QA against verified Enhanced Build31 parent."""
import pytest
from third_branch.integrated import new_campaign,save_bytes,load_bytes,validate
from third_branch import officer_battle_adapter as ba,army_cohorts as ac
from og.core.fixed import CoreError


def battle(*,cpu_defender_challenges=False):
    c=new_campaign(331)
    ac.add(c.state,'B1','p5',50);ac.refresh(c.state)
    if cpu_defender_challenges:c.state['core']['officers']['B1']['war']=95
    c.do('attack','A3',{'target':'p5','troops':150},'q11:hostile:start')
    return c


def test_cpu_defender_refusal_damages_refusing_attacker_not_challenger():
    c=battle(cpu_defender_challenges=True);b=c.state['battle']
    assert b['duel_pending']['side']=='D'
    a_before=sum(u['troops100'] for u in b['officer_units'] if u['side']=='A')
    d_before=sum(u['troops100'] for u in b['officer_units'] if u['side']=='D')
    snapshot=save_bytes(c)
    out=c.do('battle_duel_response','A3',{'accepted':False},'q11:hostile:refuse')
    assert out['duel_result']=='REFUSED'
    a_after=sum(u['troops100'] for u in c.state['battle']['officer_units'] if u['side']=='A')
    d_after=sum(u['troops100'] for u in c.state['battle']['officer_units'] if u['side']=='D')
    assert a_after<a_before and d_after==d_before
    clone=load_bytes(snapshot)
    assert clone.do('battle_duel_response','A3',{'accepted':False},'q11:hostile:refuse')==out
    assert clone.fingerprint==c.fingerprint and validate(c.state)


def test_cpu_responder_cannot_be_forged_or_declined():
    c=battle();b=c.state['battle'];ba.find(b,'A','A3')['war']=40
    assert ba.choose_ai_duel_acceptor(b,ba.find(b,'A','A3')) is not None
    before=c.fingerprint
    with pytest.raises(CoreError,match='AI_DUEL_ACCEPTANCE_MISMATCH'):
        c.do('battle_duel','A3',{'target_officer_id':'INVALID','accepted':True},'q11:hostile:bad')
    assert c.fingerprint==before
    with pytest.raises(CoreError,match='AI_DUEL_ACCEPTANCE_MISMATCH'):
        c.do('battle_duel','A3',{'accepted':False},'q11:hostile:bad2')
    assert c.fingerprint==before


def test_duel_after_first_tactical_action_rejected():
    c=battle();ba.find(c.state['battle'],'A','A3')['acted']=True
    before=c.fingerprint
    with pytest.raises(CoreError,match='DUEL_NOT_AT_BATTLE_OPENING'):
        c.do('battle_duel','A3',{'accepted':False},'q11:hostile:late')
    assert c.fingerprint==before


def test_defending_challenger_victory_is_labeled_defender(monkeypatch):
    c=battle(cpu_defender_challenges=True)
    monkeypatch.setattr(ba,'randrange',lambda state,n:max(0,n-1))
    out=c.do('battle_duel_response','A3',{'accepted':True,'target_officer_id':'A3'},'q11:hostile:side')
    assert out['duel_result']=='DEFENDER'
    assert validate(c.state)


def test_controller_response_executes_and_cold_saves_real_campaign(tmp_path):
    from types import SimpleNamespace
    from unittest.mock import Mock
    from third_branch.tactical_view import TacticalWindow
    c=battle(cpu_defender_challenges=True)
    parent=SimpleNamespace(c=c,counter=0,path=tmp_path/'battle_ui.json',
                           audio=SimpleNamespace(effect=Mock()),redraw=Mock())
    w=TacticalWindow.__new__(TacticalWindow)
    w.parent=parent;w.selected_officer='A3';w.selected_target='B2'
    w.duel_accept=False;w.message=SimpleNamespace(set=Mock());w.redraw=Mock()
    w.duel()
    assert parent.path.exists() and parent.path.stat().st_size>0
    assert parent.c.state['battle']['duel_pending'] is None
    assert parent.c.state['battle']['duel_done']
    assert validate(parent.c.state)
    assert load_bytes(save_bytes(parent.c)).fingerprint==parent.c.fingerprint
