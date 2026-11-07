"""Q08 source/port tests: selected adjacent hex may contain a friendly unit.

Original Answer Matrix v1.4 Q08 hex $818A, $C99C uses occupied-cell
INT and immediate loss independent of ownership. These verify port behavior,
NOT authenticated emulator event goldens or exact English NES menu wording.
"""
import copy
import os
import subprocess
import sys
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import Mock, patch
import pytest
from og.core.fixed import CoreError
from third_branch.integrated import save_bytes, load_bytes, validate
from third_branch import officer_battle_adapter as ba
from third_branch.tactical_view import TacticalWindow
from test_run037_q08_fire_empty_hex import _c, stub, _total


def _friendly(seed=39001):
    c=_c(seed); b=c.state['battle']
    actor=ba.find(b,'D','B9'); friend=ba.find(b,'D','B2')
    assert (actor['q'],actor['r'])==(10,5)
    assert (friend['q'],friend['r'])==(10,6)
    assert friend['troops100']>=100
    return c, actor, friend


def test_q08_friendly_occupied_hex_fire_uses_actual_occupant_int_and_casualty():
    c,actor,friend=_friendly()
    before=friend['troops100']; total=_total(c.state)
    with patch.object(ba.core,'ignite',return_value=True) as ignite:
        r=c.do('battle_attack','B9',{'officer_id':'B9','mode':'fire',
                   'q':10,'r':6},'run039:friend')
    assert r['target']=='B2' and r['ignited']
    assert ignite.call_args.args[1:4]==(actor['intel'],friend['intel'],True)
    assert [10,6] in c.state['battle']['burning']
    assert ba.find(c.state['battle'],'D','B2')['troops100']<before and _total(c.state)<=total
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_q08_explicit_friendly_target_same_hex_permitted():
    c,_,_= _friendly(39002)
    with patch.object(ba.core,'ignite',return_value=True):
        r=c.do('battle_attack','B9',{'officer_id':'B9','mode':'fire',
                  'q':10,'r':6,'target_officer_id':'B2'},'run039:friend-id')
    assert r['target']=='B2' and validate(c.state)


def test_q08_stale_enemy_id_on_selected_friendly_hex_is_atomic_rejection():
    c,_,_=_friendly(39003); before=save_bytes(c)
    with pytest.raises(CoreError,match='STALE_FIRE_TARGET'):
        c.do('battle_attack','B9',{'officer_id':'B9','mode':'fire',
                 'q':10,'r':6,'target_officer_id':'A3'},'run039:wrong-id')
    assert save_bytes(c)==before and validate(c.state)


def test_q08_nonfire_attack_cannot_hit_friendly_by_coordinates():
    c,_,_=_friendly(39004);before=save_bytes(c)
    with pytest.raises(CoreError):
        c.do('battle_attack','B9',{'officer_id':'B9','mode':'normal',
               'q':10,'r':6},'run039:no-melee-ally')
    assert save_bytes(c)==before


def test_q08_rain_prevents_friendly_cell_fire_without_rng():
    c,_,friend=_friendly(39005);b=c.state['battle'];b['weather']=3
    old_rng=copy.deepcopy(c.state['core']['rng_state']);old=friend['troops100']
    r=c.do('battle_attack','B9',{'officer_id':'B9','mode':'fire',
          'q':10,'r':6},'run039:rain')
    assert r['target']=='B2' and not r['ignited']
    assert friend['troops100']==old and [10,6] not in b['burning']
    assert c.state['core']['rng_state']==old_rng and validate(c.state)


def test_q08_gui_targets_friendly_hex_without_enemy_id_substitution():
    c,w=stub(39006)
    w.selected_target='A3'  # intentionally stale opposing ID
    old=save_bytes(c)
    w._menu_action('Attack','Fire')
    assert w.targeting
    w.on_nes_button('down');w.on_nes_button('a')
    w._issue.assert_called_once_with('battle_attack',{'mode':'fire','q':10,'r':6})
    assert save_bytes(c)==old


@pytest.mark.skipif(not os.environ.get('DISPLAY'),reason='requires actual Linux Tk/Xvfb')
def test_q08_actual_tk_friendly_fire_and_two_process_cold_load(tmp_path):
    import tkinter as tk
    c,_,friend=_friendly(39007);c.state['battle']['weather']=0
    root=tk.Tk();root.withdraw()
    parent=SimpleNamespace(root=root,c=c,path=tmp_path/'run039.ro2',counter=0,
       audio=SimpleNamespace(effect=Mock(),play=Mock()),selected='p5',redraw=Mock(),pause=Mock())
    w=TacticalWindow(parent)
    try:
        assert tuple(w.menu.labels)==('Move','Attack','Flee','View','Tactics','Wait')
        root.update_idletasks()
        assert w.root.geometry().startswith('1280x720')
        w.selected_officer='B9';w._menu_action('Attack','Fire')
        w.on_nes_button('down');w.on_nes_button('a')
        assert parent.counter==1 and parent.path.is_file()
        assert ba.find(parent.c.state['battle'],'D','B9')['acted']
        assert validate(parent.c.state)
        script=('import sys;from third_branch.integrated import load_file;'
                'print(load_file(sys.argv[1]).fingerprint)')
        r=subprocess.run([sys.executable,'-c',script,str(parent.path)],
          cwd=str(Path(__file__).parents[1]),capture_output=True,text=True,check=True)
        assert r.stdout.strip()==parent.c.fingerprint
    finally:w.close();root.destroy()
