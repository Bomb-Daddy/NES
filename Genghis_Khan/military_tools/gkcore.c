// gkcore: minimal NES (MMC1/SOROM) emulator core with tracing hooks, for ROM analysis.
// CPU: official 6502 opcodes. PPU: register/VRAM model only (no rendering); rendering done in Python.
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef uint8_t u8; typedef uint16_t u16; typedef uint32_t u32;

#define FRAME_CYCLES 29781
#define VBL_CYCLE 27394

typedef struct { u32 frame; u16 pc; u8 bank; u8 wbank; u16 addr; u8 val; u8 old; u8 kind; u8 pad; } Log; // kind 1=write 2=read
typedef struct { u16 pc; u8 bank; u8 a, x, y, sp, p; } Tr;

typedef struct {
  u8 ram[0x800]; u8 wram[0x4000]; u8 chr[0x2000]; u8 nt[0x800]; u8 pal[32]; u8 oam[256];
  u16 pc; u8 a, x, y, sp; u8 fN, fV, fD, fI, fZ, fC;
  u8 mm_shift, mm_cnt, mm_ctrl, mm_chr0, mm_chr1, mm_prg;
  u8 ppuctrl, ppumask, ppustatus, oamaddr; u16 v, t; u8 fx, w, rbuf;
  u8 strobe, shift[2], buttons[2];
  int cycles; u8 vbl_set, nmi_done; u32 frame; u8 apu[0x20];
  u8 crashed; u8 irq_pending;
} State;

static State S;
static u8 *PRG = 0; static int NPRG = 0; static int HAS_BAT = 0;
static u8 wflag[0x10000];            // bit0 write-watch, bit1 read-watch
static Log *logbuf = 0; static u32 logmax = 0, logn = 0;
static Tr *trbuf = 0; static u32 trmax = 0, trn = 0; static int tron = 0;
static u32 *cov = 0, *jcnt = 0;      // per (bank*16384 + off)
static int brk_pc = -1, brk_bank = -1; static int brk_hit = 0;
static u16 cur_pc; // pc of the instruction being executed (for logs)
typedef struct { u32 frame; u16 vpc; u16 acc; u16 fp; u16 gp; u8 op; u8 bank; u8 wbank; u8 pad; } VmRec;
static VmRec *vmbuf = 0; static u32 vmmax = 0, vmn = 0; static int vmon = 0;

static int prg_bank_for(u16 addr) {
  int mode = (S.mm_ctrl >> 2) & 3, pb = S.mm_prg & 0x0F;
  if (mode <= 1) return ((pb & ~1) + (addr >= 0xC000)) % NPRG;
  if (mode == 2) return addr < 0xC000 ? 0 : pb % NPRG;
  return addr < 0xC000 ? pb % NPRG : NPRG - 1;
}
static int wbank_sel(void) { return (S.mm_chr0 >> 3) & 1; }

static u8 ppu_rd(u16 a) {
  a &= 0x3FFF;
  if (a < 0x2000) return S.chr[a];
  if (a < 0x3F00) { int m = S.mm_ctrl & 3, i; a &= 0xFFF; if (m == 0) i = 0; else if (m == 1) i = 1; else if (m == 2) i = (a >> 10) & 1; else i = (a >> 11) & 1; return S.nt[i * 0x400 + (a & 0x3FF)]; }
  a &= 0x1F; if ((a & 3) == 0) a &= 0x0F; return S.pal[a];
}
static void ppu_wr(u16 a, u8 v) {
  a &= 0x3FFF;
  if (a < 0x2000) { S.chr[a] = v; return; }
  if (a < 0x3F00) { int m = S.mm_ctrl & 3, i; a &= 0xFFF; if (m == 0) i = 0; else if (m == 1) i = 1; else if (m == 2) i = (a >> 10) & 1; else i = (a >> 11) & 1; S.nt[i * 0x400 + (a & 0x3FF)] = v; return; }
  a &= 0x1F; if ((a & 3) == 0) a &= 0x0F; S.pal[a] = v;
}

static u8 cpu_rd(u16 a);
static void cpu_wr(u16 a, u8 v);

