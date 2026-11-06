"""Run019 Q09: select_officer_5154 ($5154/$5191) places each escaping officer
at the destination province's officer-list head. Source fixture is the
public English-mechanics SNES root.src, not a fresh hardware NES replay.
"""
from rotk2.battle import engine as bt
from rotk2.saves import codec
from test_run018_q09_army_immediate_source_insertion import _army


def test_native_whole_army_inserts_latest_officer_as_destination_head():
    state, battle = _army()
    # An original head imported from the scenario must not remain stale when
    # two new officers are linked ahead of it in the source's iteration order.
    state.provinces[3].q09_officer_head = 5
    draws = battle.combat_draws
    bt.withdraw(state, battle, 'A', 3, True, officer_id=1,
                officer_destinations={1:3, 3:3})
    assert state.provinces[3].q09_officer_head == 3
    assert state.officers[1].province_id == state.officers[3].province_id == 3
    assert battle.combat_draws == draws
    assert state.validate() == []


def test_native_individual_flee_noncommander_becomes_head_over_existing_garrison():
    state, battle = _army()
    state.provinces[3].q09_officer_head = 5
    battle.side_to_move = 'A'
    bt.withdraw(state, battle, 'A', 3, False, officer_id=3)
    assert state.provinces[3].q09_officer_head == 3
    assert state.officers[5].province_id == 3  # old head is still resident
    assert state.validate() == []


def test_native_attacked_destination_latest_officer_head_survives_disk_coldload(tmp_path):
    state, battle = _army()
    state.provinces[3].under_attack = True
    state.provinces[3].q09_officer_head = 5
    bt.withdraw(state, battle, 'A', 3, True, officer_id=1,
                officer_destinations={1:3, 3:3})
    assert state.provinces[3].q09_officer_head == 3
    assert state.officers[3].soldiers == 7455
    path = tmp_path / 'q09_head_order.ro2'
    codec.save(state, path)
    cold = codec.load(path)
    assert cold.provinces[3].q09_officer_head == 3
    assert cold.state_hash() == state.state_hash() and cold.validate() == []


def test_legacy_unknown_source_flee_does_not_fabricate_live_link():
    state, battle = _army()
    state.officers[3].q09_source_flag2 = None
    state.provinces[3].q09_officer_head = 5
    bt.withdraw(state, battle, 'A', 3, False, officer_id=3)
    assert state.provinces[3].q09_officer_head == 5
    assert state.officers[3].province_id == 1


def test_bad_controller_independent_destination_never_changes_link_head():
    import pytest
    state, battle = _army()
    state.provinces[3].q09_officer_head = 5
    before = state.state_hash()
    with pytest.raises(ValueError, match='ILLEGAL_WITHDRAW_DESTINATION'):
        bt.withdraw(state, battle, 'A', 3, True, officer_id=1,
                    officer_destinations={1:3, 3:2})
    assert state.state_hash() == before


def test_controller_prompts_insert_second_officer_as_new_head():
    from test_classic_ui_route_build13 import App
    from rotk2.input.router import Action
    from rotk2.ui.screens import BattleScreen
    from rotk2.ui.widgets import YesNo, ListPrompt
    app = App()
    state = app.engine.state
    state.provinces[3].q09_officer_head = 5
    for oid in (1, 3):
        state.officers[oid].q09_source_flag2 = 0x40
    screen = BattleScreen()
    screen.sel = 1
    app.stack.append(screen)
    screen._withdraw_menu(app)
    assert isinstance(app.stack[-1], YesNo)
    app.stack[-1].handle(app, Action.CONFIRM)
    for _ in range(2):
        menu = app.stack[-1]
        assert isinstance(menu, ListPrompt)
        while menu.menu.current() != 3:
            menu.handle(app, Action.DOWN)
        menu.handle(app, Action.CONFIRM)
    assert state.provinces[3].q09_officer_head == 3
    assert state.validate() == []
