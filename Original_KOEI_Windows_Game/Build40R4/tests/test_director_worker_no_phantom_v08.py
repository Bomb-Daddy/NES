import pytest
from third_branch.historical_s1 import new_campaign_historical_s1
from third_branch.integrated import CoreError
from third_branch.orders_ui import SPECS, WORKERS, params_for_order


def test_historical_province_player_cannot_allocate_retired_worker_bucket():
    c = new_campaign_historical_s1(balance_policy='OG_1000_DIRECTOR_V2')
    pid=next(pid for pid,owner in c.state['owners'].items() if owner is not None)
    actor=next(oid for oid,o in c.state['core']['officers'].items() if o['province_id']==pid and c.state['officer_faction'][oid]==c.state['owners'][pid])
    before=c.fingerprint
    for params in ({'province_id':pid,'source':'unassigned','target':'farmers','count':1},
                   {'province_id':pid,'source':'farmers','target':'unassigned','count':1}):
        with pytest.raises(CoreError) as exc:
            c.preview('allocate_workers',actor,params,'retired-bucket-deny')
        assert exc.value.code=='RETIRED_WORKER_CATEGORY'
        assert c.fingerprint==before


def test_approved_worker_form_never_offers_unassigned():
    assert len(WORKERS)==7
    assert 'unassigned' not in WORKERS
    cmd, fields=SPECS['Allocate Workers']
    assert cmd=='allocate_workers'
    assert fields[0][2] in WORKERS and fields[1][2] in WORKERS
    assert fields[0][2]!=fields[1][2]
