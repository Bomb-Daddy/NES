#include "gk83_reset_trace.h"
#include "gk84_reset_hypothesis.h"
#include "gk82_stock_cartridge.h"
#include "gk1_source_identity.h"
#include "gk94_prgram.h"
#include "gk97_nmi_trace.h"
#include "gk98_controller_trace.h"
#include "gk98_controller_port.h"
#include "gk98_cpu_rotate.h"
#include "gk100_cpu_cycles.h"
#include "gk101_mmc1_cycles.h"
#include "gk102_controller_frames.h"
#include "gk103_ntsc_ppu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROM_BYTES 262160u
#define PRG_START 16u
#define BANK_BYTES 16384u
#define F_C 1u
#define F_Z 2u
#define F_I 4u
#define F_D 8u
#define F_V 64u
#define F_N 128u
/* The public source contains no private cartridge data: only the expected
 * SHA256 fingerprint, also verified independently by GK82. */
static const uint8_t expected_sha[32]={
 0xad,0xa7,0x06,0x6d,0x9e,0x71,0xa8,0x13,
 0x10,0xff,0xf7,0x8b,0xc7,0x03,0x71,0x26,
 0x79,0x80,0x18,0xa6,0x9d,0xc2,0xb9,0x23,
 0x82,0xc3,0xcb,0xdc,0xd9,0xb2,0x43,0xb3
};
typedef struct {
 const uint8_t *rom;
 uint8_t ram[2048],known[2048],a,x,y,sp,p;
 Gk94PrgRam prgram; /* NES PRG-RAM: never alias 2KiB CPU work RAM */
 uint8_t shift,control,prg,chr0,chr1,control_known,prg_known;
 uint16_t pc;
 const uint8_t *hypothetical_ppustatus;
 uint32_t hypothetical_ppustatus_count, hypothetical_ppustatus_used;
 int hypothetical_one_shot_nmi, nmi_attempted;
 int hypothetical_controller_enabled;
 int ppu_schedule_enabled,ppu_schedule_gate;
 Gk103Ppu ppu;
 uint8_t hypothetical_upper_bus_bits;
 const uint8_t *scripted_frame_buttons;
 uint8_t scripted_frame_count,scripted_frame_index;
 uint32_t scripted_last_injection_instruction;
 Gk98Pad controller,controller2;
 uint16_t nmi_vector;
 uint8_t last_branch_taken;
 Gk101Mmc1Filter mapper_filter;
 uint64_t write_end_cycle; /* last bus write cycle of current fixed-cycle store/RMW */
 Gk83Trace result;
} Machine;
static void nz(Machine *m,uint8_t v){m->p=(uint8_t)((m->p&~(F_N|F_Z))|(v?0:F_Z)|(v&F_N));}
static Gk83Stop read_byte(Machine *m,uint16_t addr,uint8_t *v){
 if(addr<0x2000u){
  unsigned at=addr&0x7ffu;
  if(!m->known[at]){m->result.first_unknown_ram_read=addr;return GK83_UNKNOWN_RAM_GATE;}
  *v=m->ram[at];return GK83_INVALID;
 }
 /* EXEC0092 research-only: a caller can explore one explicitly supplied
  * hypothetical PPUSTATUS read sequence. No timing, vblank, open bus, or
  * original Mesen observation is inferred. This path is distinct from the
  * normal executable source trace, which ALWAYS stops before hardware. */
 /* EXEC0103: explicitly phase-parametrized PPU clock, separate from all
  * earlier caller-supplied status sequences and synthetic NMIs. */
 if(m->ppu_schedule_enabled && addr>=0x2000u && addr<0x4000u && (addr&7u)==2u){
  if(!m->result.first_hardware_read)m->result.first_hardware_read=addr;
  m->result.last_hardware_read=addr;
  /* LDA absolute's CPU bus read occurs on its fourth cycle. The bounded
   * source tracer does not yet model dynamic DMA stalls/CPU-PPU phase. */
  if(!gk103_ppu_status(&m->ppu,m->result.completed_cpu_cycles+3u,0u,v))
   return GK83_HARDWARE_GATE;
  m->result.ppu_scheduled_status_reads=m->ppu.status_reads;
  return GK83_INVALID;
 }
 if(addr>=0x2000u && addr<0x4000u && (addr&7u)==2u &&
    m->hypothetical_ppustatus &&
    m->hypothetical_ppustatus_used<m->hypothetical_ppustatus_count){
  if(!m->result.first_hardware_read)m->result.first_hardware_read=addr;
  m->result.last_hardware_read=addr;
  *v=m->hypothetical_ppustatus[m->hypothetical_ppustatus_used++];
  m->result.hypothetical_ppustatus_reads=m->hypothetical_ppustatus_used;
  return GK83_INVALID;
 }
 if((addr==0x4016u||addr==0x4017u) && m->hypothetical_controller_enabled){
  if(!m->result.first_hardware_read)m->result.first_hardware_read=addr;
  m->result.last_hardware_read=addr;
  Gk98Pad *p=addr==0x4016u?&m->controller:&m->controller2;
  *v=gk98_pad_read(p,m->hypothetical_upper_bus_bits);
  m->result.hypothetical_controller_reads=m->controller.reads;
  m->result.hypothetical_controller2_reads=m->controller2.reads;
  return GK83_INVALID;
 }
 if(addr>=0x6000u && addr<0x8000u){
  Gk94RamResult r=gk94_prgram_read(&m->prgram,addr,v);
  if(r==GK94_RAM_OK)return GK83_INVALID;
  /* Unseeded battery SaveRam is UNKNOWN, not a PPU/APU hardware event.
   * A fresh all-zero nesWorkRam setting cannot license these contents. */
  m->result.first_unknown_ram_read=addr;
  return GK83_UNKNOWN_RAM_GATE;
 }
 if(addr<0x8000u){
  if(addr>=0x2000u&&addr<0x6000u){
   if(!m->result.first_hardware_read)m->result.first_hardware_read=addr;
   m->result.last_hardware_read=addr;
  }
  return GK83_HARDWARE_GATE;
 }
 unsigned bank=15u;
 if(addr<0xc000u){
  m->result.first_banked_pc=m->result.first_banked_pc?m->result.first_banked_pc:addr;
  if(!m->control_known||!m->prg_known)return GK83_BANK_GATE;
  unsigned mode=(m->control>>2u)&3u;
  if(mode==3u)bank=m->prg&15u;
  else if(mode==2u)bank=0u;
  else bank=m->prg&14u;
 }else if(m->control_known){
  unsigned mode=(m->control>>2u)&3u;
  if(mode==2u)bank=m->prg&15u;
  else if(mode<2u)bank=(m->prg&14u)+1u;
  if(mode!=3u&&!m->prg_known)return GK83_BANK_GATE;
 }
 *v=m->rom[PRG_START+bank*BANK_BYTES+(addr&0x3fffu)];
 return GK83_INVALID;
}
static void mmc1_write(Machine *m,uint16_t addr,uint8_t value,uint64_t cpu_cycle){
 m->result.mapper_serial_writes++;
 Gk101Decision decision=gk101_mmc1_write(&m->mapper_filter,cpu_cycle,value);
 if(decision==GK101_IGNORED){m->result.mapper_consecutive_ignored++;return;}
 if(decision==GK101_NONMONOTONIC)return; /* no ungrounded mapper mutation */
 if(decision==GK101_RESET){
  m->shift=0x10u;
  /* RESET forces PRG mode 3, but does not establish unknown low
   * nametable/CHR bits, so control_known remains explicitly unchanged. */
  m->control|=0x0cu;m->result.mapper_bit7_resets++;return;
 }
 unsigned full=m->shift&1u;
 m->shift=(uint8_t)((m->shift>>1u)|((value&1u)<<4u));
 if(!full)return;
 unsigned group=(addr>>13u)&3u;
 if(group==0u){m->control=m->shift;m->control_known=1;}
 else if(group==1u)m->chr0=m->shift;
 else if(group==2u)m->chr1=m->shift;
 else {if(m->prg_known&&m->prg!=(m->shift&15u))m->result.rom_bank_switches++;
       m->prg=m->shift&15u;m->prg_known=1;}
 m->result.mapper_register_commits++;
 m->shift=0x10u;
}
static void write_byte(Machine *m,uint16_t addr,uint8_t v){
 if(addr<0x2000u){unsigned at=addr&0x7ffu;
  m->ram[at]=v;m->known[at]=1u;m->result.ram_writes++;
 }else if(addr>=0x6000u && addr<0x8000u){
  if(gk94_prgram_write(&m->prgram,addr,v)==GK94_RAM_OK)m->result.ram_writes++;
 }else if(addr>=0x8000u)mmc1_write(m,addr,v,m->write_end_cycle);
 else {
  if(m->ppu_schedule_enabled&&addr>=0x2000u&&addr<0x4000u){
   if((addr&7u)==0u){
    if(!gk103_ppu_ctrl(&m->ppu,m->write_end_cycle,v))m->ppu_schedule_gate=1;
   }else if((addr&7u)==1u){
    if(!gk103_ppu_mask(&m->ppu,m->write_end_cycle,v))m->ppu_schedule_gate=1;
   }
  }
  if(addr>=0x2000u&&addr<0x4000u&&(addr&7u)==0u){
   m->result.ppuctrl_write_attempts++;
   if(v&0x80u)m->result.ppuctrl_nmi_enable_attempts++;
   m->result.last_ppuctrl_source_pc=m->result.stop_pc;
   m->result.last_ppuctrl_value=v;
  }
  if(addr==0x4016u && m->hypothetical_controller_enabled){
   gk98_pad_write(&m->controller,v);
   gk98_pad_write(&m->controller2,v);
   m->result.hypothetical_controller_writes=m->controller.writes;
  }
  m->result.peripheral_writes++;
 }
}
static void push(Machine *m,uint8_t v){write_byte(m,(uint16_t)(0x100u+m->sp),v);m->sp--;}
static Gk83Stop pop(Machine *m,uint8_t *v){m->sp++;return read_byte(m,(uint16_t)(0x100u+m->sp),v);}
static Gk83Stop operand(Machine *m,uint8_t *v){Gk83Stop s=read_byte(m,m->pc,v);if(s==GK83_INVALID)m->pc++;return s;}
static Gk83Stop word(Machine *m,uint16_t *v){
 uint8_t lo,hi;Gk83Stop s=operand(m,&lo);if(s!=GK83_INVALID)return s;
 s=operand(m,&hi);if(s!=GK83_INVALID)return s;
 *v=(uint16_t)((unsigned)lo|((unsigned)hi<<8u));return GK83_INVALID;
}
static Gk83Stop branch(Machine *m,int cond){
 uint8_t rel;Gk83Stop s=operand(m,&rel);if(s!=GK83_INVALID)return s;
 m->last_branch_taken=(uint8_t)(cond?1u:0u);
 if(cond)m->pc=(uint16_t)((int)m->pc+(int)(int8_t)rel);
 return GK83_INVALID;
}
static Gk83Stop one(Machine *m){
 if(m->ppu_schedule_gate)return GK83_HARDWARE_GATE;
 uint8_t op,b,indexed_cross=0u;uint16_t addr;
 uint16_t start_pc=m->pc;
 m->last_branch_taken=0u;
 Gk83Stop s=read_byte(m,m->pc,&op);if(s!=GK83_INVALID)return s;
 m->result.stop_pc=m->pc;m->result.stop_opcode=op;m->pc++;
 /* For stores and read-modify-write opcodes, the last write is on the
  * final instruction cycle. Indexed READ penalties cannot affect these.
  * It is not valid to infer timing of other peripheral reads here. */
 m->write_end_cycle=m->result.completed_cpu_cycles+gk100_cpu_cycles(op,0,0)-1u;
 switch(op){
 case 0xea:break; /* 6502 documented NOP: two CPU cycles, no state mutation */
 case 0x78:m->p|=F_I;break; /* SEI */
 case 0xd8:m->p&=(uint8_t)~F_D;break; /* CLD */
 case 0x18:m->p&=(uint8_t)~F_C;break; /* CLC */
 case 0x38:m->p|=F_C;break; /* SEC: observed opcode in source-bound setup branch */
 case 0xa9:s=operand(m,&m->a);if(!s)nz(m,m->a);break;
 case 0xa2:s=operand(m,&m->x);if(!s)nz(m,m->x);break;
 case 0xa0:s=operand(m,&m->y);if(!s)nz(m,m->y);break;
 case 0x9a:m->sp=m->x;break; /* TXS */
 case 0xba:m->x=m->sp;nz(m,m->x);break; /* TSX */
 case 0x8a:m->a=m->x;nz(m,m->a);break;
 case 0x98:m->a=m->y;nz(m,m->a);break;
 case 0xaa:m->x=m->a;nz(m,m->x);break;
 case 0xa8:m->y=m->a;nz(m,m->y);break;
 case 0xe8:m->x++;nz(m,m->x);break;
 case 0xca:m->x--;nz(m,m->x);break;
 case 0xc8:m->y++;nz(m,m->y);break;
 case 0x88:m->y--;nz(m,m->y);break;
 case 0x0a:m->p=(uint8_t)((m->p&~F_C)|((m->a>>7u)&1u));m->a<<=1u;nz(m,m->a);break; /* ASL A */
 case 0x4a:m->p=(uint8_t)((m->p&~F_C)|(m->a&1u));m->a>>=1u;nz(m,m->a);break;
 case 0x08:push(m,(uint8_t)(m->p|0x30u));break; /* PHP: stack always sets B/unused bits */
 case 0x28:s=pop(m,&b);if(!s)m->p=(uint8_t)((b|0x20u)&~0x10u);break; /* PLP */
 case 0x48:push(m,m->a);break;
 case 0x68:s=pop(m,&m->a);if(!s)nz(m,m->a);break;
 case 0x8d:s=word(m,&addr);if(!s)write_byte(m,addr,m->a);break;
 case 0x7e:{ /* ROR absolute,X: first source-opcode gate after $4016. */
  s=word(m,&addr);
  if(!s){addr=(uint16_t)(addr+m->x);s=read_byte(m,addr,&b);}
  if(!s){Gk98Rotate r=gk98_cpu_ror(b,(uint8_t)(m->p&F_C));
   m->p=(uint8_t)((m->p&~F_C)|(r.carry?F_C:0u));
   if(addr>=0x8000u){
    /* The NMOS CPU writes the OLD byte at T-1, then new byte at T.
     * The MMC1 normally ignores the second bit0 write, except reset.
     * The first write can change the mapper in the middle of this opcode. */
    m->result.mapper_rmw_double_writes++;
    mmc1_write(m,addr,b,m->write_end_cycle-1u);
   }
   write_byte(m,addr,r.value);nz(m,r.value);}
  break;}
 case 0x26:s=operand(m,&b);if(!s){uint8_t v;
   s=read_byte(m,b,&v);if(!s){uint8_t new_c=(v>>7u)&1u;
    v=(uint8_t)((v<<1u)|(m->p&F_C));
    m->p=(uint8_t)((m->p&~F_C)|new_c);
    write_byte(m,b,v);nz(m,v);}}break; /* ROL zp */
 case 0xce:case 0xee:{ /* DEC/INC abs: NMOS 6502 reads, writes old, writes decremented */
  s=word(m,&addr);
  if(!s)s=read_byte(m,addr,&b);
  if(!s){
   if(addr>=0x8000u){m->result.mapper_rmw_double_writes++;
    mmc1_write(m,addr,b,m->write_end_cycle-1u);}
   else write_byte(m,addr,b);
   b=(uint8_t)(op==0xceu ? b-1u : b+1u);
   write_byte(m,addr,b);
   nz(m,b);
  }break;}
 case 0xc6:{ /* DEC zp: old/new source writes (NMOS RMW) */
  s=operand(m,&b);if(!s){uint8_t v;s=read_byte(m,b,&v);if(!s){write_byte(m,b,v);v--;write_byte(m,b,v);nz(m,v);}}break;}
 case 0xe6:s=operand(m,&b);if(!s){uint8_t v;
   s=read_byte(m,b,&v);if(!s){v++;write_byte(m,b,v);nz(m,v);}}break; /* INC zp */
 case 0x84:s=operand(m,&b);if(!s)write_byte(m,b,m->y);break; /* STY zp */
 case 0x86:s=operand(m,&b);if(!s)write_byte(m,b,m->x);break; /* STX zp */
 case 0x85:s=operand(m,&b);if(!s)write_byte(m,b,m->a);break;
 case 0x91:{ /* STA ($nn),Y: wrap high pointer read inside zero page. */
  uint8_t lo,hi;s=operand(m,&b);
  if(!s)s=read_byte(m,b,&lo);
  if(!s)s=read_byte(m,(uint8_t)(b+1u),&hi);
  if(!s){addr=(uint16_t)(((unsigned)lo|((unsigned)hi<<8u))+m->y);
   write_byte(m,addr,m->a);}
  break;}

 case 0x9d:s=word(m,&addr);if(!s)write_byte(m,(uint16_t)(addr+m->x),m->a);break;
 case 0x99:s=word(m,&addr);if(!s)write_byte(m,(uint16_t)(addr+m->y),m->a);break;
 case 0xad:s=word(m,&addr);if(!s){s=read_byte(m,addr,&m->a);if(!s)nz(m,m->a);}break;
 case 0xae:s=word(m,&addr);if(!s){s=read_byte(m,addr,&m->x);if(!s)nz(m,m->x);}break; /* LDX abs; preserve exact unknown/IO gate. */
 case 0xbc:s=word(m,&addr);if(!s){indexed_cross=gk100_page_crossed(addr,(uint16_t)(addr+m->x));s=read_byte(m,(uint16_t)(addr+m->x),&m->y);if(!s)nz(m,m->y);}break; /* LDY abs,X */
 case 0xac:s=word(m,&addr);if(!s){s=read_byte(m,addr,&m->y);if(!s)nz(m,m->y);}break; /* LDY abs */
 case 0xa6:s=operand(m,&b);if(!s){s=read_byte(m,b,&m->x);if(!s)nz(m,m->x);}break; /* LDX zp */
 case 0xa5:s=operand(m,&b);if(!s){s=read_byte(m,b,&m->a);if(!s)nz(m,m->a);}break;
 case 0xbd:s=word(m,&addr);if(!s){indexed_cross=gk100_page_crossed(addr,(uint16_t)(addr+m->x));s=read_byte(m,(uint16_t)(addr+m->x),&m->a);if(!s)nz(m,m->a);}break;
 case 0xb9:s=word(m,&addr);if(!s){indexed_cross=gk100_page_crossed(addr,(uint16_t)(addr+m->y));s=read_byte(m,(uint16_t)(addr+m->y),&m->a);if(!s)nz(m,m->a);}break;
 case 0xb1:{ /* LDA ($nn),Y -- 6502 wraps high pointer byte in zero page. */
  uint8_t lo,hi;s=operand(m,&b);
  if(!s)s=read_byte(m,b,&lo);
  if(!s)s=read_byte(m,(uint8_t)(b+1u),&hi);
  if(!s){indexed_cross=gk100_page_crossed((uint16_t)((unsigned)lo|((unsigned)hi<<8u)),(uint16_t)(((unsigned)lo|((unsigned)hi<<8u))+m->y));
  addr=(uint16_t)(((unsigned)lo|((unsigned)hi<<8u))+m->y);
   s=read_byte(m,addr,&m->a);if(!s)nz(m,m->a);}
  break;}

 case 0xd1:{ /* CMP ($nn),Y with zero-page pointer wrap. */
  uint8_t lo,hi;s=operand(m,&b);
  if(!s)s=read_byte(m,b,&lo);
  if(!s)s=read_byte(m,(uint8_t)(b+1u),&hi);
  if(!s){indexed_cross=gk100_page_crossed((uint16_t)((unsigned)lo|((unsigned)hi<<8u)),(uint16_t)(((unsigned)lo|((unsigned)hi<<8u))+m->y));
  addr=(uint16_t)(((unsigned)lo|((unsigned)hi<<8u))+m->y);
   s=read_byte(m,addr,&b);}
  if(!s){unsigned diff=(unsigned)m->a-b;
   m->p=(uint8_t)((m->p&~F_C)|(m->a>=b?F_C:0));nz(m,(uint8_t)diff);}
  break;}
 case 0xc0:s=operand(m,&b);if(!s){unsigned diff=(unsigned)m->y-b;
   m->p=(uint8_t)((m->p&~F_C)|(m->y>=b?F_C:0));nz(m,(uint8_t)diff);}break;
 case 0xe0:s=operand(m,&b);if(!s){unsigned diff=(unsigned)m->x-b;
   m->p=(uint8_t)((m->p&~F_C)|(m->x>=b?F_C:0));nz(m,(uint8_t)diff);}break;
 case 0xc5:s=operand(m,&b);if(!s){s=read_byte(m,b,&b);if(!s){unsigned diff=(unsigned)m->a-b;m->p=(uint8_t)((m->p&~F_C)|(m->a>=b?F_C:0));nz(m,(uint8_t)diff);}}break; /* CMP zp */
 case 0xcd:s=word(m,&addr);if(!s){s=read_byte(m,addr,&b);if(!s){unsigned diff=(unsigned)m->a-b;
   m->p=(uint8_t)((m->p&~F_C)|(m->a>=b?F_C:0));nz(m,(uint8_t)diff);}}break;
 case 0xc9:s=operand(m,&b);if(!s){unsigned diff=(unsigned)m->a-b;
   m->p=(uint8_t)((m->p&~F_C)|(m->a>=b?F_C:0));nz(m,(uint8_t)diff);}break;
 case 0x29:s=operand(m,&b);if(!s){m->a&=b;nz(m,m->a);}break;
 case 0x4d:s=word(m,&addr);if(!s){s=read_byte(m,addr,&b);if(!s){m->a^=b;nz(m,m->a);}}break; /* EOR abs */
 case 0x05:s=operand(m,&b);if(!s){s=read_byte(m,b,&b);if(!s){m->a|=b;nz(m,m->a);}}break; /* ORA zp */
 case 0x09:s=operand(m,&b);if(!s){m->a|=b;nz(m,m->a);}break;
 case 0x6d:{ /* ADC abs, NES binary mode */
  s=word(m,&addr);if(!s)s=read_byte(m,addr,&b);
  if(!s){unsigned sum=(unsigned)m->a+(unsigned)b+(m->p&F_C);
   uint8_t v=(uint8_t)sum;
   unsigned overflow=(~((unsigned)m->a^(unsigned)b)&((unsigned)m->a^v)&0x80u)!=0u;
   m->p=(uint8_t)((m->p&~(F_C|F_V))|(sum>255u?F_C:0u)|(overflow?F_V:0u));
   m->a=v;nz(m,v);}
  break;}
 case 0xe5:{ /* SBC zp, 2A03 binary arithmetic */
  s=operand(m,&b);if(!s)s=read_byte(m,b,&b);
  if(!s){b^=0xffu;unsigned sum=(unsigned)m->a+(unsigned)b+(m->p&F_C);
   uint8_t v=(uint8_t)sum;
   unsigned overflow=(~((unsigned)m->a^(unsigned)b)&((unsigned)m->a^v)&0x80u)!=0u;
   m->p=(uint8_t)((m->p&~(F_C|F_V))|(sum>255u?F_C:0u)|(overflow?F_V:0u));
   m->a=v;nz(m,v);}break;}
 case 0x65:{ /* ADC zp */
  s=operand(m,&b);
  if(!s)s=read_byte(m,b,&b);
  if(!s){unsigned sum=(unsigned)m->a+(unsigned)b+(m->p&F_C);
   uint8_t v=(uint8_t)sum;
   unsigned overflow=(~((unsigned)m->a^(unsigned)b)&((unsigned)m->a^v)&0x80u)!=0u;
   m->p=(uint8_t)((m->p&~(F_C|F_V))|(sum>255u?F_C:0u)|(overflow?F_V:0u));
   m->a=v;nz(m,v);}
  break;}
 case 0x69:case 0xe9:{ /* NES 2A03 ignores decimal-mode BCD arithmetic. */
  s=operand(m,&b);
  if(!s){unsigned cin=m->p&F_C;
   if(op==0xe9)b^=0xffu; /* SBC = A + complement(M) + C */
   unsigned sum=(unsigned)m->a+(unsigned)b+cin;
   uint8_t v=(uint8_t)sum;
   unsigned overflow=(~((unsigned)m->a^(unsigned)b)&((unsigned)m->a^v)&0x80u)!=0u;
   m->p=(uint8_t)((m->p&~(F_C|F_V))|(sum>255u?F_C:0u)|(overflow?F_V:0u));
   m->a=v;nz(m,v);
  }
  break;}
 case 0xf1:{ /* SBC ($nn),Y, binary arithmetic on NES CPU. */
  uint8_t lo,hi;s=operand(m,&b);
  if(!s)s=read_byte(m,b,&lo);
  if(!s)s=read_byte(m,(uint8_t)(b+1u),&hi);
  if(!s){indexed_cross=gk100_page_crossed((uint16_t)((unsigned)lo|((unsigned)hi<<8u)),(uint16_t)(((unsigned)lo|((unsigned)hi<<8u))+m->y));
  addr=(uint16_t)(((unsigned)lo|((unsigned)hi<<8u))+m->y);
   s=read_byte(m,addr,&b);}
  if(!s){b^=0xffu;
   unsigned sum=(unsigned)m->a+(unsigned)b+(m->p&F_C);
   uint8_t v=(uint8_t)sum;
   unsigned overflow=(~((unsigned)m->a^(unsigned)b)&((unsigned)m->a^v)&0x80u)!=0u;
   m->p=(uint8_t)((m->p&~(F_C|F_V))|(sum>255u?F_C:0u)|(overflow?F_V:0u));
   m->a=v;nz(m,v);}
  break;}
 case 0x71:{ /* ADC ($nn),Y, zero-page wrapping and binary carry. */
  uint8_t lo,hi;s=operand(m,&b);
  if(!s)s=read_byte(m,b,&lo);
  if(!s)s=read_byte(m,(uint8_t)(b+1u),&hi);
  if(!s){indexed_cross=gk100_page_crossed((uint16_t)((unsigned)lo|((unsigned)hi<<8u)),(uint16_t)(((unsigned)lo|((unsigned)hi<<8u))+m->y));
  addr=(uint16_t)(((unsigned)lo|((unsigned)hi<<8u))+m->y);
   s=read_byte(m,addr,&b);}
  if(!s){unsigned sum=(unsigned)m->a+(unsigned)b+(m->p&F_C);
   uint8_t v=(uint8_t)sum;
   unsigned overflow=(~((unsigned)m->a^(unsigned)b)&((unsigned)m->a^v)&0x80u)!=0u;
   m->p=(uint8_t)((m->p&~(F_C|F_V))|(sum>255u?F_C:0u)|(overflow?F_V:0u));
   m->a=v;nz(m,v);
  }
  break;}

 case 0xb0:s=branch(m,(m->p&F_C)!=0u);break; /* BCS */
 case 0x90:s=branch(m,(m->p&F_C)==0u);break; /* BCC */
 case 0xd0:s=branch(m,(m->p&F_Z)==0u);break;
 case 0xf0:s=branch(m,(m->p&F_Z)!=0u);break;
 case 0x10:s=branch(m,(m->p&F_N)==0u);break;
 case 0x30:s=branch(m,(m->p&F_N)!=0u);break;
 case 0x20:s=word(m,&addr);if(!s){
  uint16_t ret=(uint16_t)(m->pc-1u);push(m,(uint8_t)(ret>>8u));push(m,(uint8_t)ret);
  if(!m->result.first_jsr)m->result.first_jsr=addr;
  m->result.jsr_count++;m->pc=addr;
 }break;
 case 0x60:{uint8_t lo,hi;s=pop(m,&lo);if(!s)s=pop(m,&hi);
  if(!s){m->pc=(uint16_t)(((unsigned)lo|((unsigned)hi<<8u))+1u);}
  break;}
 case 0x6c:{ /* JMP (abs) reproduces NMOS 6502 indirect page-wrap bug. */
  uint8_t lo,hi;s=word(m,&addr);
  if(!s)s=read_byte(m,addr,&lo);
  if(!s)s=read_byte(m,(uint16_t)((addr&0xff00u)|((addr+1u)&0x00ffu)),&hi);
  if(!s)m->pc=(uint16_t)((unsigned)lo|((unsigned)hi<<8u));
  break;}
 case 0x4c:s=word(m,&addr);if(!s)m->pc=addr;break;
 case 0x40:{ /* RTI restores P, then PC low, then high. */
  uint8_t lo,hi;s=pop(m,&b);
  if(!s)s=pop(m,&lo);
  if(!s)s=pop(m,&hi);
  if(!s){m->p=(uint8_t)((b|0x20u)&~0x10u);m->pc=(uint16_t)(lo|((unsigned)hi<<8u));}
  break;
 }
 case 0x00:s=GK83_BRK_GATE;break;
 default:s=GK83_OPCODE_GATE;break;
 }
 if(s==GK83_INVALID){
  uint8_t branch_op=(uint8_t)(op==0xb0u||op==0x90u||op==0xd0u||op==0xf0u||op==0x10u||op==0x30u);
  uint8_t page=branch_op?gk100_page_crossed((uint16_t)(start_pc+2u),m->pc):indexed_cross;
  uint8_t cycles=gk100_cpu_cycles(op,m->last_branch_taken,page);
  if(cycles==0u)return GK83_OPCODE_GATE;
  m->result.completed_cpu_cycles+=cycles;
  m->result.instructions++;
 }
 return s;
}
/* A deliberately supplied single NMI request, NOT PPU timing. This uses the
 * real mapper-selected FFFA/FFFB vector and actual 6502 interrupt frame.
 * It may reveal the next original-source CPU gate, but it is not a prediction
 * of which frame original Mesen would trigger the interrupt. */
