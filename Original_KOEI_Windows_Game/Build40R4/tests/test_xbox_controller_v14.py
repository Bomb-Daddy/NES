"""Controller transport/mapping QA without claiming physical pad testing."""
from third_branch.xbox_controller import NES_BUTTONS,SDL_BUTTONS,XINPUT_FLAGS,NesInputRouter,xinput_buttons,MAPPING_LABEL

class FakePad:
    def __init__(self):self.pressed=frozenset();self.closed=False;self.connected=True
    def read(self):return self.pressed
    def close(self):self.closed=True

def test_xbox_button_mapping_is_one_to_one_nes_no_collision():
    assert set(XINPUT_FLAGS)==set(SDL_BUTTONS)==set(NES_BUTTONS)
    assert len(set(XINPUT_FLAGS.values()))==len(NES_BUTTONS)
    assert len(set(SDL_BUTTONS.values()))==len(NES_BUTTONS)
    for button,flag in XINPUT_FLAGS.items():assert xinput_buttons(flag)=={button}
    assert xinput_buttons(0)==frozenset()
    assert xinput_buttons(0xffff)==set(NES_BUTTONS)
    assert 'Menu = NES Start' in MAPPING_LABEL and 'View = NES Select' in MAPPING_LABEL

def test_pad_dpad_repeat_a_b_are_edge_not_level():
    time=[100.]; events=[];pad=FakePad()
    router=NesInputRouter(events.append,pad,lambda:time[0])
    pad.pressed={'right','a'}
    assert router.poll()==['right','a']
    time[0]+=0.1
    assert router.poll()==[]
    time[0]+=0.25
    assert router.poll()==['right']
    time[0]+=0.13
    assert router.poll()==['right']
    pad.pressed={'a'}
    assert router.poll()==[]
    pad.pressed=frozenset()
    assert router.poll()==[]
    pad.pressed={'a'}
    assert router.poll()==['a']
    assert events==['right','a','right','right','a']
    router.close();assert pad.closed

def test_pad_disconnect_reconnect_releases_held_buttons():
    pad=FakePad();got=[];pad.pressed={'b'}
    r=NesInputRouter(got.append,pad)
    assert r.poll()==['b']
    pad.pressed=set();assert r.poll()==[]
    pad.pressed={'b'};assert r.poll()==['b']
    assert got==['b','b']
