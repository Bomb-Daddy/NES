"""Bank-aware inspection helpers; private ROM supplied by GK_ROM."""
from pathlib import Path
import os,hashlib
ROM=Path(os.environ['GK_ROM'])
raw=ROM.read_bytes()
assert hashlib.sha256(raw).hexdigest()=='ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3'
prg=raw[16:];bank15=prg[-16384:]
def fixed(pc):return bank15[pc-0xc000]
def offset(pc):return 16+15*0x4000+pc-0xc000
def hx(a):return f'${a:04X}'
# Standard, legal 6502 opcodes; all unlisted bytes are treated as unknown, not decoded as CPU code.
O={}
def op(code,mnem,mode):O[code]=(mnem,mode)
for hexrow,entries in {
'00':'00 BRK imp,01 ORA indx,05 ORA zp,06 ASL zp,08 PHP imp,09 ORA imm,0A ASL acc,0D ORA abs,0E ASL abs',
'10':'10 BPL rel,11 ORA indy,15 ORA zpx,16 ASL zpx,18 CLC imp,19 ORA absy,1D ORA absx,1E ASL absx',
'20':'20 JSR abs,21 AND indx,24 BIT zp,25 AND zp,26 ROL zp,28 PLP imp,29 AND imm,2A ROL acc,2C BIT abs,2D AND abs,2E ROL abs',
'30':'30 BMI rel,31 AND indy,35 AND zpx,36 ROL zpx,38 SEC imp,39 AND absy,3D AND absx,3E ROL absx',
'40':'40 RTI imp,41 EOR indx,45 EOR zp,46 LSR zp,48 PHA imp,49 EOR imm,4A LSR acc,4C JMP abs,4D EOR abs,4E LSR abs',
'50':'50 BVC rel,51 EOR indy,55 EOR zpx,56 LSR zpx,58 CLI imp,59 EOR absy,5D EOR absx,5E LSR absx',
'60':'60 RTS imp,61 ADC indx,65 ADC zp,66 ROR zp,68 PLA imp,69 ADC imm,6A ROR acc,6C JMP ind,6D ADC abs,6E ROR abs',
'70':'70 BVS rel,71 ADC indy,75 ADC zpx,76 ROR zpx,78 SEI imp,79 ADC absy,7D ADC absx,7E ROR absx',
'80':'81 STA indx,84 STY zp,85 STA zp,86 STX zp,88 DEY imp,8A TXA imp,8C STY abs,8D STA abs,8E STX abs',
'90':'90 BCC rel,91 STA indy,94 STY zpx,95 STA zpx,96 STX zpy,98 TYA imp,99 STA absy,9A TXS imp,9D STA absx',
'A0':'A0 LDY imm,A1 LDA indx,A2 LDX imm,A4 LDY zp,A5 LDA zp,A6 LDX zp,A8 TAY imp,A9 LDA imm,AA TAX imp,AC LDY abs,AD LDA abs,AE LDX abs',
'B0':'B0 BCS rel,B1 LDA indy,B4 LDY zpx,B5 LDA zpx,B6 LDX zpy,B8 CLV imp,B9 LDA absy,BA TSX imp,BC LDY absx,BD LDA absx,BE LDX absy',
'C0':'C0 CPY imm,C1 CMP indx,C4 CPY zp,C5 CMP zp,C6 DEC zp,C8 INY imp,C9 CMP imm,CA DEX imp,CC CPY abs,CD CMP abs,CE DEC abs',
'D0':'D0 BNE rel,D1 CMP indy,D5 CMP zpx,D6 DEC zpx,D8 CLD imp,D9 CMP absy,DD CMP absx,DE DEC absx',
'E0':'E0 CPX imm,E1 SBC indx,E4 CPX zp,E5 SBC zp,E6 INC zp,E8 INX imp,E9 SBC imm,EA NOP imp,EC CPX abs,ED SBC abs,EE INC abs',
'F0':'F0 BEQ rel,F1 SBC indy,F5 SBC zpx,F6 INC zpx,F8 SED imp,F9 SBC absy,FD SBC absx,FE INC absx'
}.items():
 for token in entries.split(','):
  parts=token.strip().split();
  if len(parts)!=3: raise ValueError(repr((hexrow,token,parts)))
  op(int(parts[0],16),parts[1],parts[2])
MODLEN={'imp':1,'acc':1,'imm':2,'zp':2,'zpx':2,'zpy':2,'indx':2,'indy':2,'rel':2,'abs':3,'absx':3,'absy':3,'ind':3}

def decode(pc):
 c=fixed(pc);m,mode=O.get(c,('???','imp'));sz=MODLEN[mode];bs=bytes(fixed(pc+i) for i in range(sz));operand='';dest=None
 if sz==2:
  a=bs[1]
  if mode=='imm':operand=f'#$%02X'%a
  elif mode=='rel':dest=(pc+sz+(a-256 if a&128 else a))&0xffff;operand=hx(dest)
  elif mode in ('indx','indy'):operand=f'(${a:02X},'+('X)' if mode=='indx' else 'Y)')
  elif mode=='zp':operand=f'${a:02X}'
  elif mode=='zpx':operand=f'${a:02X},X'
  elif mode=='zpy':operand=f'${a:02X},Y'
 elif sz==3:
  a=bs[1]|(bs[2]<<8)
  if mode=='ind':operand=f'(${a:04X})'
  else:operand=hx(a)+(',X' if mode=='absx' else ',Y' if mode=='absy' else '')
  if mode=='abs':dest=a
 return {'pc':pc,'file_offset':offset(pc),'opcode':c,'mnemonic':m,'mode':mode,'size':sz,'bytes':bs.hex(' '),'operand':operand,'dest':dest,'text':f'{hx(pc)} [{offset(pc):06X}] {bs.hex(" "):<9} {m} {operand}'.rstrip()}

def lines(start,count):
 pc=start;out=[]
 for _ in range(count):
  if pc>0xffff:break
  ins=decode(pc);out.append(ins['text']);pc+=ins['size']
 return '\n'.join(out)


if __name__=='__main__':
 import sys
 print(lines(int(sys.argv[1],16),int(sys.argv[2]) if len(sys.argv)>2 else 50))