static void dolog(u16 a, u8 v, u8 old, u8 kind) {
  if (logn < logmax) { Log *l = &logbuf[logn++]; l->frame = S.frame; l->pc = cur_pc; l->bank = (cur_pc >= 0x8000) ? prg_bank_for(cur_pc) : 0xFF; l->wbank = wbank_sel(); l->addr = a; l->val = v; l->old = old; l->kind = kind; }
}

static u8 cpu_peek(u16 a) {
  if (a < 0x2000) return S.ram[a & 0x7FF];
  if (a >= 0x6000 && a < 0x8000) return S.wram[wbank_sel() * 0x2000 + (a - 0x6000)];
  if (a >= 0x8000) return PRG[(size_t)prg_bank_for(a) * 0x4000 + (a & 0x3FFF)];
  return 0;
}

static u8 cpu_rd(u16 a) {
  u8 r = 0;
  if (a < 0x2000) r = S.ram[a & 0x7FF];
  else if (a < 0x4000) {
    switch (a & 7) {
      case 2: r = (S.ppustatus & 0x80 ? 0x80 : 0) | (S.ppustatus & 0x60); S.ppustatus &= ~0x80; S.w = 0; break;
      case 4: r = S.oam[S.oamaddr]; break;
      case 7: { u16 va = S.v & 0x3FFF; if (va >= 0x3F00) { r = ppu_rd(va); S.rbuf = ppu_rd(va - 0x1000); } else { r = S.rbuf; S.rbuf = ppu_rd(va); } S.v += (S.ppuctrl & 4) ? 32 : 1; break; }
      default: r = 0;
    }
  }
  else if (a == 0x4016 || a == 0x4017) { int i = a & 1; r = 0x40 | (S.shift[i] & 1); if (!S.strobe) S.shift[i] = (S.shift[i] >> 1) | 0x80; else S.shift[i] = S.buttons[i]; }
  else if (a == 0x4015) r = 0;
  else if (a >= 0x6000 && a < 0x8000) r = S.wram[wbank_sel() * 0x2000 + (a - 0x6000)];
  else if (a >= 0x8000) r = PRG[(size_t)prg_bank_for(a) * 0x4000 + (a & 0x3FFF)];
  if (wflag[a] & 2) dolog(a, r, r, 2);
  return r;
}

static void mm_write(u16 a, u8 v) {
  if (v & 0x80) { S.mm_shift = 0; S.mm_cnt = 0; S.mm_ctrl |= 0x0C; return; }
  S.mm_shift |= (v & 1) << S.mm_cnt; S.mm_cnt++;
  if (S.mm_cnt == 5) {
    u8 d = S.mm_shift; int r = (a >> 13) & 3;
    if (r == 0) S.mm_ctrl = d; else if (r == 1) S.mm_chr0 = d; else if (r == 2) S.mm_chr1 = d; else S.mm_prg = d;
    S.mm_shift = 0; S.mm_cnt = 0;
  }
}

static void cpu_wr(u16 a, u8 v) {
  u8 old = 0;
  if (a < 0x2000) { old = S.ram[a & 0x7FF]; S.ram[a & 0x7FF] = v; }
  else if (a < 0x4000) {
    switch (a & 7) {
      case 0: S.ppuctrl = v; S.t = (S.t & 0xF3FF) | ((v & 3) << 10); break;
      case 1: S.ppumask = v; break;
      case 3: S.oamaddr = v; break;
      case 4: S.oam[S.oamaddr++] = v; break;
      case 5: if (!S.w) { S.fx = v & 7; S.t = (S.t & 0xFFE0) | (v >> 3); S.w = 1; } else { S.t = (S.t & 0x8C1F) | ((v & 7) << 12) | ((v & 0xF8) << 2); S.w = 0; } break;
      case 6: if (!S.w) { S.t = (S.t & 0x00FF) | ((v & 0x3F) << 8); S.w = 1; } else { S.t = (S.t & 0xFF00) | v; S.v = S.t; S.w = 0; } break;
      case 7: ppu_wr(S.v & 0x3FFF, v); S.v += (S.ppuctrl & 4) ? 32 : 1; break;
    }
  }
  else if (a == 0x4014) { u16 base = v << 8; for (int i = 0; i < 256; i++) S.oam[(u8)(S.oamaddr + i)] = cpu_peek(base + i); S.cycles += 513; }
  else if (a == 0x4016) { S.strobe = v & 1; if (S.strobe) { S.shift[0] = S.buttons[0]; S.shift[1] = S.buttons[1]; } }
  else if (a < 0x4020) { S.apu[a & 0x1F] = v; }
  else if (a >= 0x6000 && a < 0x8000) { int i = wbank_sel() * 0x2000 + (a - 0x6000); old = S.wram[i]; S.wram[i] = v; }
  else if (a >= 0x8000) mm_write(a, v);
  if (wflag[a] & 1) dolog(a, v, old, 1);
}

