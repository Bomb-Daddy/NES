"""Real Windows run (2026-11-03): an old-format save at the default path crashed Enhanced at startup
(CoreError MISSING_UNIT_SKILL_ARMS) and the launcher just closed. The game must keep the old file and start fresh."""
import json, os
import pytest
from third_branch.strategic_game import StrategicWindow
from third_branch.integrated import new_campaign_41, save_file, load_file

@pytest.fixture(autouse=True)
def quiet(monkeypatch):
    monkeypatch.setenv('KOEI_DISABLE_AUDIO', '1')

def _open(path):
    w = StrategicWindow(str(path)); w.root.update(); return w

def test_old_format_save_is_kept_and_new_game_starts(tmp_path):
    c = new_campaign_41(1)
    p = tmp_path / 'new_branch_41_save.json'
    save_file(c, p)
    data = json.loads(p.read_text())
    data['state']['unit_skill'] = None if 'unit_skill' in data.get('state', {}) else None
    raw = p.read_text()
    # Remove the unit skill / arms tables to reproduce the old save shape.
    state = json.loads(raw)
    def strip(o):
        if isinstance(o, dict):
            for k in [k for k in o if k in ('unit_skill','unit_arms','unit_training','unit_armed','arms','skills')]:
                del o[k]
            for v in o.values(): strip(v)
        elif isinstance(o, list):
            for v in o: strip(v)
    strip(state); p.write_text(json.dumps(state))
    with pytest.raises(Exception):
        load_file(p)                      # precondition: this really is unloadable
    w = _open(p)
    try:
        assert w.new_game_pending
        assert 'old save could not be loaded' in w.msg.get()
        kept = [f for f in os.listdir(tmp_path) if '.incompatible-' in f]
        assert len(kept) == 1 and (tmp_path / kept[0]).read_text() == json.dumps(state)
        assert not p.exists()
    finally:
        w.root.destroy()

def test_garbage_save_also_starts_new_game(tmp_path):
    p = tmp_path / 's.json'; p.write_bytes(b'\x00not json')
    w = _open(p)
    try:
        assert w.new_game_pending and len([f for f in os.listdir(tmp_path) if '.incompatible-' in f]) == 1
    finally:
        w.root.destroy()

def test_good_save_still_loads_normally(tmp_path):
    p = tmp_path / 's.json'; save_file(new_campaign_41(1), p)
    w = _open(p)
    try:
        assert not w.new_game_pending and not [f for f in os.listdir(tmp_path) if '.incompatible-' in f]
        assert w.save_notice == ''
    finally:
        w.root.destroy()
