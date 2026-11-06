"""Q09 original-source-backed Open territory Flee ownership regression.

Original English NES P20->open P29 retains Yuan Shu faction and claims province.
This is a TESTED_PORT exercise, not a fresh original-emulator measurement.
"""
import pytest
from rotk2.battle import engine as bt
from rotk2.state.models import GameState
from rotk2.state.rng import RNG
from test_build25_q09_conservation import fixture


def open_destination():
    state, battle = fixture()
    battle.side_to_move = 'A'
    state.provinces[3].owner = None
    state.officers[7].province_id = 1
    state.officers[7].soldiers = 0
    assert state.validate() == []
    return state, battle


def test_open_territory_flee_preserves_faction_and_establishes_owner():
    s, b = open_destination()
    assert 3 in bt.retreat_destinations(s,b,'A')
    bt.withdraw(s,b,'A',3,whole_army=True)
    assert b.result=='DEFENDER' and b.retreat_destinations['5']==3
    cold=GameState.from_dict(s.to_dict())
    assert cold.state_hash()==s.state_hash()
    bt.resolve_into_campaign(cold,[],RNG(9))
    assert cold.provinces[3].owner == 5
    assert cold.officers[5].ruler_id == 5
    assert cold.officers[5].province_id == 3
    assert cold.officers[5].soldiers == 100
    assert cold.provinces[2].owner == 6
    assert cold.validate()==[]
    assert GameState.from_dict(cold.to_dict()).state_hash()==cold.state_hash()


def test_friendly_flee_does_not_change_other_owners_or_flags():
    s,b=fixture();b.side_to_move='A'
    s.provinces[3].under_attack=True
    bt.withdraw(s,b,'A',1,whole_army=True)
    bt.resolve_into_campaign(s,[],RNG(9))
    assert s.provinces[1].owner==5 and s.provinces[3].owner==5
    assert s.provinces[3].under_attack is True
    assert s.officers[5].province_id==1 and s.officers[5].ruler_id==5
    assert s.validate()==[]


def test_enemy_destination_rejected_without_state_change():
    s,b=open_destination()
    s.provinces[3].owner=6
    before=s.state_hash()
    with pytest.raises(ValueError,match='ILLEGAL_WITHDRAW_DESTINATION'):
        bt.withdraw(s,b,'A',3,whole_army=True)
    assert s.state_hash()==before
