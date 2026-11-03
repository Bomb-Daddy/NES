"""Director v1.1 province layout: real-world bindings, one art source, no shadow simulation."""
import hashlib
from pathlib import Path
import pytest
from third_branch.integrated import new_campaign_41
from third_branch.province_stage import (
    ART,NAV,SEASONS,WORKERS,get_province_capital_art,render_province,snapshot,fmt
)


def test_all_164_lookups_share_the_identical_placeholder_and_no_state_mutation():
    c=new_campaign_41();old=c.fingerprint
    hashes=set()
    for i in range(1,42):
        for season in SEASONS:
            img,path,placeholder=get_province_capital_art(i,season)
            assert placeholder is True
            assert Path(path)==ART
            hashes.add(hashlib.sha256(img.tobytes()).hexdigest())
    assert len(hashes)==1
    assert c.fingerprint==old


def test_missing_and_corrupt_asset_fallback(tmp_path,caplog):
    for path in (tmp_path/'missing.png',tmp_path/'bad.png'):
        if path.name=='bad.png':path.write_bytes(b'not an image')
        img,source,placeholder=get_province_capital_art(40,'Winter',path)
        assert img.size==(360,218) and placeholder is True
        assert source=='procedural-neutral-fallback'
    assert 'unavailable' in caplog.text


def test_out_of_range_province_or_season_is_rejected():
    for pid in (0,-1,42,'1',True):
        with pytest.raises(ValueError):get_province_capital_art(pid,'Spring')
        with pytest.raises(ValueError):snapshot(new_campaign_41(),pid)
    with pytest.raises(ValueError):get_province_capital_art(1,'Sumer')


def test_view_model_reads_live_worker_and_officer_ledgers_without_double_counting():
    c=new_campaign_41();old=c.fingerprint
    vm=snapshot(c,1)
    source=c.state['core']['provinces']['p1']
    assert vm.id==1 and vm.faction==c.state['owners']['p1']
    assert dict(vm.workers)['Farmers']==source['workers']['farmers']
    assert sum(n for _,n in vm.workers)==sum(source['workers'][k] for k in ('farmers','town','artisans','metalworkers','resources','land','flood'))
    assert 'Unassigned' not in dict(vm.workers)
    assert dict(vm.metrics)['Soldiers']==source['troops']
    assert 'Masons' not in dict(vm.metrics)
    assert 'Dependents' not in dict(vm.metrics)
    assert 'Military Readiness' not in NAV
    assert 'Troop Assignments' in NAV and 'Military' in NAV
    assert len(vm.officers)>=1
    assert vm.officers[0][0]=='O01' and vm.officers[0][2]==c.state['core']['officers']['O01']['bp']
    assert c.fingerprint==old


def test_4_and_40_do_not_alias_and_foreign_province_inspection_is_read_only():
    c=new_campaign_41()
    assert snapshot(c,4).id==4 and snapshot(c,40).id==40
    assert snapshot(c,4).fingerprint==snapshot(c,40).fingerprint
    assert snapshot(c,40).faction==c.state['owners']['p40']
    assert snapshot(c,41).faction==c.state['owners']['p41']
    fingerprint=c.fingerprint
    for i in range(1,42):
        im=render_province(snapshot(c,i))
        assert im.size==(960,720)
    assert c.fingerprint==fingerprint


def test_live_world_command_and_month_refresh_projection():
    c=new_campaign_41()
    before=snapshot(c,1)
    c.do('allocate_workers','O01',{'province_id':'p1','source':'unassigned','target':'farmers','count':2},'province-ui-v16-test')
    after=snapshot(c,1)
    assert after.revision==before.revision+1
    assert after.fingerprint!=before.fingerprint
    assert dict(after.workers)['Farmers']>dict(before.workers)['Farmers']
    for _ in range(3):c.advance_month()
    summer=snapshot(c,1)
    assert summer.season=='Summer' and summer.date_month==4
    assert get_province_capital_art(1,'Summer')[1]==get_province_capital_art(40,'Spring')[1]


def test_display_never_invents_unknown_figures():
    assert fmt(None)=='Not tracked'
    c=new_campaign_41();v=snapshot(c,41)
    assert all(x not in dict(v.metrics) for x in ('Residents','Defense','Masons'))
