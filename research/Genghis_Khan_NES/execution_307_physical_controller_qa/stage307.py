#!/usr/bin/env python3
"""Stage opt-in physical XInput QA into the SAME inherited WinMain C11 app.
The stage does not touch stock ROM/byte images, game mechanics or saves.
Expected source identity is the exact inherited native shell after EXEC0306.
On any refusal all destination files are unchanged, even after injected I/O error.
"""
import hashlib
import os
import pathlib
import sys
import tempfile

BASE=pathlib.Path(__file__).resolve().parent
SHELL_SHA='ee3a8ed192670379f812554de0c3e5ba29311f3084a4336db8fa985be6e84df5'
FILES={
 'gk307_physical_pad_qa.c':'536f1f46df15a4bdf3af9d3b7b1463f685b9d11f5877cfee495efa779935e589',
 'gk307_physical_pad_qa.h':'19b8befc4dfd341b6652a94fe72ac529d8901ebb2a7bd9631b8fa96add4eb464',
 'test_gk307_physical_pad_qa.c':'4c5160111d69770c09255b2340575eeedc37155c5048e00a63f183340c357d27',
}
def digest(data):return hashlib.sha256(data).hexdigest()
def normalized(p):
 b=p.read_bytes()
 if b'\r\r' in b or b'\r' in b.replace(b'\r\n',b''):raise ValueError('Malformed source line ending')
 return b.replace(b'\r\n',b'\n')
def once(s, old, new):
 if s.count(old)!=1:raise ValueError('Source anchor missing or duplicate: '+old[:45])
 return s.replace(old,new,1)
