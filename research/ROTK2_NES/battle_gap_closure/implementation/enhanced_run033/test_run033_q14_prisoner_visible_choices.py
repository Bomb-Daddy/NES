"""Q14 visible prisoner/fate controller regression on exact Run032 source.

Source truth: post-battle prisoner treatment identifies *which prisoner* and
*which outcome*. The exact English NES pixels/warning wording are NOT proved.
A visible controller choice must agree with the transactional choice that A
will commit; this remains an Enhanced port/UI test, not a golden NES replay.
"""
import copy
import os
import subprocess
import sys
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import Mock

import pytest

from third_branch.integrated import save_bytes, load_file, validate
from third_branch.tactical_view import TacticalWindow
from test_run029_q14_prisoner_modal_identity import awaiting_two

pytestmark=pytest.mark.skipif(not os.environ.get('DISPLAY'),reason='requires actual Tk/Xvfb')


def live_window(tmp_path, seed):
    import tkinter as tk
    c=awaiting_two(seed)
    root=tk.Tk();root.withdraw()
    parent=SimpleNamespace(root=root,c=c,path=tmp_path/'run033.ro2',counter=0,
        audio=SimpleNamespace(effect=Mock(),play=Mock()),selected='p5',
        redraw=Mock(),pause=Mock())
    w=TacticalWindow(parent)
    return root,w,parent


def visible_labels(w):
    return [w.canvas.itemcget(i,'text') for i in w.canvas.find_all()
            if w.canvas.type(i)=='text']


def open_prisoner_with_controller(w):
    for _ in range(3):w.on_nes_button('down')
    w.on_nes_button('a')
    w.on_nes_button('down');w.on_nes_button('down');w.on_nes_button('a')
    assert w.pending_prisoner


def test_controller_shows_current_prisoner_and_current_fate_in_canvas(tmp_path):
    root,w,parent=live_window(tmp_path,3301)
    try:
        before=save_bytes(parent.c)
        open_prisoner_with_controller(w)
        labels=visible_labels(w)
        assert 'PRISONER B2' in labels
        assert 'RECRUIT' in labels and 'RELEASE' in labels and 'BEHEAD' in labels
        assert 'Recruit' in w.message.get() and 'B2' in w.message.get()
        w.on_nes_button('right')
        assert 'RELEASE' in visible_labels(w)
        assert 'Release' in w.message.get() and 'B2' in w.message.get()
        assert save_bytes(parent.c)==before
        assert not parent.path.exists()
    finally:
        w.close();root.destroy()


def test_select_rebinds_rendered_prisoner_without_changing_campaign(tmp_path):
    root,w,parent=live_window(tmp_path,3302)
    try:
        open_prisoner_with_controller(w)
        before=save_bytes(parent.c)
        w.on_nes_button('select')
        assert w.pending_prisoner_actor=='B3'
        assert 'PRISONER B3' in visible_labels(w)
        assert 'B3' in w.message.get()
        assert save_bytes(parent.c)==before
    finally:w.close();root.destroy()


def test_visible_release_commits_release_for_displayed_officer_cold_load(tmp_path):
    root,w,parent=live_window(tmp_path,3303)
    try:
        open_prisoner_with_controller(w)
        w.on_nes_button('right')
        assert 'RELEASE' in visible_labels(w)
        assert 'B2' in w.message.get() and 'Release' in w.message.get()
        w.on_nes_button('a')
        assert parent.c.state['battle']['prisoner_decisions']['B2']=='release'
        assert parent.path.exists() and parent.counter==1
        assert validate(parent.c.state)
        script='import sys;from third_branch.integrated import load_file;print(load_file(sys.argv[1]).fingerprint)'
        p=subprocess.run([sys.executable,'-c',script,str(parent.path)],
            cwd=str(Path(__file__).parents[1]),capture_output=True,text=True,check=True)
        assert p.stdout.strip()==parent.c.fingerprint
    finally:w.close();root.destroy()


def test_cancel_removes_fate_overlay_no_state_or_rng_consumption(tmp_path):
    root,w,parent=live_window(tmp_path,3304)
    try:
        open_prisoner_with_controller(w)
        initial=save_bytes(parent.c)
        rng=copy.deepcopy(parent.c.state['core']['rng_state'])
        w.on_nes_button('b')
        assert not w.pending_prisoner
        assert 'PRISONER B2' not in visible_labels(w)
        assert save_bytes(parent.c)==initial
        assert parent.c.state['core']['rng_state']==rng
        assert not parent.path.exists()
    finally:w.close();root.destroy()


def test_stale_queue_rejected_without_displaying_new_prisoner_as_old(tmp_path):
    root,w,parent=live_window(tmp_path,3305)
    try:
        open_prisoner_with_controller(w)
        w.on_nes_button('right')
        assert 'PRISONER B2' in visible_labels(w)
        parent.c.state['battle']['prisoner_queue']=['B3','B2']
        before=save_bytes(parent.c)
        w.on_nes_button('a')
        assert not w.pending_prisoner
        assert not parent.path.exists()
        assert save_bytes(parent.c)==before
        assert 'PRISONER B2' not in visible_labels(w)
    finally:w.close();root.destroy()


def test_pending_outcome_not_active_keeps_old_default_menu_flow(tmp_path):
    root,w,parent=live_window(tmp_path,3306)
    try:
        assert not w.pending_prisoner
        assert not any(x.startswith('PRISONER B') for x in visible_labels(w))
        assert 'PRISONER DECISIONS REQUIRED' in w.message.get()
    finally:w.close();root.destroy()
