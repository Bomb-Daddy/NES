"""Q04 English NES source-region invariant for *partially* staged battles.

Source: bundled English NES ROM-backed 41-province region records at $28934,
plus original Q04 hsovl6 placement validation $D0CF/$CE5C. A human-confirmed
unit in an unfinished placement phase must still occupy a legal, unburning,
unoccupied original entry cell. This does not assert NES cursor timing or pixels.
"""
import copy
import pytest
from og.core.fixed import CoreError
from third_branch.integrated import validate, save_bytes, load_bytes
from third_branch import rom_battle_map
from test_run044_q04_uncommitted_occupancy import two_defenders, unit


def partially_staged(seed):
    c = two_defenders(seed)
    b = c.state['battle']
    preview = unit(b, 'O15B')
    q, r = preview['q'], preview['r']
    c.do('battle_deploy', 'O15', {'officer_id':'O15','q':q,'r':r}, f'run045:stage:{seed}')
    assert c.state['battle']['q04_pending_deployment'] == ['O15B']
    return c


def test_partial_load_rejects_committed_player_off_original_legal_zone():
    c = partially_staged(4501)
    bad = copy.deepcopy(c.state)
    b = bad['battle']
    loc = next((x,y) for y in range(12) for x in range(13)
               if b['terrain'][y][x] != 'M'
               and rom_battle_map.deployment_province(15,x,y) not in (15,None))
    unit(b, 'O15')['q'],unit(b, 'O15')['r'] = loc
    with pytest.raises(CoreError, match='Q04_COMMITTED_DEPLOYMENT_INVALID'):
        validate(bad)


def test_partial_load_rejects_committed_player_stacked_on_real_enemy():
    c = partially_staged(4502)
    bad = copy.deepcopy(c.state)
    b = bad['battle']
    enemy = unit(b,'O14')
    unit(b,'O15')['q'],unit(b,'O15')['r'] = enemy['q'], enemy['r']
    with pytest.raises(CoreError, match='Q04_COMMITTED_DEPLOYMENT_INVALID'):
        validate(bad)


def test_partial_load_rejects_fire_on_already_committed_player():
    c = partially_staged(4503)
    bad = copy.deepcopy(c.state)
    b = bad['battle']
    placed = unit(b,'O15')
    b['burning'].append([placed['q'],placed['r']])
    with pytest.raises(CoreError, match='Q04_COMMITTED_DEPLOYMENT_INVALID'):
        validate(bad)


def test_source_legal_partially_staged_disk_save_and_cold_load_unchanged(tmp_path):
    c = partially_staged(4504)
    before = save_bytes(c)
    path = tmp_path/'partial.ro2'
    path.write_bytes(before)
    cold = load_bytes(path.read_bytes())
    assert validate(cold.state)
    assert cold.fingerprint == c.fingerprint
    assert save_bytes(cold) == before
    b = cold.state['battle']
    placed = unit(b,'O15')
    assert rom_battle_map.deployment_province(15,placed['q'],placed['r'])==15
    assert b['q04_pending_deployment']==['O15B']


def test_finished_player_deployment_is_not_illegally_tied_to_staging_zone():
    c = partially_staged(4505)
    b = c.state['battle']
    occupied={(u['q'],u['r']) for u in b['officer_units'] if u['id']!='O15B'}
    q,r=rom_battle_map.legal_cells(15,15,occupied=occupied,burning=b['burning'])[0]
    c.do('battle_deploy','O15B',{'officer_id':'O15B','q':q,'r':r},'run045:finish')
    assert not c.state['battle']['q04_pending_deployment']
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
