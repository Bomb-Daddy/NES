"""Run020 source-confirmed officer successor links for live Flee.

English-mechanics root.src select_officer_5154 $5159-$517D/$518E-$5195
loads the old province head, replaces it, and links the previous head as the
new officer's successor. Source evidence is static, not a new NES emulator run.
"""
import pytest
from rotk2.battle import engine as bt
from rotk2.saves import codec
from test_run018_q09_army_immediate_source_insertion import _army


def test_two_successful_flee_inserts_preserve_head_and_successor_order():
    s, b = _army()
    s.provinces[3].q09_officer_head = 5
    bt.withdraw(s, b, 'A', 3, True, officer_id=1,
                officer_destinations={1: 3, 3: 3})
    assert s.provinces[3].q09_officer_head == 3
    assert s.officers[3].q09_officer_next == 1
    assert s.officers[1].q09_officer_next == 5
    assert s.officers[5].province_id == 3
    assert s.validate() == []


def test_moving_known_head_advances_origin_to_known_successor():
    s, b = _army()
    s.provinces[1].q09_officer_head = 1
    s.officers[1].q09_officer_next = 3
    bt.withdraw(s, b, 'A', 3, False, officer_id=1)
    assert s.provinces[1].q09_officer_head == 3
    assert s.provinces[3].q09_officer_head == 1
    assert s.officers[1].q09_officer_next is None
    assert s.validate() == []


def test_unverified_next_does_not_create_fake_origin_head():
    s, b = _army()
    s.provinces[1].q09_officer_head = 1
    s.officers[1].q09_officer_next = 5  # officer 5 is in P3, not old P1
    bt.withdraw(s, b, 'A', 3, False, officer_id=1)
    assert s.provinces[1].q09_officer_head is None
    assert s.officers[1].q09_officer_next is None


def test_source_successor_order_survives_real_save_and_coldload(tmp_path):
    s, b = _army()
    s.provinces[3].q09_officer_head = 5
    bt.withdraw(s, b, 'A', 3, True, officer_id=1,
                officer_destinations={1: 3, 3: 3})
    path = tmp_path / 'run020_successors.ro2'
    codec.save(s, path)
    cold = codec.load(path)
    assert cold.state_hash() == s.state_hash()
    assert cold.officers[3].q09_officer_next == 1
    assert cold.officers[1].q09_officer_next == 5
    assert cold.validate() == []


def test_illegal_destination_is_atomic_including_successor_links():
    s, b = _army()
    s.provinces[3].q09_officer_head = 5
    before = s.state_hash()
    with pytest.raises(ValueError, match='ILLEGAL_WITHDRAW_DESTINATION'):
        bt.withdraw(s, b, 'A', 3, True, officer_id=1,
                    officer_destinations={1: 3, 3: 2})
    assert s.state_hash() == before


def test_controller_visible_whole_army_retreat_keeps_exact_successor():
    from test_classic_ui_route_build13 import App
    from rotk2.input.router import Action
    from rotk2.ui.screens import BattleScreen
    from rotk2.ui.widgets import YesNo, ListPrompt
    app=App(); s=app.engine.state
    s.provinces[3].q09_officer_head=5
    for oid in (1, 3):
        s.officers[oid].q09_source_flag2=0x40
    screen=BattleScreen(); screen.sel=1; app.stack.append(screen)
    screen._withdraw_menu(app)
    assert isinstance(app.stack[-1],YesNo)
    app.stack[-1].handle(app,Action.CONFIRM)
    for _ in (1,3):
        menu=app.stack[-1]
        assert isinstance(menu,ListPrompt)
        for __ in range(40):
            if menu.menu.current()==3:break
            menu.handle(app,Action.DOWN)
        else: raise AssertionError('destination absent from controller menu')
        menu.handle(app,Action.CONFIRM)
    assert s.provinces[3].q09_officer_head==3
    assert s.officers[3].q09_officer_next==1
    assert s.officers[1].q09_officer_next==5
    assert s.validate()==[]