static Gk83Stop inject_hypothetical_nmi(Machine *m){
 uint8_t lo,hi;
 Gk83Stop s=read_byte(m,0xfffau,&lo);
 if(s!=GK83_INVALID)return s;
 s=read_byte(m,0xfffbu,&hi);
 if(s!=GK83_INVALID)return s;
 m->nmi_vector=(uint16_t)(lo|((unsigned)hi<<8u));
 push(m,(uint8_t)(m->pc>>8u));push(m,(uint8_t)m->pc);
 push(m,(uint8_t)((m->p|0x20u)&~0x10u));
 m->p|=F_I;
 m->pc=m->nmi_vector;
 m->nmi_attempted=1;
 return GK83_INVALID;
}
/* EXEC0084: a single explicitly hypothetical RAM byte may be preseeded
 * for controlled branching; this does NOT describe a genuine NES power-on
 * value. The regular trace never presumes RAM was initialized. */
static Gk83Stop trace_impl(const char *path,uint32_t max_steps,Gk83Trace *out,
                          int use_hypothesis,uint16_t ram_address,uint8_t value,int mesen_zero_ram,
                          const uint8_t *ppu_values,uint32_t ppu_count,int hypothetical_one_shot_nmi,
                          int hypothetical_controller_enabled,uint8_t controller_buttons,
                          uint8_t controller2_buttons,
                          uint8_t hypothetical_upper_bus_bits,
                          const uint8_t *scripted_buttons,uint32_t scripted_count,
                          int ppu_scheduled,uint8_t ppu_phase){
 if(!path||!out||max_steps==0u||max_steps>1000000u||
    (use_hypothesis && ram_address>=2048u) || ppu_count>256u ||
    (ppu_count && !ppu_values) || (ppu_values && !ppu_count) ||
    (ppu_count && !mesen_zero_ram) ||
    (hypothetical_controller_enabled && ((!hypothetical_one_shot_nmi && !scripted_count) || ppu_count<6u || (hypothetical_upper_bus_bits&1u))) ||
    (scripted_count>32u) ||
    (ppu_scheduled&&(ppu_phase>2u||ppu_count||hypothetical_one_shot_nmi||scripted_count)) ||
    ((scripted_count==0u)!=(scripted_buttons==NULL)) ||
    (scripted_count && (!hypothetical_controller_enabled || ppu_count<6u || !mesen_zero_ram || hypothetical_one_shot_nmi)))return GK83_INVALID;
 Gk82RomBootstrap boot;
 if(gk82_probe_stock_rom(path,&boot)!=GK82_ROM_OK)return GK83_SOURCE_REFUSED;
 uint8_t *rom=(uint8_t*)malloc(ROM_BYTES+1u);
 if(!rom)return GK83_SOURCE_REFUSED;
 FILE *file=fopen(path,"rb");if(!file){free(rom);return GK83_SOURCE_REFUSED;}
 size_t n=fread(rom,1,ROM_BYTES+1u,file);
 int bad=ferror(file);
 if(fclose(file)!=0)bad=1;
 if(n!=ROM_BYTES||bad){free(rom);return GK83_SOURCE_REFUSED;}
 uint8_t sha[32];gk1_sha256_bytes(rom,ROM_BYTES,sha);
 if(memcmp(sha,expected_sha,32)!=0){free(rom);return GK83_SOURCE_REFUSED;}
 Machine *m=(Machine*)calloc(1,sizeof *m);
 if(!m){free(rom);return GK83_SOURCE_REFUSED;}
 m->rom=rom;m->pc=boot.reset_vector;m->shift=0x10u;
 if(ppu_scheduled){m->ppu_schedule_enabled=1;
  if(!gk103_ppu_reset(&m->ppu,ppu_phase)){free(m);free(rom);return GK83_INVALID;}}

 m->hypothetical_ppustatus=ppu_values;
 m->hypothetical_ppustatus_count=ppu_count;
 m->hypothetical_one_shot_nmi=hypothetical_one_shot_nmi;
 m->hypothetical_controller_enabled=hypothetical_controller_enabled;
 m->hypothetical_upper_bus_bits=hypothetical_upper_bus_bits;
 m->scripted_frame_buttons=scripted_buttons;
 m->scripted_frame_count=(uint8_t)scripted_count;
 if(hypothetical_controller_enabled){gk98_pad_reset(&m->controller,controller_buttons);
   gk98_pad_reset(&m->controller2,controller2_buttons);}
 /* CPU 6502 reset enters at $C000; X/SP subsequently established by ROM.
  * Unknown RAM reads fail closed; no arbitrary power-on RAM is invented. */
 m->sp=0xfdu;m->p=F_I;
 /* A new, explicitly selected MesenCE power-on mode, not an NES-hardware
  * universal. The archived fresh-reset script sets RamPowerOnState=1;
  * MesenCE SettingTypes.h maps 1 to AllZeros, and EmuSettings.cpp uses
  * memset(data,0,length). The reset interpreter remains bounded by
  * unknown peripherals/opcodes/MMC1 behavior. A different capture setting
  * must NEVER silently take this path. */
 if(mesen_zero_ram)memset(m->known,1,sizeof m->known);
 if(use_hypothesis){m->ram[ram_address]=value;m->known[ram_address]=1u;}
 Gk83Stop status=GK83_BUDGET_GATE;
 for(uint32_t i=0;i<max_steps;i++){
  /* Research script: each additional interrupt is deliberately supplied at
   * a previously observed CPU flag-wait address, not scheduled by a PPU.
   * This may expose the NEXT unsupported source/hardware gate. One input
   * event is committed only when a synthetic interrupt is actually injected. */
  int scripted_trigger=m->scripted_frame_index<m->scripted_frame_count &&
     ((m->scripted_frame_index==0u && m->pc==0xc460u && m->result.instructions>150u) ||
      (m->scripted_frame_index>0u && m->pc==0xc486u &&
       m->result.instructions-m->scripted_last_injection_instruction>=64u));
  if(scripted_trigger){
   m->controller.buttons=m->scripted_frame_buttons[m->scripted_frame_index];
   m->controller2.buttons=0u;
   m->result.nmi_requested_after_completed_cycles=m->result.completed_cpu_cycles;
   status=inject_hypothetical_nmi(m);
   if(status!=GK83_INVALID)break;
   m->result.completed_cpu_cycles+=7u;
   m->scripted_last_injection_instruction=m->result.instructions;
   m->scripted_frame_index++;
   m->result.scripted_controller_frames=m->scripted_frame_index;
   m->result.scripted_last_frame_instruction=m->result.instructions;
   m->result.scripted_last_buttons=m->controller.buttons;
   m->result.hypothetical_nmi_injections=m->scripted_frame_index;
  }
  if(m->hypothetical_one_shot_nmi&&!m->nmi_attempted&&m->pc==0xc460u&&
     m->result.instructions>150u){
   m->result.ppuctrl_nmi_requested_at_injection=(uint8_t)((m->result.last_ppuctrl_value&0x80u)!=0u);
   m->result.nmi_requested_after_completed_cycles=m->result.completed_cpu_cycles;
   status=inject_hypothetical_nmi(m);
   if(status==GK83_INVALID)m->result.completed_cpu_cycles+=7u; /* NMI hardware entry sequence, not PPU event timing */
   if(status!=GK83_INVALID)break;
  }
  if(m->ppu_schedule_enabled){
   if(!gk103_ppu_advance(&m->ppu,m->result.completed_cpu_cycles)){
    status=GK83_HARDWARE_GATE;break;
   }
   if(gk103_ppu_take_nmi(&m->ppu)){
    status=inject_hypothetical_nmi(m);
    if(status!=GK83_INVALID)break;
    m->result.ppu_scheduled_nmi_injections++;
    m->result.completed_cpu_cycles+=7u;
   }
  }
  status=one(m);if(status!=GK83_INVALID)break;
 }
 if(status==GK83_INVALID)status=GK83_BUDGET_GATE;
 m->result.stop=status;
 if(m->ppu_schedule_enabled){
  m->result.ppu_scheduled_frames=m->ppu.frames;
  m->result.ppu_scheduled_vblank=m->ppu.vblank_events;
  m->result.ppu_scheduled_nmi_edges=m->ppu.nmi_edges;
  m->result.ppu_scheduled_status_reads=m->ppu.status_reads;
  m->result.ppu_scheduled_ppu_phase=m->ppu.phase;
  m->result.ppu_scheduled_warmup_ignored=m->ppu.warmup_ignored_ctrl;
  m->result.ppu_scheduled_render_gate=m->ppu.rendering_unmodeled;
  m->result.ppu_scheduled_race_gate=m->ppu.race_gate;
 }

 m->result.a=m->a;m->result.x=m->x;m->result.y=m->y;
 m->result.sp=m->sp;m->result.flags=m->p;
 m->result.hypothetical_nmi_injections=m->scripted_frame_count?
   m->scripted_frame_index:(uint8_t)m->nmi_attempted;
 m->result.hypothetical_nmi_vector=m->nmi_vector;
 m->result.hypothetical_controller_buttons=hypothetical_controller_enabled?controller_buttons:0u;
 m->result.control=m->control;m->result.prg=m->prg;
 m->result.chr0=m->chr0;m->result.chr1=m->chr1;
 m->result.control_known=m->control_known;m->result.prg_known=m->prg_known;
 memcpy(out,&m->result,sizeof *out);
 free(m);free(rom);
 return status;
}

