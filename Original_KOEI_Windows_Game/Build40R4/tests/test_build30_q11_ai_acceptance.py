"""Q11 AI defender's original acceptance scan and persistent duel outcome."""
from third_branch.integrated import new_campaign,save_bytes,load_bytes,validate
from third_branch import officer_battle_adapter as ba,army_cohorts as ac


def battle():
    c=new_campaign(331)
    ac.add(c.state,'B1','p5',50);ac.refresh(c.state)
    c.do('attack','A3',{'target':'p5','troops':150},'q11:ai:start')
    return c


def test_ai_acceptor_nonhead_then_commander_and_reserve_exclusion():
    c=battle();b=c.state['battle']
    challenger=ba.find(b,'A','A3')
    defender=ba.find(b,'D','B1')
    assert challenger and defender
    challenger['war']=50
    defender['war']=100
    nonhead=next((u for u in b['officer_units'] if u['side']=='D' and not u['commander'] and not u.get('reserve')),None)
    assert nonhead is not None
    nonhead['war']=71
    assert ba.choose_ai_duel_acceptor(b,challenger)['id']==nonhead['id']
    nonhead['war']=70
    # Commander 100 is strictly above challenger+19; fallback is legal.
    assert ba.choose_ai_duel_acceptor(b,challenger)['id']==defender['id']
    defender['war']=69
    assert ba.choose_ai_duel_acceptor(b,challenger) is None


def test_ai_duel_selection_executes_and_cold_loads():
    c=battle();b=c.state['battle'];challenger=ba.find(b,'A','A3')
    challenger['war']=40
    defender=ba.choose_ai_duel_acceptor(b,challenger)
    assert defender is not None
    before=save_bytes(c)
    args={'officer_id':challenger['id'],'target_officer_id':defender['id'],'accepted':True}
    result=c.do('battle_duel','A3',args,'q11:ai:duel')
    clone=load_bytes(before)
    assert clone.do('battle_duel','A3',args,'q11:ai:duel')==result
    assert clone.fingerprint==c.fingerprint
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_enhanced_controller_duel_dispatches_ai_selected_defender_not_stale_target():
    from types import SimpleNamespace
    from unittest.mock import Mock
    from third_branch.tactical_view import TacticalWindow
    c=battle()
    b=c.state['battle'];challenger=ba.find(b,'A','A3')
    challenger['war']=40
    expected=ba.choose_ai_duel_acceptor(b,challenger)
    assert expected is not None
    window=TacticalWindow.__new__(TacticalWindow)
    window.parent=SimpleNamespace(c=c)
    window.selected_officer='A3'
    window.selected_target='stale-target'
    window.message=SimpleNamespace(set=Mock())
    window._issue=Mock()
    window.duel()
    assert window.selected_target==expected['id']
    window._issue.assert_called_once_with('battle_duel',
        {'accepted':True,'target_officer_id':expected['id']})
