"""Build14: officer-targeting, Q02 Rest, and controller dispatch contract."""
import pytest
from og.core.fixed import CoreError
from third_branch.integrated import load_bytes, save_bytes, validate
from tests.test_build13_officer_ledger import two_defenders


def test_wait_rest_one_mp_and_consume_action_persists_cold():
    c=two_defenders();b=c.state['battle'];u=next(x for x in b['officer_units'] if x['id']=='A3')
    u['mp']=3
    out=c.do('battle_wait','A3',{'officer_id':'A3'},'build14:wait')
    assert out['mobility_left']==4
    assert next(x for x in c.state['battle']['officer_units'] if x['id']=='A3')['acted'] is True
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    with pytest.raises(CoreError):c.do('battle_wait','A3',{'officer_id':'A3'},'build14:repeat')
    assert validate(c.state)


def test_wait_cap_and_other_officer_cannot_wait_for_selected():
    c=two_defenders();b=c.state['battle'];u=next(x for x in b['officer_units'] if x['id']=='A3')
    u['mp']=6
    assert c.do('battle_wait','A3',{'officer_id':'A3'},'build14:cap')['mobility_left']==6
    h=c.fingerprint
    with pytest.raises(CoreError):c.do('battle_wait','A3',{'officer_id':'B2'},'build14:cheat')
    assert c.fingerprint==h


def test_invalid_target_does_not_fallback_to_defender_commander():
    c=two_defenders();b=c.state['battle'];u=next(x for x in b['officer_units'] if x['id']=='A3')
    u['q'],u['r']=9,6
    h=c.fingerprint
    with pytest.raises(CoreError):c.do('battle_attack','A3',{'officer_id':'A3','target_officer_id':'NOPE'},'build14:badtarget')
    assert c.fingerprint==h


def test_ui_exposes_target_and_rest_controller_actions():
    from third_branch.tactical_view import TacticalWindow
    assert callable(getattr(TacticalWindow,'next_target',None))
    assert callable(getattr(TacticalWindow,'wait',None))
    import inspect
    src=inspect.getsource(TacticalWindow)
    assert "'Next Target'" in src and "'Rest'" in src
    assert "'target_officer_id'" in src


def test_move_enemy_drags_adjacent_lower_int_target_into_vacated_hex(monkeypatch):
    from third_branch import officer_battle_adapter as adapter
    monkeypatch.setattr(adapter,'randrange',lambda state,n:0)
    c=two_defenders();b=c.state['battle']
    actor=next(x for x in b['officer_units'] if x['id']=='A3')
    target=next(x for x in b['officer_units'] if x['id']=='B2')
    actor['q'],actor['r'],actor['mp']=9,5,6
    actor['intel']=100  # Q06 gate: must be at least target INT
    target['q'],target['r']=10,5
    out=c.do('battle_move_enemy','A3',{'officer_id':'A3','target_officer_id':'B2','q':9,'r':6},'build14:drag')
    assert out['dragged'] is True
    after=c.state['battle']['officer_units']
    assert (next(x for x in after if x['id']=='A3')['q'],next(x for x in after if x['id']=='A3')['r'])==(9,6)
    assert (next(x for x in after if x['id']=='B2')['q'],next(x for x in after if x['id']=='B2')['r'])==(9,5)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    assert validate(c.state)


def test_move_enemy_illegal_distance_rolls_back():
    c=two_defenders();h=c.fingerprint
    with pytest.raises(CoreError):
        c.do('battle_move_enemy','A3',{'officer_id':'A3','target_officer_id':'B2','q':12,'r':11},'build14:illegaldrag')
    assert c.fingerprint==h


def test_controller_move_enemy_routes_through_targeting_not_keyboard_dialog():
    import inspect
    from third_branch.tactical_view import TacticalWindow
    src=inspect.getsource(TacticalWindow.on_nes_button)
    assert "self.start_target('battle_move_enemy')" in src
    assert "self._issue(self.target_mode" in src
