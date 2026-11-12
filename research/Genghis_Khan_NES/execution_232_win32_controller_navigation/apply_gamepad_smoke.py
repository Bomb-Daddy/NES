#!/usr/bin/env python3
"""EXEC0232: exact-source opt-in WinMain/XInput/game-state navigation acceptance.
Scripted XInput values exercise the SAME live pad_actions() as a real device.
This cannot establish physical Xbox hardware behavior or original NES parity.
"""
from pathlib import Path
import sys

CHANGES = [
(
 'const int smoke_mode=gk230_win32_smoke_frames(cmd,&smoke_frames);',
 '''/* EXEC0232 scripted raw XInput values, acceptance-only; never in normal gameplay. */
 const int gk232_scripted_gamepad=strcmp(cmd,"--smoke-gamepad-nav")==0;
 if(!gk232_scripted_gamepad&&strncmp(cmd,"--smoke-gamepad",15)==0)return 4;
 const int smoke_mode=gk230_win32_smoke_frames(
   gk232_scripted_gamepad?"--smoke-frames=10":cmd,&smoke_frames);'''
),
(
 'gk231_trace("app and scenario initialized");',
 '''gk231_trace("app and scenario initialized");
 static Gk1LoadedScenario gk232_original_worlds[2];
 if(gk232_scripted_gamepad){
  if(!scenario_loaded||app.screen!=GK1_SCREEN_TITLE||
     !app.world_ready[0]||!app.world_ready[1]||
     app.worlds[0].country_count!=14u||app.worlds[1].country_count!=27u||
     app.worlds[0].phase!=GK1_PRE_SETUP_TEMPLATE||
     app.worlds[1].phase!=GK1_PRE_SETUP_TEMPLATE)return 7;
  memcpy(gk232_original_worlds,app.worlds,sizeof gk232_original_worlds);
 }'''
),
(
 'gk231_trace("before XInput");poll_xinput();gk231_trace("after XInput");',
 '''if(gk232_scripted_gamepad){
    /* 0 seed neutral, A, release, right, release, A, release, B,
     * release, A. Actual WinMain -> decoder -> router -> app dispatch. */
    static const uint16_t buttons[10]={0u,0x1000u,0u,0x0008u,0u,
                                       0x1000u,0u,0x2000u,0u,0x1000u};
    Gk99XinputRaw raw={buttons[frame],0,0};
    Gk1PadSample decoded={0};
    if(!gk99_xinput_sample(&raw,&decoded)){
     if(IsWindow(hwnd))DestroyWindow(hwnd);
     if(font)DeleteObject(font);return 8;
    }
    pad_actions(&decoded);
    const int stage_ok=
      (frame==1u?app.screen==GK1_SCREEN_CAMPAIGNS:
       frame==3u?app.screen==GK1_SCREEN_CAMPAIGNS&&app.rules_profile==0u:
       frame==5u?app.screen==GK1_SCREEN_SCENARIO_COUNTRIES&&app.active_world==0u:
       frame==7u?app.screen==GK1_SCREEN_CAMPAIGNS:
       frame==9u?app.screen==GK1_SCREEN_SCENARIO_COUNTRIES:1);
    if(!stage_ok){
     gk231_trace("EXEC0232 scripted navigation refused");
     if(IsWindow(hwnd))DestroyWindow(hwnd);
     if(font)DeleteObject(font);return 8;
    }
   }else{
    gk231_trace("before XInput");poll_xinput();gk231_trace("after XInput");
   }'''
),
(
 'int ok=running&&gk230_painted_frames>=smoke_frames;',
 '''int ok=running&&gk230_painted_frames>=smoke_frames;
  if(gk232_scripted_gamepad){
   ok=ok&&app.screen==GK1_SCREEN_SCENARIO_COUNTRIES&&
      app.rules_profile==0u&&app.active_world==0u&&
      app.worlds[0].phase==GK1_PRE_SETUP_TEMPLATE&&
      app.worlds[1].phase==GK1_PRE_SETUP_TEMPLATE&&
      memcmp(gk232_original_worlds,app.worlds,sizeof gk232_original_worlds)==0;
   gk231_trace(ok?"EXEC0232 scripted XInput menu navigation and original source immutability PASS":
                 "EXEC0232 scripted XInput menu navigation FAIL");
  }'''
)
]

def apply(source: str) -> str:
    if 'EXEC0232 scripted raw XInput' in source:
        raise ValueError('already patched')
    if 'EXEC0231: opt-in per-stage' not in source:
        raise ValueError('requires applied EXEC0231, not stale base')
    for before, after in CHANGES:
        if source.count(before) != 1:
            raise ValueError(f'predecessor anchor missing or duplicated: {before[:85]!r}, count={source.count(before)}')
        source = source.replace(before, after, 1)
    return source

def install(root: Path):
    file = root / 'execution_199/gk1_native_shell.c'
    old=file.read_text(encoding='utf-8')
    new=apply(old)
    if old == new: raise ValueError('no source changed')
    file.write_text(new,encoding='utf-8',newline='')

if __name__ == '__main__':
    try:
        if len(sys.argv)!=2: raise ValueError('usage: apply_gamepad_smoke.py HYDRATED_ROOT')
        install(Path(sys.argv[1]))
        print('EXEC0232 installed real WinMain scripted Xbox-raw controller navigation smoke')
    except (ValueError,OSError) as exc:
        print('EXEC0232 REFUSED: '+str(exc),file=sys.stderr)
        sys.exit(1)
