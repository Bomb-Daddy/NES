"""RUN008: Q09 original hsovl2 $D905 -> hex $7FED destination scan.

Original source fixture is decoded SNES shared battle mechanics (VERIFIED_STATIC),
not a fresh native Mesen run. A peaceful friendly province with no stationed
serving officer cannot be selected merely because its owner field matches.
The original stored head ordering (province+2) is not yet modeled; tests do
not claim parity for multiple mixed-faction officer heads.
"""
import pytest
from rotk2.battle import engine as bt
from rotk2.state.models import GameState, Officer, Province, Ruler
from rotk2.saves import codec
from test_officer_system_q09_q10_q11_q14_build13 import battle_game, command


def _empty_friendly():
    e=battle_game()
    # Province 3 remains owned by attacker, but has no province officer head.
    e.state.officers[5].province_id=1
    assert e.state.provinces[3].owner==1
    assert e.state.officers_in(3)==[]
    assert e.state.validate()==[]
    return e


def test_garrisonless_owned_neighbor_not_in_original_flee_destinations():
    e=_empty_friendly()
    assert 1 in bt.retreat_destinations(e.state,e.state.battle,'A',officer_id=3)
    assert 3 not in bt.retreat_destinations(e.state,e.state.battle,'A',officer_id=3)


def test_cannot_inject_garrisonless_flee_through_direct_command():
    e=_empty_friendly()
    old=e.state.state_hash()
    out=command(e,'battle_withdraw',destination=3,whole_army=False,officer_id=3)
    assert not out.success
    assert e.state.state_hash()==old


def test_real_friendly_garrison_and_open_neighbor_remain_legal():
    e=battle_game()
    assert 3 in bt.retreat_destinations(e.state,e.state.battle,'A',officer_id=3)
    e.state.provinces[3].owner=None
    assert 3 in bt.retreat_destinations(e.state,e.state.battle,'A',officer_id=3)
    assert 3 in bt.retreat_destinations(e.state,e.state.battle,'D',officer_id=4)  # still open, so either side may choose it


def test_individual_allied_officer_uses_own_liege_not_side_commander(tmp_path):
    e=battle_game(); s=e.state; b=s.battle
    s.rulers[8]=Ruler(8,'Allied')
    s.officers[3].ruler_id=8
    s.provinces[4]=Province(4,1,3,0,owner=8,rice=100,gold=100,neighbors=[2])
    s.provinces[2].neighbors.append(4)
    s.officers[8]=Officer(8,'Ally garrison',70,70,70,ruler_id=8,province_id=4,soldiers=20)
    assert s.validate()==[]
    destinations=bt.retreat_destinations(s,b,'A',officer_id=3)
    assert 4 in destinations and 1 not in destinations and 3 not in destinations
    old=s.state_hash()
    with pytest.raises(ValueError,match='ILLEGAL_WITHDRAW_DESTINATION'):
        bt.withdraw(s,b,'A',1,whole_army=False,officer_id=3)
    assert s.state_hash()==old
    bt.withdraw(s,b,'A',4,whole_army=False,officer_id=3)
    assert b.retreat_destinations['3']==4 and not b.units[0].fled
    assert s.validate()==[]
    path=tmp_path/'individual_allied_flee.ro2'
    codec.save(s,path)
    cold=codec.load(path)
    assert cold.state_hash()==s.state_hash()
    assert cold.battle.retreat_destinations['3']==4


def test_software_controller_flee_menu_hides_empty_friendly_and_b_cancels():
    # Software Action routing is not a native pygame rendering or Xbox device test.
    from test_classic_ui_route_build13 import App
    from rotk2.input.router import Action
    from rotk2.ui.screens import BattleScreen
    from rotk2.ui.widgets import ListPrompt
    app=App()
    app.engine.state.officers[5].province_id=1
    screen=BattleScreen();screen.sel=3
    before=app.engine.state.state_hash()
    screen._withdraw_menu(app)
    assert isinstance(app.stack[-1],ListPrompt)
    options=[pid for _label,pid,_enabled in app.stack[-1].menu.items]
    assert options==[1],options
    app.stack[-1].handle(app,Action.CANCEL)
    assert app.stack==[] and app.engine.state.state_hash()==before
