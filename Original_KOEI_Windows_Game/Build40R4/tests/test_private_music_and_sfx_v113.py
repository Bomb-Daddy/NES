"""Prove original third-branch music/SFX routes without claiming speaker or Windows proof."""
import json, wave
from pathlib import Path
import pytest
from third_branch import audio_system as au


def test_all_user_supplied_mp3_hashes_and_original_sfx():
    if not au.TRACKS: pytest.skip("optional private music package is not installed")
    report=au.verify_private_assets()
    assert report['ok'],report['errors']
    assert report['mp3_count']==15
    assert report['sfx_count']>=14
    for path in au.SFX.values():
        with wave.open(str(path),'rb') as f:
            assert f.getframerate()==22050 and f.getnchannels()==1 and f.getsampwidth()==2
            assert f.getnframes()>600


def test_all_five_regions_and_event_cues_are_distinct_and_deterministic():
    assert [au.track_for('map',p) for p in (1,8,19,29,39)]==[7,13,9,3,4]
    for scene,n in [('opening',1),('prologue',2),('defeat',5),('diplomacy',6),('death',8),
                    ('marriage',10),('diaochan',11),('demise',12),('battle',14),('unification',15)]:
        assert au.track_for(scene)==n
    assert len({au.track_for('map',p) for p in range(1,42)})==5
    assert [au.track_for('track:'+str(i)) for i in range(1,16)]==list(range(1,16))
    assert au.track_for('track:16') is None
    with pytest.raises(ValueError):au.track_for('map',42)


def test_sfx_stays_separate_from_bgm_and_audio_does_not_touch_campaign():
    if not au.TRACKS: pytest.skip("optional private music package is not installed")
    from third_branch.historical_s1 import new_campaign_historical_s1
    c=new_campaign_historical_s1(seed=17,balance_policy='OG_1000_DIRECTOR_V2')
    hash_before=c.fingerprint
    audio=au.GameAudio(backend='mock')
    assert audio.play('map',1) and audio.current==(7,True)
    assert audio.effect('cursor') and audio.current==(7,True)
    assert audio.play('battle') and audio.current==(14,True)
    assert audio.effect('battle')
    assert audio.play('map',1) and audio.current==(7,True)
    assert audio.play('diplomacy',loop=False) and audio.current==(6,False)
    audio.mode_set('off',1)
    assert not audio.play('map',1) and audio.current is None
    audio.effects_set(False)
    assert not audio.effect('trade')
    audio.effects_set(True)
    assert audio.effect('confirm')
    audio.close()
    assert c.fingerprint==hash_before
    assert len([x for x in audio.log if x['type']=='music'])==4


def test_silent_missing_music_and_backend_fail_visible(monkeypatch):
    audio=au.GameAudio(backend='no_backend')
    assert not audio.play('map',1)
    assert 'MP3 music unavailable' in audio.error or 'Missing private soundtrack track' in audio.error
    assert audio.effect('confirm') is False
    monkeypatch.setitem(au.TRACKS,7,Path('/nonexistent/missing.mp3'))
    assert not au.GameAudio(backend='mock').play('map',1)
    assert 'Private soundtrack files missing' in au.GameAudio(backend='mock').status()
    audio.close()


def test_pause_menu_options_use_user_controls_not_game_state():
    from third_branch.strategic_game import NesPauseMenu
    class FakeAudio:
        mode='arranged';sfx_enabled=True
        def status(self):return 'Mock playback'
    class FakeParent: audio=FakeAudio()
    class FakePause: parent=FakeParent();mode='main'
    v=NesPauseMenu.entries(FakePause())
    assert v[:6]==['Resume','Save Game','Load Game','Continue Last Save','Return to Main Menu','Exit Game']
    assert v[6]=='Music: Arranged' and v[7]=='Sound Effects: On' and v[8]=='Audio Status'
    assert v[9]=='Soundtrack Library (15 tracks)'
