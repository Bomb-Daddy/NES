from unittest.mock import patch
from third_branch.integrated import new_campaign,validate,load_bytes,save_bytes
from third_branch import officer_battle_adapter as ba,army_cohorts as ac

def test_move_enemy_dragged_target_gets_five_direction_forest_reaction():
    c=new_campaign(202)
    ac.add(c.state,'B1','p5',50);ac.refresh(c.state)
    c.do('attack','A3',{'target':'p5','troops':150},'q07drag:start')
    s=c.state;b=s['battle']
    ac.take(s,'A2','p2',20)
    ac.ensure(s,'A2')['engaged']=20
    b['participants'].append({'id':'army:A2','troops':20,'home':'p2'})
    b['officer_units'].append(ba._unit(s,'A2','A',3,20,8,5))
    ba._sync(s,b)
    actor=ba.find(b,'A','A3');target=ba.find(b,'D','B2');other=ba.find(b,'D','B1')
    actor['q'],actor['r'],actor['mp']=9,5,6
    target['q'],target['r']=10,5
    other['q'],other['r']=11,7
    row=list(b['terrain'][5]);row[8]='F';b['terrain'][5]=''.join(row)
    before=target['troops100']
    with patch.object(ba,'randrange',return_value=0):
        out=c.do('battle_move_enemy','A3',{'officer_id':'A3','target_officer_id':'B2','q':9,'r':4},'q07drag:move')
    assert out['dragged']
    b=c.state['battle'];target=ba.find(b,'D','B2')
    assert (target['q'],target['r'])==(9,5)
    assert target['troops100']<before
    assert any(e['kind']=='forest_ambush' and e['officer']=='B2' for e in b.get('combat_events',[]))
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
