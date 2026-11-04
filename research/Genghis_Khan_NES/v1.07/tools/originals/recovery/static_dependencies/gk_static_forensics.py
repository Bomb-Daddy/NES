#!/usr/bin/env python3
"""Reproducible static examination of the user's US NES Genghis Khan ROM.
Runs no emulator; does not execute ROM code, reconstruct dynamic mapper state or prove command outcomes.
"""
from pathlib import Path
from collections import Counter,defaultdict
import csv,hashlib,json,struct,sys,unittest,re
BASE=Path('/mnt/data')
ROM=BASE/'Genghis Khan (USA).nes'
OUT=BASE/'gk_static_v1_02'; OUT.mkdir(exist_ok=True)
EXPECTED='ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3'
raw=ROM.read_bytes(); header=raw[:16]; prg=raw[16:]; assert len(prg)==256*1024
bank15=prg[-0x4000:]
def fixed(pc):
    if not 0xc000<=pc<0x10000:raise ValueError(hex(pc))
    return bank15[pc-0xc000]
def fixedword(pc):return fixed(pc)|(fixed((pc+1)&0xffff)<<8)
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

# Capture handler table exactly as referenced by the interpreter's LDA $F788,X / $F888,X.
targets=[]
for code in range(256):
 low=fixed(0xF788+code); high=fixed(0xF888+code);target=low+(high<<8)
 targets.append({'opcode_dec':code,'opcode_hex':f'{code:02X}','target_hex':hx(target),'target':target,'headered_file_offset_hex':hex(offset(target)) if target>=0xc000 else '', 'target_first_bytes':bytes(fixed(target+i) for i in range(4)).hex(' ') if target>=0xc000 else ''})
with (OUT/'vm_handler_table_256.csv').open('w',newline='') as fp:
 w=csv.DictWriter(fp,fieldnames=list(targets[0]));w.writeheader();w.writerows(targets)
counts=Counter(x['target'] for x in targets)
family=[]
for i in range(256):
 t=targets[i]['target'];family.append((i,t))
groups=[];beg=0
for i in range(1,257):
 if i==256 or family[i][1]!=family[beg][1]:
  if i-beg>=2:groups.append({'opcode_start':f'{beg:02X}','opcode_end':f'{i-1:02X}','handler':hx(family[beg][1]),'count':i-beg})
  beg=i
with (OUT/'vm_handler_alias_groups.json').open('w') as f:json.dump(groups,f,indent=2)

# CPU-address mapping of each 16KiB PRG bank: the first 15 are switchable to $8000- $BFFF in MMC1 PRG mode 3;
# bank15 is assumed fixed at $C000-$FFFF. This is a CONDITIONAL address mapping, not a measured active mapper register.
banks=[]
for i in range(16):
 blk=prg[i*16384:(i+1)*16384]
 banks.append({'bank':i,'prg_start':hex(i*16384),'headered_start':hex(16+i*16384),'headered_end_exclusive':hex(16+(i+1)*16384),'sha256':hashlib.sha256(blk).hexdigest(),'zero_bytes':blk.count(0),'ff_bytes':blk.count(0xff),'mapped_cpu_window_if_mode3':('$C000-$FFFF fixed' if i==15 else '$8000-$BFFF switchable')})
with (OUT/'prg_bank_manifest.json').open('w') as f:json.dump(banks,f,indent=2)

# Find all JSR/JMP byte sequences referencing the interpreter or write handlers. These are candidate byte patterns,
# not proven instructions; data can contain arbitrary 3-byte sequences.
refs={}
for adr in (0xEFC9,0xF420,0xF422,0xF1C8,0xF1CA,0xF43D,0xF788,0xF888):
 rec=[]
 for bank in range(16):
  blk=prg[bank*16384:(bank+1)*16384]
  for opcode in (0x20,0x4c,0x6c,0xad,0xbd,0xb9,0x8d,0x9d,0x99):
   needle=bytes([opcode,adr&255,adr>>8]);start=0
   while True:
    off=blk.find(needle,start)
    if off<0:break
    rec.append({'bank':bank,'bank_offset':off,'headered_rom_offset_hex':hex(16+bank*16384+off),'cpu_address_if_mode3':hx((0xc000 if bank==15 else 0x8000)+off),'opcode_hex':hex(opcode)})
    start=off+1
 refs[hx(adr)]=rec
