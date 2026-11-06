"""NES Q03 nondeployed defending officers eat rice, with real Enhanced army ledger."""
import pytest
from og.core.world import add_officer
from third_branch.integrated import new_campaign, load_bytes, save_bytes, validate
from third_branch import army_cohorts as ac
from third_branch import officer_battle_adapter as ba
from third_branch import rotk2_battle_core as core


def new_active_battle(seed=9103):
    c = new_campaign(seed)
    c.do('attack', 'A3', {'target': 'p5', 'troops': 120, 'gold': 1}, 'q03:attack')
    return c


def station_resident_defender(c, troops=10):
    s = c.state
    add_officer(s['core'], 'B3', 'p5', (50, 55, 50, 50, 50))
    s['officer_faction']['B3'] = 'B'
    s['officer_loyalty']['B3'] = 65
    ac.add(s, 'B3', 'p5', troops)
    ac.refresh(s)
    assert not any(u['id'] == 'B3' for u in s['battle']['officer_units'])
    return s


def test_original_q03_nonfielded_defender_is_counted_from_actual_station():
    c = new_active_battle()
    s = station_resident_defender(c)
    b = s['battle']
    before = b['rice_d']
    field = sum(u['troops100'] for u in ba._all(b, 'D'))
    resident = ac.strength(s, 'B3', b['dst']) * 100
    expected = core.rice_tick(before, field, resident)
    assert resident == 1000
    assert expected < core.rice_tick(before, field)
    assert validate(s)
    c.do('battle_end_turn','A3',{},'q03:day1')
    assert c.state['battle']['rice_d'] == expected
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint == c.fingerprint


def test_no_double_count_when_defender_is_deployed():
    c = new_active_battle(9104)
    s = c.state
    b = s['battle']
    assert ba._q03_resident_defenders_troops100(s,b) == 0
    original = b['rice_d']
    original_field = sum(u['troops100'] for u in ba._all(b,'D'))
    c.do('battle_end_turn','A3',{},'q03:day1')
    assert c.state['battle']['rice_d'] == core.rice_tick(original, original_field)


def test_reject_unrelated_remote_troops_even_when_faction_matches():
    c = new_active_battle(9105)
    s = c.state
    add_officer(s['core'], 'B3', 'p4', (50, 55, 50, 50, 50))
    s['officer_faction']['B3'] = 'B'
    s['officer_loyalty']['B3'] = 65
    ac.add(s, 'B3', 'p4', 10)
    ac.refresh(s)
    assert ba._q03_resident_defenders_troops100(s,s['battle']) == 0


def test_source_numeric_food_boundary():
    assert core.rice_tick(45000,19000,1000) == 44334
    assert core.rice_tick(45000,19000,0) == 44367
