"""Director acceptance: EXACTLY Classic and expanded Enhanced; no retired third entry."""
import json
from pathlib import Path
from unittest.mock import Mock,patch
import pytest
from launcher import app
from launcher.classic import ClassicUnavailable, resolve_classic, valid_nes_rom, save_settings,settings

ROOT=Path(__file__).resolve().parents[1]

def test_exactly_two_modes_and_no_old_enhanced():
    assert app.MODES==('Classic Game','Enhanced Game')
    import subprocess,sys
    r=subprocess.run([sys.executable,'-m','launcher','--verify-modes'],cwd=ROOT,text=True,capture_output=True,timeout=10)
    assert r.returncode==0 and r.stdout.splitlines()==['Classic Game','Enhanced Game']
    assert 'Third' not in r.stdout and 'Old Enhanced' not in r.stdout

def test_classic_never_substitutes_expanded_game(tmp_path):
    with pytest.raises(ClassicUnavailable,match='your own valid English NES game ROM'):
        resolve_classic(env={},cfg={})
    rom=tmp_path/'test.nes'
    rom.write_bytes(b'NES\x1a'+bytes(12)+bytes(16384))
    assert valid_nes_rom(rom)
    em=tmp_path/'Mesen'
    em.write_text('#!/bin/sh\nexit 0\n');em.chmod(0o755)
    assert resolve_classic(env={},cfg={'rom':str(rom),'emulator':str(em)})==[str(em),str(rom)]
    rom.write_bytes(b'wrong')
    assert not valid_nes_rom(rom)

def test_classic_native_executable_path_without_rom(tmp_path):
    app_path=tmp_path/'classic.exe'
    app_path.write_text('test')
    assert resolve_classic(env={'KOEI_CLASSIC_EXE':str(app_path)},cfg={})==[str(app_path)]

def test_classic_config_persist_is_atomic_and_ignores_unapproved_keys(tmp_path):
    p=tmp_path/'settings.json'
    save_settings({'rom':'/tmp/game.nes','enhanced':'old','classic_executable':'/tmp/classic.exe'},p)
    assert settings(p)=={'rom':'/tmp/game.nes','classic_executable':'/tmp/classic.exe'}
    assert not p.with_suffix('.pending').exists()

def test_expanded_uses_real_third_branch_game():
    with patch('launcher.app.subprocess.Popen') as sub:
        with patch.object(app.sys,'frozen',False,create=True):
            app.launch_enhanced()
        command=sub.call_args.args[0]
        assert command[1:]==['-m','third_branch.strategic_game']

def test_windows_build_and_linux_entrypoint_offer_only_two_modes():
    win=(ROOT/'windows_entry.py').read_text()
    bat=(ROOT/'BUILD_WINDOWS_EXE.bat').read_text()
    linux=(ROOT/'PLAY_LINUX.sh').read_text()
    assert 'from launcher.app import main' in win
    assert 'KOEI_Classic_Enhanced' in bat
    assert '-m launcher' in linux
