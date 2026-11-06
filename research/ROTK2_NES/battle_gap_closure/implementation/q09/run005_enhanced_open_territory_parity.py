"""Run005 Enhanced Q09 independent port-equivalence control; no source mutation.

Checks legacy Enhanced already transfers the successful attacker whole-army
withdrawal to a genuinely emptied adjacent province, retaining faction and
cold-saved province owner. Not full battle parity or an original NES replay.
"""
from third_branch.integrated import new_campaign, save_bytes, load_bytes, validate
from third_branch import army_cohorts as ac


def setup():
    c = new_campaign(1717)
    s = c.state
    s['core']['officers']['C1']['province_id'] = 'p7'
    s['armies']['army:C1']['stations'] = {'p7': 240}
    s['owners']['p6'] = None
    ac.refresh(s)
    assert s['core']['provinces']['p6']['troops'] == 0
    assert validate(s)
    c.do('attack','A3',{'target':'p5','troops':120,'gold':10},'run005:en:start')
    return c


def test_enhanced_open_territory_ownership_matches_classic_semantics():
    c=setup()
    original=save_bytes(c)
    out=c.do('battle_withdraw','A3',{'destination':'p6','whole_army':True},'run005:en:withdraw')
    assert out['resolved'] and c.state['battle'] is None
    s=c.state  # transaction returns replaced current game state
    assert s['owners']['p6']=='A'
    assert s['core']['officers']['A3']['province_id']=='p6'
    assert s['officer_faction']['A3']=='A'
    assert ac.strength(s,'A3','p6')==120
    assert s['owners']['p5']=='B'
    assert validate(s)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    replay=load_bytes(original)
    replay.do('battle_withdraw','A3',{'destination':'p6','whole_army':True},'run005:en:withdraw')
    assert replay.fingerprint==c.fingerprint
