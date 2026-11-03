import subprocess, os, base64, sys
from run import LUA_HEAD
MESEN='/home/claude/mesen_linux/Mesen'; ROM='/home/claude/rom/rotk2.nes'
DEC='''
local rb={}
local bs='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/'
for i=1,64 do rb[bs:byte(i)]=i-1 end
local function unb64(s)
  local out={}
  local i=1
  while i<=#s do
    local a,b,c,d=s:byte(i,i+3)
    local n=(rb[a] or 0)<<18|(rb[b] or 0)<<12|((c~=61 and rb[c] or 0)<<6)|(d~=61 and rb[d] or 0)
    out[#out+1]=string.char((n>>16)&255)
    if c~=61 then out[#out+1]=string.char((n>>8)&255) end
    if d~=61 then out[#out+1]=string.char(n&255) end
    i=i+4
  end
  return table.concat(out)
end
'''
def gen(events, shots, saves, stop, state_b64=None, peeks=()):
    lua=LUA_HEAD+DEC
    lua+='local ev={'+','.join('{%d,"%s",%d}'%e for e in events)+'}\n'
    lua+='local shots={'+','.join('[%d]=true'%s for s in shots)+'}\n'
    lua+='local saves={'+','.join('[%d]=true'%s for s in saves)+'}\n'
    lua+='local peeks={'+','.join('[%d]={%d,%d}'%p for p in peeks)+'}\n'
    lua+='local want=nil\nlocal loadstate=%s\n'%('"'+state_b64+'"' if state_b64 else 'nil')
    lua+='''
emu.addEventCallback(function()
  local inp={}
  for _,e in ipairs(ev) do if frame>=e[1] and frame<e[1]+e[3] then inp[e[2]]=true end end
  emu.setInput(inp,0)
end, emu.eventType.inputPolled)
emu.addMemoryCallback(function()
  if loadstate then local s=unb64(loadstate); loadstate=nil; emu.loadSavestate(s); print("LOADED") end
  if want then local s=emu.createSavestate(); print("STATE "..want.." "..b64(s)); want=nil end
end, emu.callbackType.exec, 0x8000, 0xFFFF)
emu.addEventCallback(function()
  frame=frame+1
  if shots[frame] then print("SHOT "..frame.." "..b64(emu.takeScreenshot())) end
  if saves[frame] then want=frame end
  if peeks[frame] then local p=peeks[frame]; local t={} for i=0,p[2]-1 do t[#t+1]=string.format("%02X",emu.read(p[1]+i,emu.memType.nesDebug)) end print("PEEK "..frame.." "..string.format("%04X",p[1]).." "..table.concat(t," ")) end
  if frame>=STOP then emu.stop(0) end
end, emu.eventType.endFrame)
'''.replace('STOP',str(stop))
    return lua
def run(events, shots, saves, stop, tag, state=None, peeks=()):
    open('/home/claude/cap/%s.lua'%tag,'w').write(gen(events,shots,saves,stop,state,peeks))
    env=dict(os.environ, LC_ALL='C', DOTNET_SYSTEM_GLOBALIZATION_INVARIANT='1', DISPLAY=':77')
    r=subprocess.run([MESEN,'--testRunner','--enableStdout','--doNotSaveSettings','--timeout=1500','/home/claude/cap/%s.lua'%tag,ROM],capture_output=True,text=True,env=env,timeout=1700)
    d='/home/claude/cap/%s'%tag; os.makedirs(d,exist_ok=True); n=0; s=0
    for line in r.stdout.splitlines():
        if line.startswith('SHOT '):
            _,f,x=line.split(' ',2); open('%s/f%06d.png'%(d,int(f)),'wb').write(base64.b64decode(x)); n+=1
        elif line.startswith('STATE '):
            _,f,x=line.split(' ',2); open('%s/state_%06d.b64'%(d,int(f)),'w').write(x); s+=1
        elif line.startswith('PEEK '):
            open('%s/peek_%s_%s.txt'%(d,line.split()[1],line.split()[2]),'w').write(line)
        elif line=='LOADED': print(line)
    print(tag,'shots',n,'states',s,'rc',r.returncode,r.stderr[-200:] if r.returncode else '')

import adapt
def run_ad(steps, tag, state, stop=60000, savesteps=True, region=(0,239,0,255), adstart=5, maxwait=80):
    lua=gen([],[],[],stop,state)
    ad=adapt.AD.replace('STEPS','{'+','.join('"%s"'%s for s in steps)+'}').replace('ADSTART',str(adstart))
    ad=ad.replace('for y=130,226,6 do for x=14,112,6 do','for y=%d,%d,6 do for x=%d,%d,6 do'%(region[0],region[1],region[2],region[3]))
    ad=ad.replace('sw>80','sw>%d'%maxwait)
    if savesteps: ad=ad.replace('print("STEP "..si..','want=frame; print("STEP "..si..')
    marker='emu.addEventCallback(function()\n  local inp={}\n  for _,e'
    assert marker in lua
    lua=lua.replace(marker, ad+marker,1)
    open('/home/claude/cap/%s.lua'%tag,'w').write(lua)
    env=dict(os.environ, LC_ALL='C', DOTNET_SYSTEM_GLOBALIZATION_INVARIANT='1', DISPLAY=':77')
    r=subprocess.run([MESEN,'--testRunner','--enableStdout','--doNotSaveSettings','--timeout=1500','/home/claude/cap/%s.lua'%tag,ROM],capture_output=True,text=True,env=env,timeout=1700)
    d='/home/claude/cap/%s'%tag; os.makedirs(d,exist_ok=True); n=0; s=0
    for line in r.stdout.splitlines():
        if line.startswith('SHOT '):
            _,f,x=line.split(' ',2); open('%s/f%06d.png'%(d,int(f)),'wb').write(base64.b64decode(x)); n+=1
        elif line.startswith('STATE '):
            _,f,x=line.split(' ',2); open('%s/state_%06d.b64'%(d,int(f)),'w').write(x); s+=1
        elif line.startswith('STEP '): print(line)
    print(tag,'shots',n,'states',s,'rc',r.returncode,r.stderr[-200:] if r.returncode else '')
