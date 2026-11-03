"""Q18 must retain Q09 combined rice/gold ledger on CPU-vs-CPU resolution."""
from third_branch.integrated import new_campaign,save_bytes,load_bytes,validate
from third_branch import offscreen_q18


def test_cpu_six_round_settles_both_sides_remaining_pools(monkeypatch):
    c=new_campaign(2101)
    c.state['human_faction']='C'
    observed={}
    original=offscreen_q18.resolve
    def record(s,b):
        result=original(s,b)
        observed.update(rice_a=b['rice_a'],rice_d=b['rice_d'],
                        gold_a=b['gold_a_cents'],gold_d=b['gold_d_cents'])
        return result
    monkeypatch.setattr(offscreen_q18,'resolve',record)
    r=c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'q18:pool:2101')
    assert r['offscreen'] and r['resolved']
    dst=c.state['core']['provinces']['p5']
    assert dst['food_units']==min(30000,(observed['rice_a']+observed['rice_d'])//100)
    assert dst['treasury_gold_cents']==min(3000000,observed['gold_a']+observed['gold_d'])
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_cpu_pool_settlement_deterministic_cold_replay():
    def run():
        c=new_campaign(2102);c.state['human_faction']='C'
        c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'q18:pool:2102')
        return c
    a,b=run(),run()
    assert a.fingerprint==b.fingerprint
    assert load_bytes(save_bytes(a)).fingerprint==a.fingerprint


def test_human_tactical_attack_does_not_settle_cpu_pools_early():
    c=new_campaign(2103)
    c.state['human_faction']='A'
    r=c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'q18:human:2103')
    assert not r.get('offscreen',False)
    assert c.state['battle'] is not None
    assert c.state['battle']['combat_rule']=='ROTK2_UNIFIED_V1_4_OFFICER_LEDGER'
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
