"""Run037: Q08 original Fire Trick can ignite an adjacent EMPTY cell.

Answer Matrix v1.4 Q08, hsovl2 $CF49 / hex $818A and chance table
$C99C, gives the unoccupied terrain ignition predicate separately from
occupied-target INT adjustment. It is not an enemy-officer-only command.
No original NES playback or exact text/pixel acceptance is claimed here.
"""
import copy
import os
import subprocess
import sys
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import Mock
import pytest
from og.core.fixed import CoreError
from third_branch.integrated import save_bytes, load_bytes, load_file, validate
from third_branch import officer_battle_adapter as ba
from third_branch import army_cohorts as ac
from third_branch import rotk2_battle_core as core
from third_branch.tactical_view import TacticalWindow
from test_run030_q06_move_enemy_visible_target import battle_setup, stub


def _c(seed=37001):
    c=battle_setup(seed)
    a=ba.find(c.state['battle'],'D','B9')
    assert (a['q'],a['r'])==(10,5)
    assert (11,5) in core.neighbors(10,5)
    assert c.state['battle']['terrain'][5][11]=='.'
    assert not any((u['q'],u['r'])==(11,5) and u['troops100']>=100
                   and not u.get('fled') and not u.get('reserve')
                   for u in c.state['battle']['officer_units'])
    return c


def _total(s):
    return sum(ac.army_total(a) for a in s['armies'].values())


def test_real_engine_fire_can_target_empty_adjacent_grass_and_persist():
    c=_c(); b=c.state['battle']; total=_total(c.state)
    old_rng=copy.deepcopy(c.state['core']['rng_state'])
    before_units=[(u['id'],u['troops100']) for u in b['officer_units']]
    result=c.do('battle_attack','B9',{'officer_id':'B9','mode':'fire',
                'q':11,'r':5},'r037:empty-ignition')
    assert result['mode']=='fire' and result['target'] is None
    assert isinstance(result['ignited'],bool)
    assert ba.find(c.state['battle'],'D','B9')['acted']
    assert c.state['core']['rng_state']!=old_rng
    assert [(u['id'],u['troops100']) for u in c.state['battle']['officer_units']]==before_units
    assert _total(c.state)==total and validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_nonadjacent_empty_fire_is_rejected_without_any_state_or_rng():
    c=_c(37002); old=save_bytes(c)
    with pytest.raises(CoreError):
        c.do('battle_attack','B9',{'officer_id':'B9','mode':'fire',
             'q':12,'r':5},'r037:invalid-range')
    assert save_bytes(c)==old and validate(c.state)


def test_normal_attack_cannot_use_empty_fire_coordinates():
    c=_c(37003); old=save_bytes(c)
    with pytest.raises(CoreError):
        c.do('battle_attack','B9',{'officer_id':'B9','mode':'normal',
             'q':11,'r':5},'r037:ordinary-empty')
    assert save_bytes(c)==old


def test_source_rain_gate_prevents_ignition_on_empty_cell():
    c=_c(37004); c.state['battle']['weather']=3
    old=copy.deepcopy(c.state['core']['rng_state'])
    result=c.do('battle_attack','B9',{'officer_id':'B9','mode':'fire',
                'q':11,'r':5},'r037:rain')
    assert not result['ignited'] and [11,5] not in c.state['battle']['burning']
    assert c.state['core']['rng_state']==old
    assert validate(c.state)


def test_engine_existing_enemy_target_id_remains_supported():
    c=_c(37005); total=_total(c.state)
    result=c.do('battle_attack','B9',{'officer_id':'B9','mode':'fire',
                'target_officer_id':'A3'},'r037:legacy-enemy')
    assert result['target']=='A3' and _total(c.state)<=total
    assert validate(c.state)


def test_controller_visible_fire_empty_tile_uses_coordinate_not_stale_target():
    c,w=stub(37006)
    w.selected_target='B2' # stale friendly target must not be substituted
    old=save_bytes(c); old_rng=copy.deepcopy(c.state['core']['rng_state'])
    w._menu_action('Attack','Fire')
    assert w.targeting and w.target_attack_mode=='fire'
    w.on_nes_button('right');w.on_nes_button('a')
    w._issue.assert_called_once_with('battle_attack',{'mode':'fire','q':11,'r':5})
    assert not w.targeting and not w.target_actor
    assert save_bytes(c)==old and c.state['core']['rng_state']==old_rng


def test_controller_cancel_fire_is_no_op():
    c,w=stub(37007); old=save_bytes(c)
    w._menu_action('Attack','Fire');w.on_nes_button('right');w.on_nes_button('b')
    assert not w.targeting and not w._issue.called and save_bytes(c)==old


def test_fire_explicit_stale_officer_id_cannot_redirect_to_other_hex():
    c=_c(37009);old=save_bytes(c)
    # A3 is on a different adjacent hex. The source-visible (11,5) target
    # must not silently resolve to that officer when the caller has an old ID.
    with pytest.raises(CoreError,match='STALE_FIRE_TARGET'):
        c.do('battle_attack','B9',{'officer_id':'B9','mode':'fire',
             'q':11,'r':5,'target_officer_id':'A3'},'r037:stale-id')
    assert save_bytes(c)==old and validate(c.state)


def test_fire_explicit_enemy_hex_preserves_occupied_fire_path():
    c=_c(37010);before=_total(c.state)
    r=c.do('battle_attack','B9',{'officer_id':'B9','mode':'fire',
             'q':9,'r':5,'target_officer_id':'A3'},'r037:enemy-cell')
    assert r['target']=='A3' and _total(c.state)<=before
    assert validate(c.state) and load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_friendly_occupied_fire_cell_fails_closed_pending_original_source():
    c=_c(37011);old=save_bytes(c)
    with pytest.raises(CoreError,match='FIRE_ALLY_CELL_UNVERIFIED'):
        c.do('battle_attack','B9',{'officer_id':'B9','mode':'fire',
             'q':10,'r':6},'r037:ally-unknown')
    assert save_bytes(c)==old


@pytest.mark.skipif(not os.environ.get('DISPLAY'),reason='requires actual Linux Xvfb Tk')
def test_actual_1280x720_six_command_fire_empty_save_cold_load(tmp_path):
    import tkinter as tk
    c=_c(37008)
    root=tk.Tk();root.withdraw()
    parent=SimpleNamespace(root=root,c=c,path=tmp_path/'run037.ro2',counter=0,
        audio=SimpleNamespace(effect=Mock(),play=Mock()),selected='p5',redraw=Mock(),pause=Mock())
    w=TacticalWindow(parent)
    try:
        assert tuple(w.menu.labels)==('Move','Attack','Flee','View','Tactics','Wait')
        root.update_idletasks()
        assert w.root.geometry().startswith('1280x720')
        w.selected_officer='B9';w.selected_target='B2'
        w._menu_action('Attack','Fire');w.on_nes_button('right');w.on_nes_button('a')
        assert parent.counter==1 and parent.path.is_file()
        assert ba.find(parent.c.state['battle'],'D','B9')['acted']
        assert validate(parent.c.state)
        script=('import sys;from third_branch.integrated import load_file;'
                'print(load_file(sys.argv[1]).fingerprint)')
        r=subprocess.run([sys.executable,'-c',script,str(parent.path)],
                         cwd=str(Path(__file__).parents[1]),capture_output=True,text=True,check=True)
        assert r.stdout.strip()==parent.c.fingerprint
    finally:
        w.close();root.destroy()