// ---- CPU ----
#define SETZN(v) do { S.fZ = ((v) & 0xFF) == 0; S.fN = ((v) >> 7) & 1; } while (0)
static inline u8 getp(int b) { return (S.fN << 7) | (S.fV << 6) | 0x20 | (b << 4) | (S.fD << 3) | (S.fI << 2) | (S.fZ << 1) | S.fC; }
static inline void setp(u8 p) { S.fN = (p >> 7) & 1; S.fV = (p >> 6) & 1; S.fD = (p >> 3) & 1; S.fI = (p >> 2) & 1; S.fZ = (p >> 1) & 1; S.fC = p & 1; }
static inline void push(u8 v) { cpu_wr(0x100 + S.sp, v); S.sp--; }
static inline u8 pull(void) { S.sp++; return cpu_rd(0x100 + S.sp); }
static inline u8 fetch(void) { return cpu_rd(S.pc++); }
static inline u16 fetch16(void) { u8 l = fetch(); u8 h = fetch(); return l | (h << 8); }

static void adc(u8 m) { int s = S.a + m + S.fC; S.fV = (~(S.a ^ m) & (S.a ^ s) & 0x80) != 0; S.fC = s > 0xFF; S.a = s & 0xFF; SETZN(S.a); }
static void cmp(u8 r, u8 m) { int d = r - m; S.fC = r >= m; SETZN(d & 0xFF); }

static void do_nmi(void) { push(S.pc >> 8); push(S.pc & 0xFF); push(getp(0)); S.fI = 1; S.pc = cpu_rd(0xFFFA) | (cpu_rd(0xFFFB) << 8); S.cycles += 7; }