with (OUT/'candidate_absolute_references.json').open('w') as f:json.dump(refs,f,indent=2)

# Inspect exact source snippets and code around VM interpreter and memory access helpers. Restrict to confirmed linear entrypoints.
sections={'VM_INTERPRETER_EFC9':(0xEFC9,13),'ZERO_PAGE_VM_STORE_F420':(0xF400,33),'STORED_ORDER_F1CA':(0xF198,34),
 'SCRIPT_OPERAND_POINTER_F43D':(0xF43D,25),'VM_BASIC_HANDLERS_EFE1':(0xEFE1,34),'VM_RETURN_F6F4':(0xF6E8,22),
 'RESET_VECTOR_C000':(fixedword(0xfffc),35)}
with (OUT/'fixed_bank_6502_disassembly.txt').open('w') as f:
 for name,(pc,count) in sections.items():
  f.write(f'### {name}\n{lines(pc,count)}\n\n')

# Search for little-endian address operands in each 16 KiB ROM bank.
# Matches can also be graphics/data/unrelated instruction sequences and must NOT be treated as proved code references.
known_state_addresses={
 'orders':0x7B36,'market_rate':0x7A8A,'tax_rate':0x6E11,'tax_used':0x6E13,
 'morale':0x6E0F,'gold':0x6E0D,'food':0x6E03,'town':0x6DE9,
 'troops':0x6DE5,'artisan':0x6E07,'masons':0x6DED,'food_workers':0x6E0B,
 'draft_eligible':0x6E15,'economy':0x6DE7,'skill':0x6DE3}
operand_candidates=[]
for name,addr in known_state_addresses.items():
 needle=addr.to_bytes(2,'little')
 for bank in range(16):
  blob=prg[bank*0x4000:(bank+1)*0x4000]
  start=0
  while True:
   z=blob.find(needle,start)
   if z<0:break
   prior=blob[z-1] if z>0 else None
   operand_candidates.append({
    'field':name,'address':hx(addr),'bank':bank,'bank_offset_hex':hex(z),
    'headered_file_offset_hex':hex(16+bank*0x4000+z),
    'preceding_byte_hex':f'{prior:02X}' if prior is not None else '',
    'preceding_byte_vm_target':hx(targets[prior]['target']) if prior is not None else '',
    'previous_8_bytes_hex':blob[max(0,z-8):z].hex(' '),
    'next_8_bytes_hex':blob[z+2:z+10].hex(' '),
    'candidate_vm_interpretation':('A4 absolute 16-bit memory READ' if prior==0xA4 else 'A8 absolute 16-bit memory WRITE' if prior==0xA8 else 'UNDECODED OR UNALIGNED'),
    'evidence':'CANDIDATE_RAW_BYTES_NOT_CONFIRMED_EXECUTED_OPERAND'})
   start=z+1
with (OUT/'candidate_state_address_operands.csv').open('w',newline='') as fp:
 w=csv.DictWriter(fp,fieldnames=list(operand_candidates[0]));w.writeheader();w.writerows(operand_candidates)

