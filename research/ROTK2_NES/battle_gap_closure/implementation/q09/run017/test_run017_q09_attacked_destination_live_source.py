"""Run017 regression: hsovl2 D69A-D6A5 source under-attack insertion.

The original writer divides combat troops by 100 ONLY if the destination
province +F == FF. The active-siege branch keeps battle-scaled troops.
Approved original source semantics; original emulator replay not run.
"""
from rotk2.battle import engine as bt
from rotk2.saves import codec
from rotk2.state.rng import RNG
from test_officer_system_q09_q10_q11_q14_build13 import battle_game, command


def _setup():
    engine = battle_game()
    state, battle = engine.state, engine.state.battle
    officer = state.officers[3]
    officer.q09_source_flag2 = 0x40
    unit = next(u for u in battle.units if u.officer_id == 3)
    unit.troops100 = 7455
    state.provinces[3].under_attack = True
    return engine, state, battle, officer, unit


def test_live_attacked_destination_keeps_original_battle_scale_and_inserts(tmp_path):
    engine, state, battle, officer, unit = _setup()
    before = battle.combat_draws
    outcome = command(engine, 'battle_withdraw', destination=3,
                      whole_army=False, officer_id=3)
    assert outcome.success, (outcome.error_code, outcome.events)
    assert battle.result is None and not battle.committed
    assert unit.fled and unit.alive
    assert officer.province_id == 3
    assert officer.soldiers == 7455
    assert battle.combat_draws == before
    assert state.provinces[3].under_attack
    assert state.validate() == []
    file = tmp_path / 'q09_live_attacked.ro2'
    codec.save(state, file)
    cold = codec.load(file)
    assert cold.state_hash() == state.state_hash()
    assert cold.officers[3].soldiers == 7455
    assert cold.officers[3].province_id == 3


def test_source_inserted_troops_survive_unrelated_battle_settlement():
    engine, state, battle, officer, unit = _setup()
    assert command(engine, 'battle_withdraw', destination=3,
                   whole_army=False, officer_id=3).success
    battle.result = 'DEFENDER'
    bt.resolve_into_campaign(state, [], RNG(1717))
    assert officer.province_id == 3
    assert officer.soldiers == 7455
    assert battle.committed and state.validate() == []


def test_peaceful_destination_still_divides_once_and_survives_settlement():
    engine, state, battle, officer, unit = _setup()
    state.provinces[3].under_attack = False
    assert command(engine, 'battle_withdraw', destination=3,
                   whole_army=False, officer_id=3).success
    assert officer.soldiers == 74
    battle.result = 'DEFENDER'
    bt.resolve_into_campaign(state, [], RNG(1718))
    assert officer.soldiers == 74 and officer.province_id == 3
    assert state.validate() == []


def test_source_unknown_retains_explicit_legacy_fallback():
    engine, state, battle, officer, unit = _setup()
    officer.q09_source_flag2 = None
    old = (officer.province_id, officer.soldiers)
    assert command(engine, 'battle_withdraw', destination=3,
                   whole_army=False, officer_id=3).success
    assert (officer.province_id, officer.soldiers) == old
    assert battle.retreat_destinations['3'] == 3


def test_actual_software_controller_attacked_destination_visible_in_battle_ui():
    from test_classic_ui_route_build13 import App
    from rotk2.input.router import Action
    from rotk2.ui.screens import BattleScreen
    from rotk2.ui.widgets import ListPrompt
    app = App()
    state = app.engine.state
    state.officers[3].q09_source_flag2 = 0x40
    state.provinces[3].under_attack = True
    unit = next(u for u in state.battle.units if u.officer_id == 3)
    unit.troops100 = 7455
    screen = BattleScreen()
    screen.sel = 3
    screen._withdraw_menu(app)
    assert isinstance(app.stack[-1], ListPrompt)
    while app.stack[-1].menu.current() != 3:
        app.stack[-1].handle(app, Action.DOWN)
    app.stack[-1].handle(app, Action.CONFIRM)
    assert state.battle.result is None
    assert state.officers[3].province_id == 3
    assert state.officers[3].soldiers == 7455
    assert state.validate() == []
