"""Q09->Q14 cross-rule: successful ruler Flee moves ruler-owned province
list head to the selected home. Port test for decoded source root $5154/$51BD.
"""
from rotk2.battle import engine as bt
from rotk2.state.models import GameState
from rotk2.state.rng import RNG
from test_run005_q09_open_territory_ownership import open_destination
from test_build25_q09_conservation import fixture


def test_fleeing_ruler_into_open_territory_becomes_new_owned_list_head():
    s,b=open_destination()
    s.officers[5].is_ruler=True
    s.rulers[5].q14_owned_head=1
    s.rulers[6].q14_owned_head=2
    bt.withdraw(s,b,'A',3,whole_army=True)
    saved=GameState.from_dict(s.to_dict())
    bt.resolve_into_campaign(saved,[],RNG(9))
    assert saved.provinces[3].owner == 5
    assert saved.rulers[5].q14_owned_head == 3
    assert saved.rulers[6].q14_owned_head == 2
    assert saved.officers[5].province_id == 3
    assert GameState.from_dict(saved.to_dict()).state_hash() == saved.state_hash()
    assert saved.validate()==[]


def test_fleeing_ruler_to_existing_owned_home_promotes_linked_list_head():
    s,b=fixture()
    s.officers[5].is_ruler=True
    s.rulers[5].q14_owned_head=3
    bt.withdraw(s,b,'A',1,whole_army=True)
    bt.resolve_into_campaign(s,[],RNG(9))
    assert s.rulers[5].q14_owned_head == 1
    assert s.provinces[1].owner == 5
    assert s.validate()==[]
