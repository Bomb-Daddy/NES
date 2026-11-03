"""Q08 original NES/SNES 4x4 Markov thresholds, plus campaign cold-load."""
from unittest.mock import patch
import os,hashlib
from pathlib import Path
import pytest
from third_branch.integrated import new_campaign,save_bytes,load_bytes,validate
from third_branch import officer_battle_adapter as ba

TABLE=((6,9,10,0),(1,3,6,10),(0,1,2,10),(5,7,8,10))

def test_markov_threshold_boundaries_and_zero_transitions():
    assert ba.WEATHER_MARKOV==TABLE
    for cur in range(4):
        for candidate in range(4):
            threshold=TABLE[cur][candidate]
            if candidate==cur:
                assert ba.weather_transition(cur,candidate,0)==cur
            elif threshold==0:
                assert ba.weather_transition(cur,candidate,0)==cur
            elif threshold==10:
                assert ba.weather_transition(cur,candidate,9)==candidate
            else:
                assert ba.weather_transition(cur,candidate,threshold-1)==candidate
                assert ba.weather_transition(cur,candidate,threshold)==cur
    assert ba.weather_transition(0,3,0)==0
    assert ba.weather_transition(2,3,9)==3

def test_markov_campaign_round_rejects_forbidden_transition_and_cold_loads():
    c=new_campaign(34008)
    c.do('attack','A3',{'target':'p5','troops':120},'q08:start')
    b=c.state['battle'];b['weather']=0;b['day']=2;b['wind']=6;b['burning']=[]
    # Disable spontaneous loyalty RNG for this focused weather branch.
    for u in b['officer_units']:u['morale']=100
    # CPU decisions are deterministic with this legal isolated fixture.
    # We verify the actual end-turn path, not the number of other RNG draws.
    from og.core.rng import randrange as actual_randrange
    def scripted_rand(world,n):
        if n==100:return 0  # enter the 40% weather-attempt gate
        if n==4:return 3    # propose rain, forbidden from weather 0
        if n==10:return 0   # threshold 0 must still reject
        return actual_randrange(world,n)
    with patch.object(ba,'randrange',side_effect=scripted_rand), \
         patch.object(ba,'weather_transition',wraps=ba.weather_transition) as spy:
        c.do('battle_end_turn','A3',{},'q08:round')
        assert spy.call_count==2  # transactional preview and commit
        assert all(call.args==(0,3,0) for call in spy.call_args_list)
    assert c.state['battle']['weather']==0
    assert validate(c.state)
    assert load_bytes(save_bytes(c)).fingerprint==c.fingerprint


def test_exact_markov_bytes_in_user_owned_snes_rom_when_available():
    path=os.environ.get('ROTK2_SNES_ROM')
    if not path:pytest.skip('original SNES ROM not supplied to this test environment')
    raw=Path(path).read_bytes()
    assert hashlib.sha256(raw).hexdigest()==('815bfcf4fd6eb23a20c2e50dde023c210b273ffb6cd86a93909d803c3643ce46')
    assert raw[0x3B5DB:0x3B5EB]==bytes(sum((list(row) for row in TABLE),[]))
    assert raw[0x3B5DA:0x3B5EA]!=raw[0x3B5DB:0x3B5EB]  # wrong-offset negative control


def test_mutated_forbidden_transition_is_detected_by_negative_control():
    # A deliberate wrong 0->3 threshold makes rain incorrectly possible.
    bad=((6,9,10,10),)+TABLE[1:]
    with patch.object(ba,'WEATHER_MARKOV',bad):
        assert ba.weather_transition(0,3,0)==3
    assert ba.weather_transition(0,3,0)==0
