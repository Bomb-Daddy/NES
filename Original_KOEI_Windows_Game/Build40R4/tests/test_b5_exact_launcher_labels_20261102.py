"""Director-required literal launcher labels, not mode identifiers or mock text."""
from pathlib import Path
import subprocess
import sys
from launcher import app

ROOT = Path(__file__).resolve().parents[1]

def test_exact_visible_launcher_labels_and_cli_contract():
    assert app.MODES == ('Classic Game', 'Enhanced Game')
    result = subprocess.run([sys.executable, '-m', 'launcher', '--verify-modes'], cwd=ROOT, capture_output=True, text=True, timeout=10)
    assert result.returncode == 0
    assert result.stdout.splitlines() == ['Classic Game', 'Enhanced Game']

def test_enhanced_path_remains_real_integrated_entrypoint(monkeypatch):
    called=[]
    monkeypatch.setattr(app.subprocess, 'Popen', lambda cmd, **kw: called.append((cmd,kw)))
    app.launch_enhanced()
    assert called and called[0][0][1:] == ['-m', 'third_branch.strategic_game']

def test_classic_uses_distinct_launch_routing():
    source = (ROOT/'launcher'/'app.py').read_text()
    assert 'launch_classic()' in source
    assert 'launch_enhanced()' in source
