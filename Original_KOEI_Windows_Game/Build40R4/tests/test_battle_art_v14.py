"""Full 41x4 original-game battle art integration acceptance (not NES ROM parity)."""
import copy, os
from pathlib import Path
import pytest
from PIL import Image
from third_branch.battle_assets import (metadata, province_grid, battle_grid, season_for_month,
    background, asset_path, static_audit, terrain_matches, ASSET_VERSION, ROOT)
from third_branch.integrated import Campaign, new_campaign_41, save_bytes, load_bytes
from third_branch.tactical_view import render_battle, X0,Y0,CELL


def begin_target(n):
    """Construct a valid independent test scenario, keep original adjacency, start real attack."""
    c=new_campaign_41();s=c.state
    dst=f'p{n}';src=s['neighbors'][dst][0]
    s['owners'][src]='A';s['owners'][dst]='B'
    for oid,off in s['core']['officers'].items():
        if off['province_id']==src:s['officer_faction'][oid]='A'
        if off['province_id']==dst:s['officer_faction'][oid]='B'
    c=Campaign(s)
    actor=next(oid for oid,off in c.state['core']['officers'].items() if off['province_id']==src)
    result=c.do('attack',actor,{'target':dst,'troops':80},f'battle:v14:{n}')
    assert result['battle_started'] and result['map']==ASSET_VERSION
    assert c.state['battle']['dst']==dst
    return c


def test_source_alignment_and_every_asset():
    results=static_audit()
    assert results['maps']==41*4
    assert results['unverified_cells']==[
        {'province':41,'x':11,'y':11,'assumed':4},
        {'province':41,'x':12,'y':11,'assumed':4}]
    assert province_grid(1)[2][9]==6
    assert province_grid(1)[2][11]!=6  # old offset incorrectly placed castle here
    assert metadata()['corrected_byte_offset']=='0x26FF8'


@pytest.mark.parametrize('province',range(1,42))
def test_each_real_province_starts_and_saves_correct_map(province):
    c=begin_target(province)
    assert c.state['battle']['terrain']==battle_grid(province)
    raw=save_bytes(c)
    restored=load_bytes(raw)
    assert restored.fingerprint==c.fingerprint
    assert restored.state['battle']['map_source']==ASSET_VERSION
    assert terrain_matches(province,restored.state['battle']['terrain'])
    frame=render_battle(restored)
    assert frame.size==(1024,768)
    bg=background(province,'spring').resize((13*CELL,12*CELL),Image.Resampling.NEAREST)
    # A tile with no army overlay is pixel-identical to the actual 41x4 asset.
    assert frame.getpixel((X0+3*CELL+4,Y0+3*CELL+4))==bg.getpixel((3*CELL+4,3*CELL+4))


def test_all_four_seasons_are_live_and_distinct():
    c=begin_target(15)
    seen=set()
    for month in (1,4,7,10):
        # Renderer reads calendar-derived month; use a presentation-only clone here.
        snapshot=c.clone();snapshot.state['month']=month
        frame=render_battle(snapshot)
        seen.add(frame.crop((X0,Y0,X0+13*CELL,Y0+12*CELL)).tobytes())
        assert season_for_month(month) in ('spring','summer','autumn','winter')
    assert len(seen)==4
    # The actual campaign can cross the boundary before initiating a live battle.
    c=new_campaign_41()
    for _ in range(3):c.advance_month()
    assert c.state['month']==4
    # No test should accidentally introduce a winter movement mechanic.
    assert season_for_month(c.state['month'])=='summer'


def test_live_markers_and_no_baked_units():
    c=begin_target(15)
    a=render_battle(c);src=c.state['battle']['a']
    coord=(X0+src['q']*CELL+10,Y0+src['r']*CELL+11)
    before=a.getpixel(coord)
    # Q04: renderer uses real officer records, not the obsolete aggregate marker.
    unit=next(u for u in c.state['battle']['officer_units'] if u['side']=='A')
    unit['troops']=0  # presentation-only mutation, preserved from original test intent
    after=render_battle(c).getpixel(coord)
    assert before!=after
    assert after==background(15,'spring').resize((13*CELL,12*CELL),Image.Resampling.NEAREST).getpixel((src['q']*CELL+10,src['r']*CELL+11))


def test_corrupt_and_missing_source_fail_closed(monkeypatch,tmp_path):
    c=begin_target(15)
    c.state['battle']['terrain'][0]='W'+c.state['battle']['terrain'][0][1:]
    with pytest.raises(ValueError,match='differs'):render_battle(c)
    assert terrain_matches(15,battle_grid(15))
    background.cache_clear()
    monkeypatch.setattr('third_branch.battle_assets.ROOT',tmp_path)
    with pytest.raises(FileNotFoundError):background(15,'spring')
    with pytest.raises(ValueError,match='Invalid province'):province_grid(42)
    with pytest.raises(ValueError,match='Unknown season'):asset_path(1,'monsoon')
    background.cache_clear()


def test_legacy_battle_renderer_remains_available():
    from third_branch.integrated import new_campaign
    c=new_campaign()
    result=c.do('attack','A3',{'target':'p4','troops':80},'battle:legacy:v14')
    assert c.state['battle']['map_source']=='legacy-seven-province-synthetic'
    assert render_battle(c).size==(1024,768)
