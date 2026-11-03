import ctypes as C, numpy as np, os
HERE = os.path.dirname(os.path.abspath(__file__))
ROM = os.environ.get("GK_ROM", "Genghis Khan (USA).nes")
lib = C.CDLL(os.path.join(HERE, "libgk.so"))

class State(C.Structure):
    _fields_ = [("ram", C.c_uint8*0x800), ("wram", C.c_uint8*0x4000), ("chr", C.c_uint8*0x2000), ("nt", C.c_uint8*0x800),
                ("pal", C.c_uint8*32), ("oam", C.c_uint8*256), ("pc", C.c_uint16), ("a", C.c_uint8), ("x", C.c_uint8), ("y", C.c_uint8), ("sp", C.c_uint8),
                ("fN", C.c_uint8), ("fV", C.c_uint8), ("fD", C.c_uint8), ("fI", C.c_uint8), ("fZ", C.c_uint8), ("fC", C.c_uint8),
                ("mm_shift", C.c_uint8), ("mm_cnt", C.c_uint8), ("mm_ctrl", C.c_uint8), ("mm_chr0", C.c_uint8), ("mm_chr1", C.c_uint8), ("mm_prg", C.c_uint8),
                ("ppuctrl", C.c_uint8), ("ppumask", C.c_uint8), ("ppustatus", C.c_uint8), ("oamaddr", C.c_uint8), ("v", C.c_uint16), ("t", C.c_uint16),
                ("fx", C.c_uint8), ("w", C.c_uint8), ("rbuf", C.c_uint8), ("strobe", C.c_uint8), ("shift", C.c_uint8*2), ("buttons", C.c_uint8*2),
                ("cycles", C.c_int), ("vbl_set", C.c_uint8), ("nmi_done", C.c_uint8), ("frame", C.c_uint32), ("apu", C.c_uint8*0x20),
                ("crashed", C.c_uint8), ("irq_pending", C.c_uint8)]
class Log(C.Structure):
    _fields_ = [("frame", C.c_uint32), ("pc", C.c_uint16), ("bank", C.c_uint8), ("wbank", C.c_uint8), ("addr", C.c_uint16), ("val", C.c_uint8), ("old", C.c_uint8), ("kind", C.c_uint8), ("pad", C.c_uint8)]
class Tr(C.Structure):
    _fields_ = [("pc", C.c_uint16), ("bank", C.c_uint8), ("a", C.c_uint8), ("x", C.c_uint8), ("y", C.c_uint8), ("sp", C.c_uint8), ("p", C.c_uint8)]

class VmRec(C.Structure):
    _fields_ = [("frame", C.c_uint32), ("vpc", C.c_uint16), ("acc", C.c_uint16), ("fp", C.c_uint16), ("gp", C.c_uint16), ("op", C.c_uint8), ("bank", C.c_uint8), ("wbank", C.c_uint8), ("pad", C.c_uint8)]
lib.gk_vmbuf.restype = C.POINTER(VmRec); lib.gk_vm_count.restype = C.c_uint32
lib.gk_vm_alloc.argtypes = [C.c_uint32]; lib.gk_vm.argtypes = [C.c_int]
lib.gk_state.restype = C.POINTER(State)
lib.gk_log.restype = C.POINTER(Log); lib.gk_tracebuf.restype = C.POINTER(Tr)
lib.gk_cov.restype = C.POINTER(C.c_uint32); lib.gk_jcnt.restype = C.POINTER(C.c_uint32)
lib.gk_log_count.restype = C.c_uint32; lib.gk_trace_count.restype = C.c_uint32
lib.gk_peek.restype = C.c_uint8; lib.gk_peek_bank.restype = C.c_uint8
lib.gk_peek.argtypes = [C.c_uint16]; lib.gk_poke.argtypes = [C.c_uint16, C.c_uint8]
lib.gk_set_buttons.argtypes = [C.c_int, C.c_uint8]
lib.gk_peek_bank.argtypes = [C.c_int, C.c_uint16]
lib.gk_prg_bank_for.argtypes = [C.c_uint16]
lib.gk_load.argtypes = [C.c_char_p]
lib.gk_watch.argtypes = [C.c_int, C.c_int, C.c_int]
lib.gk_set_break.argtypes = [C.c_int, C.c_int]
lib.gk_log_alloc.argtypes = [C.c_uint32]; lib.gk_trace_alloc.argtypes = [C.c_uint32]
lib.gk_trace.argtypes = [C.c_int]