# Static-only technical facts and caveats.
summary={'sha256':hashlib.sha256(raw).hexdigest(),'rom_size':len(raw),'header_hex':header.hex(), 'prg_bytes':len(prg),
'prg_banks_16k':len(prg)//16384,'chr_rom_bytes':0,'mapper':((header[6]>>4)|(header[7]&0xf0)),
'vectors_if_last_bank_fixed':{label:hx(fixedword(pc)) for label,pc in [('NMI',0xfffa),('RESET',0xfffc),('IRQ',0xfffe)]},
'version_signature_at_ffe0':''.join(chr(fixed(a)) if 32<=fixed(a)<=126 else '?' for a in range(0xffe0,0xfff0)),
'vm_interpreter_efc9_bytes':bytes(fixed(a) for a in range(0xEFC9,0xEFE1)).hex(' '),
'dispatch_table_low':hx(0xF788),'dispatch_table_high':hx(0xF888),'dispatch_opcode_count':len(targets),'distinct_dispatch_targets':len(counts),
'dispatch_targets_outside_fixed_bank':sum(not 0xc000<=a<=0xffff for a in counts),
'most_common_dispatch_targets':[{'target':hx(k),'count':v} for k,v in counts.most_common(10)],'handler_alias_runs':groups,
'vm_decoded_opcode_a4':{'handler':hx(targets[0xA4]['target']),'candidate_semantics':'Read 16-bit value at absolute address following A4 into VM $08:$09; branch from $F737 always CLC on inspected path'},
'vm_decoded_opcode_a8':{'handler':hx(targets[0xA8]['target']),'candidate_semantics':'Write VM $08:$09 to absolute address following A8; $F737 reads absolute address into $00:$01'},
'code_store_snippets':{'F420':bytes(fixed(a) for a in range(0xF420,0xF425)).hex(' '),'F1C8':bytes(fixed(a) for a in range(0xF1C8,0xF1CD)).hex(' '),'F43D':bytes(fixed(a) for a in range(0xF43D,0xF44F)).hex(' ')},
'candidate_references_count':{k:len(v) for k,v in refs.items()},
'address_operand_candidates_count':{key:sum(x['field']==key for x in operand_candidates) for key in known_state_addresses},
'disclaimer':'Static ROM-byte findings only; no emulated behavior, active MMC1 bank state, full bytecode semantics, or game-command formulas established.'}
(OUT/'static_summary.json').write_text(json.dumps(summary,indent=2))

