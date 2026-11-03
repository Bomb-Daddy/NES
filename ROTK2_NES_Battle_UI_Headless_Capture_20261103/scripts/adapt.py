from run import *
import run as R
AD='''
local steps=STEPS
local AD_START=ADSTART
local si=1; local st='idle'; local hold=0; local btn=nil; local prev=nil; local same=0; local waited=0; local sw=0
local function hsh()
  local h=7
  for y=130,226,6 do for x=14,112,6 do h=(h*31+emu.getPixel(x,y))%1000000007 end end
  return h
end
emu.addEventCallback(function()
  local inp={}
  if st=='press' and btn then inp[btn]=true end
  emu.setInput(inp,0)
end, emu.eventType.inputPolled)
emu.addEventCallback(function()
  if frame<AD_START or si>#steps then return end
  if st=='idle' then
    btn=steps[si]; if btn=='x' then btn=nil end
    prev=hsh(); st='press'; hold=0; return
  end
  if st=='press' then hold=hold+1; if hold>=6 then st='change'; waited=0; btn=nil end return end
  if st=='change' then
    waited=waited+1
    if frame%3==0 then local h=hsh(); if h~=prev or waited>200 then st='stable'; same=0; sw=0; prev=h end end
    return
  end
  if st=='stable' then
    if frame%3==0 then
      local h=hsh()
      if h==prev then same=same+1 else same=0; prev=h end
      sw=sw+1
      if same>=16 or sw>80 then
        print("SHOT "..frame.." "..b64(emu.takeScreenshot()))
        print("STEP "..si.." "..steps[si].." frame "..frame)
        si=si+1; st='idle'
      end
    end
  end
end, emu.eventType.endFrame)
'''
def runad(steps, tag, stop, adstart=21800, pre=None):
    ev=pre
    lua=build(ev,[],stop)
    q=','.join('"%s"'%s for s in steps)
    lua=lua.replace('emu.addEventCallback(function()\n  local inp={}\n  for _,e', AD.replace('STEPS','{'+q+'}').replace('ADSTART',str(adstart))+'emu.addEventCallback(function()\n  local inp={}\n  for _,e',1)
    open('/home/claude/cap/%s.lua'%tag,'w').write(lua)
    env=dict(os.environ, LC_ALL='C', DOTNET_SYSTEM_GLOBALIZATION_INVARIANT='1', DISPLAY=':77')
    r=subprocess.run([MESEN,'--testRunner','--enableStdout','--doNotSaveSettings','--timeout=1500','/home/claude/cap/%s.lua'%tag,ROM],capture_output=True,text=True,env=env,timeout=1700)
    os.makedirs('/home/claude/cap/%s'%tag,exist_ok=True); n=0
    for line in r.stdout.splitlines():
        if line.startswith('SHOT '):
            _,f,d=line.split(' ',2); open('/home/claude/cap/%s/f%06d.png'%(tag,int(f)),'wb').write(base64.b64decode(d)); n+=1
        elif line.startswith('STEP '): print(line)
    print(tag,'shots',n,'rc',r.returncode,r.stderr[-300:] if r.returncode else '')
