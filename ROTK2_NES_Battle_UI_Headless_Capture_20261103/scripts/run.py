import subprocess, sys, os, re, base64, json
MESEN='/home/claude/mesen_linux/Mesen'; ROM='/home/claude/rom/rotk2.nes'
LUA_HEAD='''
local b='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/'
local function b64(data)
  local out={}
  for i=1,#data,3 do
    local a,c,d=data:byte(i,i+2)
    local n=(a<<16)|((c or 0)<<8)|(d or 0)
    local s=b:sub((n>>18)+1,(n>>18)+1)..b:sub(((n>>12)&63)+1,((n>>12)&63)+1)
    s=s..(c and b:sub(((n>>6)&63)+1,((n>>6)&63)+1) or '=')..(d and b:sub((n&63)+1,(n&63)+1) or '=')
    out[#out+1]=s
  end
  return table.concat(out)
end
local frame=0
'''
def build(events, shots, stop, peeks=()):
    # events: list of (startframe, button, holdframes); shots: frames; peeks: (frame, addr,len)
    lua=LUA_HEAD
    lua+='local ev={'+','.join('{%d,"%s",%d}'%e for e in events)+'}\n'
    lua+='local shots={'+','.join('[%d]=true'%s for s in shots)+'}\n'
    lua+='local peeks={'+','.join('[%d]={%d,%d}'%p for p in peeks)+'}\n'
    lua+='''
emu.addEventCallback(function()
  local inp={}
  for _,e in ipairs(ev) do if frame>=e[1] and frame<e[1]+e[3] then inp[e[2]]=true end end
  emu.setInput(inp,0)
end, emu.eventType.inputPolled)
emu.addEventCallback(function()
  frame=frame+1
  if shots[frame] then print("SHOT "..frame.." "..b64(emu.takeScreenshot())) end
  if peeks[frame] then local p=peeks[frame]; local t={} for i=0,p[2]-1 do t[#t+1]=string.format("%02X",emu.read(p[1]+i,emu.memType.nesDebug)) end print("PEEK "..frame.." "..string.format("%04X",p[1]).." "..table.concat(t," ")) end
  if frame>=STOPFRAME then emu.stop(0) end
end, emu.eventType.endFrame)
'''.replace('STOPFRAME',str(stop))
    return lua
def run(events, shots, stop, tag, peeks=()):
    open('/home/claude/cap/%s.lua'%tag,'w').write(build(events,shots,stop,peeks))
    env=dict(os.environ, LC_ALL='C', DOTNET_SYSTEM_GLOBALIZATION_INVARIANT='1', DISPLAY=':77')
    r=subprocess.run([MESEN,'--testRunner','--enableStdout','--doNotSaveSettings','--timeout=1500','/home/claude/cap/%s.lua'%tag,ROM],capture_output=True,text=True,env=env,timeout=1700)
    os.makedirs('/home/claude/cap/%s'%tag,exist_ok=True)
    n=0
    for line in r.stdout.splitlines():
        if line.startswith('SHOT '):
            _,f,d=line.split(' ',2)
            open('/home/claude/cap/%s/f%06d.png'%(tag,int(f)),'wb').write(base64.b64decode(d)); n+=1
        elif line.startswith('PEEK '): print(line)
    print(tag,'shots',n,'rc',r.returncode, r.stderr[-300:] if r.returncode else '')
if __name__=='__main__':
    ev=[]
    # title: Start presses
    for i in range(9): ev.append((600+i*150,'start',6))
    run(ev, list(range(300,2400,150)), 2400, 'p1')
