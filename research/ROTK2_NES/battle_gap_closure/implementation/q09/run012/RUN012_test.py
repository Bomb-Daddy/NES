"""Run012 Q09 $D582/$D6AC source score and capture, controlled RNG fixtures.

VERIFIED_STATIC original source score; TESTED_PORT controlled fixtures only.
No original-game runtime, physical controller or hardware RNG proof.
"""
import pytest
from rotk2.battle import engine as bt
from rotk2.saves import codec
from test_officer_system_q09_q10_q11_q14_build13 import battle_game, command
from test_classic_ui_route_build13 import App
from rotk2.input.router import Action
from rotk2.ui.screens import BattleScreen
from rotk2.ui.widgets import ListPrompt


def source_data(e, *, flag2=0, special=0):
    s=e.state
    for oid in (1, 3):
        s.officers[oid].q09_source_flag2=flag2
        s.officers[oid].q09_source_special_2569=special
        s.officers[oid].q09_source_movement11=4
    return s


def test_score_exact_integer_division_bonuses_and_caps():
    assert bt.q09_escape_score(3, 50, 40, 9000, 2, False, False, 4)==56
    assert bt.q09_escape_score(3, 50, 40, 9000, 2, True, True, 4)==96
    assert bt.q09_escape_score(6, 95, 30, 15000, 0, True, False, 9)==100
    assert bt.q09_escape_score(0, 0, 0, 0, 0, False, False, 0)==0
    with pytest.raises(ValueError):
        bt.q09_escape_score(-1, 50, 40, 0, 0, False, False, 0)
    with pytest.raises(ValueError):
        bt.q09_escape_score(1, 50, 40, 0, 0, False, False, 10)


