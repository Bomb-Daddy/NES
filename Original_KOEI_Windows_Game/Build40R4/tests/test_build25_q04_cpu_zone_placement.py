"""Q04: original AI chooses a random free zone tile; deterministic port RNG is replayable."""
from third_branch.integrated import new_campaign_41,save_bytes,load_bytes
from third_branch import rom_battle_map


def battle(seed):
    c=new_campaign_41(seed)
    c.state['human_faction']='A'
    c.do('attack','O14',{'target':'p15','troops':100},f'q04cpu:{seed}')
    return c


def test_cpu_deployment_uses_saveable_random_choice_in_rom_region():
    c=battle(4101); b=c.state['battle']
    assert b['deployment_source']=='NES_ROM_Q04_REGIONS_CPU_RANDOM_PORT'
    for u in b['officer_units']:
        assert rom_battle_map.deployment_province(15,u['q'],u['r'])==(14 if u['side']=='A' else 15)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    d=battle(4101)
    assert d.state['battle']['officer_units']==b['officer_units']
    assert d.fingerprint==c.fingerprint


def test_cpu_placement_changes_across_controlled_rng_seeds_without_leaving_zone():
    cells=[]
    for seed in (4101,4102,4103,4104,4105,4106):
        b=battle(seed).state['battle']
        u=next(u for u in b['officer_units'] if u['side']=='D')
        assert rom_battle_map.deployment_province(15,u['q'],u['r'])==15
        cells.append((u['q'],u['r']))
    assert len(set(cells))>=2


def test_human_deployment_keeps_provisional_auto_zone_policy():
    c=battle(4101);b=c.state['battle']
    human=next(u for u in b['officer_units'] if u['side']=='A')
    assert human['q']>=0 and human['r']>=0
    assert human['commander'] and human['slot']==2
    assert not any(u['side']=='D' and u['slot']!=0 for u in b['officer_units'])


def test_fled_pending_reinforcement_never_reappears_on_battlefield():
    from third_branch import officer_battle_adapter as ba
    from og.core.world import add_officer
    from third_branch import army_cohorts as ac
    from third_branch.integrated import new_campaign
    c=new_campaign(1788);s=c.state
    add_officer(s['core'],'A4','p3',(55,65,70,60,70))
    s['officer_faction']['A4']='A';s['officer_loyalty']['A4']=72
    ac.add(s,'A4','p3',45);ac.refresh(s)
    c.do('attack','A3',{'target':'p5','troops':120},'q04:fled-start')
    b=c.state['battle']
    ba._queue_reinforcements(s,b,{'source':'p3','officer_ids':['A4'],
        'rice':0,'gold':0,'arrival_day':2},side='A')
    u=next(u for u in b['officer_units'] if u['id']=='A4')
    u['fled']=True;b['day']=2
    assert ba._arrive_reinforcements(s,b)==[]
    assert u['reserve'] and (u['q'],u['r'])==(-1,-1)
