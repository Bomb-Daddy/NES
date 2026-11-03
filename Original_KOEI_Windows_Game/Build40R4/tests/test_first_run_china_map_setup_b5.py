"""Real Tk first-run setup; no default R0/P9, no fake province or uncommitted save."""
import os
import pytest
from third_branch.integrated import load_file

pytestmark=pytest.mark.skipif(not os.environ.get('DISPLAY'),reason='Real Tk display required (run under xvfb-run)')


def test_new_game_map_historical_ruler_cancel_invalid_and_coldload(tmp_path):
    from third_branch.strategic_game import StrategicWindow
    path=tmp_path/'fresh_s1.json'
    game=StrategicWindow(path)
    try:
        game.root.update()
        assert game.new_game_pending is True
        assert game.map_mode=='setup'
        assert game.province_stage is None
        assert not path.exists()
        original=game.c.fingerprint
        # Vacant p1 cannot produce an invented ruler or ownership.
        game.selected=1
        assert game._finish_new_game_setup() is False
        assert not path.exists() and game.c.fingerprint==original
        # 41 provinces remain reachable with controller directions.
        game.selected=41
        game.on_nes_button('right')
        assert game.selected==1
        game.selected=3  # historical R9 territory
        game.on_nes_button('a')
        assert game.ruler_dialog.winfo_exists()
        assert game.map_mode=='setup' and not path.exists()
        # Ruler from a different province cannot be selected here.
        game.ruler_listbox.selection_clear(0,'end')
        game.ruler_listbox.selection_set(0)
        assert game.ruler_confirm() is False
        assert not path.exists() and game.c.fingerprint==original
        game.on_nes_button('b')
        assert not game.ruler_dialog.winfo_exists()
        assert game.map_mode=='setup' and not path.exists()
        game.selected=4  # historical R1 territory
        game.on_nes_button('a')
        assert game.ruler_dialog.winfo_exists()
        assert game.ruler_confirm() is True
        game.root.update()
        assert path.is_file()
        assert game.player=='R1' and game.selected==4
        assert game.new_game_pending is False and game.map_mode is None
        assert game.province_stage.active()
        saved=load_file(path)
        assert saved.state['human_faction']=='R1'
        assert saved.state['human_selected_province']=='p4'
        assert saved.state['owners']['p4']=='R1'
        assert saved.state['month']==1
        assert saved.state['revision']==0
    finally:
        game.close()
    # Reopen saved campaign; never re-run new-game setup or auto-select R0/P9.
    loaded=StrategicWindow(path)
    try:
        loaded.root.update()
        assert loaded.new_game_pending is False
        assert loaded.map_mode is None
        assert loaded.player=='R1' and loaded.selected==4
        assert loaded.province_stage.active()
    finally:
        loaded.close()


def test_setup_save_failure_does_not_commit_or_advance(tmp_path,monkeypatch):
    from third_branch import strategic_game
    game=strategic_game.StrategicWindow(tmp_path/'cannot_save.json')
    try:
        game.root.update()
        game.selected=3
        game.on_nes_button('a')
        original=game.c.fingerprint
        def fail_save(*args,**kwargs):raise OSError('simulated disk full')
        monkeypatch.setattr(strategic_game,'save_file',fail_save)
        assert game.ruler_confirm() is False
        assert game.c.fingerprint==original
        assert game.new_game_pending and game.map_mode=='setup'
        assert game.ruler_dialog.winfo_exists()
        assert not game.path.exists()
    finally:
        game.close()
