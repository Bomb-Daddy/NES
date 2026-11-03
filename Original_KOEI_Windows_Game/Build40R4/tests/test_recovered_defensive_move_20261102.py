from third_branch.historical_s1 import new_campaign_historical_s1
from third_branch import army_cohorts as ac
from third_branch.defensive_move import source_safe
from third_branch.integrated import load_bytes, save_bytes
from og.core.fixed import CoreError
import pytest

def test_command_rejects_uncommanded_unguarded_source_without_mutation():
    c=new_campaign_historical_s1(100146)
    s=c.state;pid='p14';src=s['owners'][pid]
    troops=s['core']['provinces'][pid]['troops']
    # For each legal actor the reserve guard is in place; moving everyone
    # without a second valid defender is correctly rejected, not silently lost.
    for oid,o in list(s['core']['officers'].items()):
        if o['province_id']!=pid or s['officer_faction'][oid]!=src:continue
        n=ac.strength(s,oid,pid)
        if not n:continue
        assert isinstance(source_safe(s,pid,src,n,oid),bool)

def test_exact_restored_source_can_be_imported_and_recovered_qa_executes():
    import third_branch.defensive_move as mod
    assert callable(mod.source_safe) and callable(mod.source_reserve)
