"""Native PC Xbox controller input, normalized to a one-to-one NES 8-button pad.

Windows: XInput system DLL; Linux: system SDL2 GameController (supports Xbox pads).
No invented extra gameplay buttons: Xbox A/B/Menu/View map to NES A/B/Start/Select,
and the four D-pad directions map directly. Sticks, shoulder and face X/Y are
intentionally not required for NES-style game navigation.

Physical Xbox controller validation must be run on actual PC hardware; the
synthetic tests exercise the native API adapters and deterministic input router.
"""
from __future__ import annotations
import ctypes
import ctypes.util
import os
import sys
import time

NES_BUTTONS=('up','down','left','right','a','b','start','select')
XINPUT_FLAGS={
    'up':0x0001,'down':0x0002,'left':0x0004,'right':0x0008,
    'start':0x0010,'select':0x0020,'a':0x1000,'b':0x2000,
}
SDL_BUTTONS={'a':0,'b':1,'select':4,'start':6,'up':11,'down':12,'left':13,'right':14}
PC_PHYSICAL_BUTTONS=NES_BUTTONS+('x','y','lb','rb')
PC_XINPUT_FLAGS={**XINPUT_FLAGS,'x':0x4000,'y':0x8000,'lb':0x0100,'rb':0x0200}
PC_SDL_BUTTONS={**SDL_BUTTONS,'x':2,'y':3,'lb':9,'rb':10}
def pc_xinput_buttons(raw):
    return frozenset(k for k,mask in PC_XINPUT_FLAGS.items() if raw & mask)

class _XInputGamepad(ctypes.Structure):
    _fields_=[('buttons',ctypes.c_ushort),('left_trigger',ctypes.c_ubyte),
              ('right_trigger',ctypes.c_ubyte),('thumb_lx',ctypes.c_short),
              ('thumb_ly',ctypes.c_short),('thumb_rx',ctypes.c_short),('thumb_ry',ctypes.c_short)]
class _XInputState(ctypes.Structure):
    _fields_=[('packet_number',ctypes.c_ulong),('gamepad',_XInputGamepad)]

def xinput_buttons(raw):
    return frozenset(k for k,mask in XINPUT_FLAGS.items() if raw & mask)

class WindowsXInput:
    """Reads Windows' XInputGetState, scanning the four supported user indices."""
    def __init__(self):
        lib=None
        for name in ('xinput1_4','xinput1_3','xinput9_1_0'):
            try:lib=ctypes.WinDLL(name);break
            except (OSError,AttributeError):pass
        if lib is None:raise OSError('Windows XInput library not available')
        self.get=lib.XInputGetState
        self.get.argtypes=[ctypes.c_uint,ctypes.POINTER(_XInputState)]
        self.get.restype=ctypes.c_uint
        self.connected=False
    def read(self):
        for index in range(4):
            state=_XInputState()
            if self.get(index,ctypes.byref(state))==0:
                self.connected=True
                return pc_xinput_buttons(state.gamepad.buttons)
        self.connected=False
        return frozenset()
    def close(self):pass

