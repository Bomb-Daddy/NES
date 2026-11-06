"""Run015 source-backed Q09 $D6AC–$D6C8 captured-list port regressions.

The source-decoded handoff says a *failed* individual Flee unlinks the
battlefield officer and links the officer into the OPPONENT's captured list
before the battle ends. These are Classic port tests, not an original ROM replay.
"""
import pytest
from rotk2.battle import engine as bt
from rotk2.saves import codec
from rotk2.state.models import GameState
from test_build25_q09_conservation import fixture


def _failed_flee(monkeypatch, side='A'):
    state, battle = fixture()
    battle.side_to_move = side
    if side == 'A':
        officer_id, destination, captor = 5, 1, 'D'
    else:
        state.provinces[3].owner=6
        state.officers[7].ruler_id=6
        officer_id, destination, captor = 6, 3, 'A'
    # Only force the result of the source roll; this leaves destination
    # validation, outcome, unlink and capture bookkeeping REAL.
    monkeypatch.setattr(bt, 'q09_escape_check', lambda *_: False)
    bt.withdraw(state,battle,side,destination,whole_army=False,officer_id=officer_id)
    return state,battle,officer_id,captor


@pytest.mark.parametrize('side', ['A','D'])
def test_failed_flee_links_opponent_not_own_side(monkeypatch,side):
    state,battle,officer_id,captor = _failed_flee(monkeypatch,side)
    assert battle.q09_captured_by_side == {captor: [officer_id]}
    unit = next(u for u in battle.units if u.officer_id==officer_id)
    assert not unit.alive and not unit.fled and unit.mp==0
    assert bt.unit_at(battle,unit.q,unit.r) is None
    assert state.validate() == []


def test_failed_flee_capture_registry_survives_checksum_disk_cold_load(monkeypatch,tmp_path):
    state,battle,officer_id,captor = _failed_flee(monkeypatch)
    file = tmp_path/'q09_capture.ro2'
    codec.save(state,file)
    restored = codec.load(file)
    assert restored.battle.q09_captured_by_side == {captor:[officer_id]}
    assert restored.state_hash()==state.state_hash()
    assert codec.encode(restored)==codec.encode(state)


def test_success_and_source_unknown_do_not_create_false_capture(monkeypatch):
    for override in (True,):
        state,battle=fixture()
        battle.side_to_move='A'
        monkeypatch.setattr(bt,'q09_escape_check',lambda *_:override)
        bt.withdraw(state,battle,'A',1,whole_army=False,officer_id=5)
        assert battle.q09_captured_by_side=={}
        assert battle.units[0].fled and battle.units[0].alive


def test_invalid_destination_rejected_before_capture_list_or_random_draw(monkeypatch):
    state,battle=fixture()
    battle.side_to_move='A'
    used=[]
    monkeypatch.setattr(bt,'q09_escape_check',lambda *_:used.append(True) or False)
    before=state.state_hash()
    with pytest.raises(ValueError,match='ILLEGAL_WITHDRAW_DESTINATION'):
        bt.withdraw(state,battle,'A',2,whole_army=False,officer_id=5)
    assert state.state_hash()==before
    assert used==[]


def test_saved_capture_registry_rejects_wrong_side_or_duplicate_ids(monkeypatch):
    state,battle,officer_id,captor=_failed_flee(monkeypatch)
    battle.q09_captured_by_side={'A':[officer_id], 'D':[officer_id]}
    assert any('capture' in err.lower() for err in state.validate())
    battle.q09_captured_by_side={captor:[officer_id,officer_id]}
    assert any('capture' in err.lower() for err in state.validate())


def test_actual_source_rng_failed_flee_links_opponent_without_fake_roll(monkeypatch, tmp_path):
    """Use real q09_escape_check, including both controlled RNG draws."""
    from test_officer_system_q09_q10_q11_q14_build13 import battle_game
    e=battle_game()
    state=e.state
    battle=state.battle
    officer=state.officers[3]
    officer.war=0
    officer.training=0
    officer.q09_source_flag2=0
    officer.q09_source_special_2569=0
    officer.q09_source_movement11=2
    departing=next(u for u in battle.units if u.officer_id==3)
    departing.troops100=100
    before=battle.combat_draws
    bt.withdraw(state,battle,'A',destination=1,whole_army=False,officer_id=3)
    assert battle.combat_draws==before+2
    assert not departing.alive and not departing.fled
    assert battle.q09_captured_by_side=={'D':[3]}
    assert 'escaped=False' in next(log for log in battle.log if 'Q09 officer 3 movement=' in log)
    assert state.validate()==[]
    target=tmp_path/'source_capture.ro2'
    codec.save(state,target)
    assert codec.load(target).state_hash()==state.state_hash()
