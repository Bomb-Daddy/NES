"""Actual Tk run under virtual Linux display, with assertions and screenshot."""
import json,os,tempfile
from pathlib import Path
from PIL import ImageGrab
from third_branch.strategic_game import StrategicWindow
from third_branch.integrated import load_file

with tempfile.TemporaryDirectory(prefix='koei_help_v118_') as tmp:
    w=StrategicWindow(str(Path(tmp)/'live_save.json'))
    w.root.update()
    initial=w.c.fingerprint
    w.on_nes_button('help');w.root.update()
    assert w.help_dialog is not None and w.help_dialog.winfo_exists()
    assert w.c.fingerprint==initial
    w.on_nes_button('help');w.root.update();assert w.help_dialog is None
    w.on_nes_button('select');w.command_index=9;w.on_nes_button('help');w.root.update()
    assert w.help_dialog.winfo_exists() and 'All Orders' in w.help_dialog.title()
    assert w.c.fingerprint==initial
    w.on_nes_button('b');w.root.update()
    # Save source identity using actual original game's player-ruler selector dialog.
    w.choose_ruler();w.root.update()
    w.ruler_listbox.selection_clear(0,'end');w.ruler_listbox.selection_set(1)
    w.ruler_confirm();w.root.update()
    assert w.player=='R1' and load_file(w.path).state['human_faction']=='R1'
    w._adopt_loaded(load_file(w.path));assert w.player=='R1'
    w.pause();w.root.update()
    m=w.pause_dialog
    assert 'Controller Bindings' in m.entries() and 'Keyboard Bindings' in m.entries()
    m.index=10;m.choose();assert m.mode=='pad_bindings'
    m.index=8;m.choose();assert m.mode=='pad_capture'
    assert w._raw_pad_button('x') is True
    assert m.mode=='pad_bindings' and w.bindings.pad['help']=='x'
    m.index=10;m.choose();assert m.mode=='main'
    m.index=11;m.choose();assert m.mode=='key_bindings'
    m.index=8;m.choose();assert m.mode=='key_capture'
    class E: keysym='F2';widget=w.root
    assert w._keyboard_action(E())=='break'
    assert m.mode=='key_bindings' and w.bindings.keyboard['help']=='F2'
    m.index=9;m.choose();assert w.bindings.keyboard['help']=='F1'
    m.close();w.pause_dialog=None;w.root.update()
    w.controller_focus='actions';w.command_index=9;w._action_cursor();w.root.update()
    w.on_nes_button('help');w.root.update()
    assert w.help_dialog.winfo_exists()
    image_path=Path('/mnt/data/KOEI_v118_Help_Actual_Linux_Tk_Smoke.png')
    # Native screenshot, not a PIL synthetic reproduction of the app.
    ImageGrab.grab(bbox=(0,0,1280,720)).save(image_path)
    w.on_nes_button('help');w.root.update()
    print(json.dumps({'status':'PASS','real_Tk':True,'linux_display':os.environ.get('DISPLAY'),
        'source':'v1.17 + proposed v1.18 help/ruler patch','initial':initial,
        'selected_ruler_loaded':w.player,'help_did_not_mutate':True,
        'controller_remap_then_keyboard_remap':True,'screenshot':str(image_path)},sort_keys=True))
    w.close()
