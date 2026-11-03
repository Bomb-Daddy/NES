"""Focused, hostile PC Y-help/remap/human identity tests."""
import json
import pytest
from third_branch.context_help import check_help_coverage,help_for,ORDER_HELP,STRATEGIC_HELP
from third_branch.input_bindings import InputBindings,BindingError,DEFAULT_PAD,DEFAULT_KEYS
from third_branch.xbox_controller import PC_XINPUT_FLAGS,pc_xinput_buttons,NesInputRouter
from third_branch.historical_s1 import new_campaign_historical_s1
from third_branch.integrated import save_bytes,load_bytes
from third_branch.strategic_game import StrategicWindow

class Pad:
    def __init__(self):self.value=frozenset()
    def read(self):return self.value
    def close(self):pass

def test_all_22_real_orders_and_12_province_commands_have_nonplaceholder_help():
    assert check_help_coverage()
    assert len(ORDER_HELP)==len(__import__("third_branch.orders_ui",fromlist=["SPECS"]).SPECS)
    assert set(STRATEGIC_HELP)=={'Map','Province target selection'}  # NO old 15-button toolbar
    for label in ORDER_HELP:
        t,body=help_for(label,'orders','units')
        assert t==label and '\n\n' in body and len(body)>130 and 'Current field: units' in body
    assert 'Cancel' in help_for('Cancel Alliance','orders')[0]

def test_xinput_y_is_help_and_never_a():
    assert 'y' in pc_xinput_buttons(0x8000)
    assert 'a' not in pc_xinput_buttons(0x8000)
    assert PC_XINPUT_FLAGS['y'] != PC_XINPUT_FLAGS['a']
    got=[];pad=Pad();tick=[0.0]
    b=InputBindings('/tmp/does_not_exist_koei_binding.json')
    r=NesInputRouter(got.append,pad,clock=lambda:tick[0],bindings=b)
    pad.value={'y'};assert r.poll()==['help']
    tick[0]=0.5;assert r.poll()==[]
    pad.value=set();assert r.poll()==[]
    pad.value={'a'};assert r.poll()==['a']
    assert got==['help','a']

def test_binding_swaps_duplicates_and_roundtrips_without_saves(tmp_path):
    cfg=tmp_path/'settings'/'controls.json'
    b=InputBindings(cfg)
    assert b.remap('pad','help','x') is None
    assert b.remap('pad','help','a')=='a'
    assert b.pad['a']=='x' and b.pad['help']=='a'
    assert b.remap('keyboard','help','F2') is None
    b.save()
    reread=InputBindings.load(cfg)
    assert reread.pad==b.pad and reread.keyboard==b.keyboard
    assert cfg.parent.name=='settings' and 'state' not in json.loads(cfg.read_text())

@pytest.mark.parametrize('device,action,physical',[('pad','b','x'),('pad','help','b'),('pad','a','start'),('keyboard','b','F2'),('keyboard','help','Escape'),('keyboard','a','F10')])
def test_reserved_recovery_keys_never_reassigned(tmp_path,device,action,physical):
    b=InputBindings(tmp_path/'control.json')
    with pytest.raises(BindingError):b.remap(device,action,physical)
    assert b.pad['b']=='b' and b.pad['start']=='start'
    assert b.keyboard['b']=='Escape' and b.keyboard['start']=='F10'

def test_corrupt_settings_revert_to_defaults(tmp_path):
    path=tmp_path/'controls.json';path.write_text('{"format":"KOEI-CONTROLS-1","pad":{"b":"y"}}')
    a=InputBindings.load(path)
    assert a.pad==DEFAULT_PAD and a.keyboard==DEFAULT_KEYS

def test_human_ruler_identity_persisted_with_campaign_hash(tmp_path):
    c=new_campaign_historical_s1(balance_policy='OG_1000_DIRECTOR_V2')
    c.state['human_faction']='R1'
    blob=save_bytes(c)
    loaded=load_bytes(blob)
    assert loaded.state['human_faction']=='R1'
    assert loaded.fingerprint==c.fingerprint
    class P:
        def redraw(self):self.did_redraw=True
    p=P();StrategicWindow._adopt_loaded(p,loaded)
    assert p.player=='R1' and p.did_redraw
    assert loaded.state['owners'][f'p{p.selected}']=='R1'
    loaded.state['human_faction']='R15'
    from og.core.fixed import CoreError
    with pytest.raises(CoreError):save_bytes(loaded)

def test_readonly_help_never_changes_campaign_fingerprint():
    c=new_campaign_historical_s1(balance_policy='OG_1000_DIRECTOR_V2')
    before=c.fingerprint
    for name in ORDER_HELP:help_for(name,'orders')
    assert c.fingerprint==before

@pytest.mark.parametrize('ruler_index',range(15))
def test_all_fifteen_historical_human_rulers_survive_save_load(ruler_index):
    c=new_campaign_historical_s1(balance_policy='OG_1000_DIRECTOR_V2')
    selected=f'R{ruler_index}'
    assert selected in c.state['factions']
    c.state['human_faction']=selected
    later=load_bytes(save_bytes(c))
    assert later.state['human_faction']==selected
    assert later.fingerprint==c.fingerprint
