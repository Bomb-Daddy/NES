#!/usr/bin/env python3
"""EXEC0233 exact-source WinMain Enhanced-only save/cold-resume/corruption acceptance.
Scripted raw XInput exercises live decoder, router, app, and disk persistence.
This does NOT certify original NES Spring or physical Xbox hardware.
"""
from pathlib import Path
import sys

CHANGES = [
(
 'const int gk232_scripted_gamepad=strcmp(cmd,"--smoke-gamepad-nav")==0;\n if(!gk232_scripted_gamepad&&strncmp(cmd,"--smoke-gamepad",15)==0)return 4;\n const int smoke_mode=gk230_win32_smoke_frames(\n   gk232_scripted_gamepad?"--smoke-frames=10":cmd,&smoke_frames);',
 '''const int gk232_scripted_gamepad=strcmp(cmd,"--smoke-gamepad-nav")==0;
 /* EXEC0233: private, Enhanced-only, multi-process Windows disk test. */
 const int gk233_practice_mode=
   strcmp(cmd,"--smoke-gamepad-practice-save")==0?1:
   strcmp(cmd,"--smoke-gamepad-practice-resume")==0?2:
   strcmp(cmd,"--smoke-gamepad-practice-verify")==0?3:
   strcmp(cmd,"--smoke-gamepad-practice-corrupt")==0?4:0;
 if(!gk232_scripted_gamepad&&!gk233_practice_mode&&
    strncmp(cmd,"--smoke-gamepad",15)==0)return 4;
 const char *gk233_bounded=gk232_scripted_gamepad?"--smoke-frames=10":
   gk233_practice_mode==1?"--smoke-frames=26":
   gk233_practice_mode==2?"--smoke-frames=8":
   gk233_practice_mode?"--smoke-frames=4":cmd;
 const int smoke_mode=gk230_win32_smoke_frames(gk233_bounded,&smoke_frames);'''
),
(
 'if(gk232_scripted_gamepad){\n  if(!scenario_loaded',
 'if(gk232_scripted_gamepad||gk233_practice_mode){\n  if(!scenario_loaded'
),
(
 '''   }else{
    gk231_trace("before XInput");poll_xinput();gk231_trace("after XInput");
   }''',
 '''   }else if(gk233_practice_mode){
    /* Only test mode supplies simulated raw gamepad; ordinary play still
     * calls poll_xinput() and respects real XInput hotplug/active slot. */
    uint16_t bits=0u;
    if(gk233_practice_mode==1){
     if(frame==1u||frame==3u||frame==5u||frame==9u||
        frame==11u||frame==13u||frame==15u||frame==17u||frame==19u)
       bits=0x1000u; /* XInput A */
     else if(frame==7u)bits=0x0010u; /* XInput START */
     else if(frame==21u)bits=0x4000u; /* XInput X: save */
     else if(frame==23u)bits=0x2000u; /* XInput B: exit */
    }else{
     if(frame==1u)bits=0x8000u; /* Y on title: cold resume */
     else if(gk233_practice_mode==2 && frame==3u)bits=0x1000u;
     else if(gk233_practice_mode==2 && frame==5u)bits=0x4000u;
     else if((gk233_practice_mode==2 && frame==7u)||
             (gk233_practice_mode==3 && frame==3u))bits=0x2000u;
    }
    Gk99XinputRaw raw={bits,0,0};Gk1PadSample decoded={0};
    if(!gk99_xinput_sample(&raw,&decoded)){
     if(IsWindow(hwnd))DestroyWindow(hwnd);
     if(font)DeleteObject(font);return 9;
    }
    pad_actions(&decoded);
    int valid=1;
    if(gk233_practice_mode==1){
     if(frame==7u)valid=app.screen==GK1_SCREEN_SETUP_WIZARD;
     if(frame==15u)valid=app.screen==GK1_SCREEN_SETUP_WIZARD&&
       app.setup_stage[0]==GK53_SETUP_AWAIT_SOURCE;
     if(frame==17u)valid=app.screen==GK1_SCREEN_ENHANCED_SPRING_LAB&&
       app.port_spring.remaining==3u;
     if(frame==19u)valid=app.port_spring.actions_committed==1u&&
       app.port_spring.remaining==2u;
     if(frame==21u)valid=strstr(app.status,"SOURCE-REPLAY VERIFIED")!=NULL;
     if(frame==23u)valid=app.screen==GK1_SCREEN_SCENARIO_COUNTRIES;
    }else if(gk233_practice_mode==2){
     if(frame==1u)valid=app.screen==GK1_SCREEN_ENHANCED_SPRING_LAB&&
       app.port_spring.actions_committed==1u&&app.port_spring.remaining==2u;
     if(frame==3u)valid=app.port_spring.actions_committed==2u&&
       app.port_spring.remaining==1u;
     if(frame==5u)valid=strstr(app.status,"SOURCE-REPLAY VERIFIED")!=NULL;
     if(frame==7u)valid=app.screen==GK1_SCREEN_SCENARIO_COUNTRIES;
    }else if(gk233_practice_mode==3){
     if(frame==1u)valid=app.screen==GK1_SCREEN_ENHANCED_SPRING_LAB&&
       app.port_spring.actions_committed==2u&&app.port_spring.remaining==1u;
     if(frame==3u)valid=app.screen==GK1_SCREEN_SCENARIO_COUNTRIES;
    }else if(gk233_practice_mode==4 && frame==1u){
     valid=app.screen==GK1_SCREEN_TITLE&&!app.port_spring.active&&
       strstr(app.status,"CORRUPT - UNCHANGED")!=NULL;
    }
    if(!valid){
     gk231_trace("EXEC0233 native Windows practice controller transition REFUSED");
     if(IsWindow(hwnd))DestroyWindow(hwnd);
     if(font)DeleteObject(font);return 9;
    }
   }else{
    gk231_trace("before XInput");poll_xinput();gk231_trace("after XInput");
   }'''
),
(
 '''  if(gk232_scripted_gamepad){
   ok=ok&&app.screen==GK1_SCREEN_SCENARIO_COUNTRIES''',
 '''  if(gk233_practice_mode){
   ok=ok&&app.rules_profile==1u&&app.active_world==0u&&
     app.worlds[0].phase==GK1_PRE_SETUP_TEMPLATE&&
     app.worlds[1].phase==GK1_PRE_SETUP_TEMPLATE&&
     memcmp(gk232_original_worlds,app.worlds,sizeof gk232_original_worlds)==0;
   if(gk233_practice_mode==4)
     ok=ok&&app.screen==GK1_SCREEN_TITLE&&!app.port_spring.active;
   else
     ok=ok&&app.screen==GK1_SCREEN_SCENARIO_COUNTRIES;
   gk231_trace(ok?"EXEC0233 Win32 Enhanced save/cold-load/corruption stage PASS":
                   "EXEC0233 Win32 Enhanced persistence stage FAIL");
  }
  if(gk232_scripted_gamepad){
   ok=ok&&app.screen==GK1_SCREEN_SCENARIO_COUNTRIES'''
)
]

def apply(source: str) -> str:
    if 'EXEC0233: private, Enhanced-only' in source:
        raise ValueError('already patched')
    if 'EXEC0232 scripted raw XInput' not in source:
        raise ValueError('requires exact EXEC0232 predecessor')
    for before, after in CHANGES:
        count=source.count(before)
        if count!=1:
            raise ValueError(f'changed, missing or duplicated anchor: {before[:85]!r} count={count}')
        source=source.replace(before,after,1)
    return source

def install(root: Path):
    target=root/'execution_199/gk1_native_shell.c'
    before=target.read_text(encoding='utf-8')
    after=apply(before)
    if before==after:raise ValueError('no source changed')
    target.write_text(after,encoding='utf-8',newline='')

if __name__=='__main__':
    try:
        if len(sys.argv)!=2:raise ValueError('usage: apply_practice_smoke.py HYDRATED_ROOT')
        install(Path(sys.argv[1]))
        print('EXEC0233 integrated WinMain 3-process Enhanced practice save/quit/resume and corruption guards')
    except (OSError,ValueError) as exc:
        print('EXEC0233 REFUSED: '+str(exc),file=sys.stderr)
        sys.exit(1)