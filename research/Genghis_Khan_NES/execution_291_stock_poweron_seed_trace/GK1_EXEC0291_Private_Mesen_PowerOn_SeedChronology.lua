-- EXEC0291 original-stock *private* MesenCE power-on CPU-source recorder.
-- Copyright-clean script; captured W/C/season values are PRIVATE ROM evidence.
-- Requires the owner's independently SHA256-authenticated USA NES ROM,
-- MesenCE with Lua IO enabled and GK291_TRACE_PRIVATE_PATH set to a private file.
-- Does NOT certify a stock Spring generator or synthesize a working campaign.
local expected='ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3'
local filename=os.getenv('GK291_TRACE_PRIVATE_PATH')
if not filename or filename=='' then error('private trace path required') end
local f=assert(io.open(filename,'wb'))
f:write('GK1_291_STOCK_TRACE_V1,'..expected..',FRESH_RESET_WORKRAM\n')
local frame, entering, calls, writes=0,nil,0,0
local pending={}
local MAX_CALLS,MAX_WRITES=8192,100000
local function state() return emu.getCpuState() end
local function seed()
 local t={}
 for a=0x84,0x89 do t[#t+1]=string.format('%02x',emu.read(a,emu.memType.nesDebug)) end
 return table.concat(t)
end
local function mapped_bank(pc)
 local m=emu.convertAddress(pc,emu.memType.nesMemory)
 if not m or m.memType~=emu.memType.nesPrgRom then return -1 end
 local physical=m.address
 local logical=(pc<0xc000) and (pc-0x8000) or (pc-0xc000)
 if physical%0x4000~=logical then return -1 end
 return math.floor(physical/0x4000)
end
local function flush_writes()
 for _,w in ipairs(pending) do
  local actual=emu.read(w.addr,emu.memType.nesDebug)
  local confirmed=actual==w.value and 1 or 0
  f:write(string.format('W,%d,%d,%d,%d,%d,%d,%d,%d,%d\n',
   w.cycle,w.pc,w.bank,w.addr,w.before,w.value,actual,confirmed,w.frame))
 end
 pending={}
end
emu.addMemoryCallback(function(addr,value)
 if entering then return end -- C289's 48-bit seed mutation belongs to one C event.
 if writes>=MAX_WRITES then error('seed-write cap exceeded') end
 local s=state(); local pc=s.pc or -1
 writes=writes+1
 pending[#pending+1]={cycle=s.cycleCount or 0,pc=pc,bank=mapped_bank(pc),
   addr=addr,before=emu.read(addr,emu.memType.nesDebug),value=value,frame=frame}
end,emu.callbackType.write,0x84,0x89)
-- Deferred completion checks: write callbacks fire before the actual RAM store.
emu.addMemoryCallback(function() if #pending>0 then flush_writes() end end,
 emu.callbackType.exec,0x8000,0xffff)
emu.addMemoryCallback(function()
 flush_writes()
 if entering then error('nested C289') end
 if mapped_bank(0xc289)~=15 then error('C289 fixed bank not mapped') end
 local s=state()
 entering={cycle=s.cycleCount or 0,pre=seed()}
end,emu.callbackType.exec,0xc289,0xc289)
emu.addMemoryCallback(function()
 if not entering then error('C289 exit without entry') end
 if mapped_bank(0xc2eb)~=15 then error('C289 exit wrong bank') end
 local s=state(); calls=calls+1
 if calls>MAX_CALLS then error('C289 call cap exceeded') end
 local draw=256*emu.read(0x67,emu.memType.nesDebug)+emu.read(0x66,emu.memType.nesDebug)
 f:write(string.format('C,%d,%d,15,%s,%s,%d,%d\n',
   entering.cycle,s.cycleCount or 0,entering.pre,seed(),draw,frame))
 entering=nil
 f:flush()
end,emu.callbackType.exec,0xc2eb,0xc2eb)
local steps={'start','a','a','a','a','b','a','a','a','wait','wait','wait','a','wait'}
local start,finish={},{}
local at=240
for i,action in ipairs(steps) do
 start[i]=at;at=at+(action=='wait' and 300 or 125);finish[i]=at
end
local done=false
local function word(a)
 return emu.read(a,emu.memType.nesDebug)+256*emu.read(a+1,emu.memType.nesDebug)
end
emu.addEventCallback(function()
 if done then return end
 local p={start=false,a=false,b=false}
 for i,action in ipairs(steps) do
  if frame>=start[i] and frame<start[i]+5 then p[action]=true;break end
 end
 emu.setInput(p,0)
end,emu.eventType.inputPolled)
emu.addEventCallback(function()
 if done then return end
 frame=frame+1
 for i=1,#steps do
  if frame==finish[i] then
   flush_writes()
   local m=emu.convertAddress(0x6dcd,emu.memType.nesMemory)
   local work=(m and m.memType==emu.memType.nesWorkRam) and 1 or 0
   f:write(string.format('S,%d,%d,%d,%d,%d,%d,%d\n',
    i,frame,word(0x65bb),word(0x65bd),word(0x7b36),work,calls))
   f:flush()
  end
 end
 if frame>=at then
  done=true;flush_writes();f:flush();f:close();emu.stop(0)
 end
end,emu.eventType.endFrame)