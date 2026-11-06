"""Run011 Q09 source-backed hex $7FED province officer-list head qualification.

When a source-confirmed head pointer is present, the original compares the
HEAD officer's liege, not province ownership nor any arbitrary garrison member.
No RNG, capture, or original-emulator assertion is implied.
"""
from rotk2.battle import engine as bt
from rotk2.state.models import Officer, GameState
from rotk2.saves import codec
from test_officer_system_q09_q10_q11_q14_build13 import battle_game, command
from test_classic_ui_route_build13 import App
from rotk2.ui.screens import BattleScreen
from rotk2.ui.widgets import ListPrompt
from rotk2.input.router import Action


def conflicting_head():
    e=battle_game();s=e.state
    # A province can retain its owner 1 yet have a head officer serving 2
    # during an active battle. This is a decoded-source fixture, not a claim
    # ordinary campaign operations routinely create such a province.
    s.officers[6]=Officer(6,'Other liege head',55,55,55,ruler_id=2,province_id=3,soldiers=10)
    s.provinces[3].q09_officer_head=6
    assert s.officers_in(3,1) and s.officers_in(3,2)
    assert s.validate()==[]
    return e


def test_original_head_liege_overrides_owner_and_nonhead_members():
    e=conflicting_head();s=e.state;b=s.battle
    assert 3 not in bt.retreat_destinations(s,b,'A',officer_id=3)
    assert 3 in bt.retreat_destinations(s,b,'D',officer_id=4)
    old=s.state_hash();draws=b.combat_draws
    r=command(e,'battle_withdraw',destination=3,whole_army=False,officer_id=3)
    assert not r.success and r.error_code=='ILLEGAL_WITHDRAW_DESTINATION'
    assert s.state_hash()==old and b.combat_draws==draws
    assert s.validate()==[]


def test_verified_head_can_qualify_even_if_owner_is_different():
    e=battle_game();s=e.state;b=s.battle
    s.provinces[3].owner=2
    s.provinces[3].q09_officer_head=5
    assert 3 in bt.retreat_destinations(s,b,'A',officer_id=3)
    # Adversarial forged/missing head must not become a blanket allowance.
    s.provinces[3].q09_officer_head=999999
    assert 3 not in bt.retreat_destinations(s,b,'A',officer_id=3)


def test_controller_menu_reads_same_head_as_command_and_cancel_is_atomic():
    app=App();s=app.engine.state;b=s.battle
    s.officers[6]=Officer(6,'Foreign head',55,55,55,ruler_id=2,province_id=3,soldiers=10)
    s.provinces[3].q09_officer_head=6
    screen=BattleScreen();screen.sel=3
    old=s.state_hash()
    screen._withdraw_menu(app)
    assert isinstance(app.stack[-1],ListPrompt)
    options=[pid for _label,pid,_enabled in app.stack[-1].menu.items]
    assert 3 not in options and 1 in options
    app.stack[-1].handle(app,Action.CANCEL)
    assert app.stack==[] and s.state_hash()==old


def test_known_head_survives_real_disk_cold_load_and_unknown_old_save_compatible(tmp_path):
    e=conflicting_head();s=e.state
    path=tmp_path/'run011_head.ro2';codec.save(s,path)
    cold=codec.load(path)
    assert cold.state_hash()==s.state_hash()
    assert cold.provinces[3].q09_officer_head==6
    assert 3 not in bt.retreat_destinations(cold,cold.battle,'A',officer_id=3)
    assert cold.validate()==[]
    old=s.to_dict()
    for p in old['provinces'].values():p.pop('q09_officer_head',None)
    legacy=GameState.from_dict(old)
    assert legacy.provinces[3].q09_officer_head is None
    assert legacy.validate()==[]
