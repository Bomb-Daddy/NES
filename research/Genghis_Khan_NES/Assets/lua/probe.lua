local OUT = os.getenv("GK_OUT") or "/tmp/snap"
local TOTAL = tonumber(os.getenv("GK_FRAMES") or "1800")
local EVERY = tonumber(os.getenv("GK_EVERY") or "120")
local function dump(tag)
  local f = io.open(OUT.."/"..tag..".bin","wb")
  -- pattern tables 0x0000-0x1FFF
  for a=0,0x1FFF do f:write(string.char(ppu.readbyte(a))) end
  -- nametables 0x2000-0x2FFF
  for a=0x2000,0x2FFF do f:write(string.char(ppu.readbyte(a))) end
  -- palettes 0x3F00-0x3F1F
  for a=0x3F00,0x3F1F do f:write(string.char(ppu.readbyte(a))) end
  -- CPU RAM 0x0000-0x07FF (includes OAM shadow at 0x0200 if used)
  for a=0,0x7FF do f:write(string.char(memory.readbyte(a))) end
  f:close()
  gui.savescreenshotas(OUT.."/"..tag..".png")
end
for fr=1,TOTAL do
  -- press Start / A periodically to advance past title/menus
  if fr % 90 == 30 then joypad.set(1,{start=true}) end
  if fr % 90 == 60 then joypad.set(1,{A=true}) end
  emu.frameadvance()
  if fr % EVERY == 0 then dump(string.format("f%05d",fr)) end
end
emu.exit()
