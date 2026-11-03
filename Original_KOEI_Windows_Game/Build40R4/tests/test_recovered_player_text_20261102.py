from third_branch.player_text import province_name,faction_name,officer_name,describe_effects
from third_branch.historical_s1 import new_campaign_historical_s1

def test_recovered_read_only_text_source_has_no_mutation():
    c=new_campaign_historical_s1()
    before=c.fingerprint
    assert 'Province' in province_name(c.state,'p1')
    assert faction_name(c.state,'R0')
    assert officer_name(c.state,'nes:000')
    assert 'Food' in describe_effects(c.state,{'food_units':10})
    assert c.fingerprint==before
