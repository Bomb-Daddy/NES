"""Cross-mode ROM-backed 41 province tactical map and actual officer placement."""
import os
from pathlib import Path
import pytest
from third_branch.integrated import new_campaign_41,save_bytes,load_bytes
from third_branch import rom_battle_map

ROM = Path(os.environ['ROTK2_TEST_ROM']) if os.environ.get('ROTK2_TEST_ROM') else None
requires_private_rom = pytest.mark.skipif(ROM is None or not ROM.is_file(),
    reason='NOT RUN: private original NES ROM path ROTK2_TEST_ROM unavailable; ROM byte parity unverified')

@requires_private_rom
def test_all_41_terrain_and_regions_match_owned_original_rom():
    from rotk2.battle import nes_terrain
    original = nes_terrain.load_all(ROM)
    for m in original:
        e = rom_battle_map.province(m.province_id)
        assert e['terrain'] == [''.join('.FHM WTC'.replace(' ','')[v] for v in row) for row in m.terrain]
        assert e['neighbors'] == list(m.neighbors)
        assert e['zones'] == [''.join(str(v) for v in row) for row in m.zones]
        assert tuple(e['castle']) == m.castle_xy

def test_real_campaign_places_officers_in_original_regions_and_survives_cold_load():
    c=new_campaign_41(123)
    c.do('attack','O14',{'target':'p15','troops':100},'q04:attack')
    b=c.state['battle']; assert b['terrain_source']=='NES_ROM_VERIFIED_41'
    assert b['terrain']==rom_battle_map.province(15)['terrain']
    assert b['src']=='p14'
    for u in b['officer_units']:
        assert rom_battle_map.deployment_province(15,u['q'],u['r']) == (14 if u['side']=='A' else 15)
        assert b['terrain'][u['r']][u['q']] != 'M'
    assert len({(u['q'],u['r']) for u in b['officer_units']})==len(b['officer_units'])
    raw=save_bytes(c); restored=load_bytes(raw)
    assert restored.fingerprint==c.fingerprint
    assert restored.state['battle']['officer_units']==b['officer_units']

def test_original_map_invalid_province_rejected():
    with pytest.raises(ValueError):rom_battle_map.province(0)
    with pytest.raises(ValueError):rom_battle_map.province(42)

def test_reinforcement_enters_original_source_zone_and_cannot_teleport():
    from third_branch import officer_battle_adapter as ba
    from copy import deepcopy
    c=new_campaign_41(321)
    c.do('attack','O14',{'target':'p15','troops':100},'q04:reinforce')
    b=deepcopy(c.state['battle'])
    cell=ba._reinforcement_spot(b,'A','p14')
    assert cell is not None
    assert rom_battle_map.deployment_province(15,*cell)==14
    assert b['terrain'][cell[1]][cell[0]]!='M'
    # Fill the entire region with actual occupied officer cells: no illegal
    # fallback outside the source region, no replacement of another officer.
    for i,(x,y) in enumerate(rom_battle_map.legal_cells(15,14)):
        u=deepcopy(b['officer_units'][0]);u.update(id=f'blocked{i}',q=x,r=y)
        b['officer_units'].append(u)
    assert ba._reinforcement_spot(b,'A','p14') is None

def test_rom_bundle_tamper_rejected_without_fallback(tmp_path,monkeypatch):
    raw=rom_battle_map.BUNDLE_PATH.read_bytes()
    tampered=tmp_path/'nes_original_41.json'
    tampered.write_bytes(raw.replace(b'"schema":"NES_ROM_VERIFIED_41"',b'"schema":"FAKE_ROM_VERIFIED_41"'))
    rom_battle_map._bundle.cache_clear()
    monkeypatch.setattr(rom_battle_map,'BUNDLE_PATH',tampered)
    try:
        with pytest.raises(ValueError,match='SHA-256'):
            rom_battle_map.province(15)
    finally:
        rom_battle_map._bundle.cache_clear()

@requires_private_rom
def test_independent_extraction_script_byte_matches_installed_bundle(tmp_path):
    from third_branch.tools.build_original_q04_bundle import extract
    import json,hashlib
    regenerated=(json.dumps(extract(ROM.read_bytes()),sort_keys=True,separators=(',',':'))+'\n').encode()
    assert regenerated==rom_battle_map.BUNDLE_PATH.read_bytes()
    assert hashlib.sha256(regenerated).hexdigest()==rom_battle_map.BUNDLE_SHA256
