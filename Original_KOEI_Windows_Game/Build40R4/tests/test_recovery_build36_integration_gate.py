"""Hard gate: all gameplay commands must have a real province or tactical UI path."""
from third_branch.integrated import COMMANDS
from third_branch.orders_ui import SPECS, BATTLE_ONLY
from third_branch import tactical_view

def test_main_engine_commands_partition_without_orphans():
    main = {cmd for cmd,_ in SPECS.values()}
    assert main.isdisjoint(BATTLE_ONLY)
    assert main | set(BATTLE_ONLY) == set(COMMANDS)
    assert 'battle_duel_response' in BATTLE_ONLY
    assert 'battle_duel_response' not in main

def test_tactical_duel_response_handler_is_present():
    assert callable(getattr(tactical_view.TacticalWindow,'duel',None))
