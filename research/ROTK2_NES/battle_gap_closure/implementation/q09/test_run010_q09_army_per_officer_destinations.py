"""Run010: original hsovl2 D89D iterates separate destinations for each officer.

VERIFIED_STATIC source fixture, TESTED_PORT scope; original r10/r100 escape/capture
is explicitly NOT YET implemented and no new NES replay is claimed.
"""
import pytest
from rotk2.battle import engine as bt
from rotk2.saves import codec
from test_classic_ui_route_build13 import App  # registers lightweight pygame shim
from rotk2.input.router import Action
from rotk2.ui.widgets import ListPrompt, YesNo
from test_officer_system_q09_q10_q11_q14_build13 import battle_game, command


def test_army_officers_can_select_different_legal_destinations_and_cold_load(tmp_path):
    e=battle_game(); s=e.state; b=s.battle
    assert 1 in bt.retreat_destinations(s,b,'A',officer_id=1)
    assert 3 in bt.retreat_destinations(s,b,'A',officer_id=3)
    out=command(e,'battle_withdraw',destination=1,officer_destinations={1:1,3:3},
                whole_army=True,officer_id=1)
    assert out.success, (out.error_code,out.events)
    assert b.retreat_destinations['1']==1
    assert b.retreat_destinations['3']==3
    assert b.units[0].fled and b.units[1].fled
    assert s.validate()==[]
    p=tmp_path/'separate_destinations.ro2'
    codec.save(s,p)
    cold=codec.load(p)
    assert cold.state_hash()==s.state_hash()
    # Terminal withdrawal resolves battle into the campaign at the command boundary.
    assert cold.battle is None and s.battle is None
    assert cold.officers[1].province_id==1
    assert cold.officers[3].province_id==3
    assert cold.officers[3].ruler_id==1
    assert cold.validate()==[]


def test_atomically_reject_illegal_second_officer_destination():
    e=battle_game(); s=e.state; b=s.battle
    before=s.state_hash(); draws=b.combat_draws
    out=command(e,'battle_withdraw',destination=1,officer_destinations={1:1,3:2},
                whole_army=True,officer_id=1)
    assert not out.success and out.error_code=='ILLEGAL_WITHDRAW_DESTINATION'
    assert s.state_hash()==before and b.combat_draws==draws
    assert s.validate()==[]


def test_atomically_reject_incomplete_and_extra_destination_maps():
    for chosen in ({1:1}, {1:1,3:3,2:1}, {1:1,3:3,'2':1}):
        e=battle_game(); s=e.state; before=s.state_hash()
        out=command(e,'battle_withdraw',destination=1,officer_destinations=chosen,
                    whole_army=True,officer_id=1)
        assert not out.success and out.error_code=='INVALID_WITHDRAW_DESTINATIONS'
        assert s.state_hash()==before and s.validate()==[]


def test_cannot_select_per_officer_destinations_by_regular_or_opponent():
    for oid in (3,2):
        e=battle_game();s=e.state;before=s.state_hash()
        out=command(e,'battle_withdraw',destination=1,officer_destinations={1:1,3:3},
                    whole_army=True,officer_id=oid)
        assert not out.success and out.error_code=='ONLY_COMMANDER_WHOLE_WITHDRAW'
        assert s.state_hash()==before


def test_software_controller_prompts_per_officer_and_submits_full_mapping():
    from test_classic_ui_route_build13 import App
    from rotk2.ui.screens import BattleScreen
    app=App(); screen=BattleScreen(); screen.sel=1
    initial=app.engine.state.state_hash(); calls=[]
    screen._apply=lambda _app,kind,**kw:calls.append((kind,kw))
    screen._withdraw_menu(app)
    assert isinstance(app.stack[-1],YesNo)
    app.stack[-1].handle(app,Action.CONFIRM) # Yes: whole army
    first=app.stack[-1]
    assert isinstance(first,ListPrompt)
    first.handle(app,Action.CONFIRM) # first displayed province 1
    assert isinstance(app.stack[-1],ListPrompt)
    assert calls==[] and app.engine.state.state_hash()==initial
    # Second officer's destination uses a different province.
    options=[v for _label,v,_enabled in app.stack[-1].menu.items]
    assert 1 in options and 3 in options
    while app.stack[-1].menu.items[app.stack[-1].menu.cursor][1]!=3:
        app.stack[-1].handle(app,Action.DOWN)
    app.stack[-1].handle(app,Action.CONFIRM)
    assert len(calls)==1
    assert calls[0][0]=='battle_withdraw'
    assert calls[0][1]['officer_destinations']=={1:1,3:3}
    assert calls[0][1]['destination']==1
    assert calls[0][1]['whole_army'] is True
    assert app.engine.state.state_hash()==initial


def test_software_controller_later_cancel_reprompts_but_initial_cancel_is_atomic():
    from test_classic_ui_route_build13 import App
    from rotk2.ui.screens import BattleScreen
    app=App(); screen=BattleScreen(); screen.sel=1
    before=app.engine.state.state_hash();record=[]
    screen._apply=lambda _app,kind,**kw:record.append((kind,kw))
    screen._withdraw_menu(app);app.stack[-1].handle(app,Action.CONFIRM)
    app.stack[-1].handle(app,Action.CONFIRM) # first officer chosen
    app.stack[-1].handle(app,Action.CANCEL) # later: original reprompts
    assert isinstance(app.stack[-1],ListPrompt)
    assert record==[] and app.engine.state.state_hash()==before
    app.stack[-1].handle(app,Action.CONFIRM)
    assert len(record)==1
    app2=App();screen2=BattleScreen();screen2.sel=1
    old=app2.engine.state.state_hash();commands=[]
    screen2._apply=lambda _app,kind,**kw:commands.append(kw)
    screen2._withdraw_menu(app2);app2.stack[-1].handle(app2,Action.CONFIRM)
    app2.stack[-1].handle(app2,Action.CANCEL) # first: abort
    assert commands==[] and app2.stack==[]
    assert app2.engine.state.state_hash()==old


def test_per_officer_allied_liege_destination_can_differ_without_forgery(tmp_path):
    from rotk2.state.models import Officer, Province, Ruler
    e=battle_game(); s=e.state; b=s.battle
    s.rulers[8]=Ruler(8,'Ally')
    s.officers[3].ruler_id=8
    s.officers[3].province_id=4  # ally's prebattle station matches his liege
    s.provinces[4]=Province(4,1,3,0,owner=8,rice=100,gold=100,neighbors=[2])
    s.provinces[2].neighbors.append(4)
    s.officers[8]=Officer(8,'Garrison',60,60,60,ruler_id=8,province_id=4,soldiers=20)
    assert s.validate()==[]
    assert 4 in bt.retreat_destinations(s,b,'A',officer_id=3)
    assert 1 not in bt.retreat_destinations(s,b,'A',officer_id=3)
    out=command(e,'battle_withdraw',destination=1,officer_destinations={1:1,3:4},
                whole_army=True,officer_id=1)
    assert out.success, (out.error_code,out.events)
    assert b.retreat_destinations['3']==4
    assert s.officers[3].province_id==4
    assert s.officers[3].ruler_id==8
    assert s.validate()==[]
    p=tmp_path/'allied_separate.ro2'; codec.save(s,p)
    cold=codec.load(p)
    assert cold.state_hash()==s.state_hash()
    assert cold.officers[3].province_id==4 and cold.officers[3].ruler_id==8