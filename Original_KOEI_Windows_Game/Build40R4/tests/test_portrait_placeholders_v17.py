"""Director's v1.7 neutral ruler/advisor/general pictures (no invented IDs)."""
import hashlib
from pathlib import Path
import pytest
from PIL import Image
from third_branch.integrated import new_campaign_41
from third_branch.province_stage import (
    PORTRAIT_DIR, PORTRAIT_ROLES, get_role_portrait, snapshot, render_province
)


def test_three_role_images_are_real_pngs_and_distinct():
    signatures=[]
    for role in PORTRAIT_ROLES:
        file=PORTRAIT_DIR/f'{role}_placeholder.png'
        image, origin, placeholder=get_role_portrait(role)
        assert file.is_file() and origin==str(file) and placeholder is True
        assert image.size == (128,160)
        with Image.open(file) as check:
            assert check.format=='PNG'
            check.verify()
        signatures.append(hashlib.sha256(image.tobytes()).hexdigest())
    assert len(set(signatures))==3


def test_role_placeholder_fallback_on_missing_or_bad_files(tmp_path,caplog):
    for role in PORTRAIT_ROLES:
        for path in [tmp_path/'missing.png',tmp_path/f'broken_{role}.png']:
            if path.name.startswith('broken_'):path.write_bytes(b'not PNG')
            img,origin,placeholder=get_role_portrait(role,path)
            assert origin=='procedural-neutral-portrait-fallback'
            assert img.size==(128,160) and placeholder
    assert 'unavailable' in caplog.text
    with pytest.raises(ValueError):get_role_portrait('historical_person_unknown')


def test_ruler_advisor_and_general_appear_with_no_state_changes():
    campaign=new_campaign_41()
    initial=campaign.fingerprint
    for province in (1,4,40,41):
        vm=snapshot(campaign,province)
        background=render_province(vm)
        assert background.size==(960,720)
        for x,role in [(651,'ruler'),(802,'advisor')]:
            expected=get_role_portrait(role)[0].resize((128,152),Image.Resampling.NEAREST)
            # Check an interior swatch of the displayed role portrait, not the panel border.
            assert background.crop((x+10,58,x+110,178)).tobytes()==expected.crop((10,10,110,130)).tobytes()
        assert 'Unassigned' not in vm.faction  # do not pretend anyone was appointed
        assert all(off[0].startswith('O') for off in vm.officers)
    assert campaign.fingerprint==initial


def test_speaker_general_image_is_contextual_and_not_an_identity():
    c=new_campaign_41()
    idle=render_province(snapshot(c,1))
    speaking=render_province(snapshot(c,1,'OFFICER SPEAKING:O01'))
    assert idle.crop((192,560,345,667)).tobytes()!=speaking.crop((192,560,345,667)).tobytes()
    expected=get_role_portrait('general')[0].resize((153,107),Image.Resampling.NEAREST)
    assert speaking.getpixel((200,570))==expected.getpixel((8,10))
    assert c.fingerprint==new_campaign_41().fingerprint


def test_officer_general_icons_are_not_rendered_on_permanent_province_panel():
    c=new_campaign_41();vm=snapshot(c,1)
    frame=render_province(vm)
    icon=get_role_portrait('general')[0].resize((22,24),Image.Resampling.NEAREST)
    assert frame.crop((190,330,212,354)).tobytes()!=icon.tobytes()  # roster explicitly prohibited