A, B, SELECT, START, UP, DOWN, LEFT, RIGHT = 1, 2, 4, 8, 16, 32, 64, 128

class Core:
    def __init__(self, rom=ROM):
        r = lib.gk_load(rom.encode()); assert r == 16, r
        lib.gk_reset(); self.S = lib.gk_state().contents
        self.nprg = 16
    # --- running
    def frame(self, buttons=0, p2=0):
        lib.gk_set_buttons(0, buttons); lib.gk_set_buttons(1, p2)
        return lib.gk_frame()
    def run(self, n, buttons=0):
        for _ in range(n):
            r = self.frame(buttons)
            if r: return r
        return 0
    def press(self, btn, hold=3, wait=30):
        self.run(hold, btn); return self.run(wait, 0)
    # --- memory
    def rd(self, a): return lib.gk_peek(a)
    def rd16(self, a): return lib.gk_peek(a) | (lib.gk_peek(a+1) << 8)
    def wr(self, a, v): lib.gk_poke(a, v)
    def ram(self): return np.ctypeslib.as_array((C.c_uint8*0x800).from_address(C.addressof(self.S.ram)))
    def wram(self): return np.ctypeslib.as_array((C.c_uint8*0x4000).from_address(C.addressof(self.S.wram)))
    def snap(self):
        """copy of all CPU-visible RAM (2K + 16K WRAM)"""
        return bytes(self.S.ram) + bytes(self.S.wram)
    # --- states
    def save(self):
        buf = State(); C.memmove(C.byref(buf), C.byref(self.S), C.sizeof(State)); return buf
    def load(self, st): C.memmove(C.byref(self.S), C.byref(st), C.sizeof(State))
    # --- watch/log
    def watch(self, lo, hi, w=True, r=False): lib.gk_watch(lo, hi, (1 if w else 0) | (2 if r else 0))
    def unwatch(self): lib.gk_clear_watch()
    def logs(self):
        n = lib.gk_log_count(); L = lib.gk_log(); out = []
        for i in range(n):
            l = L[i]; out.append((l.frame, l.pc, l.bank, l.wbank, l.addr, l.val, l.old, l.kind))
        return out
    def prgbank(self, a): return lib.gk_prg_bank_for(a)
    # --- rendering
    def render(self, full=False):
        return render(self.S, full)

PAL = [(84,84,84),(0,30,116),(8,16,144),(48,0,136),(68,0,100),(92,0,48),(84,4,0),(60,24,0),(32,42,0),(8,58,0),(0,64,0),(0,60,0),(0,50,60),(0,0,0),(0,0,0),(0,0,0),
       (152,150,152),(8,76,196),(48,50,236),(92,30,228),(136,20,176),(160,20,100),(152,34,32),(120,60,0),(84,90,0),(40,114,0),(8,124,0),(0,118,40),(0,102,120),(0,0,0),(0,0,0),(0,0,0),
       (236,238,236),(76,154,236),(120,124,236),(176,98,236),(228,84,236),(236,88,180),(236,106,100),(212,136,32),(160,170,0),(116,196,0),(76,208,32),(56,204,108),(56,180,204),(60,60,60),(0,0,0),(0,0,0),
       (236,238,236),(168,204,236),(188,188,236),(212,178,236),(236,174,236),(236,174,212),(236,180,176),(228,196,144),(204,210,120),(180,222,120),(168,226,144),(152,226,180),(160,214,228),(160,162,160),(0,0,0),(0,0,0)]
PAL = np.array(PAL, dtype=np.uint8)

def tile_pixels(chr_, base, idx):
    idx = int(idx); base = int(base)
    t = chr_[base + idx*16: base + idx*16 + 16]
    lo = np.unpackbits(t[:8, None], axis=1); hi = np.unpackbits(t[8:, None], axis=1)
    return lo + 2*hi  # 8x8 values 0..3