# Tests that would fail if the table were mis-offset, byte order reversed, bank confused, or source bytes mutated.
class StaticChecks(unittest.TestCase):
 def test_sha(self):self.assertEqual(summary['sha256'],EXPECTED)
 def test_file_length(self):self.assertEqual(len(raw),262160)
 def test_ines(self):self.assertEqual(header[:4],b'NES\x1a')
 def test_mapper(self):self.assertEqual(summary['mapper'],1)
 def test_prg_count(self):self.assertEqual(summary['prg_banks_16k'],16)
 def test_chr_ram(self):self.assertEqual((header[5]),0)
 def test_reset_vector(self):self.assertEqual(summary['vectors_if_last_bank_fixed']['RESET'],'$C000')
 def test_dispatch_load_low_opcode(self):self.assertEqual(bytes(fixed(a) for a in range(0xEFD4,0xEFD7)),b'\xbd\x88\xf7')
 def test_dispatch_load_high_opcode(self):self.assertEqual(bytes(fixed(a) for a in range(0xEFD9,0xEFDC)),b'\xbd\x88\xf8')
 def test_dispatch_jump_indirect(self):self.assertEqual(bytes(fixed(a) for a in range(0xEFDE,0xEFE1)),b'\x6c\x00\x00')
 def test_dispatch_tables_256(self):self.assertEqual(len(targets),256)
 def test_dispatch_targets_fixed(self):self.assertEqual(sum(0xc000<=a<=0xffff for a in counts),len(counts))
 def test_dispatch_unique_target_count(self):self.assertEqual(len(counts),163)
 def test_dispatch_alias_first_16(self):self.assertEqual([x['target'] for x in targets[:16]],[0xefe1+4*i for i in range(12)]+[0xf032,0xf02e,0xf02a,0xf026])
 def test_store_F420(self):self.assertEqual(bytes(fixed(a) for a in (0xF420,0xF421)),b'\x91\x0c')
 def test_after_store_F422(self):self.assertEqual(bytes(fixed(a) for a in range(0xF422,0xF425)),b'\x4c\xc9\xef')
 def test_order_F1C8(self):self.assertEqual(bytes(fixed(a) for a in (0xF1C8,0xF1C9)),b'\x91\x00')
 def test_F1CA_not_subtraction(self):self.assertEqual(fixed(0xF1CA),0xC8)
 def test_F43D_pointer_read(self):self.assertEqual(bytes(fixed(a) for a in range(0xF43D,0xF447)),b'\xb1\x02\x85\x0c\xc8\xb1\x02\x85\x0d\x18')
 def test_fileoffset_F420(self):self.assertEqual(offset(0xF420),0x3F430)
 def test_rom_bank_manifest(self):self.assertEqual(len(banks),16)
 def test_rom_banks_exactly_cover_PRG(self):self.assertEqual(sum(16384 for b in banks),len(prg))
 def test_table_written(self):
  with (OUT/'vm_handler_table_256.csv').open() as f:self.assertEqual(len(list(csv.DictReader(f))),256)
 def test_order_address_literal_count(self):self.assertEqual(sum(x['field']=='orders' for x in operand_candidates),22)
 def test_market_rate_address_literal_count(self):self.assertEqual(sum(x['field']=='market_rate' for x in operand_candidates),7)
 def test_no_false_tax_pointer_claim(self):self.assertEqual(sum(x['field']=='tax_rate' for x in operand_candidates),0)
 def test_market_candidate_preceding_known_vm(self):self.assertTrue(any(x['field']=='market_rate' and x['preceding_byte_hex']=='A4' for x in operand_candidates))
 def test_vm_a4_target(self):self.assertEqual(targets[0xA4]['target'],0xF1A5)
 def test_vm_a8_target(self):self.assertEqual(targets[0xA8]['target'],0xF1D7)
 def test_vm_a4_helper(self):self.assertEqual(bytes(fixed(a) for a in range(0xF1A5,0xF1A9)),b'\x20\x37\xf7\x90')
 def test_vm_a8_helper(self):self.assertEqual(bytes(fixed(a) for a in range(0xF1D7,0xF1DB)),b'\x20\x37\xf7\x90')
 def test_vm_absolute_pointer_reader(self):self.assertEqual(bytes(fixed(a) for a in range(0xF737,0xF73F)),b'\xb1\x06\x85\x00\xc8\xb1\x06\x85')
 def test_vm_a4_order_literal(self):self.assertTrue(any(prg[b*0x4000:b*0x4000+0x4000].find(b'\xa4\x36\x7b')>=0 for b in range(16)))
 def test_vm_a8_order_literal(self):self.assertTrue(any(prg[b*0x4000:b*0x4000+0x4000].find(b'\xa8\x36\x7b')>=0 for b in range(16)))
 def test_vm_a4_market_rate_literal(self):self.assertTrue(any(prg[b*0x4000:b*0x4000+0x4000].find(b'\xa4\x8a\x7a')>=0 for b in range(16)))
 def test_mutated_dispatch_low_negative_control(self):
  corrupted=bytearray(bank15); corrupted[0xF788-0xC000]^=0x80
  actual=corrupted[0xF788-0xC000] | (fixed(0xF888)<<8)
  self.assertNotEqual(actual,targets[0]['target'])
 def test_reversed_dispatch_tables_negative_control(self):
  reversed_word=(fixed(0xF788)<<8)|fixed(0xF888)
  self.assertNotEqual(reversed_word,targets[0]['target'])
 def test_wrong_bank_negative_control(self):
  wrong_bank=prg[14*0x4000:15*0x4000]
  self.assertNotEqual(wrong_bank[0xF420-0xC000:0xF422-0xC000],b'\x91\x0c')
 def test_no_rom_copied(self):self.assertFalse(any(p.suffix.lower() in ('.nes','.sfc','.smc') for p in OUT.iterdir()))

if __name__=='__main__':
 suite=unittest.defaultTestLoader.loadTestsFromTestCase(StaticChecks)
 result=unittest.TextTestRunner(stream=sys.stdout,verbosity=2).run(suite)
 sys.exit(0 if result.wasSuccessful() else 1)
