import hashlib, json
from pathlib import Path
from PIL import Image
from third_branch.historical_art import manifest, ruler_portrait, seasonal_still, event_still_for_committed_record, EVENT_KIND_TO_IMAGE
from third_branch.historical_s1 import new_campaign_historical_s1
from third_branch.province_stage import snapshot,render_province

def test_all_original_assets_have_valid_bytes_and_dims():
    m=manifest()
    assert [len(m[x]) for x in ('events','seasons','story','rulers')]==[11,4,8,28]
    root=Path(__file__).resolve().parents[1]/'third_branch'
    for cat in ('events','seasons','story','rulers'):
        for rec in m[cat].values():
            p=root/rec['path'];assert p.is_file()
            assert hashlib.sha256(p.read_bytes()).hexdigest()==rec['sha256']
            with Image.open(p) as im:
                im.verify();assert list(im.size)==rec['dimensions']

def test_historical_owner_is_actual_ruler_not_generic_role():
    c=new_campaign_historical_s1(balance_policy='OG_1000_DIRECTOR_V2')
    vm=snapshot(c,9)
    assert vm.ruler_nes_id==0 and vm.ruler_name=='Cao Cao'
    assert ruler_portrait(vm.ruler_nes_id).size==(40,48)
    assert snapshot(c,19).ruler_nes_id==4
    assert snapshot(c,1).ruler_nes_id is None
    assert render_province(vm).size==(960,720)

def test_seasons_and_events_fail_closed():
    for s in ('Spring','Summer','Autumn','Winter'):
        assert seasonal_still(s).size==(40,48)
    for k in EVENT_KIND_TO_IMAGE:
        assert event_still_for_committed_record({'kind':k,'province_id':'p9'},9).size==(56,40)
        assert event_still_for_committed_record({'kind':k,'province_id':'p9'},10) is None
    for kind in ('month','attack','distribute_goods','order','nonsense'):
        assert event_still_for_committed_record({'kind':kind,'province_id':'p9'},9) is None
    assert ruler_portrait(9999) is None

def test_snapshot_only_displays_recorded_matched_event():
    c=new_campaign_historical_s1(balance_policy='OG_1000_DIRECTOR_V2')
    before=c.fingerprint
    assert snapshot(c,9).event_kind is None
    c.state['log'].append({'id':'test-fixture','kind':'flood','province_id':'p9'})
    assert snapshot(c,9).event_kind=='flood'
    assert snapshot(c,10).event_kind is None
    c.state['log'].append({'id':'other','kind':'month','month':1})
    assert snapshot(c,9).event_kind is None


def test_art_mutation_is_rejected_before_display(tmp_path,monkeypatch):
    from third_branch import historical_art as ha
    import copy
    original=ha.manifest()
    sample=original['events']['event_03_flood']
    source=Path(__file__).resolve().parents[1]/'third_branch'/sample['path']
    changed=tmp_path/'flood.png';changed.write_bytes(source.read_bytes())
    with Image.open(changed) as im:
        modified=im.convert('RGB')
    modified.putpixel((0,0),(255,0,0));modified.save(changed)
    assert changed.stat().st_size>0
    def spoof():
        fake=copy.deepcopy(original)
        fake['events']['event_03_flood']['path']=str(changed)
        return fake
    # Absolute out-of-bundle path rejected, even if file is a valid PNG.
    monkeypatch.setattr(ha,'manifest',spoof)
    assert ha.event_still_for_committed_record({'kind':'flood'}) is None
