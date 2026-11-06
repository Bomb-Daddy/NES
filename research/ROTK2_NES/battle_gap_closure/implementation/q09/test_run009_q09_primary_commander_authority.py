"""Run009 Q09 primary-slot commander authority -- original shared-source D905/D89D.

This is a port regression against decoded original-source behavior (VERIFIED_STATIC),
not a claim of a fresh Mesen measurement or finished Q09 RNG/capture parity.
"""
from rotk2.state.models import BattleUnit
from rotk2.saves import codec
from test_officer_system_q09_q10_q11_q14_build13 import battle_game, command


def test_noncommander_cannot_forge_whole_army_withdraw_without_state_change():
    e = battle_game()
    before = e.state.state_hash()
    draws = e.state.battle.combat_draws
    result = command(e, 'battle_withdraw', destination=1, whole_army=True, officer_id=3)
    assert not result.success and result.error_code == 'ONLY_COMMANDER_WHOLE_WITHDRAW'
    assert e.state.state_hash() == before
    assert e.state.battle.combat_draws == draws
    assert e.state.validate() == []


def test_opposing_commander_cannot_forge_player_army_withdraw():
    e = battle_game()
    before = e.state.state_hash()
    result = command(e, 'battle_withdraw', destination=1, whole_army=True, officer_id=2)
    assert not result.success and result.error_code == 'ONLY_COMMANDER_WHOLE_WITHDRAW'
    assert e.state.state_hash() == before


def test_main_commander_retains_whole_army_withdraw_and_cold_load(tmp_path):
    e = battle_game()
    result = command(e, 'battle_withdraw', destination=1, whole_army=True, officer_id=1)
    assert result.success, (result.error_code, result.events)
    assert e.state.validate() == []
    path = tmp_path / 'run009_main_commander.ro2'
    codec.save(e.state, path)
    reloaded = codec.load(path)
    assert reloaded.state_hash() == e.state.state_hash()
    assert reloaded.validate() == []


def test_nonprimary_reinforcement_commander_is_not_offered_army_choice():
    from test_classic_ui_route_build13 import App
    from rotk2.input.router import Action
    from rotk2.ui.screens import BattleScreen
    from rotk2.ui.widgets import ListPrompt, YesNo
    app = App()
    b = app.engine.state.battle
    b.units.append(BattleUnit(5,'A',3,4,50,slot=3,commander=True,mp=6,troops100=5000))
    assert app.engine.state.validate() == []
    screen = BattleScreen()
    screen.sel = 5
    before = app.engine.state.state_hash()
    screen._withdraw_menu(app)
    assert isinstance(app.stack[-1], ListPrompt), type(app.stack[-1]).__name__
    assert not isinstance(app.stack[-1], YesNo)
    app.stack[-1].handle(app, Action.CANCEL)
    assert app.stack == []
    assert app.engine.state.state_hash() == before


def test_main_commander_still_gets_yes_no_menu():
    from test_classic_ui_route_build13 import App
    from rotk2.ui.screens import BattleScreen
    from rotk2.ui.widgets import YesNo
    app = App(); screen = BattleScreen(); screen.sel = 1
    before = app.engine.state.state_hash()
    screen._withdraw_menu(app)
    assert isinstance(app.stack[-1], YesNo)
    assert app.engine.state.state_hash() == before
