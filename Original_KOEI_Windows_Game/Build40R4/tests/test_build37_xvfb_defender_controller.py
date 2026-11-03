"""Actual Tk window with NES-button events on a Linux Xvfb display.

Not a physical Xbox controller or native Windows test.
"""
import os
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import Mock
import pytest
from third_branch.integrated import load_file
from third_branch.tactical_view import TacticalWindow
from tests.test_build37_human_defender_controller import started


def test_live_tk_defender_rest_from_controller_only(tmp_path):
    if not os.environ.get('DISPLAY'):
        pytest.skip('requires X11/Xvfb display')
    import tkinter as tk
    root=tk.Tk();root.withdraw()
    try:
        c=started(3711)
        parent=SimpleNamespace(root=root,c=c,path=tmp_path/'defender_save.json',
                               counter=0,audio=SimpleNamespace(effect=Mock(),play=Mock()),selected='p5',
                               redraw=Mock(),pause=Mock())
        w=TacticalWindow(parent)
        try:
            assert w.selected_officer in ('B1','B2')
            # The actual NES input handler moves down 3 menu entries: Rest.
            assert w.controller_actions[3][0]=='Rest'
            w.on_nes_button('down')
            assert w.controller_index==3
            w.on_nes_button('a')
            root.update_idletasks()
            assert parent.path.exists()
            assert next(u for u in parent.c.state['battle']['officer_units']
                        if u['id']==w.selected_officer)['acted']
            cold=load_file(parent.path)
            assert cold.fingerprint==parent.c.fingerprint
        finally:
            w.close()
    finally:
        root.destroy()
