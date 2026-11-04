local OUT=os.getenv("GK_OUT")
local seq={}
for b in string.gmatch(os.getenv("GK_SEQ") or "","[^,]+") do seq[#seq+1]=b end
local up=io.open(OUT.."/tileup.log","w")
local fr=0
memory.registerwrite(0x2007,1,function()
  if memory.getregister("pc")==0xC36A then
    local zp={}
    for a=0x50,0x70 do zp[#zp+1]=string.format("%02X",memory.readbyte(a)) end
    up:write(fr," ",table.concat(zp,","),"\n")
  end
end)
local function dump(tag)
  local f=io.open(OUT.."/"..tag..".bin","wb")
  for a=0,0x1FFF do f:write(string.char(ppu.readbyte(a))) end
  for a=0x2000,0x2FFF do f:write(string.char(ppu.readbyte(a))) end
  for a=0x3F00,0x3F1F do f:write(string.char(ppu.readbyte(a))) end
  for a=0,0x7FF do f:write(string.char(memory.readbyte(a))) end
  f:close(); gui.savescreenshotas(OUT.."/"..tag..".png")
end
local function wait(n) for i=1,n do fr=fr+1; emu.frameadvance() end end
wait(400); dump("s000")
for i,b in ipairs(seq) do
  local t={}; t[b]=true
  for k=1,5 do joypad.set(1,t); fr=fr+1; emu.frameadvance() end
  wait(150)
  dump(string.format("s%03d_%s",i,b))
end
up:close(); emu.exit()
