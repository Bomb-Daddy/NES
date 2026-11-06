"""Run014 Q09 $D648 source-backed immediate peaceful province insertion.

Original source: Q09 STATIC_FINDINGS_v1.00 section 3, hsovl2 $D648,
$D679/$D69A/$D6A5, root $5154/$507C. The port uses an explicit source
metadata gate; this is TESTED_PORT, not fresh original-emulator execution.
"""
from rotk2.saves import codec
from rotk2.battle import engine as bt
from test_officer_system_q09_q10_q11_q14_build13 import battle_game, command
from test_classic_ui_route_build13 import App
from rotk2.input.router import Action
from rotk2.ui.screens import BattleScreen
from rotk2.ui.widgets import ListPrompt


def _guaranteed(e):
    o=e.state.officers[3]
    # Original source byte $40 permits a guaranteed escape without RNG.
    o.q09_source_flag2=0x40
    return o


def test_source_success_transfers_immediately_during_live_battle_and_scales_peaceful_troops(tmp_path):
    e=battle_game();s=e.state;b=s.battle
    officer=_guaranteed(e);unit=next(u for u in b.units if u.officer_id==3)
    unit.troops100=7455
    old_draws=b.combat_draws
    out=command(e,'battle_withdraw',destination=3,whole_army=False,officer_id=3)
    assert out.success,(out.error_code,out.events)
    assert b.result is None and not b.committed, 'individual flee did not end the live battle'
    assert unit.fled and unit.alive
    assert b.retreat_destinations['3']==3
    assert officer.province_id==3, 'source $D6A5 calls root $5154 immediately, not at Q14 settlement'
    assert officer.soldiers==74, 'source $D69A divides 7455 by 100 on peaceful insertion'
    assert b.combat_draws==old_draws, 'source +2 flag $40 consumes no RNG'
    assert s.validate()==[]
    path=tmp_path/'run014_live_flee.ro2';codec.save(s,path)
    loaded=codec.load(path)
    assert loaded.state_hash()==s.state_hash() and loaded.validate()==[]
    assert loaded.officers[3].province_id==3 and loaded.officers[3].soldiers==74


def test_source_flee_to_open_province_ownership_and_empty_head_immediate(tmp_path):
    e=battle_game();s=e.state;b=s.battle
    _guaranteed(e)
    # Source empty province+2 officer list. Move the fixture's unrelated
    # reserve officer away before converting this neighbor to unowned.
    s.officers[5].province_id=1
    s.provinces[3].owner=None
    assert s.officers_in(3)==[]
    source_head=s.rulers[1].q14_owned_head
    result=command(e,'battle_withdraw',destination=3,whole_army=False,officer_id=3)
    assert result.success,(result.error_code,result.events)
    assert b.result is None and s.officers[3].province_id==3
    assert s.provinces[3].owner==1, 'root $507C owner writer executes before end of battle'
    assert s.provinces[3].q09_officer_head==3, 'root $5154 starts a genuinely empty province officer list'
    assert s.rulers[1].q14_owned_head==source_head, 'non-ruler cannot invent capital/head promotion'
    assert s.validate()==[]
    path=tmp_path/'run014_open_owner.ro2';codec.save(s,path)
    assert codec.load(path).state_hash()==s.state_hash()


def test_unknown_original_fields_keep_legacy_deferred_movement():
    e=battle_game();s=e.state;b=s.battle
    old=s.officers[3].province_id
    assert s.officers[3].q09_source_flag2 is None
    out=command(e,'battle_withdraw',destination=3,whole_army=False,officer_id=3)
    assert out.success and b.result is None and b.units[1].fled
    assert s.officers[3].province_id==old, 'unhydrated original bytes are not asserted source-correct'
    assert b.retreat_destinations['3']==3
    assert s.validate()==[]


def test_illegal_destination_and_controller_cancel_are_atomic():
    e=battle_game();s=e.state;_guaranteed(e)
    state_before=s.state_hash()
    out=command(e,'battle_withdraw',destination=2,whole_army=False,officer_id=3)
    assert not out.success and out.error_code=='ILLEGAL_WITHDRAW_DESTINATION'
    assert s.state_hash()==state_before
    app=App();app.engine.state.officers[1].q09_source_flag2=0x40
    screen=BattleScreen();screen.sel=1
    before=app.engine.state.state_hash()
    screen._withdraw_menu(app)
    assert app.stack
    app.stack[-1].handle(app,Action.CANCEL)
    assert app.engine.state.state_hash()==before


def test_source_random_success_and_existing_head_retained_during_battle():
    e=battle_game();s=e.state;b=s.battle
    officer=s.officers[3]
    officer.q09_source_flag2=0
    officer.q09_source_special_2569=0
    officer.q09_source_movement11=129
    officer.war=0;officer.training=0
    b.units[1].troops100=3456
    b.combat_seed=52
    s.provinces[3].q09_officer_head=5  # original linked list's existing head
    result=command(e,'battle_withdraw',destination=3,whole_army=False,officer_id=3)
    assert result.success and b.result is None
    assert b.combat_draws==2
    assert officer.province_id==3 and officer.soldiers==34
    assert s.provinces[3].q09_officer_head==5, 'ordinary appended officer cannot displace existing HEAD'
    assert s.validate()==[]


def test_source_failed_escape_does_not_transfer_or_claim_open_province():
    e=battle_game();s=e.state;b=s.battle
    officer=s.officers[3]
    officer.q09_source_flag2=0
    officer.q09_source_special_2569=0
    officer.q09_source_movement11=130
    officer.war=0;officer.training=0
    b.units[1].troops100=100
    b.combat_seed=52
    s.officers[5].province_id=1
    s.provinces[3].owner=None
    before=(officer.province_id,officer.soldiers,s.provinces[3].owner)
    result=command(e,'battle_withdraw',destination=3,whole_army=False,officer_id=3)
    assert result.success and b.combat_draws==2
    assert not b.units[1].alive and not b.units[1].fled
    assert (officer.province_id,officer.soldiers,s.provinces[3].owner)==before
    assert s.provinces[3].q09_officer_head is None
    assert s.validate()==[]


def test_software_controller_confirm_executes_real_live_source_writer():
    app=App();s=app.engine.state
    s.officers[3].q09_source_flag2=0x40
    screen=BattleScreen();screen.sel=3
    screen._withdraw_menu(app)
    assert isinstance(app.stack[-1],ListPrompt)
    # Pick the alternate friendly province using actual controller actions.
    while app.stack[-1].menu.current()!=3:
        app.stack[-1].handle(app,Action.DOWN)
    app.stack[-1].handle(app,Action.CONFIRM)
    assert s.battle is not None and s.battle.result is None
    assert s.officers[3].province_id==3
    assert s.battle.retreat_destinations['3']==3
    assert s.validate()==[]
