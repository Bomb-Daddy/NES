-- EXEC0282 native ORIGINAL STOCK Mesen C289 callback recorder; source script only.
-- Operate ONLY with SHA-authenticated owner's private USA ROM and matched Mesen.
-- This script itself is copyright-clean; its output contains PRIVATE gameplay.
-- No synthetic source seed, no automatic first-Spring claim.
local path = os.getenv('GK282_TRACE_PRIVATE_PATH')
if not path or path == '' then error('GK282_TRACE_PRIVATE_PATH required') end
local out = assert(io.open(path, 'wb'))
out:write('GK1_C289_TRACE_V1,fresh-workram\n')
local frame, entering, calls = 0, nil, 0
local max_calls = 8192
local function current_seed()
  local t = {}
  for a=0x84,0x89 do t[#t+1]=string.format('%02x',emu.read(a,emu.memType.nesDebug)) end
  return table.concat(t)
end
local function cpu_clock()
  local s = emu.getCpuState()
  return s.cycleCount or 0
end
local function source_is_fixed(pc)
  local mapped = emu.convertAddress(pc, emu.memType.nesMemory)
  return mapped and mapped.memType == emu.memType.nesPrgRom and
    mapped.address == 15*0x4000+(pc-0xc000)
end
emu.addMemoryCallback(function()
  if not source_is_fixed(0xc289) then error('GK282 not original fixed bank') end
  if entering then error('GK282 nested C289 entry') end
  entering = {pre=current_seed(), cycle=cpu_clock(), frame=frame}
end, emu.callbackType.exec, 0xc289, 0xc289)
emu.addMemoryCallback(function()
  if not entering then error('GK282 C2EB returned without entry') end
  if not source_is_fixed(0xc2eb) then error('GK282 non-original exit mapping') end
  calls=calls+1
  if calls>max_calls then error('GK282 bounded call overflow') end
  local hi=emu.read(0x67,emu.memType.nesDebug)
  local lo=emu.read(0x66,emu.memType.nesDebug)
  local draw=hi*256+lo
  out:write(string.format('C,%d,%d,%s,%s,%d\n', frame,cpu_clock(),entering.pre,current_seed(),draw))
  entering=nil
  out:flush()
end, emu.callbackType.exec, 0xc2eb,0xc2eb)
local pending=false
emu.addMemoryCallback(function() pending=true end,emu.callbackType.write,0x7a8a,0x7a8b)
emu.addMemoryCallback(function()
  if pending and calls>0 then
    pending=false
    local rate=emu.read(0x7a8a,emu.memType.nesDebug)+256*emu.read(0x7a8b,emu.memType.nesDebug)
    out:write(string.format('V,%d,%d,%d\n',frame,cpu_clock(),rate))
    out:flush()
  end
end,emu.callbackType.exec,0x8000,0xffff)
emu.addEventCallback(function() frame=frame+1 end,emu.eventType.endFrame)
-- Use matching bridge/test-runner to drive controller events and close out.
-- This probe does not itself issue a forged successful Spring or emu.stop(0).
