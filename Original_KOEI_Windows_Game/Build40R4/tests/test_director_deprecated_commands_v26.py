"""Director supersession: old commands are inert archival records, never executable."""
import pytest
from og.core.fixed import CoreError
from third_branch.integrated import COMMANDS,hash_state
from third_branch.historical_s1 import new_campaign_historical_s1
from third_branch.historical_ai_v3 import rank_choices
from third_branch.orders_ui import SPECS
from third_branch.province_stage import NAV,CATEGORY_ORDERS

@pytest.mark.parametrize("kind",["cultivate","occupy_vacant"])
def test_no_old_command_can_preview_or_commit_or_mutate(kind):
    c=new_campaign_historical_s1(37,balance_policy="OG_1000_DIRECTOR_V2")
    oid=next(k for k,o in c.state["core"]["officers"].items()
             if c.state["owners"][o["province_id"]]==c.state["officer_faction"][k])
    before=hash_state(c.state)
    assert kind not in COMMANDS
    for f in (c.preview,c.do,c.execute):
        with pytest.raises(CoreError) as e:f(kind,oid,{"province_id":"p1"},"no-old-kind")
        assert e.value.code=="UNKNOWN_KIND"
        assert hash_state(c.state)==before

def test_no_obsolete_menu_or_ai_candidate():
    c=new_campaign_historical_s1(37,balance_policy="OG_1000_DIRECTOR_V2")
    assert len(NAV)==12
    assert all("Cultivate" not in x and "Occupy Vacant" not in x for x in SPECS)
    assert all("Cultivate" not in group and "Occupy Vacant" not in group for group in CATEGORY_ORDERS.values())
    from third_branch.historical_ai import STYLES
    for i,style in enumerate(STYLES):
        faction="R"+str(i)
        ranks=rank_choices(c,faction,style)
        assert all(ch.kind not in ("cultivate","occupy_vacant") for _,ch in ranks)

def test_move_army_is_only_neutral_entry_gateway():
    c=new_campaign_historical_s1(37,balance_policy="OG_1000_DIRECTOR_V2")
    for oid,o in sorted(c.state["core"]["officers"].items()):
        src=o["province_id"]
        if c.state["owners"].get(src)!=c.state["officer_faction"][oid]:continue
        for dst in c.state["neighbors"][src]:
            if c.state["owners"][dst] is not None:continue
            before=c.fingerprint
            try:
                q=c.preview("relocate_army",oid,{"target":dst},"neutral-move-test")
            except CoreError:continue
            assert c.fingerprint==before
            r=c.execute("relocate_army",oid,{"target":dst},"neutral-move-test",q["quote_id"],q["revision"])
            assert r["took_vacant"] and c.state["owners"][dst]==c.state["officer_faction"][oid]
            assert all(a["kind"]!="occupy_vacant" for a in c.state["log"])
            return
    pytest.fail("No legally defensible neutral movement found")
