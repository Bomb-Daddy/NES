"""Run013: hsovl2 Q09 native $BFBB 8-bit subtraction N-flag movement refresh.

The Director's unified SNES/NES mechanics source is the published Q09
ESCAPE_SCORE_ADDENDUM. A normal Python max() is not equivalent to native BMI
when an injected officer movement byte is high (>= 130 with trained floor 2).
Original emulator execution is NOT claimed by this port fixture.
"""
from rotk2.battle import engine as bt
from rotk2.saves import codec
from rotk2.state.rng import RNG
from test_officer_system_q09_q10_q11_q14_build13 import battle_game, command
from test_classic_ui_route_build13 import App
from rotk2.input.router import Action
from rotk2.ui.screens import BattleScreen


def _known(e, oid=3, movement=130):
    s=e.state
    o=s.officers[oid]
    o.q09_source_flag2=0
    o.q09_source_special_2569=0
    o.q09_source_movement11=movement
    o.war=0
    o.training=0
    return s,o


def test_bfbb_native_N_flag_not_python_max_at_wrap_boundaries():
    assert bt.q09_effective_movement_byte(0, 0)==2
    assert bt.q09_effective_movement_byte(1, 0)==2
    assert bt.q09_effective_movement_byte(2, 0)==2
    assert bt.q09_effective_movement_byte(127, 0)==127
    assert bt.q09_effective_movement_byte(128, 0)==128
    assert bt.q09_effective_movement_byte(129, 0)==129  # (129-2)&255=127, N=0
    assert bt.q09_effective_movement_byte(130, 0)==2    # (130-2)&255=128, N=1
    assert bt.q09_effective_movement_byte(255, 0)==2
    assert bt.q09_effective_movement_byte(3, 100)==6
    assert bt.q09_effective_movement_byte(8, 100)==8


def test_high_source_byte_changes_observable_escape_vs_python_max(tmp_path):
    e=battle_game();s,o=_known(e,movement=130);b=s.battle
    u=next(u for u in b.units if u.officer_id==3)
    u.troops100=100
    # Same deterministic replay seed: first roll 0..9 then 0..99.
    b.combat_seed=52;b.combat_draws=0
    rng=RNG(52);r10,r100=rng.randint(0,9),rng.randint(0,99)
    assert (r10,r100)==(2,88)
    assert bt.q09_opposing_neighbor_count(b,u)==0
    before=({v.officer_id:(v.alive,v.fled,v.soldiers) for v in b.units},s.provinces[b.target_province].owner)
    result=command(e,'battle_withdraw',destination=1,whole_army=False,officer_id=3)
    assert result.success,(result.error_code,result.events)
    assert b.combat_draws==2 and b.combat_seed==rng.state
    assert any('movement=2 ' in msg and 'escaped=False' in msg for msg in b.log)
    assert not u.alive and not u.fled
    assert str(u.officer_id) not in b.retreat_destinations
    assert before[1]==s.provinces[b.target_province].owner
    assert s.validate()==[]
    p=tmp_path/'run013_bfbb_escape.ro2';codec.save(s,p)
    cold=codec.load(p)
    assert cold.state_hash()==s.state_hash() and cold.validate()==[]
    assert cold.officers[3].q09_source_movement11==130


def test_adjacent_high_boundary_keeps_native_no_N_and_guaranteed_escape():
    e=battle_game();s,o=_known(e,movement=129);b=s.battle
    u=next(u for u in b.units if u.officer_id==3)
    u.troops100=100;b.combat_seed=52;b.combat_draws=0
    result=command(e,'battle_withdraw',destination=1,whole_army=False,officer_id=3)
    assert result.success
    assert 'movement=129 ' in b.log[-2]
    assert u.fled and u.alive and b.combat_draws==2
    assert b.retreat_destinations[str(u.officer_id)]==1
    assert s.validate()==[]


def test_invalid_byte_rejects_pre_random_and_cancel_keeps_unmodified_state():
    e=battle_game();s,o=_known(e,movement=256);b=s.battle
    before=(s.state_hash(),b.combat_seed,b.combat_draws)
    result=command(e,'battle_withdraw',destination=1,whole_army=False,officer_id=3)
    assert not result.success and result.error_code=='INVALID_Q09_SOURCE_BYTES'
    assert (s.state_hash(),b.combat_seed,b.combat_draws)==before
    app=App();screen=BattleScreen();screen.sel=1
    state_before=app.engine.state.state_hash()
    screen._withdraw_menu(app)
    app.stack[-1].handle(app,Action.CANCEL)
    assert app.engine.state.state_hash()==state_before
