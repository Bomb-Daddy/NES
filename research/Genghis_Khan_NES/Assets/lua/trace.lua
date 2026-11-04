local OUT=os.getenv("GK_OUT") or "/tmp/trace"
local TOTAL=tonumber(os.getenv("GK_FRAMES") or "900")
local f=io.open(OUT.."/ppuw.log","w")
local fr=0
local function w(a,sz,v)
  local pc=memory.getregister("pc")
  f:write(string.format("%d %04X %04X %s\n",fr,a,pc,tostring(v)))
end
memory.registerwrite(0x2000,1,function(a,s,v) w(a,s,v) end)
memory.registerwrite(0x2006,1,function(a,s,v) w(a,s,v) end)
memory.registerwrite(0x2007,1,function(a,s,v) w(a,s,v) end)
for i=1,TOTAL do
  fr=i
  if i%90==30 then joypad.set(1,{start=true}) end
  if i%90==60 then joypad.set(1,{A=true}) end
  emu.frameadvance()
end
f:close(); emu.exit()
