"""Run043: Original Q04 player deployment is not a completed field position.

Port observable correctness oracle is the original ROM-backed Q04 pending list:
only player-CONFIRMED positions should receive live troop markers. This does
not assert exact English NES placement sprite timing or any pixel-perfect art.
"""
from __future__ import annotations
import copy
import os
from types import SimpleNamespace
from unittest.mock import Mock
from PIL import ImageDraw
import pytest
from third_branch.integrated import new_campaign_41, save_bytes, load_bytes
from third_branch import rom_battle_map
from third_branch.tactical_view import render_battle, X0, Y0, CELL, SIDES


def battle(seed=4301, manual=True, human='A'):
    c=new_campaign_41(seed)
    c.state['human_faction']=human
    if manual:c.state['q04_manual_deployment']=True
    c.do('attack','O14',{'target':'p15','troops':100},f'run043:start:{seed}')
    return c


def marker_pixel(c,oid):
    b=c.state['battle']; u=next(u for u in b['officer_units'] if u['id']==oid)
    return render_battle(c).getpixel((X0+u['q']*CELL+19,Y0+u['r']*CELL+20))


def test_player_uncommitted_q04_army_does_not_appear_as_already_deployed():
    c=battle(); b=c.state['battle']
    assert b['q04_pending_deployment']==['O14']
    assert marker_pixel(c,'O14')!=SIDES['a']  # before manual confirmation
    assert marker_pixel(c,'O15')==SIDES['d']  # CPU placement is real
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_deployed_player_army_appears_after_actual_committed_hex_and_cold_save():
    c=battle(4302);b=c.state['battle']
    u=next(u for u in b['officer_units'] if u['id']=='O14')
    target=int(b['dst'][1:]);source=int(b['src'][1:])
    occ={(v['q'],v['r']) for v in b['officer_units'] if v['id']!='O14'}
    loc=next(pos for pos in rom_battle_map.legal_cells(target,source,
                occupied=occ,burning=b['burning']) if pos!=(u['q'],u['r']))
    before=c.state['core']['rng_state']
    c.do('battle_deploy','O14',{'officer_id':'O14','q':loc[0],'r':loc[1]},'run043:commit')
    assert not c.state['battle']['q04_pending_deployment']
    assert marker_pixel(c,'O14')==SIDES['a']
    assert c.state['core']['rng_state']==before
    cold=load_bytes(save_bytes(c))
    assert marker_pixel(cold,'O14')==SIDES['a']
    assert cold.fingerprint==c.fingerprint


def test_historical_auto_default_retains_army_markers():
    c=battle(4303,manual=False)
    assert 'q04_pending_deployment' not in c.state['battle']
    assert marker_pixel(c,'O14')==SIDES['a']


def test_prebattle_does_not_expose_inactive_battle_command_hints(monkeypatch):
    c=battle(4304)
    seen=[]; original=ImageDraw.ImageDraw.text
    def capture(self,xy,text,*args,**kwargs):
        seen.append(str(text))
        return original(self,xy,text,*args,**kwargs)
    monkeypatch.setattr(ImageDraw.ImageDraw,'text',capture)
    render_battle(c)
    assert any('DEPLOY' in t for t in seen)
    assert not any('Move / Normal / Charge' in t for t in seen)
    seen.clear()
    b=c.state['battle'];u=next(u for u in b['officer_units'] if u['id']=='O14')
    occ={(v['q'],v['r']) for v in b['officer_units'] if v['id']!='O14'}
    q,r=next(pos for pos in rom_battle_map.legal_cells(15,14,occupied=occ,burning=b['burning']) if pos!=(u['q'],u['r']))
    c.do('battle_deploy','O14',{'officer_id':'O14','q':q,'r':r},'run043:visible')
    render_battle(c)
    assert any('Move / Normal / Charge' in t for t in seen)


@pytest.mark.skipif(not os.environ.get('DISPLAY'),reason='real Tk/Xvfb required')
def test_real_tk_controller_previews_then_commits_only_actual_player_marker(tmp_path):
    import tkinter as tk
    from third_branch.tactical_view import TacticalWindow
    from third_branch.integrated import load_file
    root=tk.Tk();root.withdraw()
    try:
        c=battle(4305)
        parent=SimpleNamespace(root=root,c=c,path=tmp_path/'q04-run043.ro2',counter=0,
               audio=SimpleNamespace(effect=Mock(),play=Mock()),selected='p15',
               redraw=Mock(),pause=Mock())
        w=TacticalWindow(parent)
        try:
            assert marker_pixel(c,'O14')!=SIDES['a']
            b=c.state['battle'];u=next(u for u in b['officer_units'] if u['id']=='O14')
            occ={(v['q'],v['r']) for v in b['officer_units'] if v['id']!='O14'}
            q,r=next(pos for pos in rom_battle_map.legal_cells(15,14,
                   occupied=occ,burning=b['burning']) if pos!=(u['q'],u['r']))
            for _ in range(abs(q-w.deployment_q)):
                w.on_nes_button('right' if q>w.deployment_q else 'left')
            for _ in range(abs(r-w.deployment_r)):
                w.on_nes_button('down' if r>w.deployment_r else 'up')
            before=c.state['core']['rng_state']
            w.on_nes_button('a')
            assert parent.c.state['battle']['q04_pending_deployment']==[]
            assert marker_pixel(parent.c,'O14')==SIDES['a']
            assert parent.c.state['core']['rng_state']==before
            assert parent.path.exists() and load_file(parent.path).fingerprint==parent.c.fingerprint
        finally:w.close()
    finally:root.destroy()
