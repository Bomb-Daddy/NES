"""Director: old Cultivate and Occupy commands must be absent from ALL executable AI and UI paths."""
import ast
from pathlib import Path
import pytest
from og.core.fixed import CoreError
from third_branch.historical_s1 import new_campaign_historical_s1
from third_branch import ai, balance_ai_candidate, balance_ai_candidate_v2
from third_branch.historical_ai_v3 import rank_choices, STYLES

RETIRED={"cultivate", "occupy_vacant"}

def test_no_executable_retired_command_constants():
    root=Path(__file__).resolve().parents[1]/'third_branch'
    for file in root.rglob('*.py'):
        tree=ast.parse(file.read_text(encoding='utf-8'), filename=str(file))
        for node in ast.walk(tree):
            if isinstance(node, ast.Constant) and node.value in RETIRED:
                pytest.fail(f'retired command compiled in {file}:{node.lineno}: {node.value}')

def test_retired_command_rejected_before_mutating_world():
    c=new_campaign_historical_s1(31)
    actor=next(k for k,v in c.state['officer_faction'].items() if v=='R0')
    pid=c.state['core']['officers'][actor]['province_id'];old=c.fingerprint
    for kind, params in [("cultivate",{"province_id":pid}),("occupy_vacant",{"target":next(iter(c.state['neighbors'][pid]))})]:
        with pytest.raises(CoreError):c.preview(kind,actor,params, 'retired:'+kind)
        assert c.fingerprint==old

def test_all_active_ai_generators_no_retired_kinds():
    c=new_campaign_historical_s1(31)
    for i,style in enumerate(STYLES):
        faction='R'+str(i)
        assert not RETIRED.intersection(x.kind for _,x in rank_choices(c,faction,style))
        for mod in (balance_ai_candidate, balance_ai_candidate_v2):
            assert not RETIRED.intersection(kind for actor,kind,params in mod.candidates(c,faction,style))
    assert 'cultivate' not in ai.take_turn.__code__.co_consts


def test_every_optional_ai_mode_can_execute_one_live_turn():
    for mod in (balance_ai_candidate,balance_ai_candidate_v2):
        for i,style in enumerate(STYLES):
            c=new_campaign_historical_s1(43)
            action=mod.take_turn(c,f'R{i}',style,prefix='qa-no-old')
            assert action['kind'] not in RETIRED
            assert action['kind'] not in {'naval','ship'}
