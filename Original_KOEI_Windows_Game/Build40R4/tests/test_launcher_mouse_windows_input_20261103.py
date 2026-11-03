"""Launcher input regressions reported from a real Windows run (2026-11-03):
mouse did nothing; keyboard focus not taken; Enhanced closed the launcher and nothing opened."""
import sys, types
from pathlib import Path
import pytest
from launcher import app

ROOT = Path(__file__).resolve().parents[1]

@pytest.fixture
def launcher(monkeypatch):
    monkeypatch.setenv('KOEI_DISABLE_AUDIO', '1')
    class Quiet:  # no gamepad hardware in tests
        def poll(self): pass
        def close(self): pass
    L = app.Launcher(controller=Quiet())
    L.root.update()
    yield L
    try: L.root.destroy()
    except Exception: pass

def test_mouse_click_on_each_label_selects_and_chooses(launcher, monkeypatch):
    seen = []
    monkeypatch.setattr(launcher, 'choose', lambda: seen.append(launcher.index))
    for i in (1, 0):
        launcher.buttons[i].event_generate('<Button-1>', x=5, y=5); launcher.root.update()
    assert seen == [1, 0]

def test_mouse_hover_moves_highlight(launcher):
    launcher.buttons[1].event_generate('<Enter>'); launcher.root.update()
    assert launcher.index == 1

def test_click_ignored_while_file_picker_open(launcher, monkeypatch):
    launcher.picker = types.SimpleNamespace(root=types.SimpleNamespace(winfo_exists=lambda: True))
    seen = []; monkeypatch.setattr(launcher, 'choose', lambda: seen.append(1))
    launcher.click(1)
    assert seen == []

def test_window_takes_focus_and_releases_topmost(launcher):
    launcher.root.update(); launcher.root.after(500, lambda: None)
    import time; end = time.time() + 0.8
    while time.time() < end: launcher.root.update(); time.sleep(0.02)
    assert not launcher.root.attributes('-topmost')

def test_windows_enhanced_runs_in_this_process_not_a_child(launcher, monkeypatch):
    monkeypatch.setattr(app.sys, 'platform', 'win32')
    spawned = []; monkeypatch.setattr(app.subprocess, 'Popen', lambda *a, **k: spawned.append(a))
    launcher.index = 1; launcher.choose()
    assert launcher.result == 'enhanced' and spawned == []

def test_main_starts_game_after_launcher_closes_on_windows(monkeypatch):
    monkeypatch.setattr(app.sys, 'platform', 'win32')
    class FakeLauncher:
        result = 'enhanced'
        def run(self): pass
    monkeypatch.setattr(app, 'Launcher', FakeLauncher)
    called = []
    import third_branch.strategic_game as sg
    monkeypatch.setattr(sg, 'main', lambda: called.append(1) or 0)
    assert app.main([]) == 0 and called == [1]

def test_non_windows_enhanced_still_spawns_child(launcher, monkeypatch):
    monkeypatch.setattr(app.sys, 'platform', 'linux')
    spawned = []; monkeypatch.setattr(app.subprocess, 'Popen', lambda cmd, **k: spawned.append(cmd))
    launcher.index = 1; launcher.choose()
    assert spawned and spawned[0][1:] == ['-m', 'third_branch.strategic_game']

def test_installed_entry_logs_to_file_and_shows_crash_box():
    src = (ROOT / 'tools' / 'windows_installer' / 'run_game.pyw').read_text()
    assert 'game.log' in src and 'sys.stderr = _fh' in src and 'MessageBoxW' in src