def generate(app):
 sh=normalized(app/'gk1_native_shell.c')
 if digest(sh)!=SHELL_SHA:raise ValueError('Unknown inherited WinMain shell source SHA')
 s=sh.decode('utf8')
 s=once(s,'#include "gk99_xinput_bridge.h"','#include "gk99_xinput_bridge.h"\n#include "gk307_physical_pad_qa.h"')
 s=once(s,'static void poll_xinput(void){','static int gk307_physical_mode=0;\nstatic Gk307PhysicalQa gk307_physical;\nstatic void poll_xinput(void){')
 s=once(s,'if(!get_state){gk1_pad_disconnect(&pad);active_slot=0xffu;return;}',
   '''if(!get_state){gk1_pad_disconnect(&pad);active_slot=0xffu;
  if(gk307_physical_mode){
   gk307_physical_disconnect(&gk307_physical);
   snprintf(app.status,sizeof app.status,"PHYSICAL XBOX QA: CONNECT A CONTROLLER");
  }
  return;}''')
 s=once(s,'gk1_pad_disconnect(&pad);active_slot=0xffu;return;\n }\n if(selected!=active_slot)',
  '''gk1_pad_disconnect(&pad);active_slot=0xffu;
  if(gk307_physical_mode){
   gk307_physical_disconnect(&gk307_physical);
   snprintf(app.status,sizeof app.status,"PHYSICAL XBOX QA: CONNECT A CONTROLLER");
  }
  return;
 }
 if(selected!=active_slot)''')
 s=once(s,' Gk1PadSample sample;\n if(!gk99_xinput_sample(&raw,&sample))',
  ''' /* Opt-in QA uses only actual XInputGetState data and never executes
  * gameplay commands or simulated gamepad acceptance. A user must close
  * the window to return 0 after all 8 edges have been physically supplied. */
 if(gk307_physical_mode){
  if(!gk307_physical_step(&gk307_physical,selected,&raw))return;
  snprintf(app.status,sizeof app.status,"PHYSICAL XBOX QA: %s",
    gk307_physical_instruction(&gk307_physical));
  return;
 }
 Gk1PadSample sample;
 if(!gk99_xinput_sample(&raw,&sample))''')
 s=once(s,'int WINAPI WinMain(HINSTANCE instance,HINSTANCE prev,LPSTR cmd,int show){\n (void)prev;',
  '''int WINAPI WinMain(HINSTANCE instance,HINSTANCE prev,LPSTR cmd,int show){
 (void)prev;
 gk307_physical_mode=strcmp(cmd,"--physical-xbox-qa")==0;
 gk307_physical_init(&gk307_physical);''')
 s=once(s,'const char *gk233_bounded=gk232_scripted_gamepad?"--smoke-frames=10":',
  'const char *gk233_bounded=gk307_physical_mode?"":\n   gk232_scripted_gamepad?"--smoke-frames=10":')
 s=once(s,'scenario_loaded,smoke_mode!=0,getenv("GK1_OWNER_STOCK_FRESH_JSON"),',
  'scenario_loaded,smoke_mode!=0||gk307_physical_mode,getenv("GK1_OWNER_STOCK_FRESH_JSON"),')
 s=once(s,' DeleteObject(font);return 0;\n}\n#else',
  ''' if(font)DeleteObject(font);
 /* Closing an incomplete diagnostic is NOT a successful physical-pad test;
  * no CI job runs this mode, and normal gameplay still returns zero. */
 return gk307_physical_mode && !gk307_physical.complete ? 10 : 0;
}
#else''')
 cm=normalized(app/'CMakeLists.txt').decode('utf8')
 required=['target_link_libraries(gk1_native_shell PRIVATE gk99_xinput_bridge)',
 'GK1_099_Win32ControllerMappingAndHotplug_HostileLinuxQA',
 'GK1_257_CurrentWin32Workflow_RejectsStaleSourceAndFalsePass']
 if any(w not in cm for w in required):raise ValueError('Missing inherited native CMake identity')
 cm=once(cm,'target_link_libraries(gk1_native_shell PRIVATE gk99_xinput_bridge)',
   '''target_link_libraries(gk1_native_shell PRIVATE gk99_xinput_bridge)
# EXEC0307: real XInput physical-controller-only diagnostic (no simulated
# controller or complete-game release acceptance). Both Win32/Linux compile.
add_library(gk307_physical_pad_qa STATIC gk307_physical_pad_qa.c)
target_include_directories(gk307_physical_pad_qa PUBLIC .)
target_link_libraries(gk1_native_shell PRIVATE gk307_physical_pad_qa)
add_executable(test_gk307_physical_pad_qa test_gk307_physical_pad_qa.c)
target_link_libraries(test_gk307_physical_pad_qa PRIVATE gk307_physical_pad_qa)
add_test(NAME GK1_307_PhysicalXboxDiagnosticInputFSM_NoHardwareClaim COMMAND test_gk307_physical_pad_qa)''')
 result={'gk1_native_shell.c':s.encode(),'CMakeLists.txt':cm.encode()}
 for file,sha in FILES.items():
  b=normalized(BASE/file)
  # Windows Git checkout CRLF is canonicalized; malformed carriage returns still fail.
  if digest(b)!=sha:raise ValueError('Corrupt published EXEC0307 module: '+file)
  if (app/file).exists():raise ValueError('Duplicate staging: '+file)
  result[file]=b
 return result

def stage(app, inject_failure_after=0):
 app=pathlib.Path(app)
 changes=generate(app)
 backups={n:(app/n).read_bytes() if (app/n).exists() else None for n in changes}
 pending=[]
 try:
  for n,b in changes.items():
   fd,tmp=tempfile.mkstemp(prefix='.gk307.',dir=str(app));pending.append(tmp)
   with os.fdopen(fd,'wb') as f:f.write(b);f.flush();os.fsync(f.fileno())
  for i,(n,b) in enumerate(changes.items(),1):
   os.replace(pending.pop(0),str(app/n))
   if inject_failure_after==i:raise OSError('HOSTILE QA injected staged-write failure')
 except Exception:
  for n,b in backups.items():
   if b is None:(app/n).unlink(missing_ok=True)
   else:(app/n).write_bytes(b)
  raise
 finally:
  for p in pending:
   try:os.unlink(p)
   except FileNotFoundError:pass
 for n,b in changes.items():
  if (app/n).read_bytes()!=b:raise OSError('Unverified postimage: '+n)
 return {n:digest(b) for n,b in changes.items()}

if __name__=='__main__':
 if len(sys.argv)!=2:raise SystemExit('Usage: stage307.py EXISTING_C11_APP_DIRECTORY')
 try:
  for n,h in stage(sys.argv[1]).items():print('EXEC0307 SHA256',n,h)
 except (ValueError,OSError) as ex:
  print('EXEC0307 FAIL CLOSED:',ex,file=sys.stderr);sys.exit(1)
