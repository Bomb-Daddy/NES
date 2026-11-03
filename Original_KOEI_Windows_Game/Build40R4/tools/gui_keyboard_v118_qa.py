"""Test exactly one keyboard navigation step in real Tk province UI."""
import tempfile,os,json
from pathlib import Path
from third_branch.strategic_game import StrategicWindow
with tempfile.TemporaryDirectory() as d:
 w=StrategicWindow(str(Path(d)/'test.json'))
 w.open_province();stage=w.province_stage
 stage.root.focus_force();w.root.update()
 before=stage.index
 stage.root.event_generate('<Down>',when='tail');w.root.update()
 after=stage.index
 assert after==(before+1)%12,(before,after)
 stage.root.event_generate('<F1>',when='tail');w.root.update()
 assert w.help_dialog and w.help_dialog.winfo_exists()
 assert 'Military' in w.help_dialog.title(),w.help_dialog.title()
 w.on_nes_button('help');w.root.update();assert w.help_dialog is None
 print(json.dumps({'result':'PASS','stage_before':before,'stage_after':after,'f1_help':'Military','actual_tk':True}))
 w.close()
