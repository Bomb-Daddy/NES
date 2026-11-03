"""Director's latest approved player UI, actual menu command routing and map modes."""
from pathlib import Path
from third_branch.province_stage import NAV,CATEGORY_ORDERS,WORKERS,snapshot,render_province
from third_branch.orders_ui import SPECS
from third_branch.integrated import new_campaign_41


def test_only_director_approved_top_level_categories_and_no_phantom_roster():
    assert NAV==('Domestic','Military','Troop Assignments','Personnel','Market',
                 'Foreign Affairs','Information','Advisor','Save / Load',
                 'Options / Help','End Month','Back to Strategic Map')
    assert 'Unassigned' not in dict(WORKERS)
    text=Path('third_branch/province_stage.py').read_text()
    assert 'OFFICERS / BODY POINTS / FACILITIES' not in text
    assert 'PROVINCE CAPITAL | DEVELOPMENT PLACEHOLDER' not in text
    assert all(x not in NAV for x in ('Commerce','Functions','Strategy','Quick Orders','All Orders'))


def test_all_approved_category_commands_have_real_existing_engine_routes():
    names=[x for v in CATEGORY_ORDERS.values() for x in v]
    assert len(names)==len(set(names))
    assert all(name in SPECS for name in names)
    assert 'Cultivate' not in names
    assert 'Occupy Vacant Province' not in names
    assert {'Train Troops','Equip Arms','Transport','Plan Attack','Attack'}.issubset(names)
    assert {'Ally','Joint','Marry','Give','Cancel Alliance','Threat'}.issubset(names)


def test_no_china_map_command_widgets_or_backdoor_dispatch():
    s=Path('third_branch/strategic_game.py').read_text()
    assert "bar=tk.Frame(self.root" not in s
    assert 'for i,(title,method) in enumerate(self.action_items)' not in s
    assert 'self.action_items[self.command_index][1]()' not in s
    assert 'def _finish_map_target' in s
    assert "self.map_mode!='target'" in s


def test_dynamic_renderer_has_no_permanent_roster_and_is_4_by_3():
    c=new_campaign_41();fp=c.fingerprint
    image=render_province(snapshot(c,1),nav_choices=CATEGORY_ORDERS['Military'])
    assert image.size==(960,720)
    assert c.fingerprint==fp
