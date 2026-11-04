local OUT=os.getenv("GK_OUT")
local TOTAL=tonumber(os.getenv("GK_FRAMES") or "900")
local f=io.open(OUT.."/tileup.log","w")
local fr=0
local last=-10
memory.registerwrite(0x2007,1,function()
  local pc=memory.getregister("pc")
  if pc==0xC36A then
    local zp={}
    for a=0x50,0x70 do zp[#zp+1]=string.format("%02X",memory.readbyte(a)) end
    f:write(fr," ",table.concat(zp,","),"\n")
  end
end)
for i=1,TOTAL do
  fr=i
  if i%90==30 then joypad.set(1,{start=true}) end
  if i%90==60 then joypad.set(1,{A=true}) end
  emu.frameadvance()
end
f:close(); emu.exit()
