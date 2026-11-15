-- EXEC0293: source-authentic *private-only* MesenCE power-on RNG recorder.
-- Authenticates loaded ROM SHA-1 before recording. V3 fixes the CPU/physical PRG-RAM mapping; retains CPU timestamps,
-- deterministic stage checks, and fail-closed finalization. No gameplay cloning.
local original_sha256='ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3'
local original_sha1='98b39b610933d0d82a6ccbcb962a28ba7dc71a85'
local rom=assert(emu.getRomInfo(),'Mesen ROM info missing')
assert(type(rom.fileSha1Hash)=='string' and rom.fileSha1Hash:lower()==original_sha1,
       'WRONG LOADED ROM: original USA NES SHA-1 required')
local filename=os.getenv('GK293_PRIVATE_TRACE_PATH')
assert(type(filename)=='string' and filename~='','private trace path required')
local file=assert(io.open(filename,'wb'))
file:write('GK1_293_STOCK_TRACE_V3,'..original_sha256..','..original_sha1..',FRESH_RESET_WORKRAM\n')
local frame, entering, calls, writes=0,nil,0,0
local pending={}
local MAX_CALLS,MAX_WRITES=8192,100000
local function state()
 local s
 if type(emu.getCpuState)=='function' then s=emu.getCpuState() end
 if not s and type(emu.getState)=='function' then
  local full=emu.getState(); s=full and full.cpu
 end
 assert(type(s)=='table' and type(s.pc)=='number','Mesen CPU PC unavailable')
 local cycle=s.cycleCount
 if type(emu.getCpuCycleCount)=='function' then cycle=emu.getCpuCycleCount() end
 assert(type(cycle)=='number' and cycle>0,'Mesen CPU cycle unavailable')
 return s.pc,cycle
end
local function seed()
 local parts={}
 for addr=0x84,0x89 do
  parts[#parts+1]=string.format('%02x',emu.read(addr,emu.memType.nesDebug))
 end
 return table.concat(parts)
end
local function bank(pc)
 if type(pc)~='number' or pc<0x8000 or pc>0xffff then return -1 end
 local m=emu.convertAddress(pc,emu.memType.nesMemory)
 if not m or m.memType~=emu.memType.nesPrgRom then return -1 end
 local address=m.address
 local logical=pc<0xc000 and pc-0x8000 or pc-0xc000
 if address%0x4000~=logical then return -1 end
 return math.floor(address/0x4000)
end
local function flush()
 for _,w in ipairs(pending) do
  local actual=emu.read(w.addr,emu.memType.nesDebug)
  local confirmed=(actual==w.value) and 1 or 0
  file:write(string.format('W,%d,%d,%d,%d,%d,%d,%d,%d,%d\n',
   w.cycle,w.pc,w.bank,w.addr,w.before,w.value,actual,confirmed,w.frame))
 end
 pending={}
end
emu.addMemoryCallback(function(addr,value)
 if entering then return end -- C289 internal seed mutation belongs to the C event.
 if writes>=MAX_WRITES then error('seed write cap') end
 local pc,cycle=state()
 local source_bank=bank(pc)
 assert(source_bank>=0 and source_bank<=15,'source store ROM bank not mapped')
 writes=writes+1
 pending[#pending+1]={cycle=cycle,pc=pc,bank=source_bank,addr=addr,
     before=emu.read(addr,emu.memType.nesDebug),value=value,frame=frame}
end,emu.callbackType.write,0x84,0x89)
emu.addMemoryCallback(function()
 if #pending>0 then flush() end
end,emu.callbackType.exec,0x8000,0xffff)
emu.addMemoryCallback(function()
 flush()
 assert(not entering,'nested C289')
 assert(bank(0xc289)==15,'C289 fixed bank not mapped')
 local _,cycle=state()
 entering={cycle=cycle,pre=seed()}
end,emu.callbackType.exec,0xc289,0xc289)
emu.addMemoryCallback(function()
 assert(entering,'C289 exit without entry')
 assert(bank(0xc2eb)==15,'C289 exit bank wrong')
 local _,cycle=state()
 calls=calls+1
 assert(calls<=MAX_CALLS,'C289 call cap')
 local draw=emu.read(0x66,emu.memType.nesDebug)+256*emu.read(0x67,emu.memType.nesDebug)
 file:write(string.format('C,%d,%d,15,%s,%s,%d,%d\n',
    entering.cycle,cycle,entering.pre,seed(),draw,frame))
 entering=nil
 file:flush()
end,emu.callbackType.exec,0xc2eb,0xc2eb)
local actions={'start','a','a','a','a','b','a','a','a','wait','wait','wait','a','wait'}
local begin,finish={},{}
local limit=240
for i,action in ipairs(actions) do
 begin[i]=limit;limit=limit+(action=='wait' and 300 or 125);finish[i]=limit
end
-- NES CPU $6000-$7FFF is cartridge PRG RAM. CPU $6DCD maps to
-- physical 0x0DCD. CPU $0DCD is a mirror of internal $05CD system RAM;
-- using it for the original country bank is an error (EXEC0292 defect).
local function country_ram_bank()
 local m=emu.convertAddress(0x6dcd,emu.memType.nesMemory)
 local wrong=emu.convertAddress(0x0dcd,emu.memType.nesMemory)
 assert(not wrong or wrong.memType~=emu.memType.nesWorkRam or
        wrong.address~=0x0dcd,'INTERNAL RAM MIRROR FALSE WORKRAM PROOF')
 local good=(m and m.memType==emu.memType.nesWorkRam and m.address==0x0dcd)
 return good and 1 or 0,good and 0x0dcd or 0
end
local done=false
local function word(addr)
 return emu.read(addr,emu.memType.nesDebug)+256*emu.read(addr+1,emu.memType.nesDebug)
end
emu.addEventCallback(function()
 if done then return end
 local buttons={start=false,a=false,b=false}
 for i,action in ipairs(actions) do
  if frame>=begin[i] and frame<begin[i]+5 and action~='wait' then
   buttons[action]=true;break
  end
 end
 emu.setInput(buttons,0)
end,emu.eventType.inputPolled)
emu.addEventCallback(function()
 if done then return end
 frame=frame+1
 for i=1,#actions do
  if frame==finish[i] then
   flush()
   local work,physical=country_ram_bank()
   -- V3 makes the CPU->physical source bank claim inspectable by the auditor.
   file:write(string.format('S,%d,%d,%d,%d,%d,%d,%d,%d,%d\n',
      i,frame,word(0x65bb),word(0x65bd),word(0x7b36),work,calls,0x6dcd,physical))
   file:flush()
  end
 end
 if frame>=limit then
  done=true;flush();file:flush();file:close();emu.stop(0)
 end
end,emu.eventType.endFrame)
