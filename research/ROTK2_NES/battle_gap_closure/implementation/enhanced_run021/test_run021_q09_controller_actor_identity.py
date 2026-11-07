"""Run021: NES controller must execute as the selected HUMAN battle officer.

Source-backed observable rule: selected battle officer and issued order must
address the same real general on the human faction. The Run020 game repaired
stale selection *after* resolving actor, causing mismatched command origin.
Tests exercise actual Campaign transactions, Tk GUI, and disk cold-load.
No automatic simultaneous battle or original emulator golden is asserted.
"""
from types import SimpleNamespace
from unittest.mock import Mock
import pytest
from third_branch.integrated import load_file, validate
from third_branch.tactical_view import TacticalWindow
from test_run020_q09_human_defender_selected import recipient


def open_window(c,tmp_path):
    import tkinter as tk
    root=tk.Tk();root.withdraw()
    parent=SimpleNamespace(root=root,c=c,path=tmp_path/'run021.ro2',counter=0,
        audio=SimpleNamespace(effect=Mock(),play=Mock()),selected='p6',
        redraw=Mock(),pause=Mock())
    window=TacticalWindow(parent)
    return root,parent,window


@pytest.mark.skipif(not __import__('os').environ.get('DISPLAY'),reason='Tk requires Xvfb')
def test_stale_enemy_cursor_command_origin_is_selected_defender(tmp_path):
    c=recipient(21001)
    root,parent,w=open_window(c,tmp_path)
    try:
        w.selected_officer='C2'  # stale attacker cursor from old-save/UI phase
        w.on_nes_button('left') # NES root Wait
        assert w.menu.labels[w.menu.index]=='Wait'
        w.on_nes_button('a');w.on_nes_button('a')  # Wait -> Wait officer
        assert parent.c.state['log'][-1]['kind']=='battle_wait'
        assert parent.c.state['log'][-1]['actor']=='B9'
        assert parent.c.state['log'][-1]['result']['officer']=='B9'
        assert w.selected_officer=='B9'
        assert load_file(parent.path).fingerprint==parent.c.fingerprint
        assert validate(parent.c.state)
    finally:
        w.close();root.destroy()


@pytest.mark.skipif(not __import__('os').environ.get('DISPLAY'),reason='Tk requires Xvfb')
def test_stale_nonfield_ally_cursor_reconciles_to_source_head(tmp_path):
    c=recipient(21002)
    root,parent,w=open_window(c,tmp_path)
    try:
        w.selected_officer='B1' # same faction, not in the receiving battle
        w._issue('battle_wait',{})
        assert parent.c.state['log'][-1]['actor']=='B9'
        assert parent.c.state['log'][-1]['result']['officer']=='B9'
        assert validate(load_file(parent.path).state)
    finally:
        w.close();root.destroy()


@pytest.mark.skipif(not __import__('os').environ.get('DISPLAY'),reason='Tk requires Xvfb')
def test_valid_human_attacker_cursor_remains_unchanged(tmp_path):
    from test_run019_q09_recipient_source_attach import pending_two,attack_recipient
    c=pending_two(21003)
    c.state['human_faction']='C'
    attack_recipient(c,'run021:attacker')
    root,parent,w=open_window(c,tmp_path)
    try:
        assert w.selected_officer=='C2'
        w._issue('battle_wait',{})
        assert parent.c.state['log'][-1]['actor']=='C2'
        assert parent.c.state['log'][-1]['result']['officer']=='C2'
        assert validate(parent.c.state)
    finally:
        w.close();root.destroy()


@pytest.mark.skipif(not __import__('os').environ.get('DISPLAY'),reason='Tk requires Xvfb')
def test_cancel_no_command_does_not_edit_army_or_actor(tmp_path):
    c=recipient(21004)
    root,parent,w=open_window(c,tmp_path)
    try:
        before=parent.c.fingerprint
        w.selected_officer='C2'
        w.on_nes_button('b')
        assert parent.c.fingerprint==before
        assert parent.counter==0
        assert not parent.path.exists()
    finally:
        w.close();root.destroy()
