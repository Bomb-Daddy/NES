"""B5-0041: real-launcher contextual Help routing and battle-control help coverage."""
import os
import pytest
from third_branch.context_help import BATTLE_HELP, DEFAULT_HELP, help_for

pytestmark = pytest.mark.skipif(not os.environ.get('DISPLAY'), reason='Real Tk DISPLAY required')


def test_real_province_submenu_help_names_the_selected_order(tmp_path):
    from third_branch.strategic_game import StrategicWindow
    game = StrategicWindow(tmp_path/'b5_help_fresh.json')
    try:
        game.root.update()
        game.selected = 4
        game.on_nes_button('a')
        assert game.ruler_confirm() is True
        game.root.update()
        stage = game.province_stage
        assert stage.active()
        stage.index = 1  # Military
        stage.on_nes_button('a')
        assert stage.submenu is not None
        stage.index = stage.submenu.index('Attack')
        original = game.c.fingerprint
        game.on_nes_button('help')
        assert game.help_dialog is not None
        assert game.help_dialog.title() == 'HELP — Attack'
        assert game.c.fingerprint == original
    finally:
        game.close()


def test_every_visible_tactical_control_has_specific_help():
    names = ('Move','Next Officer','Next Target','Rest','Move Enemy','Bribe','Duel',
             'Reinforce','Withdraw','Prisoner Fate','Normal','Charge','Simult.',
             'Fire','End Turn','Auto Resolve','Close')
    missing = [name for name in names if name not in BATTLE_HELP or help_for(name,'battle')[1] == '\n\n'.join(DEFAULT_HELP)]
    assert not missing, f'Visible tactical controls without meaningful help: {missing}'


def test_every_approved_province_order_opens_its_exact_live_form_and_help(tmp_path):
    from third_branch.strategic_game import StrategicWindow
    from third_branch.province_stage import NAV, CATEGORY_ORDERS
    from third_branch.orders_ui import SPECS
    game=StrategicWindow(tmp_path/'b5_route_41.json')
    observed=[]
    try:
        game.root.update()
        game.selected=4
        game.on_nes_button('a')
        assert game.ruler_confirm() is True
        game.root.update()
        stage=game.province_stage
        assert stage.active()
        original=game.c.fingerprint
        for category,commands in CATEGORY_ORDERS.items():
            stage.submenu=None
            stage.index=NAV.index(category)
            game.on_nes_button('a')
            assert stage.submenu==commands
            for i,command in enumerate(commands):
                assert command in SPECS
                stage.index=i
                game.on_nes_button('help')
                assert game.help_dialog.title()=='HELP — '+command
                game.on_nes_button('b')
                assert game.help_dialog is None
                game.on_nes_button('a')
                dialog=game.orders_dialog
                assert dialog.root.winfo_exists()
                assert dialog.order.get()==command
                assert dialog.allowed_orders==commands
                observed.append((category,command))
                game.on_nes_button('b')
                assert not dialog.root.winfo_exists()
                assert game.c.fingerprint==original
        assert len(observed)==sum(map(len,CATEGORY_ORDERS.values()))==27
    finally:
        game.close()


def test_troop_assignments_real_controller_and_contextual_help(tmp_path):
    from third_branch.strategic_game import StrategicWindow
    from third_branch.province_stage import NAV
    from third_branch.troop_assignments_ui import ACTIONS
    game=StrategicWindow(tmp_path/'troop_ui_41.json')
    try:
        game.root.update()
        game.selected=4
        game.on_nes_button('a')
        assert game.ruler_confirm() is True
        game.root.update()
        stage=game.province_stage
        stage.index=NAV.index('Troop Assignments')
        before=game.c.fingerprint
        game.on_nes_button('a')
        troop=stage.troop_dialog
        assert troop.active()
        assert tuple(troop.action.get() for _ in [0]) == (ACTIONS[0],)
        assert len(troop.fields)==4
        game.on_nes_button('help')
        assert game.help_dialog.title()=='HELP — Action'
        game.on_nes_button('b')
        assert game.help_dialog is None
        game.on_nes_button('right')
        assert troop.action.get()==ACTIONS[1]
        game.on_nes_button('help')
        assert game.help_dialog.title()=='HELP — Action'
        game.on_nes_button('b')
        game.on_nes_button('down')
        game.on_nes_button('help')
        assert game.help_dialog.title()=='HELP — General'
        game.on_nes_button('b')
        game.on_nes_button('b')
        assert not troop.active()
        assert game.c.fingerprint==before
    finally:
        game.close()