Gk83Stop gk83_trace_original_reset(const char *path,uint32_t max_steps,Gk83Trace *out){
 return trace_impl(path,max_steps,out,0,0u,0u,0,NULL,0,0,0,0u,0u,0u,NULL,0u,0,0u);
}
Gk83Stop gk84_trace_reset_single_ram_hypothesis(const char *path,
 uint16_t ram_address,uint8_t value,uint32_t max_steps,Gk83Trace *out){
 return trace_impl(path,max_steps,out,1,ram_address,value,0,NULL,0,0,0,0u,0u,0u,NULL,0u,0,0u);
}

/* EXEC0091: exact archived fresh Mesen script selected zero-filled CPU RAM.
 * This is source-driven emulation of that particular setting, NOT a new
 * Mesen runtime observation or a completed original Spring generator. */
Gk83Stop gk91_trace_reset_mesen_allzeros(const char *path,
 uint32_t max_steps,Gk83Trace *out){
 return trace_impl(path,max_steps,out,0,0u,0u,1,NULL,0,0,0,0u,0u,0u,NULL,0u,0,0u);
}

/* EXEC0092: strictly COUNTERFACTUAL PPUSTATUS values supplied by caller.
 * The original Mesen fresh-ROM transcript lacks CPU-cycle-addressed $2002
 * reads, so this is NOT an observed stock trace or a vblank emulator.
 * The sequence is consumed once and every further PPU read fails closed. */
