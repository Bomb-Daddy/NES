"""Windows port source tests on Linux. These are NOT native Windows runtime evidence."""
from __future__ import annotations
import io
from pathlib import Path
from unittest.mock import Mock
import pytest
from third_branch.platform_paths import default_save_path, prepare_save_path
from third_branch.strategic_game import enable_windows_dpi_awareness, StrategicWindow
from third_branch.audio_system import GameAudio, TRACKS


def test_new_windows_default_uses_localappdata_without_changing_existing_save(tmp_path):
    dest=default_save_path(platform='win32',environ={'LOCALAPPDATA':str(tmp_path/'App Data')},home=tmp_path,legacy_dir=tmp_path)
    assert dest == tmp_path/'App Data'/'KOEI_Original_Third_Branch'/'saves'/'new_branch_41_save.json'
    assert prepare_save_path(dest).parent.exists()
    assert not dest.exists()
    legacy=tmp_path/'new_branch_41_save.json'
    legacy.write_text('old save is retained')
    assert default_save_path(platform='win32',environ={'LOCALAPPDATA':str(tmp_path/'App Data')},home=tmp_path,legacy_dir=tmp_path)==legacy
    assert legacy.read_text()=='old save is retained'


def test_linux_default_uses_xdg_and_preserves_legacy(tmp_path):
    path=default_save_path(platform='linux',environ={'XDG_STATE_HOME':str(tmp_path/'state')},home=tmp_path,legacy_dir=tmp_path)
    assert path==tmp_path/'state'/'koei-original-third-branch'/'saves'/'new_branch_41_save.json'
    assert enable_windows_dpi_awareness('linux') is False


def test_winmm_playback_command_and_pcm_effect_routing_without_real_device():
    if not TRACKS: pytest.skip("optional private music package is not installed")
    a=GameAudio(backend='winmm-mci')
    fake=Mock()
    a.winmm=fake
    winsound=Mock()
    winsound.SND_FILENAME=1
    winsound.SND_ASYNC=2
    winsound.SND_NODEFAULT=4
    a.winsound=winsound
    cmds=[]
    def fake_mci(command):cmds.append(command);return True
    a._mci=fake_mci
    assert a.play('map',9)
    assert any(c.startswith('open ') and 'koei_bgm' in c for c in cmds)
    assert 'play koei_bgm repeat' in cmds
    assert a.effect('confirm')
    winsound.PlaySound.assert_called_once()
    a.close()
    assert any(c=='close koei_bgm' for c in cmds)


def test_attack_uses_controller_order_ui_not_keyboard_only_simpledialog():
    class FakeOrders:
        def __init__(self):self.order=Mock();self._fields=Mock()
    window=object.__new__(StrategicWindow)
    window.orders=Mock()
    window.orders_dialog=FakeOrders()
    window.attack()
    window.orders.assert_called_once()
    window.orders_dialog.order.set.assert_called_once_with('Attack')
    window.orders_dialog._fields.assert_called_once()


def test_windows_launch_scripts_are_source_relative_and_offer_native_exe_build():
    base=Path(__file__).resolve().parents[1]
    bat=(base/'PLAY_WINDOWS.bat').read_text()
    assert 'cd /d "%~dp0"' in bat
    # Current product uses a self-contained EXE first, not an old broken Tk venv.
    assert 'KOEI_Classic_Enhanced.exe' in bat
    assert '-m launcher' in bat
    assert 'requirements.txt' in bat
    build=(base/'BUILD_WINDOWS_EXE.bat').read_text()
    assert '--onedir' in build
    assert '--add-data "third_branch\\assets;third_branch\\assets"' in build
    assert 'PyInstaller' in build
