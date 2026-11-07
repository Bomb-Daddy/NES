"""Q15 original no-fog officer View (English NES Answer Matrix v1.4 Q15).
Controller-visible View must inspect either side on the map without consuming
turn/RNG, rather than simply cycling player officers. This is port behavior
fixture; pixels and original NES exact text remain unauthenticated.
"""
import copy
import os
from types import SimpleNamespace
from unittest.mock import Mock
import pytest
from third_branch.integrated import save_bytes, validate
from third_branch.tactical_view import TacticalWindow
from test_run030_q06_move_enemy_visible_target import battle_setup, Message


def stub(seed=31001):
    c=battle_setup(seed)
    w=TacticalWindow.__new__(TacticalWindow)
    w.parent=SimpleNamespace(c=c,counter=0)
    w.selected_officer='B9';w.selected_target=None
    w.targeting=False;w.target_actor=None;w.target_phase=None
    w.target_enemy=None;w.target_enemy_hex=None
    from third_branch.nes_battle_menu import NESBattleMenu
    w.menu=NESBattleMenu();w.action_buttons=[];w.controller_index=0
    w.view_officer_id=None;w.pending_withdraw=None;w.pending_prisoner=False
    w.message=Message();w.redraw=Mock()
    return c,w


def test_view_enemy_from_hex_reports_actual_fields_no_game_transaction():
    c,w=stub()
    original=save_bytes(c);rng=copy.deepcopy(c.state['core']['rng_state'])
    w._menu_action('View','Officer')
    assert w.targeting and w.target_mode=='battle_view_officer'
    w.on_nes_button('left')
    w.on_nes_button('a')
    a=next(u for u in c.state['battle']['officer_units'] if u['id']=='A3')
    assert w.view_officer_id=='A3' and not w.targeting
    assert f'War {a["war"]}' in w.message.value
    assert f'Int {a["intel"]}' in w.message.value
    assert f'Skill {a["training"]}' in w.message.value
    assert 'Arm ' in w.message.value
    assert save_bytes(c)==original and c.state['core']['rng_state']==rng
    assert w.parent.counter==0 and validate(c.state)


def test_view_friendly_from_own_cell_not_just_enemy():
    c,w=stub(31002)
    w._menu_action('View','Officer');w.on_nes_button('a')
    assert w.view_officer_id=='B9' and not w.targeting
    assert 'B9' in w.message.value


def test_view_empty_hex_stays_in_targeting_then_b_cancels():
    c,w=stub(31003);old=save_bytes(c)
    w._menu_action('View','Officer')
    w.on_nes_button('up');w.on_nes_button('a')
    assert w.targeting and w.view_officer_id is None
    assert 'No officer' in w.message.value
    w.on_nes_button('b')
    assert not w.targeting
    assert save_bytes(c)==old


def test_view_target_after_officer_flees_cannot_show_phantom():
    c,w=stub(31004)
    w._menu_action('View','Officer');w.on_nes_button('left')
    a=next(u for u in c.state['battle']['officer_units'] if u['id']=='A3')
    a['fled']=True
    old=save_bytes(c)
    w.on_nes_button('a')
    assert w.view_officer_id is None and w.targeting
    assert save_bytes(c)==old


def test_original_six_command_root_is_untouched():
    from third_branch.nes_battle_menu import ROOT
    assert ROOT==('Move','Attack','Flee','View','Tactics','Wait')


@pytest.mark.skipif(not os.environ.get('DISPLAY'),reason='Linux Tk Xvfb required')
def test_real_tk_controller_view_enemy_and_screenshot(tmp_path):
    import tkinter as tk
    c=battle_setup(31006)
    root=tk.Tk();root.withdraw()
    parent=SimpleNamespace(root=root,c=c,path=tmp_path/'view.ro2',counter=0,
        audio=SimpleNamespace(effect=Mock(),play=Mock()),selected='p5',
        redraw=Mock(),pause=Mock())
    w=TacticalWindow(parent)
    try:
        w.selected_officer='B9'; before=save_bytes(c)
        w._menu_action('View','Officer');w.on_nes_button('left');w.on_nes_button('a')
        assert w.view_officer_id=='A3' and 'War ' in w.message.get()
        root.update_idletasks();root.update()
        assert not parent.path.exists() and save_bytes(parent.c)==before
        assert parent.counter==0
        w.on_nes_button('b')
    finally:
        w.close();root.destroy()
