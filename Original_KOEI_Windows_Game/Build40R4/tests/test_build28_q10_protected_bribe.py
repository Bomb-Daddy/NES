"""Q10 protected target source bit auto-switches through the actual campaign ledger."""
from unittest.mock import patch
from third_branch.integrated import new_campaign,validate,save_bytes,load_bytes
from third_branch import officer_battle_adapter as ba,army_cohorts as ac


def test_protected_target_auto_bribe_spends_gold_and_conserves_named_army():
    c=new_campaign(2810)
    ac.add(c.state,'B1','p5',50);ac.refresh(c.state)
    c.do('attack','A3',{'target':'p5','troops':150,'gold':99},'q10:protected:start')
    s=c.state;b=s['battle'];target=ba.find(b,'D','B2')
    target['protected']=True
    before_gold=b['gold_a_cents']
    before_total=sum(ac.army_total(a) for a in s['armies'].values())
    with patch.object(ba,'randrange',side_effect=lambda state,n: 0) as roll:
        result=c.do('battle_bribe','A3',{'officer_id':'A3','target_officer_id':'B2','gold':1},'q10:protected:bribe')
    assert result['success'] is True
    assert all(call.args[1]!=1 for call in roll.call_args_list)
    assert c.state['battle']['gold_a_cents']==before_gold-100
    assert c.state['officer_faction']['B2']=='A'
    assert c.state['officer_loyalty']['B2']==80
    assert ba.find(c.state['battle'],'A','B2')['slot']==2
    assert sum(ac.army_total(a) for a in c.state['armies'].values())==before_total
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