class LinuxSDLGameController:
    """SDL2 GameController polls standardized Xbox-style mapping with hotplug."""
    SDL_INIT_GAMECONTROLLER=0x2000
    def __init__(self):
        path=ctypes.util.find_library('SDL2') or 'libSDL2-2.0.so.0'
        self.lib=ctypes.CDLL(path)
        self.lib.SDL_InitSubSystem.argtypes=[ctypes.c_uint32]
        self.lib.SDL_InitSubSystem.restype=ctypes.c_int
        self.lib.SDL_NumJoysticks.argtypes=[]
        self.lib.SDL_NumJoysticks.restype=ctypes.c_int
        self.lib.SDL_IsGameController.argtypes=[ctypes.c_int]
        self.lib.SDL_IsGameController.restype=ctypes.c_int
        self.lib.SDL_GameControllerOpen.argtypes=[ctypes.c_int]
        self.lib.SDL_GameControllerOpen.restype=ctypes.c_void_p
        self.lib.SDL_GameControllerClose.argtypes=[ctypes.c_void_p]
        self.lib.SDL_GameControllerUpdate.argtypes=[]
        self.lib.SDL_PumpEvents.argtypes=[]
        self.lib.SDL_GameControllerGetAttached.argtypes=[ctypes.c_void_p]
        self.lib.SDL_GameControllerGetAttached.restype=ctypes.c_int
        self.lib.SDL_GameControllerGetButton.argtypes=[ctypes.c_void_p,ctypes.c_int]
        self.lib.SDL_GameControllerGetButton.restype=ctypes.c_ubyte
        self.lib.SDL_QuitSubSystem.argtypes=[ctypes.c_uint32]
        if self.lib.SDL_InitSubSystem(self.SDL_INIT_GAMECONTROLLER)!=0:
            raise OSError('SDL2 GameController init failed')
        self.handle=None
        self.connected=False
    def read(self):
        self.lib.SDL_PumpEvents()  # refresh hotplug and button state even with no pad attached
        if self.handle and not self.lib.SDL_GameControllerGetAttached(self.handle):
            self.lib.SDL_GameControllerClose(self.handle);self.handle=None
        if not self.handle:
            for n in range(max(0,self.lib.SDL_NumJoysticks())):
                if self.lib.SDL_IsGameController(n):
                    self.handle=self.lib.SDL_GameControllerOpen(n)
                    if self.handle:break
        self.connected=bool(self.handle)
        if not self.handle:return frozenset()
        self.lib.SDL_GameControllerUpdate()
        return frozenset(name for name,n in PC_SDL_BUTTONS.items()
                         if self.lib.SDL_GameControllerGetButton(self.handle,n))
    def close(self):
        if self.handle:self.lib.SDL_GameControllerClose(self.handle);self.handle=None
        self.lib.SDL_QuitSubSystem(self.SDL_INIT_GAMECONTROLLER)

def open_native_controller(platform=None):
    platform=sys.platform if platform is None else platform
    try:
        if platform=='win32':return WindowsXInput()
        if platform.startswith('linux'):return LinuxSDLGameController()
    except OSError:
        pass
    return None

class NesInputRouter:
    """Edge detection plus D-pad repeat; injectable provider for deterministic QA."""
    def __init__(self,receiver,provider=None,clock=time.monotonic,bindings=None,raw_receiver=None):
        self.receiver=receiver
        self.bindings=bindings
        self.raw_receiver=raw_receiver
        self.provider=open_native_controller() if provider is None else provider
        self.clock=clock
        self.held=frozenset()
        self.next_repeat={}
    def poll(self):
        if self.provider is None:return []
        try:pressed=frozenset(self.provider.read()) & frozenset(PC_PHYSICAL_BUTTONS)
        except (OSError,ValueError):pressed=frozenset()
        now=self.clock()
        emitted=[]
        # Device events are physical; mapping to logical commands happens after capture.
        # Read-only help on physical Y never becomes a gameplay commit.
        for name in PC_PHYSICAL_BUTTONS:
            if name not in pressed:
                self.next_repeat.pop(name,None)
                continue
            if name not in self.held:
                emitted.append(name)
                if name in ('up','down','left','right'):self.next_repeat[name]=now+0.34
            elif name in self.next_repeat and now>=self.next_repeat[name]:
                emitted.append(name)
                self.next_repeat[name]=now+0.12
        self.held=pressed
        logical=[]
        for name in emitted:
            if self.raw_receiver is not None and self.raw_receiver(name):
                continue  # settings captured this physical button
            action=self.bindings.translate('pad',name) if self.bindings is not None else ('help' if name=='y' else name)
            if action in (*NES_BUTTONS,'help'):
                logical.append(action);self.receiver(action)
        return logical
    def close(self):
        if self.provider is not None:self.provider.close()

MAPPING_LABEL='Xbox D-pad = NES D-pad | A = NES A | B = NES B | Menu = NES Start | View = NES Select | Y = Help'