Gk83Stop gk92_trace_reset_ppustatus_hypothesis(const char *rom,
 const uint8_t *hypothetical_status_reads,uint32_t read_count,
 uint32_t max_steps,Gk83Trace *out){
 return trace_impl(rom,max_steps,out,0,0u,0u,1,
                   hypothetical_status_reads,read_count,0,0,0u,0u,0u,NULL,0u,0,0u);
}

/* EXEC0097: one expressly counterfactual NMI at source-loop $C460 after
 * zero-filled work RAM and the first PPUSTATUS warmup sequence. */
Gk83Stop gk97_trace_reset_one_hypothetical_nmi(const char *rom,
 const uint8_t *status_reads,uint32_t status_count,
 uint32_t max_steps,Gk83Trace *out){
 if(!rom||!*rom||!out||status_count<6u)return GK83_INVALID;
 return trace_impl(rom,max_steps,out,0,0u,0u,1,status_reads,status_count,1,0,0u,0u,0u,NULL,0u,0,0u);
}

/* EXEC0098: explicitly provided controller input, NOT an original Mesen
 * controller-event trace. The $4016 serial register is fully separate from
 * CPU RAM/SaveRAM. Unknown non-controller I/O still stops fail-closed. */
Gk83Stop gk98_trace_reset_controller_hypothesis(const char *rom,
 const uint8_t *status_reads,uint32_t status_count,
 uint8_t buttons,uint8_t port2_buttons,uint8_t upper_bus_bits,uint32_t max_steps,Gk83Trace *out){
 if(!rom||!*rom||!out||status_count<6u||!status_reads||
    (upper_bus_bits&1u))return GK83_INVALID;
 return trace_impl(rom,max_steps,out,0,0u,0u,1,status_reads,status_count,1,
                   1,buttons,port2_buttons,upper_bus_bits,NULL,0u,0,0u);
}

