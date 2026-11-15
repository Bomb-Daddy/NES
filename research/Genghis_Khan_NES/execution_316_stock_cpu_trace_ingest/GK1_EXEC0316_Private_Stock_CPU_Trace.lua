-- EXEC0316: fresh original-stock Mesen CPU-claim recorder for first-Spring work.
-- Copyright-clean script. It writes PRIVATE original gameplay telemetry; NEVER
-- put its output, source ROM, SRAM, screenshots or checkpoints on public GitHub.
-- This is a NEW combined original-source trace, not a replay of MESEN-GAP-0001.
-- Requires the owner's independently authenticated exact ORIGINAL USA ROM,
-- matched Mesen Core, and Lua I/O. It must run from true fresh reset.
-- Generated records are CPU CALLBACK CLAIMS until validated externally with
-- exact original ROM and independent reviewer; not first-Spring generation.
local path=os.getenv('GK316_TRACE_PRIVATE_PATH')
if not path or path=='' then error('GK316_TRACE_PRIVATE_PATH required') end
local out=assert(io.open(path,'wb'))
local fixed_hash='ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3'
local max_events=4096
local events={}
local pending=nil
local active_rng=nil
local frames=0
local complete=false
local function state() return emu.getCpuState() end
local function seed()
 local t={}
 for a=0x84,0x89 do t[#t+1]=string.format('%02x',emu.read(a,emu.memType.nesDebug)) end
 return table.concat(t)
end
local initial=seed()
local function fixed(pc)
 local mapped=emu.convertAddress(pc,emu.memType.nesMemory)
 return mapped and mapped.memType==emu.memType.nesPrgRom and
   mapped.address==15*0x4000+(pc-0xc000)
end
local function bank_for(pc)
 local m=emu.convertAddress(pc,emu.memType.nesMemory)
 if not m or m.memType~=emu.memType.nesPrgRom then error('GK316 unmapped CPU PC') end
 return math.floor(m.address/16384)
end
local function add(cycle,line)
 if #events>=max_events then error('GK316 event limit; discard incomplete private trace') end
 if not cycle or cycle<=0 or cycle%1~=0 then error('GK316 missing source CPU cycle') end
 events[#events+1]={cycle=cycle,line=line}
end
local function confirm()
 if not pending then return end
 local r=pending
 pending=nil
 local after=emu.read(r.address,emu.memType.nesDebug)
 if after~=r.value then error('GK316 original STORE not committed in CPU RAM') end
 add(r.cycle,string.format('S,%d,%04x,%d,%02x,%02x,%02x,%02x,%02x,%02x',
  r.cycle,r.pc,r.bank,r.address,r.before,r.value,r.a,r.x,r.y))
end
local function on_store(addr,value)
 if active_rng then return end -- C289's internal $89 writes are modeled atomically.
 if pending then error('GK316 multiple CPU seed stores without confirmation') end
 local s=state()
 local pc=s.pc or -1
 if pc<0x8000 then error('GK316 seed write from unmapped CPU PC') end
 -- NMI INC/DEC/RMW callbacks write the same addresses but are NOT STA/STX/STY.
 -- GK302 already models NMI +2 as a single event; never double-count it.
 local opcode=emu.read(pc,emu.memType.nesDebug)
 if opcode~=0x84 and opcode~=0x85 and opcode~=0x86 and
    opcode~=0x8c and opcode~=0x8d and opcode~=0x8e then return end
 pending={cycle=s.cycleCount,pc=pc,bank=bank_for(pc),address=addr,
  before=emu.read(addr,emu.memType.nesDebug),value=value,
  a=s.a or 0,x=s.x or 0,y=s.y or 0}
end
emu.addMemoryCallback(on_store,emu.callbackType.write,0x84,0x89)
emu.addMemoryCallback(confirm,emu.callbackType.exec,0x8000,0xffff)
emu.addMemoryCallback(function()
 if not fixed(0xc106) then error('GK316 fixed-bank NMI source mapping changed') end
 if active_rng then error('GK316 NMI interrupted C289; atomic replay cannot represent this event sequence') end
 local s=state()
 local gate=emu.read(0x8a,emu.memType.nesDebug)
 add(s.cycleCount,string.format('N,%d,%02x',s.cycleCount,gate))
end,emu.callbackType.exec,0xc106,0xc106)
emu.addMemoryCallback(function()
 if not fixed(0xc289) then error('GK316 original C289 mapping mismatch') end
 if active_rng then error('GK316 nested C289; reject nonserializable trace') end
 local s=state()
 active_rng={cycle=s.cycleCount,before=seed()}
end,emu.callbackType.exec,0xc289,0xc289)
emu.addMemoryCallback(function()
 if not active_rng or not fixed(0xc2eb) then error('GK316 C289 exit without entry') end
 local r=active_rng
 active_rng=nil
 local draw=emu.read(0x67,emu.memType.nesDebug)*256+emu.read(0x66,emu.memType.nesDebug)
 if draw>32767 then error('GK316 original draw out of range') end
 add(r.cycle,string.format('C,%d,c289,15,%s,%s,%d',r.cycle,r.before,seed(),draw))
end,emu.callbackType.exec,0xc2eb,0xc2eb)
-- Same established 14-step fresh-boot input pattern, but a DIFFERENT capture
-- mission: unified source clocks/registers, NOT previous G02/G03 tactical map.
local steps={'start','a','a','a','a','b','a','a','a',
 'wait','wait','wait','a','wait'}
local starts,ends={},{}
local at=240
for i,action in ipairs(steps) do
 starts[i]=at
 at=at+(action=='wait' and 300 or 125)
 ends[i]=at
end
local finish_at=at
emu.addEventCallback(function()
 if complete then return end
 local start_on,a_on,b_on=false,false,false
 for i,action in ipairs(steps) do
  if frames>=starts[i] and frames<starts[i]+5 then
   if action=='start' then start_on=true end
   if action=='a' then a_on=true end
   if action=='b' then b_on=true end
   break
  end
 end
 emu.setInput({start=start_on,a=a_on,b=b_on},0)
end,emu.eventType.inputPolled)
emu.addEventCallback(function()
 frames=frames+1
 if complete or frames<finish_at then return end
 complete=true
 confirm()
 if active_rng then error('GK316 partial C289 at capture termination') end
 table.sort(events,function(a,b)return a.cycle<b.cycle end)
 for i=2,#events do
  if events[i].cycle<=events[i-1].cycle then
   error('GK316 duplicate/nonchronological callback cycles; discard private trace')
  end
 end
 out:write('GK1_STOCK_CPU_CLAIMS_V1,',fixed_hash,',M,',initial,'\n')
 for _,e in ipairs(events) do out:write(e.line,'\n') end
 out:flush();out:close()
 emu.stop(0) -- only bounded PRIVATE capture completed; NOT game acceptance.
end,emu.eventType.endFrame)