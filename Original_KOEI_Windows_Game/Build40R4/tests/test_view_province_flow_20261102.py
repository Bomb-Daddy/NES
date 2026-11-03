"""Director ruling 2026-11-02: no inspect-only menu. View -> China map -> pick a province -> that province's
standard UI opens; for a province you do not control its only command is Exit."""
import os, tkinter, pytest
pytestmark=pytest.mark.skipif(not os.environ.get('DISPLAY'),reason='Real Tk display required (run under xvfb-run)')

def _start(tmp_path):
    from third_branch.strategic_game import StrategicWindow
    game=StrategicWindow(tmp_path/'s.json');game.root.update()
    owned=next(int(p[1:]) for p,o in game.c.state['owners'].items() if o is not None)
    game.selected=owned;game._finish_new_game_setup();game.root.update()
    assert game.ruler_confirm() is True;game.root.update()
    assert game.province_stage is not None and game.province_stage.active()
    return game
def _provinces(game):
    o=game.c.state['owners'];me=game.player
    own=sorted(int(p[1:]) for p,v in o.items() if v==me)
    foreign=sorted(int(p[1:]) for p,v in o.items() if v not in (None,me))
    vacant=sorted(int(p[1:]) for p,v in o.items() if v is None)
    return own,foreign,vacant
def _toplevels(game):return [d for d in game.root.winfo_children() if isinstance(d,tkinter.Toplevel)]

def test_map_pick_foreign_province_opens_standard_ui_with_only_exit(tmp_path):
    game=_start(tmp_path)
    try:
        own,foreign,_=_provinces(game);home=game.selected
        stage=game.province_stage
        assert stage.view_only is False
        stage.on_nes_button('select');game.root.update()
        assert game.map_mode=='manual'
        game.selected=foreign[0];game.redraw()
        game.on_nes_button('a');game.root.update()
        assert game.map_mode is None and game.selected==foreign[0]
        stage=game.province_stage
        assert stage.view_only is True
        assert stage.vm.id==foreign[0]
        for button in ('up','down','left','right','x','y'):
            before=(game.selected,len(_toplevels(game)));stage.on_nes_button(button);game.root.update()
            assert (game.selected,len(_toplevels(game)))==before,button
        for button in ('a','b','select'):
            game.selected=foreign[0];game.map_mode=None;stage.root.deiconify()
            stage.on_nes_button(button);game.root.update()
            assert game.map_mode=='manual' and game.selected==foreign[0],button   # Exit -> map, same province highlighted
            assert getattr(game,'orders_dialog',None) is None or not game.orders_dialog.root.winfo_exists()
        game.on_nes_button('b');game.root.update()       # map B -> back to the player's own province
        assert game.map_mode is None and game.selected==home and game.province_stage.view_only is False
    finally:game.root.destroy()

def test_view_only_province_cannot_open_orders_or_other_menus(tmp_path):
    game=_start(tmp_path)
    try:
        own,foreign,vacant=_provinces(game)
        for target in (foreign[0],vacant[0]):
            game.province_stage.on_nes_button('select');game.root.update()
            game.selected=target;game.redraw();game.on_nes_button('a');game.root.update()
            stage=game.province_stage;assert stage.view_only
            for index in range(0,12):
                game.map_mode=None;game.selected=target;stage.index=index;stage.root.deiconify()
                stage.choose();game.root.update()   # every row resolves to Exit
                assert game.map_mode=='manual'
                game.on_nes_button('a');game.root.update();stage=game.province_stage
            assert getattr(game,'orders_dialog',None) is None or not game.orders_dialog.root.winfo_exists()
            game.on_nes_button('b');game.root.update()
    finally:game.root.destroy()

def test_picking_own_province_on_map_opens_full_commands(tmp_path):
    game=_start(tmp_path)
    try:
        own,_,_=_provinces(game)
        game.province_stage.on_nes_button('select');game.root.update()
        game.selected=own[-1];game.redraw();game.on_nes_button('a');game.root.update()
        assert game.map_mode is None and game.province_stage.view_only is False
        game.province_stage.index=0;game.province_stage.choose();game.root.update()
        assert game.province_stage.submenu is not None   # Domestic submenu: real commands are available
    finally:game.root.destroy()

def test_left_right_on_province_screen_no_longer_changes_province(tmp_path):
    game=_start(tmp_path)
    try:
        home=game.selected
        for button in ('left','right','left'):game.province_stage.on_nes_button(button)
        assert game.selected==home
    finally:game.root.destroy()

def test_click_highlighted_province_on_map_opens_it_and_orders_refuse_foreign(tmp_path):
    from third_branch.strategic_game import SLOTS,MAP_X,MAP_Y,MAP_DW,MAP_DH
    from third_branch.orders_ui import OrdersWindow
    game=_start(tmp_path)
    try:
        own,foreign,_=_provinces(game)
        game.province_stage.on_nes_button('select');game.root.update()
        x,y,w,h=SLOTS[str(foreign[0])]['rect_px']
        class E:pass
        e=E();e.x=MAP_X+(x+w/2)*MAP_DW/1448;e.y=MAP_Y+(y+h/2)*MAP_DH/1086
        game.on_click(e);assert game.map_mode=='manual' and game.selected==foreign[0]   # first click highlights
        game.on_click(e);game.root.update();assert game.map_mode is None and game.province_stage.view_only
        before=len(_toplevels(game))
        with pytest.raises(ValueError):OrdersWindow(game)           # no inspect-only window exists
        assert len(_toplevels(game))==before
    finally:game.root.destroy()

def test_selected_province_marker_is_drawn_even_for_vacant_provinces():
    from third_branch.integrated import new_campaign_41
    from third_branch.historical_s1 import new_campaign_historical_s1
    from third_branch.strategic_game import render,SLOTS,MAP_X,MAP_Y,MAP_DW,MAP_DH
    c=new_campaign_historical_s1()
    for pid,owner in c.state['owners'].items():
        n=int(pid[1:])
        if owner is not None and n%7: continue   # all vacant ones, plus a few owned
        x,y,w,h=SLOTS[str(n)]['rect_px']
        img=render(c,n)
        cx=MAP_X+(x+w/2)*MAP_DW/1448;cy=MAP_Y+(y+h/2)*MAP_DH/1086;r=max(w*MAP_DW/1448,h*MAP_DH/1086)/2+9
        px=img.getpixel((int(cx),int(cy-r+1)))   # top edge of the marker
        assert px==(255,235,60),(n,owner,px)