static int step_cpu(void) {
  cur_pc = S.pc;
  if (S.pc >= 0x8000) { int b = prg_bank_for(S.pc); u32 k = (u32)b * 0x4000 + (S.pc & 0x3FFF); if (cov) cov[k]++; }
  if (tron && trn < trmax) { Tr *t = &trbuf[trn++]; t->pc = S.pc; t->bank = S.pc >= 0x8000 ? prg_bank_for(S.pc) : 0xFF; t->a = S.a; t->x = S.x; t->y = S.y; t->sp = S.sp; t->p = getp(0); }
  if (brk_pc >= 0 && S.pc == brk_pc && (brk_bank < 0 || prg_bank_for(S.pc) == brk_bank)) { brk_hit = 1; return -1; }
  if (vmon && S.pc == 0xEFD3 && vmn < vmmax) { VmRec *r = &vmbuf[vmn++]; r->frame = S.frame; r->op = S.a; r->vpc = S.ram[6] | (S.ram[7] << 8); r->acc = S.ram[8] | (S.ram[9] << 8); r->fp = S.ram[4] | (S.ram[5] << 8); r->gp = S.ram[2] | (S.ram[3] << 8); r->bank = S.mm_prg & 0x0F; r->wbank = wbank_sel(); }
  u8 op = fetch(); int cyc = 2; u16 ea = 0; int cross = 0;
  int cc = op & 3, bbb = (op >> 2) & 7, aaa = op >> 5;
  // addressing
  #define ZP() ea = fetch()
  #define ZPX() ea = (u8)(fetch() + S.x)
  #define ZPY() ea = (u8)(fetch() + S.y)
  #define ABS() ea = fetch16()
  #define ABSX() do { u16 b = fetch16(); ea = b + S.x; cross = (b & 0xFF00) != (ea & 0xFF00); } while (0)
  #define ABSY() do { u16 b = fetch16(); ea = b + S.y; cross = (b & 0xFF00) != (ea & 0xFF00); } while (0)
  #define INDX() do { u8 z = fetch() + S.x; ea = cpu_rd(z) | (cpu_rd((u8)(z + 1)) << 8); } while (0)
  #define INDY() do { u8 z = fetch(); u16 b = cpu_rd(z) | (cpu_rd((u8)(z + 1)) << 8); ea = b + S.y; cross = (b & 0xFF00) != (ea & 0xFF00); } while (0)
  if (op == 0x00) { S.pc++; push(S.pc >> 8); push(S.pc & 0xFF); push(getp(1)); S.fI = 1; S.pc = cpu_rd(0xFFFE) | (cpu_rd(0xFFFF) << 8); cyc = 7; }
  else if (op == 0x20) { u16 t = fetch16(); S.pc--; push(S.pc >> 8); push(S.pc & 0xFF); S.pc = t; if (jcnt && t >= 0x8000) jcnt[(u32)prg_bank_for(t) * 0x4000 + (t & 0x3FFF)]++; cyc = 6; }
  else if (op == 0x40) { setp(pull()); u8 l = pull(); u8 h = pull(); S.pc = l | (h << 8); cyc = 6; }
  else if (op == 0x60) { u8 l = pull(); u8 h = pull(); S.pc = (l | (h << 8)) + 1; cyc = 6; }
  else if (op == 0x08) { push(getp(1)); cyc = 3; }
  else if (op == 0x28) { setp(pull()); cyc = 4; }
  else if (op == 0x48) { push(S.a); cyc = 3; }
  else if (op == 0x68) { S.a = pull(); SETZN(S.a); cyc = 4; }
  else if ((op & 0x1F) == 0x10) { // branches
    int8_t off = (int8_t)fetch(); int cond = 0;
    switch (op >> 6) { case 0: cond = S.fN; break; case 1: cond = S.fV; break; case 2: cond = S.fC; break; case 3: cond = S.fZ; break; }
    if (!((op >> 5) & 1)) cond = !cond;
    cyc = 2; if (cond) { u16 t = S.pc + off; cyc += 1 + ((t & 0xFF00) != (S.pc & 0xFF00)); S.pc = t; }
  }
  else if ((op & 0x1F) == 0x18) { switch (op) { case 0x18: S.fC = 0; break; case 0x38: S.fC = 1; break; case 0x58: S.fI = 0; break; case 0x78: S.fI = 1; break; case 0x98: S.a = S.y; SETZN(S.a); break; case 0xB8: S.fV = 0; break; case 0xD8: S.fD = 0; break; case 0xF8: S.fD = 1; break; } }
  else if (op == 0x8A) { S.a = S.x; SETZN(S.a); }
  else if (op == 0x9A) { S.sp = S.x; }
  else if (op == 0xAA) { S.x = S.a; SETZN(S.x); }
  else if (op == 0xBA) { S.x = S.sp; SETZN(S.x); }
  else if (op == 0xCA) { S.x--; SETZN(S.x); }
  else if (op == 0xEA) { }
  else if (op == 0x88) { S.y--; SETZN(S.y); }
  else if (op == 0xA8) { S.y = S.a; SETZN(S.y); }
  else if (op == 0xC8) { S.y++; SETZN(S.y); }
  else if (op == 0xE8) { S.x++; SETZN(S.x); }
  else if (cc == 1) {
    switch (bbb) { case 0: INDX(); cyc = 6; break; case 1: ZP(); cyc = 3; break; case 2: ea = S.pc++; cyc = 2; break; case 3: ABS(); cyc = 4; break; case 4: INDY(); cyc = 5 + (cross && aaa != 4); break; case 5: ZPX(); cyc = 4; break; case 6: ABSY(); cyc = 4 + (cross && aaa != 4); break; case 7: ABSX(); cyc = 4 + (cross && aaa != 4); break; }
    if (aaa == 4) { if (bbb == 2) { S.crashed = 1; return -2; } cpu_wr(ea, S.a); if (bbb == 4 || bbb == 6 || bbb == 7) cyc += 1; }
    else { u8 m = cpu_rd(ea);
      switch (aaa) { case 0: S.a |= m; SETZN(S.a); break; case 1: S.a &= m; SETZN(S.a); break; case 2: S.a ^= m; SETZN(S.a); break; case 3: adc(m); break; case 5: S.a = m; SETZN(S.a); break; case 6: cmp(S.a, m); break; case 7: adc(m ^ 0xFF); break; } }
  }
  else if (cc == 2) {
    int acc = (bbb == 2);
    switch (bbb) { case 0: ea = S.pc++; cyc = 2; break; case 1: ZP(); cyc = 5; break; case 2: cyc = 2; break; case 3: ABS(); cyc = 6; break; case 5: if (aaa == 4 || aaa == 5) ZPY(); else ZPX(); cyc = 6; break; case 7: if (aaa == 5) ABSY(); else ABSX(); cyc = 7; break; default: S.crashed = 1; return -2; }
    if (aaa == 4) { if (bbb == 0 || bbb == 2 || bbb == 7) { S.crashed = 1; return -2; } cpu_wr(ea, S.x); cyc = bbb == 1 ? 3 : 4; }
    else if (aaa == 5) { u8 m = cpu_rd(ea); S.x = m; SETZN(S.x); cyc = bbb == 1 ? 3 : (bbb == 7 ? 4 + cross : 4); }
    else {
      if (bbb == 0) { S.crashed = 1; return -2; }
      u8 m = acc ? S.a : cpu_rd(ea); u8 r = m;
      switch (aaa) { case 0: S.fC = m >> 7; r = m << 1; break; case 1: { u8 c = S.fC; S.fC = m >> 7; r = (m << 1) | c; break; } case 2: S.fC = m & 1; r = m >> 1; break; case 3: { u8 c = S.fC; S.fC = m & 1; r = (m >> 1) | (c << 7); break; } case 6: r = m - 1; break; case 7: r = m + 1; break; }
      SETZN(r); if (acc) S.a = r; else cpu_wr(ea, r);
    }
  }
  else if (cc == 0) {
    switch (bbb) { case 0: ea = S.pc++; cyc = 2; break; case 1: ZP(); cyc = 3; break; case 3: ABS(); cyc = 4; break; case 5: ZPX(); cyc = 4; break; case 7: ABSX(); cyc = 4 + cross; break; default: S.crashed = 1; return -2; }
    if (aaa == 1) { u8 m = cpu_rd(ea); S.fZ = (S.a & m) == 0; S.fN = m >> 7; S.fV = (m >> 6) & 1; }
    else if (aaa == 2) { S.pc = ea; if (bbb == 3) { /* JMP abs */ } cyc = 3; }
    else if (aaa == 3) { u16 p = ea; u8 l = cpu_rd(p); u8 h = cpu_rd((p & 0xFF00) | ((p + 1) & 0xFF)); S.pc = l | (h << 8); cyc = 5; }
    else if (aaa == 4) { cpu_wr(ea, S.y); cyc = bbb == 1 ? 3 : 4; }
    else if (aaa == 5) { u8 m = cpu_rd(ea); S.y = m; SETZN(S.y); }
    else if (aaa == 6) { cmp(S.y, cpu_rd(ea)); }
    else if (aaa == 7) { cmp(S.x, cpu_rd(ea)); }
    else { S.crashed = 1; return -2; }
  }
  else { S.crashed = 1; return -2; }
  return cyc + (cross && cc == 1 && aaa != 4 && (bbb == 4 || bbb == 6 || bbb == 7) ? 0 : 0);
}

