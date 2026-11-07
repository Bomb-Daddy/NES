"""Q09 controller fidelity: individual Flee must never silently substitute another officer.

Original hsovl2 D905 issues Flee on the selected officer, not the first
remaining unit.  Source record/flag40 fixtures exercise the existing engine;
this is a PORT regression and not a newly acquired emulator golden.
"""
from types import SimpleNamespace
from unittest.mock import Mock
import pytest
from third_branch.tactical_view import TacticalWindow
from third_branch.integrated import validate, load_file
from test_run022_q09_primary_commander_retreat import commissioned


class Message:
    def __init__(self): self.value = ''
    def set(self, value): self.value = value


def stub(c, selected):
    w = TacticalWindow.__new__(TacticalWindow)
    w.parent = SimpleNamespace(c=c)
    w.selected_officer = selected
    w.pending_withdraw = None
    w.withdraw_chosen = {}
    w.message = Message()
    w.redraw = lambda: None
    w._issue = lambda *_: (_ for _ in ()).throw(AssertionError('Unexpected order'))
    return w


def after_primary_flees(seed):
    c = commissioned(seed)
    outcome = c.do('battle_withdraw','B2',{'officer_id':'B2',
        'destination':'p6'},f'r023:{seed}:primary')
    assert outcome['withdrawn']==['B2'] and c.state['battle'] is not None
    assert validate(c.state)
    return c


def test_stale_fled_primary_must_not_retarget_b9():
    c = after_primary_flees(23001)
    w = stub(c, 'B2')
    before = c.fingerprint
    w._withdraw_prompt(False)
    assert w.pending_withdraw is None
    assert 'selected' in w.message.value.lower()
    assert c.fingerprint == before and validate(c.state)


def test_enemy_cursor_must_not_select_friendly_officer():
    c = commissioned(23002)
    w = stub(c, 'A3')
    before = c.fingerprint
    w._withdraw_prompt(False)
    assert w.pending_withdraw is None
    assert c.fingerprint == before and validate(c.state)


def test_missing_cursor_must_not_default_to_another_officer():
    c = commissioned(23003)
    w = stub(c, None)
    before = c.fingerprint
    w._withdraw_prompt(False)
    assert w.pending_withdraw is None
    assert c.fingerprint == before and validate(c.state)


def test_eligible_selected_b9_retains_ordinary_flee_and_cancel():
    c = commissioned(23004)
    w = stub(c, 'B9')
    before = c.fingerprint
    w._withdraw_prompt(False)
    assert w.pending_withdraw == {'whole':False,'officers':['B9']}
    w.on_nes_button('b')
    assert w.pending_withdraw is None
    assert c.fingerprint == before and validate(c.state)


@pytest.mark.skipif(not __import__('os').environ.get('DISPLAY'),reason='requires Xvfb Tk')
def test_real_tk_six_command_controller_stale_flee_is_atomic(tmp_path):
    import tkinter as tk
    c = after_primary_flees(23005)
    root=tk.Tk();root.withdraw()
    parent=SimpleNamespace(root=root,c=c,path=tmp_path/'r023.ro2',counter=0,
        audio=SimpleNamespace(effect=Mock(),play=Mock()),selected='p5',
        redraw=Mock(),pause=Mock())
    w=TacticalWindow(parent)
    try:
        # Preserve a cursor from the preceding fleeing commander. The game
        # must not quietly issue an order as the remaining officer B9.
        w.selected_officer='B2'
        before=c.fingerprint
        w.on_nes_button('down');w.on_nes_button('down')
        assert w.menu.labels[w.menu.index]=='Flee'
        w.on_nes_button('a')
        assert w.menu.labels==('Officer','Whole army')
        w.on_nes_button('a')
        assert w.pending_withdraw is None
        assert 'selected' in w.message.get().lower()
        assert parent.c.fingerprint==before and parent.counter==0
        assert not parent.path.exists()
        assert validate(parent.c.state)
    finally:
        w.close();root.destroy()