def render(S, full=False):
    chr_ = np.frombuffer(bytes(S.chr), dtype=np.uint8); nt = np.frombuffer(bytes(S.nt), dtype=np.uint8)
    pal = np.frombuffer(bytes(S.pal), dtype=np.uint8)
    mm = S.mm_ctrl & 3
    def ntidx(n):  # n = 0..3 logical nametable
        return [0, 1, n & 1, (n >> 1) & 1][mm] if mm >= 2 else mm
    # mm 2=vertical -> idx = n&1 ; mm 3=horizontal -> idx = n>>1
    def ntbase(n):
        if mm == 0: i = 0
        elif mm == 1: i = 1
        elif mm == 2: i = n & 1
        else: i = (n >> 1) & 1
        return i*0x400
    W, H = 512, 480
    bg = np.zeros((H, W), dtype=np.uint8); bgp = np.zeros((H, W), dtype=np.uint8)  # color idx, palette sel
    bgbase = 0x1000 if (S.ppuctrl & 0x10) else 0
    for n in range(4):
        b = ntbase(n); ox = (n & 1)*256; oy = (n >> 1)*240
        for ty in range(30):
            for tx in range(32):
                ti = nt[b + ty*32 + tx]
                attr = nt[b + 0x3C0 + (ty//4)*8 + (tx//4)]
                sh = ((ty//2) & 1)*4 + ((tx//2) & 1)*2
                p = (attr >> sh) & 3
                px = tile_pixels(chr_, bgbase, ti)
                bg[oy+ty*8:oy+ty*8+8, ox+tx*8:ox+tx*8+8] = px; bgp[oy+ty*8:oy+ty*8+8, ox+tx*8:ox+tx*8+8] = p
    def color(ci, p, spr=False):
        base = (0x10 if spr else 0) + p*4
        idx = np.where(ci == 0, pal[0], pal[(base + ci) & 0x1F]) & 0x3F
        return PAL[idx]
    img = color(bg, bgp)
    if not (S.ppumask & 0x08): img = np.broadcast_to(PAL[pal[0] & 0x3F], img.shape).copy()
    # sprites
    if S.ppumask & 0x10:
        h16 = bool(S.ppuctrl & 0x20); sbase = 0x1000 if (S.ppuctrl & 0x08) else 0
        scx = ((S.t & 0x1F) << 3 | S.fx) + 256*((S.t >> 10) & 1); scy = (((S.t >> 5) & 0x1F) << 3 | ((S.t >> 12) & 7)) + 240*((S.t >> 11) & 1)
        oam = bytes(S.oam)
        for i in range(63, -1, -1):
            y, ti, at, x = oam[i*4], oam[i*4+1], oam[i*4+2], oam[i*4+3]
            if y >= 0xEF: continue
            hh = 16 if h16 else 8
            for ty in range(hh//8):
                if h16: tb = 0x1000 if (ti & 1) else 0; tn = (ti & 0xFE) + (ty if not (at & 0x80) else 1-ty)
                else: tb = sbase; tn = ti
                px = tile_pixels(chr_, tb, tn)
                if at & 0x40: px = px[:, ::-1]
                if at & 0x80 and not h16: px = px[::-1, :]
                if at & 0x80 and h16: px = px[::-1, :]
                col = color(px, at & 3, True)
                yy = y + 1 + ty*8 + scy; xx = x + scx
                for r in range(8):
                    for c in range(8):
                        if px[r, c] == 0: continue
                        Y = (yy + r) % H; X = (xx + c) % W
                        if (at & 0x20) and bg[Y, X] != 0: continue
                        img[Y, X] = col[r, c]
    if full: return img
    scx = ((S.t & 0x1F) << 3 | S.fx) + 256*((S.t >> 10) & 1); scy = (((S.t >> 5) & 0x1F) << 3 | ((S.t >> 12) & 7)) + 240*((S.t >> 11) & 1)
    big = np.tile(img, (2, 2, 1))
    return big[scy:scy+240, scx:scx+256]
