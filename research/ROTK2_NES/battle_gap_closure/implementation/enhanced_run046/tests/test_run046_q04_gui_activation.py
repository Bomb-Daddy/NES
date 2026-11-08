"""Run046: original English NES Q04 manual staging is the visible GUI default.

Source basis: the bundled English NES ROM-derived province zones ($28934),
Q04 hsovl6 $D0CF/$CE5C player deployment selection. This tests port
activation, not original NES pixels, cursor timing or an emulator golden.
"""
from __future__ import annotations
import os
from pathlib import Path
import pytest
from third_branch.integrated import (new_campaign_41,new_campaign,
                                    save_bytes,load_bytes,validate)
from third_branch.strategic_game import StrategicWindow
import third_branch.strategic_game as sg


def test_41_province_gui_entry_enables_authentic_manual_deployment(tmp_path):
    if not os.environ.get('DISPLAY'):pytest.skip('requires actual Tk/Xvfb')
    w=StrategicWindow(tmp_path/'new.ro2')
    try:
        assert w.c.state['source_label']=='ORIGINAL_GAME_HISTORICAL_S1_PROPOSED'
        assert w.c.state['q04_manual_deployment'] is True
        assert w.root.winfo_reqwidth()<=1280 and w.root.winfo_reqheight()<=720
        w.choose_ruler()
        w.ruler_listbox.selection_clear(0,'end')
        w.ruler_listbox.selection_set(0)
        w.ruler_confirm()
        assert w.c.state['human_faction']==w.player
        assert w.c.state['q04_manual_deployment'] is True
        assert w.path.exists()
        cold=load_bytes(w.path.read_bytes())
        assert cold.fingerprint==w.c.fingerprint
        assert cold.state['q04_manual_deployment'] is True
    finally:
        w.root.destroy()


def test_41_province_live_human_battle_starts_in_original_q04_phase():
    c=new_campaign_41(4601)
    c.state['human_faction']='A'
    sg._activate_original_q04_gui(c)
    assert c.state['q04_manual_deployment'] is True
    c.do('attack','O14',{'target':'p15','troops':100},'run046:attack')
    b=c.state['battle']
    assert b['q04_pending_deployment']==['O14']
    assert b['selected_officer']=='O14'
    assert b['day']==1
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_legacy_seven_province_fixture_has_no_new_source_only_phase():
    c=new_campaign(4602)
    sg._activate_original_q04_gui(c)
    assert c.state.get('q04_manual_deployment') is not True
    assert validate(c.state)


def test_in_flight_41_province_battle_is_not_retroactively_restarted():
    c=new_campaign_41(4603)
    c.state['human_faction']='A'
    c.do('attack','O14',{'target':'p15','troops':100},'run046:old_inflight')
    before=save_bytes(c)
    assert 'q04_pending_deployment' not in c.state['battle']
    sg._activate_original_q04_gui(c)
    assert save_bytes(c)==before
    assert validate(c.state)


def test_core_41_synthetic_fixture_keeps_explicit_auto_placement_without_gui():
    c=new_campaign_41(4604)
    c.state['human_faction']='A'
    c.do('attack','O14',{'target':'p15','troops':100},'run046:fixture')
    assert 'q04_pending_deployment' not in c.state['battle']
    assert validate(c.state)


def test_gui_activated_q04_real_tk_software_controller_commits_and_saves(tmp_path):
    if not os.environ.get('DISPLAY'):pytest.skip('requires actual Tk/Xvfb')
    import tkinter as tk
    from types import SimpleNamespace
    from unittest.mock import Mock
    from third_branch.tactical_view import TacticalWindow
    from third_branch.integrated import load_file
    c=new_campaign_41(4605);c.state['human_faction']='A'
    sg._activate_original_q04_gui(c)
    c.do('attack','O14',{'target':'p15','troops':100},'run046:tk:attack')
    root=tk.Tk();root.withdraw()
    p=SimpleNamespace(root=root,c=c,path=tmp_path/'controller.ro2',counter=0,
        audio=SimpleNamespace(effect=Mock(),play=Mock()),selected='p15',
        redraw=Mock(),pause=Mock())
    try:
        w=TacticalWindow(p)
        try:
            assert w.selected_officer=='O14'
            assert p.c.state['battle']['q04_pending_deployment']==['O14']
            # Source-supported original-zone provisional coordinate is a legal
            # controller confirmation, not a new combat order or RNG draw.
            before_rng=p.c.state['core']['rng_state']
            before_day=p.c.state['battle']['day']
            w.on_nes_button('a')
            assert p.c.state['battle']['q04_pending_deployment']==[]
            assert p.c.state['battle']['day']==before_day
            assert p.c.state['core']['rng_state']==before_rng
            assert p.path.exists()
            assert load_file(p.path).fingerprint==p.c.fingerprint
        finally:w.close()
    finally:root.destroy()
