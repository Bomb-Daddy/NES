#!/usr/bin/env python3
"""Original NES scenario-loader instruction replay. CPU only; deterministic mapper-1 PRG mode3 adapter; no emulator/UI claims."""
from pathlib import Path
import sys,importlib.util,hashlib,json,csv,os
sys.dont_write_bytecode=True
ROOT=Path(__file__).resolve().parent;BASE=ROOT.parents[1]
# Read existing validated CPU harness; prevent imported bytecode-cache writes outside this task's directory.
sys.path.insert(0,str(BASE/'batches/seasonal_source'))
from source_cpu import Cpu as BaseCpu
from inspect_source import prg,raw
ROM_SHA='ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3'
NAMES=['Fur','Silk','Jewels','Metals','Medicines','Woodworks','Ceramics','Glass','Textiles','Perfume','Food']
FIELDS={'Arms':0x14,'Skill':0x16,'Troops':0x18,'Economy':0x1a,'Town':0x1c,'Defense':0x1e,'Masons':0x20,'Commodity_history':0x38,'Artisans':0x3a,'Food_history':0x3c,'Food_workers':0x3e,'Gold':0x40,'Morale':0x42,'Tax':0x44,'Extra_tax_used':0x46,'Undrafted_residents':0x49}
class LoaderCpu(BaseCpu):
 def __init__(self,bank=0):
  super().__init__(bank);self.m[0x73]=bank;self.prg_bank=bank;self.serial=0;self.serial_count=0;self.bank_events=[]
 def step(self):
  # Execute STA $FFF0 exactly as a 6502 write; apply MMC1's five serial PRG bits instead of mutating ROM.
  if self.m[self.pc:self.pc+3]==bytes.fromhex('8d f0 ff'):
   pc=self.pc;self.pc=(self.pc+3)&65535;self.count+=1
   if self.a&0x80:self.serial=self.serial_count=0
   else:
    self.serial|=(self.a&1)<<self.serial_count;self.serial_count+=1
    if self.serial_count==5:
     self.prg_bank=self.serial&15;self.m[0x8000:0xc000]=prg[self.prg_bank*16384:(self.prg_bank+1)*16384]
     self.bank_events.append({'cpu_pc':hex(pc),'step':self.count,'bank':self.prg_bank,'serial_value':self.serial})
     self.serial=self.serial_count=0
   return
  super().step()

def loader(scenario):
 if scenario not in (1,2):raise ValueError('Scenario selector must be1 or2')
 c=LoaderCpu(0);c.setword(2,0x1800);c.setword(4,0x700);c.setword(6,0x97dd);c.setword(0x70b,scenario)
 c.run(0x9851,maxsteps=1000000,trace=True)
 return c

def string(m,p,n):return bytes(m[p:p+n]).split(b'\0')[0].decode('ascii').replace('^',' ')
def snapshot(scenario,c):
 count=c.word(0x65b9);src=0x8004 if scenario==1 else 0x96b4;rows=[]
 for i in range(count):
  p=0x6dcd+83*i;region=0x7692+12*i;spec=c.m[region]
  row={'scenario_selector':scenario,'scenario':'Mongol Conquest' if scenario==1 else 'World Conquest','stage':'LOADED_TEMPLATE_BEFORE_SETUP_AND_SEASONAL_PROCESSING','country_id_display':i+1,'country_index_zero_based':i,'country_name':string(c.m,0x63ee+17*i,17),'initial_lord_name':string(c.m,c.word(p+0x51),13),'specialty_index':spec,'specialty_name':NAMES[spec],'country_sram_base':f'${p:04X}','country_rom_file_offset':f'0x{16+7*16384+(src+(p-0x63ea)-0x8000):06X}','specialty_sram_address':f'${region:04X}','specialty_rom_file_offset':f'0x{16+7*16384+(src+(region-0x63ea)-0x8000):06X}','name_rom_file_offset':f'0x{16+7*16384+(src+(0x63ee+17*i-0x63ea)-0x8000):06X}'}
  for j,n in enumerate(NAMES):row[n]=c.word(p+0x22+2*j)
  for n,off in FIELDS.items():row[n]=c.word(p+off)
  row['Draft_eligibility_percent']=c.m[p+0x48];row['Drafted_total']=sum(row[n] for n in ['Troops','Town','Masons','Artisans','Food_workers']);row['Combined_population']=row['Drafted_total']+row['Undrafted_residents'];row['Eligible_draft_count']=row['Undrafted_residents']*row['Draft_eligibility_percent']//100
  rows.append(row)
 return rows
if __name__=='__main__':
 allrows=[];proof=[]
 for sc in [1,2]:
  c=loader(sc);rows=snapshot(sc,c);allrows+=rows
  proof.append({'scenario':sc,'count':len(rows),'source_year':c.word(0x65bb),'source_season':c.word(0x65bd),'steps':c.count,'bank_events':c.bank_events,'sram_sha256':hashlib.sha256(c.m[0x6000:0x8000]).hexdigest(),'template_bytes_sha256':hashlib.sha256(c.m[0x63ea:0x79d4]).hexdigest(),'trace':c.vmtrace,'writes':c.writes})
  print(sc,len(rows),c.word(0x65bb),c.word(0x65bd),c.count,c.bank_events)
 (ROOT/'scenario_economy_data.json').write_text(json.dumps(allrows,indent=2)+'\n')
 with (ROOT/'scenario_economy_data.csv').open('w',newline='') as f:
  w=csv.DictWriter(f,fieldnames=allrows[0].keys());w.writeheader();w.writerows(allrows)
 (ROOT/'scenario_loader_replay.json').write_text(json.dumps(proof,indent=2)+'\n')
