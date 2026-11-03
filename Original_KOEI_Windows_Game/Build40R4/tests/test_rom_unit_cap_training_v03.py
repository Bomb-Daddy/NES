"""ROM-derived behavior: general group cap and observed NES Train 6502 VM result.

Provenance: October 2026 Mesen trace 2*WAR/13; exact edge cases unknown.
"""
from pathlib import Path
import copy
import pytest
from og.core.fixed import CoreError
from third_branch.historical_s1 import new_campaign_historical_s1
from third_branch import army_cohorts as ac
from third_branch.integrated import validate,save_bytes,load_bytes


def s1():return new_campaign_historical_s1(100111,balance_policy='OG_1000_DIRECTOR_V2')


def test_all_original_generals_and_army_metrics_exist_and_are_capped():
    c=s1();s=c.state
    assert len(s['unit_training'])==len(s['unit_armed_men'])==107
    assert len(s['armies'])==100
    assert sum(ac.army_total(a) for a in s['armies'].values())==2670
    assert all(ac.army_total(a)<=100 for a in s['armies'].values())
    assert s['unit_training']['nes:009']==10
    assert s['unit_armed_men']['nes:009']==1000
    assert validate(s)


def test_100_plus_one_transfer_rejected_before_any_mutation():
    c=s1();before=c.fingerprint
    with pytest.raises(CoreError) as e:
        c.do('assign_troops','nes:019',{'target_officer':'nes:009','count':1},'overcap')
    assert e.value.code=='GENERAL_TROOP_CAP'
    assert c.fingerprint==before
    assert ac.strength(c.state,'nes:009','p3')==100
    assert ac.strength(c.state,'nes:019','p3')==20


def test_100_plus_one_direct_add_rejected_without_mutation():
    c=s1();before=copy.deepcopy(c.state['armies'])
    with pytest.raises(CoreError) as e:ac.add(c.state,'nes:009','p3',1)
    assert e.value.code=='GENERAL_TROOP_CAP'
    assert c.state['armies']==before


def test_corrupted_overcap_save_rejected_even_if_province_total_refreshed():
    c=s1();s=copy.deepcopy(c.state)
    s['armies']['army:nes:009']['stations']['p3']=101
    ac.refresh(s)
    with pytest.raises(CoreError) as e:validate(s)
    assert e.value.code=='GENERAL_TROOP_CAP'


def test_train_one_general_changes_every_local_unit_skill_and_not_war():
    c=s1();s=c.state
    old_war={oid:s['core']['officers'][oid]['war'] for oid in ['nes:009','nes:019']}
    before={oid:s['unit_training'][oid] for oid in old_war}
    delta=2*old_war['nes:009']//13
    response=c.do('train_troops','nes:009',{},'train-s1')
    assert response['skill_gain_nominal']==delta
    for oid in old_war:
        assert c.state['unit_training'][oid]==min(100,before[oid]+delta)
        assert c.state['core']['officers'][oid]['war']==old_war[oid]
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_train_group_sums_floor_2war_over13_caps_at_100_and_rollback():
    c=s1();s=c.state
    war0=s['core']['officers']['nes:009']['war'];war1=s['core']['officers']['nes:019']['war']
    before=c.fingerprint
    with pytest.raises(CoreError):c.do('train_troops','nes:009',{'trainers':['nes:009','nes:009']},'dup')
    assert c.fingerprint==before
    c.state['unit_training']['nes:009']=98
    gain=2*war0//13+2*war1//13
    r=c.do('train_troops','nes:009',{'trainers':['nes:009','nes:019']},'group')
    assert r['skill_gain_nominal']==gain
    assert c.state['unit_training']['nes:009']==100
    assert c.state['unit_training']['nes:019']==min(100,10+gain)


def test_arm_equipment_deducts_stock_and_cannot_double_equip():
    c=s1();s=c.state
    before_arms=s['extras']['p3']['arms'];eq_before=s['unit_armed_men']['nes:009']
    response=c.do('equip_arms','nes:009',{'general':'nes:009','units':2},'equip')
    assert response['weapons_equipped']==200
    assert c.state['unit_armed_men']['nes:009']==eq_before+200
    assert c.state['extras']['p3']['arms']==before_arms-2
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_provisional_empty_soldiers_have_no_real_weapons():
    c=s1();s=c.state
    zero=[oid for oid in s['unit_training'] if oid not in {a['commander'] for a in s['armies'].values()}]
    assert len(zero)==7
    assert all(s['unit_armed_men'][oid]==0 for oid in zero)
    name=zero[0];pid=s['core']['officers'][name]['province_id'];f=s['officer_faction'][name]
    actor=next(o for o in s['core']['officers'] if s['core']['officers'][o]['province_id']==pid and s['officer_faction'][o]==f)
    before=c.fingerprint
    with pytest.raises(CoreError) as e:c.do('equip_arms',actor,{'general':name,'units':1},'bad')
    assert e.value.code=='NO_SOLDIERS';assert c.fingerprint==before
