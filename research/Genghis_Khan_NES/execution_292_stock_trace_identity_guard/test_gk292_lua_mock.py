#!/usr/bin/env python3
"""Run actual recorder Lua under liblua5.4 with an explicitly MOCKED emu API.
Tests API structure and transcript wiring, NOT genuine Mesen or stock gameplay.
"""
from __future__ import annotations
import ctypes
import os
import pathlib
import subprocess
import tempfile
import unittest
from gk292_verify_stock_trace_v2 import verify, MULT, MASK, ROM_SHA1

BASE=pathlib.Path(__file__).resolve().parent
LUA=BASE/'GK1_EXEC0292_Private_Mesen_PowerOn_Recorder_V2.lua'
lib=ctypes.CDLL('liblua5.4.so.0')
lib.luaL_newstate.restype=ctypes.c_void_p
lib.luaL_openlibs.argtypes=[ctypes.c_void_p]
lib.luaL_loadstring.argtypes=[ctypes.c_void_p,ctypes.c_char_p]
lib.luaL_loadstring.restype=ctypes.c_int
lib.luaL_loadfilex.argtypes=[ctypes.c_void_p,ctypes.c_char_p,ctypes.c_char_p]
lib.luaL_loadfilex.restype=ctypes.c_int
lib.lua_pcallk.argtypes=[ctypes.c_void_p,ctypes.c_int,ctypes.c_int,ctypes.c_int,ctypes.c_int,ctypes.c_void_p]
lib.lua_pcallk.restype=ctypes.c_int
lib.lua_tolstring.argtypes=[ctypes.c_void_p,ctypes.c_int,ctypes.c_void_p]
lib.lua_tolstring.restype=ctypes.c_char_p
lib.lua_close.argtypes=[ctypes.c_void_p]
SEED=bytes((0x12,0x23,0x34,0x45,0x56,0x67))
NEXT=(-((int.from_bytes(SEED,'big') | 1)*MULT))&MASK
AFTER=NEXT.to_bytes(6,'big')
DRAW=(NEXT>>32)&32767

PRELUDE=r'''
local loaded_hash = '<<<SHA>>>'
ram = { [0x65bb]=0x97, [0x65bc]=4, [0x65bd]=0,
        [0x65be]=0,[0x7b36]=3,[0x7b37]=0 }
callbacks={}
events={}
cycle,pc=100,0x8030
stopped=false
emu={
 memType={nesDebug=1,nesMemory=2,nesPrgRom=3,nesWorkRam=4},
 callbackType={write=1,exec=2},
 eventType={inputPolled=3,endFrame=4}
}
function emu.getRomInfo() return {fileSha1Hash=loaded_hash} end
function emu.getCpuState() return {cycleCount=cycle,pc=pc} end
function emu.getCpuCycleCount() return cycle end
function emu.read(a,t) return ram[a] or 0 end
function emu.convertAddress(a,t)
 if a>=0xc000 then return {memType=3,address=15*0x4000+(a-0xc000)} end
 if a>=0x8000 then return {memType=3,address=a-0x8000} end
 if a==0x0dcd then return {memType=4,address=0x0dcd} end
 return nil
end
function emu.addMemoryCallback(f,t,low,high)
 callbacks[#callbacks+1]={f=f,t=t,low=low,high=high}
 return #callbacks
end
function emu.addEventCallback(f,t)
 events[t]=f
 return t
end
function emu.setInput(buttons,port) return end
function emu.stop(code) stopped=(code==0) end
'''
FOLLOWUP='''
-- Six witnessed 6502 seed writes, then exact original C289 arithmetic.
local first={18,35,52,69,86,103}
for i=1,6 do
 pc=0x8030;cycle=100+i
 callbacks[1].f(0x83+i,first[i])
 ram[0x83+i]=first[i]
 callbacks[2].f()
end
pc=0xc289;cycle=120;callbacks[3].f()
local final={<<<BYTES>>>}
for i=1,6 do ram[0x83+i]=final[i] end
ram[0x66]=<<<DRAWLOW>>>;ram[0x67]=<<<DRAWHIGH>>>
pc=0xc2eb;cycle=180;callbacks[4].f()
for i=1,2690 do
 events[3]()
 events[4]()
end
assert(stopped,'recorder did not call emu.stop(0)')
'''

def run_lua(source):
    lua=lib.luaL_newstate()
    try:
        lib.luaL_openlibs(lua)
        status=lib.luaL_loadstring(lua,source.encode())
        if status:
            raise RuntimeError('Lua source syntax: '+str(lib.lua_tolstring(lua,-1,None)))
        status=lib.lua_pcallk(lua,0,0,0,0,None)
        if status:
            raise RuntimeError('Lua runtime: '+(lib.lua_tolstring(lua,-1,None) or b'?').decode(errors='replace'))
    finally:
        lib.lua_close(lua)

class LuaMockTests(unittest.TestCase):
    def test_lua_source_compiles(self):
        lua=lib.luaL_newstate()
        try:
            self.assertEqual(lib.luaL_loadfilex(lua,str(LUA).encode(),None),0)
        finally:lib.lua_close(lua)
    def test_wrong_loaded_rom_rejected_before_trace_created(self):
        with tempfile.TemporaryDirectory() as d:
            dest=pathlib.Path(d)/'blocked.log'
            os.environ['GK292_PRIVATE_TRACE_PATH']=str(dest)
            code=PRELUDE.replace('<<<SHA>>>','0'*40)+'\n'+LUA.read_text()
            with self.assertRaisesRegex(RuntimeError,'WRONG LOADED ROM'):
                run_lua(code)
            self.assertFalse(dest.exists())
    def test_mock_lua_to_python_wire_format(self):
        with tempfile.TemporaryDirectory() as d:
            dest=pathlib.Path(d)/'mock.trace'
            os.environ['GK292_PRIVATE_TRACE_PATH']=str(dest)
            suffix=FOLLOWUP.replace('<<<BYTES>>>',','.join(map(str,AFTER)))
            suffix=suffix.replace('<<<DRAWLOW>>>',str(DRAW&255)).replace('<<<DRAWHIGH>>>',str(DRAW>>8))
            code=PRELUDE.replace('<<<SHA>>>',ROM_SHA1)+'\n'+LUA.read_text()+'\n'+suffix
            run_lua(code)
            contents=dest.read_text().splitlines()
            result=verify(contents)
            self.assertEqual((result['writes'],result['calls'],result['stages']),(6,1,14))
            self.assertTrue(result['stock_first_spring_state_candidate'])
            self.assertFalse(result['mesen_execution_independently_proven'])
            self.assertFalse(result['native_stock_spring_implemented'])

if __name__=='__main__': unittest.main(verbosity=2)