def test_same_side_neighbor_not_an_enemy_and_two_draw_order():
    e=battle_game();s=source_data(e);b=s.battle;u=b.units[1]
    # source $C00A compares army-slot bit 1: 0/1 defender; 2/3 invader
    assert bt.q09_opposing_neighbor_count(b,u)==0
    b.units[2].q,b.units[2].r=3,5  # opposing main unit adjacent
    assert bt.q09_opposing_neighbor_count(b,u)==1
    b.units[0].q,b.units[0].r=5,5  # same side neighboring never counts
    assert bt.q09_opposing_neighbor_count(b,u)==1
    from rotk2.state.rng import RNG
    rng=RNG(52)
    expected=(rng.randint(0,9),rng.randint(0,99))
    seed=b.combat_seed
    outcome=bt.q09_escape_check(s,b,u)
    assert outcome in (True,False)
    assert b.combat_draws==2
    assert b.combat_seed==rng.state and b.combat_seed!=seed
    score=bt.q09_escape_score(max(s.officers[u.officer_id].q09_source_movement11,(max(s.officers[u.officer_id].training-1,0)//20)+2),
            s.officers[u.officer_id].war,s.officers[u.officer_id].training,u.troops100,1,
            False,False,expected[0])
    assert outcome==(expected[1]<score)


def test_source_movement11_not_current_ui_action_mp_and_unknown_gated():
    e=battle_game();s=source_data(e);b=s.battle;u=b.units[1]
    s.officers[u.officer_id].training=0
    u.mp=0
    s.officers[u.officer_id].q09_source_movement11=6
    assert bt.q09_escape_score(6, s.officers[u.officer_id].war, 0, u.troops100, 0, False, False, 2)>0
    bt.q09_escape_check(s,b,u)
    assert 'movement=6 ' in b.log[-1] # original officer +11, not remaining action MP
    assert b.combat_draws==2
    s.officers[u.officer_id].q09_source_movement11=None
    previous=b.combat_draws
    assert bt.q09_escape_check(s,b,u) is True # do not invent unknown source byte
    assert b.combat_draws==previous


def test_source_flag_40_forces_escape_without_consuming_rng(tmp_path):
    e=battle_game();s=source_data(e,flag2=0x40);b=s.battle
    initial=(b.combat_seed,b.combat_draws)
    assert bt.q09_escape_check(s,b,b.units[1]) is True
    assert (b.combat_seed,b.combat_draws)==initial
    out=command(e,'battle_withdraw',destination=1,whole_army=False,officer_id=3)
    assert out.success and b.units[1].fled and b.units[1].alive
    assert (b.combat_seed,b.combat_draws)==initial
    p=tmp_path/'known_flag.ro2';codec.save(s,p);cold=codec.load(p)
    assert cold.state_hash()==s.state_hash()
    assert cold.officers[3].q09_source_flag2==0x40
    assert cold.validate()==[]


def test_controlled_failed_escape_capture_and_prisoner_ui(tmp_path):
    e=battle_game();s=source_data(e);b=s.battle
    # Limit war/movement/troops and crowd the departing officer with enemies.
    for oid in (1,3):
        s.officers[oid].war=0;s.officers[oid].training=0
    b.units[0].troops100=100;b.units[0].mp=0
    b.units[1].troops100=100;b.units[1].mp=0
    initial=b.combat_draws
    out=command(e,'battle_withdraw',destination=1,officer_destinations={1:1,3:3},whole_army=True,officer_id=1)
    assert out.success,(out.error_code,out.events)
    assert b.combat_draws==initial+4
    assert any(not u.alive for u in b.units[:2]), 'controlled seed must exercise failed escape'
    assert all(not u.fled for u in b.units[:2] if not u.alive)
    assert all(str(u.officer_id) not in b.retreat_destinations for u in b.units[:2] if not u.alive)
    assert s.validate()==[]
    p=tmp_path/'failed_escape.ro2';codec.save(s,p);cold=codec.load(p)
    assert cold.state_hash()==s.state_hash() and cold.validate()==[]
    assert set(bt.prepare_prisoners(s,b))=={u.officer_id for u in b.units[:2] if not u.alive}


def test_bad_destination_prevalidated_before_rng_or_capture():
    e=battle_game();s=source_data(e);b=s.battle
    before=s.state_hash();draws=b.combat_draws
    out=command(e,'battle_withdraw',destination=1,officer_destinations={1:1,3:2},whole_army=True,officer_id=1)
    assert not out.success and out.error_code=='ILLEGAL_WITHDRAW_DESTINATION'
    assert s.state_hash()==before and b.combat_draws==draws


def test_unknown_old_source_fields_keep_historical_run011_behavior_no_rng():
    e=battle_game();s=e.state;b=s.battle
    assert s.officers[3].q09_source_flag2 is None
    assert s.officers[3].q09_source_special_2569 is None
    before=(b.combat_seed,b.combat_draws)
    out=command(e,'battle_withdraw',destination=1,whole_army=False,officer_id=3)
    assert out.success and b.units[1].fled
    assert (b.combat_seed,b.combat_draws)==before


def test_controller_menu_offers_source_gated_capture_path_and_cancel_atomic():
    app=App();s=app.engine.state;b=s.battle
    s.officers[1].q09_source_flag2=0
    s.officers[1].q09_source_special_2569=0
    screen=BattleScreen();screen.sel=1
    before=s.state_hash()
    screen._withdraw_menu(app)
    assert app.stack
    app.stack[-1].handle(app,Action.CANCEL)
    assert s.state_hash()==before and b.combat_draws==0


def test_invalid_recovered_source_byte_rejected_atomically():
    e=battle_game();s=source_data(e);b=s.battle
    s.officers[3].q09_source_movement11=999
    before=(s.state_hash(),b.combat_draws)
    with pytest.raises(ValueError, match='INVALID_Q09_SOURCE_BYTES'):
        bt.withdraw(s,b,'A',destination=1,whole_army=False,officer_id=3)
    assert (s.state_hash(),b.combat_draws)==before
