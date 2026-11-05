"""Independent GK1 music sequencer model (not a 6502 emulator or APU synthesizer).

Produces ordered (APU register, byte) writes per native driver invocation.
Inputs are private, hash-pinned ROM data and optional captured driver state.
Control semantics reconstructed from bank14 $8000-$8508. No ROM bytes bundled.
"""
from __future__ import annotations
from dataclasses import dataclass, field
import hashlib
from functools import lru_cache

@lru_cache(maxsize=2)
def validate_rom(rom:bytes):
    if hashlib.sha256(rom).hexdigest()!=ROM_SHA:raise ValueError('wrong source ROM')

ROM_SHA='4764d46359581be1e7d12bcb85bd375c84cd4497d1084d9d306a6e884afbe7c5'

@dataclass
class SoundDriver:
    memory: bytearray
    writes: list[tuple[int,int]] = field(default_factory=list)
    tokens: list[tuple[int,int,int]] = field(default_factory=list)
    def __post_init__(self):
        if len(self.memory)!=65536: raise ValueError('need exactly 64 KiB logical data space')
    @classmethod
    def from_trace(cls,rom:bytes,record:dict)->SoundDriver:
        validate_rom(rom)
        m=bytearray(65536);m[0x8000:0xc000]=rom[0x38010:0x3c010]
        z=bytes.fromhex(record['zero']);loops=bytes.fromhex(record['loops'])
        if len(z)!=78 or len(loops)!=64:raise ValueError('invalid native state sizes')
        m[0xb2:0x100]=z;m[0x731:0x771]=loops
        return cls(m)
    @classmethod
    def from_rom(cls, rom: bytes) -> SoundDriver:
        """Start from deterministic zeroed sound state. Does not run any NES CPU."""
        validate_rom(rom)
        return cls.from_sound_bank(rom[0x38010:0x3c010])
    @classmethod
    def from_sound_bank(cls, bank: bytes) -> SoundDriver:
        """Consume a privately supplied, pinned 16-KiB sound bank; never bundled."""
        if len(bank) != 16384 or hashlib.sha256(bank).hexdigest() != '36d2110ac1b67f11c6dfcb4685e8baed2d0d37de6c7e74185fb2c80e9d9cc5a8':
            raise ValueError('wrong sound bank')
        memory=bytearray(65536);memory[0x8000:0xc000]=bank
        return cls(memory)
    def queue_cue(self, cue: int, *, repeat: bool = False) -> None:
        """Queue one of 52 source cues. Zero pointers preserve existing voices."""
        if type(cue) is not int or not 0 <= cue < 52:
            raise ValueError('cue must be an integer from 0 through 51')
        if type(repeat) is not bool:
            raise TypeError('repeat must be bool')
        self.setword(0xb3, self.word(0x8608 + 2*cue))
        self.memory[0xb2]=int(repeat)
        self.memory[0xf5]=1;self.memory[0xf6]=0
    def request_stop(self) -> None:
        """Queue the native STOP branch; next tick emits channel silencing writes."""
        self.memory[0xb2]=0;self.memory[0xf5]=0;self.memory[0xf6]=1
    def export_state(self) -> dict:
        """Serializable sequencer state, not a complete APU/audio-waveform state."""
        return {'format':'gk1-sound-state-v1','zero':self.zero_hex(),'loops':self.loop_hex()}
    def import_state(self, state: dict) -> None:
        if state.get('format') != 'gk1-sound-state-v1':
            raise ValueError('unsupported sound state format')
        zero=bytes.fromhex(state['zero']);loops=bytes.fromhex(state['loops'])
        if len(zero)!=78 or len(loops)!=64:
            raise ValueError('invalid sound state sizes')
        self.memory[0xb2:0x100]=zero;self.memory[0x731:0x771]=loops
    def zero_hex(self):return self.memory[0xb2:0x100].hex()
    def loop_hex(self):return self.memory[0x731:0x771].hex()
    def word(self,p):return self.memory[p]|(self.memory[(p+1)&65535]<<8)
    def put(self,p,v):self.memory[p]=v&255
    def setword(self,p,v):self.put(p,v);self.put(p+1,v>>8)
    def apu(self,p,v):self.writes.append((p,v&255))
    def stop(self,x:int,clear:bool=True):
        m=self.memory
        if clear:self.setword(0xb5+x,0)
        if x in (0,2):m[0xe3+x]=0;self.apu(0x4000+2*x,0x10)
        elif x==4:m[0xef]=0;self.apu(0x4008,0)
        elif x==6:self.apu(0x400c,0x10)
        else:raise ValueError(x)
    def initialize(self):
        m=self.memory
        if m[0xf5]:
            if m[0xf5]==2:
                for x in (0,2,4):self.setword(0xb5+x,self.word(0xbd+x));self.setword(0xc5+x,1)
                for x in (0,2,4):m[0xf7+x]=8*x
                return
            p=self.word(0xb3)
            for x in (0,2,4,6):
                value=self.word(p+x)
                if value:
                    self.setword(0xb5+x,value);self.setword(0xbd+x,value);self.setword(0xc5+x,1)
            tempo=m[p+8]
            if tempo:self.setword(0xd5,tempo)
            m[0xfd]=0x30
            if tempo:
                for x in (0,2,4):m[0xf7+x]=8*x
        elif m[0xf6]:
            for x in (0,2,4,6):self.stop(x)
    def add_duration(self,x):
        self.setword(0xc5+x,self.word(0xc5+x)+8*self.word(0xcd+x))
    @staticmethod
    def pop_index(y):return ((y&0xf0)|((y-4)&15),True) if y&15 else (y,False)
    def parse(self,x):
        m=self.memory;m[0xff]=x;p=self.word(0xb5+x);y=0
        def take():
            nonlocal y
            val=m[(p+y)&65535];y=(y+1)&255;return val
        for _ in range(20000):
            loc=(p+y)&65535;t=take();self.tokens.append((x,loc,t))
            if t==255:
                self.setword(0xb5+x,0);return None
            if t==0x7e:self.add_duration(x);continue
            if t<0x80:
                self.add_duration(x);self.setword(0xb5+x,p+y);return t
            if t<0xc0:
                self.setword(0xcd+x,self.word(0x8509+2*(t&31)));continue
            if t>=0xe0:
                ptr=take();ptr|=take()<<8
                if t in (0xe0,0xe1):
                    for dest,k in [(0xd8,0),(0xdb,1),(0xd7,2),(0xdc,3)]:m[dest+x]=m[ptr+k]
                    self.setword(0xdf+x,ptr+4)
                elif t==0xe2:m[0xeb:0xef]=m[ptr:ptr+4]
                elif t==0xe3:self.setword(0xf3,ptr)
                continue
            if t<0xcf:
                p=(p+y)&65535;y=0;sp=m[0xf7+x]
                m[0x731+sp]=0;m[0x732+sp]=t&15;self.setword(0x733+sp,p)
                m[0xf7+x]=(sp&0xf0)|((sp+4)&15)
                continue
            # CF repeats loop; Dn selects alternative loop ending n.
            while True:
                if t==0xcf:
                    sp,valid=self.pop_index(m[0xf7+x])
                    if valid:
                        m[0x731+sp]=(m[0x731+sp]+1)&255
                        if m[0x731+sp]!=m[0x732+sp]:
                            p=self.word(0x733+sp);y=0;break
                        m[0xf7+x]=sp
                    # After normal repeat termination skip alternate markers.
                    while m[(p+y)&65535]&0xf0==0xd0:y=(y+1)&255
                    break
                sp,_valid=self.pop_index(m[0xf7+x])
                if m[0x731+sp]==((t&15)-1)&255:
                    while m[(p+y)&65535]&0xf0==0xd0:y=(y+1)&255
                    break
                # Original byte scanner consumes through a Dn or CF marker.
                for _j in range(65536):
                    t=take()
                    if y==0:p=(p+0x100)&65535
                    if t==0xcf or t&0xf0==0xd0:break
                else:raise ValueError('unterminated alternative ending')
        raise ValueError('sound stream command budget exceeded')
    def timer_due(self,x):
        m=self.memory
        if m[0xb6+x]==0:return False
        self.setword(0xc5+x,self.word(0xc5+x)-self.word(0xd5))
        return bool(m[0xc6+x]&128)
    def pulse(self,x):
        m=self.memory
        if not self.timer_due(x):return
        note=self.parse(x)
        if note is None:self.stop(x);return
        if note==0x7f:self.stop(x,False);return
        table=0x852f+2*note
        self.apu(0x4001+2*x,m[0xdb+x]);self.apu(0x4002+2*x,m[table+1]);self.apu(0x4003+2*x,m[table]|m[0xd7+x])
        if m[0xdc+x]&128:
            self.apu(0x4000+2*x,m[0xd8+x]);m[0xe3+x]=0
        else:
            m[0xe4+x]=0;m[0xe7+x]=0;m[0xe3+x]=1
    def envelope(self,x):
        m=self.memory
        if not m[0xe3+x]:return
        index=m[0xe4+x]
        if index!=63:
            m[0xe7+x]=(m[0xe7+x]-1)&255
            if m[0xe7+x]&128:m[0xe7+x]=m[0xdc+x];m[0xe4+x]=(index+1)&255
        volume=m[(self.word(0xdf+x)+index)&65535]&15
        self.apu(0x4000+2*x,(m[0xd8+x]&0xc0)|0x30|volume)
    def triangle(self):
        m=self.memory
        if not self.timer_due(4):return
        note=self.parse(4)
        if note is None:self.stop(4);return
        if note==0x7f:self.stop(4,False);return
        table=0x852f+2*note;m[0xf0]=m[table]
        self.apu(0x400b,m[table]|m[0xec]);self.apu(0x400a,m[table+1])
        shape=m[0xed]
        if shape not in (1,2):self.apu(0x4008,m[0xeb]);m[0xef]=0;return
        m[0xef]=1
        if shape==1:
            v=m[0xee]*m[0xd5]
        else:
            d=(m[0xee]-1)&255;product=d*m[0xd5]
            if m[0xee]==0:v=product
            else:
                v=self.word(0xc9)-product
                if v<0:self.stop(4,False);return
        self.setword(0xf1,v);self.apu(0x4008,0xbf);self.apu(0x400b,m[0xf0]|8)
    def triangle_gate(self):
        m=self.memory
        if m[0xef]:
            v=self.word(0xf1)-self.word(0xd5);self.setword(0xf1,v)
            if v<0:self.apu(0x4008,0);m[0xef]=0
    def noise(self):
        m=self.memory
        if not self.timer_due(6):return
        note=self.parse(6)
        if note is None:self.stop(6);return
        if note==0x7f or note<4:self.stop(6,False);return
        p=self.word(0xf3)+3*((note-4)%12)
        for j,a in enumerate((0x400c,0x400e,0x400f)):self.apu(a,m[p+j])
    def tick(self):
        m=self.memory;self.writes=[];self.tokens=[];self.initialize()
        prev=m[0xff];m[0xff]=255
        # The native driver resumes after the last channel which consumed a note.
        d=(prev-1)&255
        if d&128:start=1
        elif (d-1)&255==0:start=2
        elif (d-3)&255==0:start=3
        else:start=0
        for channel in range(start,4):
            if channel<2:self.pulse(channel*2);self.envelope(channel*2)
            elif channel==2:self.triangle();self.triangle_gate()
            else:self.noise();m[0xff]=6
            if m[0xff]<128:break
        m[0xf5]=0;m[0xf6]=0
        if m[0xb2] and not (m[0xb6] or m[0xb8] or m[0xba]):m[0xf5]=2
        return self.writes
