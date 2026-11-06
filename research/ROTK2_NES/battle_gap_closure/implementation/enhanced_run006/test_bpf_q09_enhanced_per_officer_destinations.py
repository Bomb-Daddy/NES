"""Run006 Q09: original D89D whole-army retreat chooses per-officer destinations.

VERIFIED_STATIC source fixture: hsovl2 D89D loops officers with individual choice.
TESTED_PORT: Enhanced actual campaign ledger, strict rejection, save/cold-load.
Not a claim about the still-missing native escape/capture probability.
"""
import pytest
from og.core.fixed import CoreError
from og.core.world import add_officer
from third_branch.integrated import new_campaign, save_bytes, load_bytes, validate
from third_branch import army_cohorts as ac, officer_battle_adapter as ba
def setup(seed):
    c=new_campaign(seed)
    add_officer(c.state['core'],'B9','p5',(60,65,55,70,60))
    c.state['officer_faction']['B9']='B';c.state['officer_loyalty']['B9']=70
    ac.add(c.state,'B9','p5',50);ac.refresh(c.state)
    c.state['human_faction']='B'
    for pid,oid in (('p6','C1'),('p7','C2')):
        c.state['owners'][pid]='B'
        c.state['officer_faction'][oid]='B'
    assert validate(c.state)
    c.do('attack','A3',{'target':'p5','troops':150,'gold':10},'q09:run006:start')
    return c


def test_multi_officer_withdraw_uses_independent_selected_homes_and_cold_load():
    c=setup(6011)
    assert c.state['battle'] is not None
    units=[u for u in c.state['battle']['officer_units'] if u['side']=='D' and not u.get('reserve')]
    assert {u['id'] for u in units} == {'B2','B9'}
    original={u['id']:ac.strength(c.state,u['id'],'p5') for u in units}
    selections={'B2':'p6','B9':'p7'}
    before=save_bytes(c)
    out=c.do('battle_withdraw','B2',{'whole_army':True,'destination':'p6',
            'officer_destinations':selections},'q09:run006:multi')
    assert out['resolved'] and c.state['battle'] is None
    assert out['officer_destinations']==selections
    for oid,home in selections.items():
        assert c.state['core']['officers'][oid]['province_id']==home
        assert ac.strength(c.state,oid,home)==original[oid]
        assert ac.strength(c.state,oid,'p5')==0
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint == c.fingerprint
    rerun=load_bytes(before)
    rerun.do('battle_withdraw','B2',{'whole_army':True,'destination':'p6',
            'officer_destinations':selections},'q09:run006:multi')
    assert rerun.fingerprint==c.fingerprint


@pytest.mark.parametrize('choices',[
    {'B2':'p6'},                          # missing officer
    {'B2':'p6','B9':'p7','B3':'p7'},        # surplus officer
    {'B2':'p7','B9':'p6'},                # commander disagrees with root choice
    {'B2':'p6','B9':'p4'},                # illegal neighboring destination
    {'B2':'p6','B9':'p5'},                # contested rather than retreat province
    {'B2':'p6','B9':1},                   # wrong value type
    {'B2':'p6','B9':None},                # missing value
])
def test_bad_per_officer_selection_rollback_is_byte_exact(choices):
    c=setup(6012)
    before=save_bytes(c)
    with pytest.raises(CoreError):
        c.do('battle_withdraw','B2',{'whole_army':True,'destination':'p6',
               'officer_destinations':choices},'q09:run006:reject')
    assert save_bytes(c)==before
    assert validate(c.state)


def test_single_officer_cannot_supply_whole_army_destinations():
    c=setup(6013);before=save_bytes(c)
    with pytest.raises(CoreError):
        c.do('battle_withdraw','B2',{'officer_id':'B2','destination':'p7',
               'officer_destinations':{'B2':'p7'}},'q09:run006:reject-single')
    assert save_bytes(c)==before


def test_legacy_single_destination_still_operates_as_historical_compatibility():
    c=setup(6014)
    r=c.do('battle_withdraw','B2',{'whole_army':True,'destination':'p7'},'q09:run006:legacy')
    assert r['resolved'] and validate(c.state)
    assert c.state['core']['officers']['B9']['province_id']=='p7'
    assert c.state['core']['officers']['B2']['province_id']=='p7'


@pytest.mark.parametrize('explicit', [False, True])
def test_pending_reinforcement_returns_actual_source_with_or_without_officer_map(explicit):
    c=setup(6015 if explicit else 6016)
    s=c.state; b=s['battle']
    # C1 remains resident at p6, a legitimate real officer home. Its queued
    # army is not permitted to teleport to the chosen p7 retreat destination.
    s['officer_faction']['C1']='B'
    ba._queue_reinforcements(s,b,{'source':'p6','officer_ids':['C1'],
        'rice':0,'gold':0,'arrival_day':2},side='D')
    pending=next(u for u in b['officer_units'] if u['id']=='C1')
    assert pending['reserve'] and pending['home']=='p6'
    before=ac.army_total(s['armies']['army:C1'])
    args={'whole_army':True,'destination':'p7'}
    if explicit:args['officer_destinations']={'B2':'p7','B9':'p6'}
    result=c.do('battle_withdraw','B2',args,'q09:run006:reserve')
    s=c.state
    assert result['resolved'] and s['battle'] is None
    assert result['officer_destinations']['C1']=='p6'
    assert s['core']['officers']['C1']['province_id']=='p6'
    assert ac.army_total(s['armies']['army:C1'])==before
    assert ac.strength(s,'C1','p6')==before
    assert validate(s)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
