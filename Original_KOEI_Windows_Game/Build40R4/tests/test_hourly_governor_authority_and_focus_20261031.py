"""True fail-before / pass-after of audited HQA-018-03 and HQA-018-04."""
import copy
import pytest
from og.core.fixed import CoreError
from third_branch.integrated import new_campaign,validate

def test_nonruler_cannot_appoint_governor_and_state_is_unchanged():
    c=new_campaign(98); before=c.fingerprint
    with pytest.raises(CoreError,match="REQUIRES_RULER"):
        c.do("appoint_governor","A2",{"officer":"A2","focus":"personnel"},"hourly-nonruler")
    assert c.fingerprint==before
    assert not c.state["governors"]

def test_ruler_can_appoint_valid_governor():
    c=new_campaign(98)
    result=c.do("appoint_governor","A1",{"officer":"A1","focus":"personnel"},"hourly-ruler")
    assert result["governor"]=="A1"
    assert validate(c.state)
    assert c.clone().fingerprint==c.fingerprint

@pytest.mark.parametrize("focus",["BAD",None,42])
def test_invalid_focus_rejected_by_validator_not_monthly_keyerror(focus):
    c=new_campaign(98)
    c.state["governors"]["p2"]={"officer":"A2","focus":focus}
    with pytest.raises(CoreError,match="BAD_GOVERNOR_FOCUS"):
        validate(c.state)


def test_ruler_appoints_other_friendly_province_not_own():
    c=new_campaign(98)
    result=c.do('appoint_governor','A1',{'officer':'A2','focus':'defense'},'remote-governor')
    assert result['province']=='p2'
    assert c.state['governors']['p2']['officer']=='A2'
    assert c.clone().fingerprint==c.fingerprint

def test_mismatch_explicit_destination_does_not_modify_state():
    c=new_campaign(98);before=c.fingerprint
    with pytest.raises(CoreError, match='BAD_GOVERNOR'):
        c.do('appoint_governor','A1',{'officer':'A2','focus':'farms','province_id':'p1'},'bad-location')
    assert c.fingerprint==before
