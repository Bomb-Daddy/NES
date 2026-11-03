"""Director ruling 2026-11-02: every screen/UI is 4:3. No OS dialogs; no window can be resized out of 4:3."""
import os, tkinter, pytest
pytestmark=pytest.mark.skipif(not os.environ.get('DISPLAY'),reason='Real Tk display required (run under xvfb-run)')
from tests.test_view_province_flow_20261102 import _start

def _is43(w,h):return abs(w*3-h*4)<=4

def test_all_open_windows_are_4x3_and_not_resizable(tmp_path):
    game=_start(tmp_path)
    try:
        game.root.update()
        wins=[game.root,game.province_stage.root]
        game.show_context_help();game.root.update()
        wins+= [d for d in game.root.winfo_children() if isinstance(d,tkinter.Toplevel)]
        for w in wins:
            w.update()
            assert _is43(w.winfo_width(),w.winfo_height()),(w.title(),w.winfo_width(),w.winfo_height())
        for w in (game.root,game.province_stage.root):
            assert w.resizable()==(False,False),w.title()
    finally:game.root.destroy()

def test_confirm_dialog_is_in_game_4x3_and_answers(tmp_path):
    game=_start(tmp_path)
    try:
        seen={}
        def press(answer):
            def go():
                w=game.confirm_win;w.update()
                seen['size']=(w.winfo_width(),w.winfo_height())
                w.event_generate('<Return>' if answer else '<Escape>')
            return go
        for answer in (True,False):
            game.root.after(200,press(answer))
            assert game.confirm_dialog('Confirm live order','x')==answer
            assert _is43(*seen['size']),seen
    finally:game.root.destroy()

def test_no_os_dialogs_left_in_shipped_ui():
    import pathlib,re
    root=pathlib.Path(__file__).resolve().parent.parent
    bad=[]
    for p in list((root/'third_branch').glob('*.py'))+list((root/'launcher').glob('*.py')):
        if 'rollback' in p.name:continue
        for i,l in enumerate(p.read_text().splitlines(),1):
            if re.search(r'simpledialog\.|messagebox\.|filedialog\.',l) and not l.strip().startswith('#'):bad.append((p.name,i))
    assert bad==[],bad

def test_tactical_move_buttons_use_in_window_targeting(tmp_path):
    from unittest.mock import Mock
    from third_branch.tactical_view import TacticalWindow
    t=object.__new__(TacticalWindow);t.parent=Mock();t.parent.c.state={'battle':{'a':{'q':1,'r':1}}}
    t.start_target=Mock();t.selected_target=None;t.message=Mock()
    t.move();t.start_target.assert_called_once_with()
    t.start_target.reset_mock();t.move_enemy();t.start_target.assert_not_called()
    t.selected_target='x';t.move_enemy();t.start_target.assert_called_once_with('battle_move_enemy')

def test_troop_assignments_window_is_4x3_and_fixed(tmp_path):
    from third_branch.troop_assignments_ui import TroopAssignmentsWindow
    game=_start(tmp_path)
    try:
        stage=game.province_stage
        stage.troop_dialog=TroopAssignmentsWindow(stage);game.root.update()
        w=stage.troop_dialog.root;w.update()
        assert _is43(w.winfo_width(),w.winfo_height()),(w.winfo_width(),w.winfo_height())
        assert w.resizable()==(False,False)
    finally:game.root.destroy()

def test_allocate_workers_form_defaults_are_a_legal_order(tmp_path):
    """Regression (found by layer-2 harness): the Allocate Workers form pre-filled Target with a province id,
    so pressing Confirm with the shown defaults was refused ('target is not in the allowed list')."""
    from third_branch.orders_ui import OrdersWindow, params_for_order, WORKERS
    game=_start(tmp_path)
    try:
        ow=OrdersWindow(game,initial_order='Allocate Workers');game.root.update()
        vals={k:v.get() for k,v in ow.fields.items()}
        assert vals['target'] in WORKERS and vals['source'] in WORKERS,vals
        kind,params=params_for_order('Allocate Workers',vals,f'p{game.selected}')
        assert kind=='allocate_workers' and params['target'] in WORKERS
    finally:game.root.destroy()

def test_closing_the_battle_window_clears_the_parent_reference(tmp_path):
    from unittest.mock import Mock
    from third_branch.tactical_view import TacticalWindow
    parent=Mock();t=object.__new__(TacticalWindow);t.root=Mock();t.parent=parent;parent.tactical=t
    t.close()
    assert parent.tactical is None

def _actor_in_selected(game):
    return next(o for o,rec in sorted(game.c.state['core']['officers'].items())
                if rec['province_id']==f'p{game.selected}' and game.c.state['officer_faction'][o]==game.player)

def test_rolling_autosaves_are_separate_from_the_five_manual_slots(tmp_path):
    from third_branch.integrated import load_file
    game=_start(tmp_path)
    try:
        fps=[]
        for i in range(5):       # five committed months -> only the newest three autosaves remain
            game.month();game.root.update();fps.append(game.c.fingerprint)
        assert load_file(game.autosave_path(2)).fingerprint==fps[-2]
        assert load_file(game.autosave_path(3)).fingerprint==fps[-3]
        assert game.autosave_path(1).exists() and game.autosave_path(2).exists() and game.autosave_path(3).exists()
        assert not game.autosave_path(4).exists()                       # exactly three rolling files
        assert load_file(game.autosave_path(1)).fingerprint==game.c.fingerprint   # newest = current
        names=sorted(p.name for p in tmp_path.iterdir())
        assert not any('_slot' in n for n in names)                      # autosaves never occupy a manual slot
        assert all(n.count('_auto')==1 for n in names if '_auto' in n)
    finally:game.root.destroy()

def test_load_menu_lists_autosaves_and_loads_one_save_menu_does_not(tmp_path):
    from third_branch.strategic_game import NesPauseMenu
    game=_start(tmp_path)
    try:
        game.month();game.root.update();s1=game.c.fingerprint
        game.month();game.root.update();s2=game.c.fingerprint
        assert s1!=s2
        menu=NesPauseMenu(game)
        menu.mode='save';assert len(menu.entries())==6 and not any('Autosave' in e for e in menu.entries())
        menu.mode='load';e=menu.entries()
        assert [x.split(':')[0] for x in e]==['Slot 1','Slot 2','Slot 3','Slot 4','Slot 5','Autosave 1','Autosave 2','Autosave 3','Back']
        assert e[5].endswith('Occupied') and e[6].endswith('Occupied') and e[7].endswith('Empty')
        menu.mode='load';menu.index=6;menu.choose()          # Autosave 2 = the state before the last month
        assert 'Loaded' in menu.status.get(),menu.status.get()
        assert game.c.fingerprint==s1
        menu.mode='load';menu.index=8;menu.choose()          # Back
        assert menu.mode=='main'
        menu.root.destroy()
    finally:game.root.destroy()
