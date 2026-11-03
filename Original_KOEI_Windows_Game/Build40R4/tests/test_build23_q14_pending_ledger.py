"""Q14 capture menu precedes the province ledger; no premature visible settlement."""
from third_branch.integrated import new_campaign,save_bytes,load_bytes,validate


def pending():
    c=new_campaign(2323)
    c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'q14:pending:start')
    before=(c.state['core']['provinces']['p5']['food_units'],
            c.state['core']['provinces']['p5']['treasury_gold_cents'])
    c.state['battle']['rice_d']=0
    c.do('battle_end_turn','A3',{},'q14:pending:win')
    assert c.state['battle']['pending_outcome']=='ATTACKER'
    return c,before


def test_pending_prisoner_has_not_settled_food_or_gold():
    c,before=pending()
    dst=c.state['core']['provinces']['p5']
    assert (dst['food_units'],dst['treasury_gold_cents'])==before
    assert c.state['owners']['p5']=='B'
    cold=load_bytes(save_bytes(c))
    assert cold.fingerprint==c.fingerprint
    assert validate(c.state)


def test_final_prisoner_choice_commits_remaining_pools_exactly_once():
    c,before=pending()
    b=c.state['battle']
    food=(b['rice_a']+b['rice_d'])//100
    gold=b.get('gold_a_cents',0)+b.get('gold_d_cents',0)
    c.do('battle_prisoner','A3',{'officer_id':'B2','decision':'release'},'q14:pending:finish')
    dst=c.state['core']['provinces']['p5']
    assert (dst['food_units'],dst['treasury_gold_cents'])==(min(30000,food),min(3000000,gold))
    assert c.state['owners']['p5']=='A'
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
