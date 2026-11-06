"""Run021: source-list removing escaped middle/tail officer from an imported
province-linked list. Root $50EF/$50F9 ($69EA) and $5154 officer-list insert.
Source fixture, NOT original NES emulator replay. Unknown links are not guessed.
"""
import pytest
from rotk2.battle import engine as bt
from rotk2.saves import codec
from rotk2.state.models import Officer
from test_run018_q09_army_immediate_source_insertion import _army


def _chain():
    s,b=_army()
    # P1 source head=1, followed by officer 3 (the departing officer).
    # Source next may include a third officer only when it is resident.
    s.provinces[1].q09_officer_head=1
    s.officers[1].q09_officer_next=3
    s.officers[7]=Officer(7, "Source tail", 45, 50, 50, ruler_id=1,province_id=1,soldiers=0)
    return s,b


def test_source_middle_departure_relinks_known_predecessor():
    s,b=_chain()
    s.officers[3].q09_officer_next=7
    s.provinces[3].q09_officer_head=5
    bt.withdraw(s,b,'A',3,False,officer_id=3)
    assert s.provinces[1].q09_officer_head==1
    assert s.officers[1].q09_officer_next==7
    assert s.officers[3].q09_officer_next==5
    assert s.provinces[3].q09_officer_head==3
    assert s.validate()==[]


def test_source_tail_departure_clears_old_predecessor_link():
    s,b=_chain()
    s.officers[3].q09_officer_next=None
    bt.withdraw(s,b,'A',3,False,officer_id=3)
    assert s.provinces[1].q09_officer_head==1
    assert s.officers[1].q09_officer_next is None
    assert s.validate()==[]


def test_untrusted_head_does_not_allow_guessing_a_predecessor():
    s,b=_chain()
    s.provinces[1].q09_officer_head=None
    bt.withdraw(s,b,'A',3,False,officer_id=3)
    assert s.officers[1].q09_officer_next==3  # untouched unknown order


def test_corrupt_cross_province_successor_not_imported_as_head():
    s,b=_chain()
    s.officers[3].q09_officer_next=5  # P3 resident, not a P1 successor
    bt.withdraw(s,b,'A',3,False,officer_id=3)
    assert s.officers[1].q09_officer_next is None
    assert s.provinces[3].q09_officer_head==3


def test_real_ro2_save_cold_load_keeps_unlinked_middle(tmp_path):
    s,b=_chain()
    s.officers[3].q09_officer_next=7
    s.provinces[3].q09_officer_head=5
    bt.withdraw(s,b,'A',3,False,officer_id=3)
    save=tmp_path/'run021_q09_unlink.ro2'
    codec.save(s,save)
    cold=codec.load(save)
    assert cold.state_hash()==s.state_hash()
    assert cold.officers[1].q09_officer_next==7
    assert cold.officers[3].q09_officer_next==5
    assert cold.validate()==[]


def test_bad_destination_atomic_and_no_chain_damage():
    s,b=_chain()
    before=s.state_hash()
    with pytest.raises(ValueError,match='ILLEGAL_WITHDRAW_DESTINATION'):
        bt.withdraw(s,b,'A',2,False,officer_id=3)
    assert s.state_hash()==before


def test_actual_controller_flee_non_head_repairs_linked_origin():
    """Actual battle menu, Linux software Action.CONFIRM; not physical Xbox."""
    from test_classic_ui_route_build13 import App
    from rotk2.input.router import Action
    from rotk2.ui.screens import BattleScreen
    from rotk2.ui.widgets import ListPrompt
    app=App()
    s=app.engine.state
    s.provinces[1].q09_officer_head=1
    s.officers[1].q09_officer_next=3
    s.officers[3].q09_source_flag2=0x40
    screen=BattleScreen();screen.sel=3;app.stack.append(screen)
    screen._withdraw_menu(app)
    menu=app.stack[-1]
    assert isinstance(menu,ListPrompt)
    for _ in range(20):
        if menu.menu.current()==3:break
        menu.handle(app,Action.DOWN)
    else:raise AssertionError('P3 absent in actual controller flee menu')
    menu.handle(app,Action.CONFIRM)
    assert s.officers[1].q09_officer_next is None
    assert s.provinces[3].q09_officer_head==3
    assert s.validate()==[]
