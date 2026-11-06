"""Run018: whole-army D89D calls the same native D648 insertion per escaped officer.

Source fixture: approved hsovl2 D648-D6A5, D89D iterative officer withdrawal;
NOT an original-emulator replay. Q09 raw campaign bytes remain unhydrated.
"""
import pytest
from rotk2.battle import engine as bt
from rotk2.saves import codec
from rotk2.state.rng import RNG
from test_officer_system_q09_q10_q11_q14_build13 import battle_game


def _army():
    e=battle_game()
    s,b=e.state,e.state.battle
    for oid in (1,3):
        s.officers[oid].q09_source_flag2=0x40  # original unconditional source escape
    b.units[0].troops100=15131
    b.units[1].troops100=7455
    return s,b


def test_whole_army_source_known_officers_insert_during_withdraw_not_settlement(tmp_path):
    s,b=_army()
    before=b.combat_draws
    bt.withdraw(s,b,'A',3,whole_army=True,officer_id=1,
                officer_destinations={1:3,3:3})
    assert b.result=='DEFENDER' and not b.committed
    assert {str(oid):dst for oid,dst in [(1,3),(3,3)]}==b.retreat_destinations
    assert s.officers[1].province_id==3 and s.officers[1].soldiers==151
    assert s.officers[3].province_id==3 and s.officers[3].soldiers==74
    assert b.combat_draws==before and all(u.fled for u in b.units[:2])
    assert s.validate()==[]
    p=tmp_path/'whole_army_immediate.ro2'
    codec.save(s,p)
    cold=codec.load(p)
    assert cold.state_hash()==s.state_hash() and cold.validate()==[]
    assert cold.officers[1].soldiers==151 and cold.officers[3].soldiers==74


def test_whole_army_independent_destination_is_applied_immediately():
    s,b=_army()
    bt.withdraw(s,b,'A',1,whole_army=True,officer_id=1,
                officer_destinations={1:1,3:3})
    assert b.retreat_destinations=={'1':1,'3':3}
    assert s.officers[1].province_id==1 and s.officers[1].soldiers==151
    assert s.officers[3].province_id==3 and s.officers[3].soldiers==74
    assert s.validate()==[]


def test_whole_army_live_insert_not_rescaled_when_settlement_follows():
    s,b=_army()
    s.provinces[3].under_attack=True
    bt.withdraw(s,b,'A',3,whole_army=True,officer_id=1,
                officer_destinations={1:3,3:3})
    assert s.officers[1].soldiers==15131 and s.officers[3].soldiers==7455
    bt.resolve_into_campaign(s, [], RNG(919))
    assert s.officers[1].province_id==3 and s.officers[1].soldiers==15131
    assert s.officers[3].province_id==3 and s.officers[3].soldiers==7455
    assert s.validate()==[]


def test_whole_army_unknown_source_metadata_is_not_promoted_to_parity():
    s,b=_army()
    s.officers[3].q09_source_flag2=None
    old=(s.officers[3].province_id,s.officers[3].soldiers)
    bt.withdraw(s,b,'A',3,whole_army=True,officer_id=1,
                officer_destinations={1:3,3:3})
    assert s.officers[1].province_id==3 and s.officers[1].soldiers==151
    assert (s.officers[3].province_id,s.officers[3].soldiers)==old
    assert b.retreat_destinations=={'1':3,'3':3}
    assert s.validate()==[]


def test_whole_army_bad_officer_destination_is_atomic_before_any_live_insert():
    s,b=_army()
    before=s.state_hash()
    with pytest.raises(ValueError,match='ILLEGAL_WITHDRAW_DESTINATION'):
        bt.withdraw(s,b,'A',3,whole_army=True,officer_id=1,
                    officer_destinations={1:3,3:2})
    assert s.state_hash()==before


def test_software_controller_whole_army_two_prompts_commits_immediate_source_outcome():
    """Actual BattleScreen/YesNo/ListPrompt callback route, software only."""
    from test_classic_ui_route_build13 import App
    from rotk2.input.router import Action
    from rotk2.ui.screens import BattleScreen
    from rotk2.ui.widgets import YesNo, ListPrompt
    app=App()
    s=app.engine.state
    for oid in (1,3):s.officers[oid].q09_source_flag2=0x40
    s.battle.units[0].troops100=15131
    s.battle.units[1].troops100=7455
    screen=BattleScreen();screen.sel=1
    app.stack.append(screen)  # real battle scene remains beneath modal stack
    screen._withdraw_menu(app)
    assert isinstance(app.stack[-1],YesNo)
    app.stack[-1].handle(app,Action.CONFIRM)  # Yes: whole army
    assert isinstance(app.stack[-1],ListPrompt)
    # Select P3 for both the primary commander and the second officer.
    for oid in (1,3):
        menu=app.stack[-1]
        assert isinstance(menu,ListPrompt)
        while menu.menu.current()!=3:
            menu.handle(app,Action.DOWN)
        menu.handle(app,Action.CONFIRM)
    assert s.officers[1].province_id==3 and s.officers[1].soldiers==151
    assert s.officers[3].province_id==3 and s.officers[3].soldiers==74
    assert s.validate()==[]
