import sys, os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import gk
# 6502 disassembler (official opcodes)
OPS={}
def _add(op,name,mode): OPS[op]=(name,mode)
modes={'imp':1,'acc':1,'imm':2,'zp':2,'zpx':2,'zpy':2,'abs':3,'abx':3,'aby':3,'ind':3,'izx':2,'izy':2,'rel':2}
g1=['ORA','AND','EOR','ADC','STA','LDA','CMP','SBC']; m1=['izx','zp','imm','abs','izy','zpx','aby','abx']
for a,n in enumerate(g1):
    for b,m in enumerate(m1):
        if n=='STA' and m=='imm': continue
        _add(a<<5|b<<2|1,n,m)
g2=['ASL','ROL','LSR','ROR','STX','LDX','DEC','INC']; m2=['imm','zp','acc','abs',None,'zpx','','abx']
for a,n in enumerate(g2):
    for b,m in enumerate(m2):
        if m is None or m=='': continue
        mm=m
        if n in('STX','LDX') and m=='zpx': mm='zpy'
        if n=='LDX' and m=='abx': mm='aby'
        if n=='STX' and m in('imm','acc','abx'): continue
        if n in ('ASL','ROL','LSR','ROR','DEC','INC') and m=='imm': continue
        _add(a<<5|b<<2|2,n,mm)
for op,n in [(0x24,'BIT'),(0x2C,'BIT')]: _add(op,n,'zp' if op==0x24 else 'abs')
_add(0x4C,'JMP','abs'); _add(0x6C,'JMP','ind')
for op,n,m in [(0x84,'STY','zp'),(0x94,'STY','zpx'),(0x8C,'STY','abs'),(0xA0,'LDY','imm'),(0xA4,'LDY','zp'),(0xB4,'LDY','zpx'),(0xAC,'LDY','abs'),(0xBC,'LDY','abx'),
               (0xC0,'CPY','imm'),(0xC4,'CPY','zp'),(0xCC,'CPY','abs'),(0xE0,'CPX','imm'),(0xE4,'CPX','zp'),(0xEC,'CPX','abs')]: _add(op,n,m)
for op,n in [(0x10,'BPL'),(0x30,'BMI'),(0x50,'BVC'),(0x70,'BVS'),(0x90,'BCC'),(0xB0,'BCS'),(0xD0,'BNE'),(0xF0,'BEQ')]: _add(op,n,'rel')
for op,n in [(0x00,'BRK'),(0x40,'RTI'),(0x60,'RTS'),(0x08,'PHP'),(0x28,'PLP'),(0x48,'PHA'),(0x68,'PLA'),(0x88,'DEY'),(0xA8,'TAY'),(0xC8,'INY'),(0xE8,'INX'),
             (0x18,'CLC'),(0x38,'SEC'),(0x58,'CLI'),(0x78,'SEI'),(0x98,'TYA'),(0xB8,'CLV'),(0xD8,'CLD'),(0xF8,'SED'),(0x8A,'TXA'),(0x9A,'TXS'),(0xAA,'TAX'),(0xBA,'TSX'),(0xCA,'DEX'),(0xEA,'NOP')]: _add(op,n,'imp')
_add(0x20,'JSR','abs')
def fmt(mode,v,pc):
    if mode in('imp',): return ''
    if mode=='acc': return 'A'
    if mode=='imm': return '#$%02X'%v
    if mode=='zp': return '$%02X'%v
    if mode=='zpx': return '$%02X,X'%v
    if mode=='zpy': return '$%02X,Y'%v
    if mode=='abs': return '$%04X'%v
    if mode=='abx': return '$%04X,X'%v
    if mode=='aby': return '$%04X,Y'%v
    if mode=='ind': return '($%04X)'%v
    if mode=='izx': return '($%02X,X)'%v
    if mode=='izy': return '($%02X),Y'%v
    if mode=='rel': return '$%04X'%((pc+2+(v-256 if v>127 else v))&0xFFFF)
def dis(rd, pc, n=30, stop_rts=False):
    """rd(addr)->byte. returns list of strings"""
    out=[]
    for _ in range(n):
        op=rd(pc)
        if op not in OPS: out.append("%04X: %02X        .db $%02X"%(pc,op,op)); pc+=1; continue
        name,mode=OPS[op]; ln=modes[mode]
        v=0
        if ln==2: v=rd(pc+1)
        elif ln==3: v=rd(pc+1)|rd(pc+2)<<8
        raw=" ".join("%02X"%rd(pc+i) for i in range(ln))
        out.append("%04X: %-8s  %s %s"%(pc,raw,name,fmt(mode,v,pc)))
        pc=(pc+ln)&0xFFFF
        if stop_rts and name in('RTS','RTI'): break
    return out
def bank_reader(c,bank):
    def rd(a):
        if a>=0xC000: return gk.lib.gk_peek_bank(15,a)
        return gk.lib.gk_peek_bank(bank,a)
    return rd
