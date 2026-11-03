"""Actual historical-schema campaign battle: cap-blocked remote garrison never enters prisoner menu."""
from third_branch.historical_s1 import new_campaign_historical_s1
from third_branch import army_cohorts as ac
from third_branch.integrated import validate,save_bytes,load_bytes
from third_branch import officer_battle_adapter as adapter


def test_cap_blocked_capture_settles_without_creating_illegal_army():
    c=new_campaign_historical_s1(3810,balance_policy='OG_1000_DIRECTOR_V2')
    s=c.state
    # Synthetic war setup using genuine 41-province historical schema:
    # R1 attacker occupies adjacent p34; R6 has a remote p30 garrison.
    s['owners']['p34']='R1'
    s['core']['officers']['nes:001']['province_id']='p34'
    s['armies']['army:nes:001']['stations']={'p34':72}
    s['core']['provinces']['p34']['food_units']=5000
    s['core']['provinces']['p34']['treasury_gold_cents']=500000
    s['core']['officers']['nes:040']['province_id']='p34'
    s['armies']['army:nes:040']['stations']={'p34':33}
    s['armies']['army:nes:006']['stations']={'p33':20,'p30':80}
    # No alternative R6 commander has room to inherit the 80 remote groups.
    for oid,o in sorted(s['core']['officers'].items()):
        if oid=='nes:006' or s['officer_faction'][oid]!='R6':continue
        a=ac.ensure(s,oid);have=ac.army_total(a)
        if have<100:
            pid=o['province_id'];a['stations'][pid]=a['stations'].get(pid,0)+100-have
    ac.refresh(s)
    assert validate(s)
    c.do('attack','nes:001',{'target':'p33','troops':50,'gold':1},'b5:cap:attack')
    b=c.state['battle']
    unit=next(u for u in b['officer_units'] if u['id']=='nes:006')
    unit['troops100']=0;unit['troops']=0
    adapter._sync(c.state,b)
    b['rice_d']=0
    if b.get('duel_pending'):
        c.do('battle_duel_response','nes:001',{'accepted':False},'b5:cap:duel')
    c.do('battle_end_turn','nes:001',{},'b5:cap:win')
    assert c.state['battle'] is None
    assert c.state['officer_faction']['nes:006']=='R6'
    assert ac.army_total(c.state['armies']['army:nes:006'])<=100
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
    c.advance_month()
    assert validate(c.state)
