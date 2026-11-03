from pathlib import Path
from third_branch.orders_ui import WORKERS
from third_branch.province_stage import ART, PORTRAIT_DIR
from launcher.app import MODES

ROOT=Path(__file__).resolve().parents[1]

def test_only_two_approved_release_paths():
    assert MODES==('Classic Game','Enhanced Game')
    assert len(MODES)==2
    assert 'Third Branch' not in MODES and 'Old Enhanced' not in MODES

def test_workforce_uses_exactly_seven_real_jobs():
    assert WORKERS==('farmers','town','artisans','metalworkers','resources','land','flood')

def test_player_art_uses_original_neutral_nes_source_and_real_portrait_folder():
    assert 'nes_illustrations' in str(ART)
    assert 'story_palace' in ART.name
    assert ART.exists()
    for kind in ('ruler','advisor','general'):
        assert (PORTRAIT_DIR/(kind+'_placeholder.png')).exists()

def test_windows_and_linux_packagers_include_resources():
    w=(ROOT/'BUILD_WINDOWS_EXE.bat').read_text()
    l=(ROOT/'BUILD_LINUX_EXE.sh').read_text()
    for s in (w,l):
        assert 'third_party' in s
        assert 'tools' in s and 'bundle_mesen.py' in s
        assert 'PyInstaller' in s
        assert 'KOEI_Classic_Enhanced' in s
    bat=(ROOT/'INSTALL_WINDOWS.bat').read_text()
    assert 'KOEI_Classic_Enhanced_Setup.exe' in bat and 'start ""' in bat  # one step: the bat only launches the one-click installer
    nsi=(ROOT/'tools'/'windows_installer'/'setup.nsi').read_text()
    assert 'OutFile "KOEI_Classic_Enhanced_Setup.exe"' in nsi
    assert 'CreateShortcut "$DESKTOP' in nsi and 'run_game.pyw' in nsi and 'WriteUninstaller' in nsi
    assert (ROOT/'tools'/'windows_installer'/'build_installer.sh').is_file() and (ROOT/'tools'/'windows_installer'/'run_game.pyw').is_file()
    assert 'LAUNCH_INSTALLED_LINUX.sh' in (ROOT/'INSTALL_LINUX.sh').read_text()
