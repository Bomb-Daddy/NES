"""Q11 source-backed AI challenge and defender answer in real campaign."""
import pytest
from og.core.fixed import CoreError
from third_branch.integrated import new_campaign,save_bytes,load_bytes,validate
from third_branch import officer_battle_adapter as ba,army_cohorts as ac

def battle_with_ai_challenge():
    c=new_campaign(331)
    ac.add(c.state,'B1','p5',50);ac.refresh(c.state)
    c.state['core']['officers']['B1']['war']=95
    c.do('attack','A3',{'target':'p5','troops':150},'q11:opening:start')
    return c

def test_ai_challenger_pending_and_refusal_cold_load():
    c=battle_with_ai_challenge()
    b=c.state['battle']
    assert b['duel_pending']['side']=='D'
    assert b['duel_pending']['officer_id']=='B1'
    before=save_bytes(c)
    with pytest.raises(CoreError):
        c.do('battle_wait','A3',{},'q11:opening:blocked')
    assert save_bytes(c)==before
    result=c.do('battle_duel_response','A3',{'accepted':False},'q11:opening:response')
    assert result['duel_result']=='REFUSED'
    assert c.state['battle']['duel_pending'] is None
    assert c.state['battle']['duel_done']
    replay=load_bytes(before)
    assert replay.do('battle_duel_response','A3',{'accepted':False},'q11:opening:response')==result
    assert replay.fingerprint==c.fingerprint
    assert validate(c.state)

def test_human_defender_can_accept_cpu_attacker_challenge():
    c=new_campaign(337)
    ac.add(c.state,'B1','p5',50);ac.refresh(c.state)
    c.state['human_faction']='B'
    c.state['core']['officers']['A3']['war']=95
    c.do('attack','A3',{'target':'p5','troops':150},'q11:human-defender:start')
    b=c.state['battle']
    assert b['duel_pending']['side']=='A'
    defender=next(u['id'] for u in b['officer_units'] if u['side']=='D' and u['troops100']>=100)
    result=c.do('battle_duel_response',defender,{'accepted':True,'target_officer_id':defender},'q11:human-defender:answer')
    assert result['duel_result'] in ('ATTACKER','DEFENDER','DRAW')
    assert c.state['battle'] is None or c.state['battle']['duel_done']

def test_enhanced_opening_controller_dispatch_and_negative_control():
    from types import SimpleNamespace
    from unittest.mock import Mock
    from third_branch.tactical_view import TacticalWindow
    c=battle_with_ai_challenge()
    b=c.state['battle']
    window=TacticalWindow.__new__(TacticalWindow)
    window.parent=SimpleNamespace(c=c)
    window.selected_officer='A3'
    window.duel_accept=False
    window.message=SimpleNamespace(set=Mock())
    window._issue=Mock()
    window.duel()
    window._issue.assert_called_once_with('battle_duel_response',{'accepted':False})
    assert c.state['battle']['duel_pending'] is not None
    # An attacker cannot impersonate a defender in a human-defender game.
    c.state['human_faction']='B'
    before=save_bytes(c)
    with pytest.raises(CoreError):
        c.do('battle_duel_response','A3',{'accepted':False},'q11:opening:impostor')
    assert save_bytes(c)==before

def test_human_attacker_accepts_cpu_defender_challenge():
    c=battle_with_ai_challenge()
    b=c.state['battle']
    assert b['duel_pending']['side']=='D'
    result=c.do('battle_duel_response','A3',
        {'accepted':True,'target_officer_id':'A3'},'q11:opening:accept')
    assert result['duel_result'] in ('ATTACKER','DEFENDER','DRAW')
    assert c.state['battle'] is None or c.state['battle']['duel_pending'] is None