// ---- API ----
int gk_load(const char *path) {
  FILE *f = fopen(path, "rb"); if (!f) return -1;
  u8 h[16]; if (fread(h, 1, 16, f) != 16) return -2;
  NPRG = h[4]; HAS_BAT = (h[6] >> 1) & 1; if (h[6] & 4) fseek(f, 512, SEEK_CUR);
  free(PRG); PRG = (u8 *)malloc((size_t)NPRG * 0x4000); if (fread(PRG, 1, (size_t)NPRG * 0x4000, f) != (size_t)NPRG * 0x4000) return -3; fclose(f);
  free(cov); free(jcnt); cov = (u32 *)calloc((size_t)NPRG * 0x4000, 4); jcnt = (u32 *)calloc((size_t)NPRG * 0x4000, 4);
  return NPRG;
}
void gk_reset(void) {
  memset(&S, 0, sizeof S); S.mm_ctrl = 0x0C; S.sp = 0xFD; S.fI = 1; S.pc = cpu_peek(0xFFFC) | (cpu_peek(0xFFFD) << 8);
  memset(S.ram, 0, sizeof S.ram);
}
void gk_set_buttons(int p, u8 b) { S.buttons[p & 1] = b; }
void gk_poke(u16 a, u8 v) { if (a < 0x2000) S.ram[a & 0x7FF] = v; else if (a >= 0x6000 && a < 0x8000) S.wram[wbank_sel() * 0x2000 + a - 0x6000] = v; }
u8 gk_peek(u16 a) { return cpu_peek(a); }
u8 gk_peek_bank(int bank, u16 a) { return PRG[(size_t)bank * 0x4000 + (a & 0x3FFF)]; }
int gk_prgbanks(void) { return NPRG; }
u8 *gk_prg(void) { return PRG; }
int gk_prg_bank_for(u16 a) { return prg_bank_for(a); }
// returns frames run; stops early on break/crash. result: 0 ok, 1 break, 2 crash
int gk_frame(void) {
  int endc = S.cycles + FRAME_CYCLES; int start = S.cycles; (void)start; S.cycles = 0; S.vbl_set = 0; S.nmi_done = 0; S.ppustatus &= ~0x80;
  while (S.cycles < FRAME_CYCLES) {
    if (!S.vbl_set && S.cycles >= VBL_CYCLE) { S.vbl_set = 1; S.ppustatus |= 0x80; if ((S.ppuctrl & 0x80) && !S.nmi_done) { S.nmi_done = 1; do_nmi(); } }
    int c = step_cpu(); if (c == -1) return 1; if (c == -2) return 2; S.cycles += c;
  }
  (void)endc; S.frame++; return 0;
}
void gk_getstate(State *out) { memcpy(out, &S, sizeof S); }
void gk_setstate(const State *in) { memcpy(&S, in, sizeof S); }
int gk_statesize(void) { return sizeof(State); }
State *gk_state(void) { return &S; }
void gk_watch(int lo, int hi, int flags) { for (int a = lo; a <= hi; a++) wflag[a] = flags; }
void gk_clear_watch(void) { memset(wflag, 0, sizeof wflag); }
void gk_log_alloc(u32 n) { free(logbuf); logbuf = (Log *)malloc((size_t)n * sizeof(Log)); logmax = n; logn = 0; }
void gk_log_reset(void) { logn = 0; }
u32 gk_log_count(void) { return logn; }
Log *gk_log(void) { return logbuf; }
int gk_log_size(void) { return sizeof(Log); }
void gk_trace_alloc(u32 n) { free(trbuf); trbuf = (Tr *)malloc((size_t)n * sizeof(Tr)); trmax = n; trn = 0; }
void gk_trace(int on) { tron = on; }
void gk_trace_reset(void) { trn = 0; }
u32 gk_trace_count(void) { return trn; }
Tr *gk_tracebuf(void) { return trbuf; }
int gk_trace_size(void) { return sizeof(Tr); }
u32 *gk_cov(void) { return cov; }
u32 *gk_jcnt(void) { return jcnt; }
void gk_cov_reset(void) { memset(cov, 0, (size_t)NPRG * 0x4000 * 4); memset(jcnt, 0, (size_t)NPRG * 0x4000 * 4); }
void gk_set_break(int pc, int bank) { brk_pc = pc; brk_bank = bank; brk_hit = 0; }

void gk_vm_alloc(u32 n) { free(vmbuf); vmbuf = (VmRec *)malloc((size_t)n * sizeof(VmRec)); vmmax = n; vmn = 0; }
void gk_vm(int on) { vmon = on; }
void gk_vm_reset(void) { vmn = 0; }
u32 gk_vm_count(void) { return vmn; }
VmRec *gk_vmbuf(void) { return vmbuf; }
int gk_vm_size(void) { return sizeof(VmRec); }
