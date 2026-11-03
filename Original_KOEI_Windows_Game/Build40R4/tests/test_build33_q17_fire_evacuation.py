"""Q17 source-backed burning-cell evacuation, not full original tactical AI."""
from third_branch.integrated import new_campaign,save_bytes,load_bytes,validate
from third_branch import officer_battle_adapter as ba


def test_cpu_escapes_burning_hex_before_attacking_adjacent_enemy():
    c=new_campaign(33017)
    c.do('attack','A3',{'target':'p5','troops':120},'q17:start')
    s=c.state;b=s['battle']
    actor=ba.find(b,'A','A3');foe=ba.find(b,'D','B2')
    actor.update(q=5,r=5,mp=6,acted=False)
    foe.update(q=6,r=5)
    b['terrain']=['.'*13 for _ in range(12)]
    b['burning']=[[5,5]]
    initial=(actor['q'],actor['r'])
    ba._auto_ai(s,b,'A')
    assert (actor['q'],actor['r'])!=initial, 'Q17: fire escape must precede adjacent attack'
    assert [actor['q'],actor['r']] not in b['burning']


def test_defending_cpu_evacuation_through_real_end_turn_cold_load_and_replay():
    snapshots=[]
    for _ in range(2):
        c=new_campaign(33018)
        c.do('attack','A3',{'target':'p5','troops':120},'q17:begin')
        b=c.state['battle']
        actor=ba.find(b,'D','B2');attacker=ba.find(b,'A','A3')
        actor.update(q=5,r=5,mp=6,acted=False)
        attacker.update(q=6,r=5)
        b['burning']=[[5,5]]
        c.do('battle_end_turn','A3',{},'q17:cpu-turn')
        assert c.state['battle'] is not None
        actor=ba.find(c.state['battle'],'D','B2')
        assert (actor['q'],actor['r'])!=(5,5)
        assert validate(c.state)
        assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint
        snapshots.append((actor['q'],actor['r'],c.fingerprint))
    assert snapshots[0]==snapshots[1]


def test_burning_cpu_with_no_mobility_does_not_teleport():
    c=new_campaign(33019)
    c.do('attack','A3',{'target':'p5','troops':120},'q17:begin')
    s=c.state;b=s['battle']
    actor=ba.find(b,'D','B2')
    actor['mp']=0;b['burning']=[[actor['q'],actor['r']]]
    initial=(actor['q'],actor['r'])
    ba._auto_ai(s,b,'D')
    assert (actor['q'],actor['r'])==initial
