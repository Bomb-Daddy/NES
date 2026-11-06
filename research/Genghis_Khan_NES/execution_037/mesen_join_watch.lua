-- EXEC0037 additive MesenCE breakpoint script, for the previously VERIFIED
-- matching developer-core bridge only. NOT executed in EXEC0037.
-- Append in the Lua bridge scope with json and frame_count available. Keep
-- raw output under a private research path (never commit it to public GitHub).
-- An actual source writer requires CPU PC + mapper translation, not a bare
-- VM byte-pointer value. Snapshot timing alone cannot identify source writes.
local join_trace_path = os.getenv('GK1_EXEC0037_JOIN_TRACE')
if join_trace_path then
  local jf = assert(io.open(join_trace_path, 'w'))
  local n = 0
  local function rd(a) return emu.read(a,emu.memType.nesMemory) end
  local function wd(a) return rd(a)+256*rd(a+1) end
  local function unit(a)
    return {soldiers=wd(a),x=rd(a+2),y=rd(a+3),type=rd(a+4),mobility=rd(a+5)}
  end
  local function obs(kind,addr,value)
    if n>=12000 then return end
    n=n+1
    local cpu = emu.getCpuState(emu.cpuType.nes)
    local vp=wd(6)
    local mapping=emu.convertAddress(vp,emu.memType.nesMemory,emu.cpuType.nes)
    local ev={kind=kind,addr=addr,value=value,
      frame=frame_count,cycle=emu.getCpuCycleCount(emu.cpuType.nes),
      cpu_pc=cpu.pc,vm_source_pointer=vp,
      vm_source_prg_offset=mapping and mapping.address or -1,
      vm_stack=wd(2),vm_frame=wd(4),
      unit0=unit(0x6060),unit1=unit(0x606b),
      scratch_word=wd(0x613e),scratch_614c=rd(0x614c),scratch_614e=rd(0x614e),
      tile_8_7=rd(0x61d4)}
    jf:write(json.encode(ev),'\n');jf:flush()
  end
  emu.addMemoryCallback(function(addr,val) obs('unit_record_write',addr,val) end,
      emu.callbackType.write,0x6060,0x6075,emu.cpuType.nes,emu.memType.nesMemory)
  emu.addMemoryCallback(function(addr,val) obs('word_613e_write',addr,val) end,
      emu.callbackType.write,0x613e,0x613f,emu.cpuType.nes,emu.memType.nesMemory)
  emu.addMemoryCallback(function(addr,val) obs('scratch_write',addr,val) end,
      emu.callbackType.write,0x614c,0x614e,emu.cpuType.nes,emu.memType.nesMemory)
  emu.addMemoryCallback(function() obs('native_handler_F1C8',0xF1C8,nil) end,
      emu.callbackType.exec,0xF1C8,0xF1C8,emu.cpuType.nes,emu.memType.nesMemory)
  emu.addMemoryCallback(function() obs('native_handler_F1CD',0xF1CD,nil) end,
      emu.callbackType.exec,0xF1CD,0xF1CD,emu.cpuType.nes,emu.memType.nesMemory)
  emu.addEventCallback(function() jf:flush() end,emu.eventType.endFrame)
end
