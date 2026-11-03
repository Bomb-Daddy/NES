"""Source-gated six-to-one retreat and Q14 CPU capture decision parity."""
from third_branch.integrated import new_campaign, save_bytes, load_bytes, validate
from third_branch import offscreen_q18
from third_branch import army_cohorts as ac


def _battle(seed, free_neighbor=False):
    c=new_campaign(seed); s=c.state
    if free_neighbor:
        s['owners']['p6']='B';s['officer_faction']['C1']='B'
    s['human_faction']='A'
    assert validate(s)
    c.do('attack','A3',{'target':'p5','troops':120,'gold':1},f'q18:setup:{seed}')
    s=c.state
    s['human_faction']='C'
    s['battle']['rice_a']=s['battle']['rice_d']=1000000
    return c


def test_q18_early_retreat_six_to_one_requires_free_neighbor(monkeypatch):
    c=_battle(2020,True);s=c.state
    monkeypatch.setattr(offscreen_q18,'_strength',lambda state,battle,side:70 if side=='A' else 10)
    report=offscreen_q18.resolve(s,s['battle'])
    assert report['winner']=='ATTACKER' and report['rounds']==1
    assert report['reason']=='SIX_TO_ONE_RETREAT'
    assert s['battle']['q18_retreat_destination']=='p6'
    assert validate(s)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_q18_six_to_one_without_neighbor_does_not_retreat(monkeypatch):
    c=_battle(2021,False);s=c.state
    monkeypatch.setattr(offscreen_q18,'_strength',lambda state,battle,side:70 if side=='A' else 10)
    report=offscreen_q18.resolve(s,s['battle'])
    assert report['rounds']==6 and report['reason']=='SIX_ROUND_STRENGTH_GATE'
    assert s['battle'].get('q18_retreat_destination') is None


def test_q14_cpu_beheads_low_int_war_officer_not_automatically_recruited():
    c=new_campaign(2022);s=c.state;s['human_faction']='C'
    s['owners']['p4']='C';s['officer_faction']['B1']='C'
    s['core']['officers']['B2']['intelligence']=20
    s['core']['officers']['B2']['war']=20
    n=ac.strength(s,'B2','p5');ac.take(s,'B2','p5',n-1);ac.refresh(s)
    assert validate(s)
    r=c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'q14:low-stats')
    assert r['offscreen'] and r['resolved']
    s=c.state
    assert s['history'][-1]['prisoner_decisions']=={'B2':'behead'}
    assert 'B2' not in s['core']['officers']
    assert validate(s)


def test_q18_early_retreat_real_campaign_transaction_and_cold_load(monkeypatch):
    c=new_campaign(2023);s=c.state
    s['owners']['p6']='B';s['officer_faction']['C1']='B'
    s['human_faction']='C'
    assert validate(s)
    monkeypatch.setattr(offscreen_q18,'_strength',lambda state,battle,side:70 if side=='A' else 10)
    result=c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'q18:integrated:retreat')
    assert result['offscreen'] and result['resolved']
    h=c.state['history'][-1]
    assert h['q18_reason']=='SIX_TO_ONE_RETREAT' and len(h['q18_rounds'])==1
    assert any(x['status']=='retreated' and x['province']=='p6'
               for x in h['defender_officer_dispositions'])
    assert c.state['owners']['p5']=='A'
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_q14_cpu_exact_seventy_threshold_recruits():
    c=new_campaign(2024);s=c.state;s['human_faction']='C'
    s['owners']['p4']='C';s['officer_faction']['B1']='C'
    s['core']['officers']['B2']['intelligence']=20
    s['core']['officers']['B2']['war']=50
    n=ac.strength(s,'B2','p5');ac.take(s,'B2','p5',n-1);ac.refresh(s)
    assert validate(s)
    r=c.do('attack','A3',{'target':'p5','troops':120,'gold':1},'q14:boundary:70')
    assert r['offscreen'] and r['resolved']
    assert c.state['history'][-1]['prisoner_decisions']=={'B2':'recruit'}
    assert c.state['officer_faction']['B2']=='A'
    assert validate(c.state)
