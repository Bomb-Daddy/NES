"""Run016: source sengo $84F6 -> root $4EB0 no-refuge free-officer release.

Source: STATIC_FINDINGS_v1.00 section 2, original SNES bytes verified against
Director-approved NES/SNES mechanics. Only the ordinary officer, no-refuge
branch is corrected. Special flagged-officer branch remains parity OPEN.
"""
from rotk2.battle import engine as bt
from rotk2.saves import codec
from rotk2.state.rng import RNG
from rotk2.state.models import GameState
from test_bpf_q14_release_destination import captured


def _release_fixture():
    state, battle = captured(officer_id=4)
    officer = state.officers[4]
    assert not officer.is_ruler
    assert bt.ruler_release_destination(state,battle,officer.ruler_id) is None
    officer.training = 17
    bt.choose_prisoner(state,battle,4,'release')
    return state,battle


def test_no_refuge_release_resets_training_and_free_officer_in_real_campaign(tmp_path):
    s,b = _release_fixture()
    old = s.state_hash()
    events=[]
    bt.resolve_into_campaign(s,events,RNG(1616))
    officer = s.officers[4]
    assert s.state_hash()!=old and b.committed
    assert officer.alive
    assert officer.ruler_id is None
    assert officer.province_id == b.target_province
    assert officer.soldiers == 0
    assert officer.training == 100  # $4EB0 writes source training byte 100
    assert s.validate() == []
    path=tmp_path/'run016_free_officer.ro2'
    codec.save(s,path)
    cold=codec.load(path)
    assert cold.state_hash()==s.state_hash()
    assert cold.officers[4].training == 100
    assert cold.officers[4].ruler_id is None


def test_no_refuge_free_officer_remains_free_after_json_cold_load():
    s,_ = _release_fixture()
    bt.resolve_into_campaign(s,[],RNG(1617))
    cold=GameState.from_dict(s.to_dict())
    assert cold.validate()==[]
    assert cold.officers[4].ruler_id is None
    assert cold.officers[4].training==100


def test_refuge_release_does_not_reset_training_or_force_free_officer():
    s,b = captured(officer_id=4)
    s.provinces[3].owner = None  # source-admissible neutral adjacent refuge
    assert bt.ruler_release_destination(s,b,2)==3
    s.officers[4].training=17
    bt.choose_prisoner(s,b,4,'release')
    bt.resolve_into_campaign(s,[],RNG(1618))
    assert s.officers[4].province_id==3
    assert s.officers[4].ruler_id==2
    assert s.officers[4].training==17
    assert s.validate()==[]


def test_recruit_captured_officer_does_not_take_free_officer_training_branch():
    s,b=captured(officer_id=4)
    s.officers[4].training=17
    bt.choose_prisoner(s,b,4,'recruit')
    bt.resolve_into_campaign(s,[],RNG(1619))
    assert s.officers[4].ruler_id==b.attacker_ruler
    assert s.officers[4].training==17
    assert s.validate()==[]


def test_software_controller_release_commits_original_training_reset():
    from test_classic_ui_route_build13 import App
    from rotk2.input.router import Action
    from rotk2.ui.screens import BattleScreen
    from rotk2.ui.widgets import ListPrompt
    app=App()
    s,b=captured(officer_id=4)
    s.officers[4].training=17
    app.engine.state=s
    screen=BattleScreen()
    app.push(screen)  # normal UI stack has the battle screen below its modal
    screen._prisoner_menu(app)
    assert app.stack and isinstance(app.stack[-1],ListPrompt)
    prompt=app.stack[-1]
    options=[v for _,v,enabled in prompt.menu.items if enabled]
    assert options==['recruit','release','behead']
    # Enter through the actual software-controller menu, not a direct helper.
    while prompt.menu.current()!='release':
        prompt.handle(app,Action.DOWN)
    prompt.handle(app,Action.CONFIRM)
    assert app.engine.state.battle is None
    assert app.engine.state.officers[4].training==100
    assert app.engine.state.officers[4].ruler_id is None
    assert app.engine.state.validate()==[]
