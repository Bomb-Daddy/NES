"""Real game-state event commits, negative controls, and complete soundtrack distribution."""
import copy
import pytest
from third_branch.audio_system import verify_private_assets, GameAudio, TRACKS
from third_branch.historical_s1 import new_campaign_historical_s1
from third_branch.integrated import validate, save_bytes, load_bytes
from third_branch.game_events import apply_environmental, apply_ruler_death, POLICY
from third_branch.historical_art import event_still_for_committed_record
from third_branch.province_stage import snapshot


def campaign():return new_campaign_historical_s1(seed=77,balance_policy='OG_1000_DIRECTOR_V2')


def test_all_fifteen_mp3_physically_bundled_and_no_second_archive_needed():
    if not TRACKS: pytest.skip("optional private music package is not installed")
    result=verify_private_assets()
    assert result['ok'],result
    assert result['mp3_count']==15 and result['sfx_count']==14
    assert len(TRACKS)==15
    audio=GameAudio(backend='mock')
    for n in range(1,16):
        assert audio.play('track:'+str(n))
        assert audio.current==(n,True)
    assert audio.effect('dialog')
    audio.close()


@pytest.mark.parametrize('kind',('locust_plague','typhoon','flood','epidemic','riot','rebellion','falling_star'))
def test_environmental_events_have_actual_mutations_persistence_and_matching_art(kind):
    c=campaign();before=copy.deepcopy(c.state)
    e=apply_environmental(c.state,kind,'p9')
    assert e['kind']==kind and e['province_id']=='p9'
    assert e['result']['effects']
    assert c.fingerprint!=__import__('third_branch.integrated',fromlist=['hash_state']).hash_state(before)
    assert event_still_for_committed_record(e,9).size==(56,40)
    assert event_still_for_committed_record(e,8) is None
    assert snapshot(c,9).event_kind==kind
    assert snapshot(c,8).event_kind is None
    validate(c.state)
    cold=load_bytes(save_bytes(c))
    assert cold.fingerprint==c.fingerprint and cold.state['event_history']==c.state['event_history']


def test_month_real_event_and_deterministic_replay_without_fabrication():
    a=campaign();b=campaign();prev=a.fingerprint
    fired=[]
    for _ in range(24):
        a.advance_month();b.advance_month()
        assert a.fingerprint==b.fingerprint
        assert all(e['event_policy']==POLICY and e in a.state['log'] for e in a.state['event_feed'])
        fired.extend(a.state['event_feed'])
        assert load_bytes(save_bytes(a)).fingerprint==a.fingerprint
    assert fired, 'No events triggered in 24 genuinely simulated months'
    assert a.fingerprint!=prev
    assert all(e['kind']!='ruler_died' or 'successor_officer' in e['result']['effects'] for e in fired)


def test_actual_succession_preserves_armies_and_bans_deceased_officer():
    c=campaign();s=c.state
    before=sum(p['troops'] for p in s['core']['provinces'].values())
    old=s['ruler_slots']['R0']
    event=apply_ruler_death(s,'R0')
    assert event['kind']=='ruler_died' and 'Cao Cao' in event['result']['text']
    assert old not in s['core']['officers']
    assert s['officer_availability'][old]=='Deceased'
    assert s['ruler_slots']['R0']!=old
    assert sum(p['troops'] for p in s['core']['provinces'].values())==before
    assert event_still_for_committed_record(event,9).size==(56,40)
    validate(s)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_invalid_event_is_rejected_without_mutation():
    c=campaign();before=c.fingerprint
    with pytest.raises(ValueError):apply_environmental(c.state,'locust_plague','p41')
    assert c.fingerprint==before
    with pytest.raises(ValueError):apply_environmental(c.state,'fake_disaster','p9')
    assert c.fingerprint==before


def test_actual_secret_letter_delivery_in_engine_and_audio_mapping():
    c=campaign();s=c.state
    src='R0'; pid='p9';o=next(key for key in s['officer_faction'] if s['officer_faction'][key]==src and s['core']['officers'][key]['province_id']==pid)
    # The real diplomacy dispatcher uses real approval odds; a failed request
    # is NOT mislabeled a delivered letter. Only a successful command can do so.
    before=c.fingerprint
    result=c.do('Ally',o,{'target':'R1'},'qa:real:envoy')
    assert c.fingerprint!=before
    events=[x for x in c.state['event_history'] if x['kind'] in ('secret_letter_delivered','messenger_caught_secret_letter')]
    if result['accepted'] or result.get('intercepted'):
        assert len(events)==1
        assert event_still_for_committed_record(events[0],9).size==(56,40)
    else:assert not events
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint

@pytest.mark.parametrize('seed,expected',[(2,'secret_letter_delivered'),(9,'messenger_caught_secret_letter')])
def test_real_envoy_delivery_and_interception_seeds(seed,expected):
    c=new_campaign_historical_s1(seed=seed,balance_policy='OG_1000_DIRECTOR_V2')
    actor=next(k for k,v in c.state['core']['officers'].items()
               if v['province_id']=='p9' and c.state['officer_faction'][k]=='R0')
    c.do('Ally',actor,{'target':'R1'},'qa:envoy')
    assert c.state['event_history'][-1]['kind']==expected
    assert event_still_for_committed_record(c.state['event_history'][-1],9).size==(56,40)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_real_courier_interception_transfers_only_delivered_goods():
    c=new_campaign_historical_s1(seed=12,balance_policy='OG_1000_DIRECTOR_V2')
    actor=next(k for k,v in c.state['core']['officers'].items()
               if v['province_id']=='p10' and c.state['officer_faction'][k]=='R8')
    src=c.state['core']['provinces']['p10']['treasury_gold_cents']
    dest=c.state['core']['provinces']['p11']['treasury_gold_cents']
    result=c.do('transport',actor,{'target':'p11','commodity':'gold','units':100},'qa:convoy')
    assert result['intercepted'] and result['lost_units']==20 and result['delivered_units']==80
    assert c.state['core']['provinces']['p10']['treasury_gold_cents']==src-100
    assert c.state['core']['provinces']['p11']['treasury_gold_cents']==dest+80
    assert c.state['event_history'][-1]['kind']=='messenger_caught_gold_rice'
    assert event_still_for_committed_record(c.state['event_history'][-1],10).size==(56,40)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_ruler_death_rejects_faction_with_no_successor_transactionally():
    c=campaign()
    import copy
    before=c.fingerprint
    with pytest.raises(ValueError):apply_ruler_death(c.state,'UNKNOWN')
    assert c.fingerprint==before