/* EXEC0102: multiple caller-scripted controller frames. These use a known
 * cartridge and actual implemented CPU source but deliberately ASSUME the
 * selected NMI instants; this does not simulate PPU frame cycles. */
Gk83Stop gk102_trace_scripted_controller_frames(const char *rom,
 const uint8_t *status_reads,uint32_t status_count,
 const uint8_t *controller_frames,uint32_t frame_count,
 uint32_t max_steps,Gk83Trace *out){
 if(!rom||!*rom||!out||!controller_frames||frame_count==0u||
    frame_count>32u||!status_reads||status_count<6u)return GK83_INVALID;
 return trace_impl(rom,max_steps,out,0,0u,0u,1,status_reads,status_count,
                   0,1,0u,0u,0u,controller_frames,frame_count,0,0u);
}

/* EXEC0103: PPU clock-scheduled *hypothesis* from explicit NTSC frame origin.
 * No artificial $C460/$C486 NMI injection or hand-fed $2002 values.
 * Original ROM remains private. Rendering/DMA and unknown peripherals gate. */
Gk83Stop gk103_trace_original_ppu_schedule(const char *rom,uint8_t phase,
 uint32_t max_steps,Gk83Trace *out){
 if(!rom||!*rom||!out||phase>2u)return GK83_INVALID;
 return trace_impl(rom,max_steps,out,0,0u,0u,1,NULL,0u,0,0,0u,0u,0u,
                   NULL,0u,1,phase);
}
