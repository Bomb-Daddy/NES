#!/usr/bin/env python3
"""Replay original country setup; condition-labelled results, no first-player-turn claim."""
import sys
sys.dont_write_bytecode=True
import json,hashlib
import extract_scenarios as e
from test_scenarios import execute,stockvector
out=[]
for scenario,players in ((1,[0]),(2,[0,6,10,18])):
 for player in players:
  for difficulty in (1,3,5):
   c=e.loader(scenario);before=stockvector(c,scenario);before_fields=e.snapshot(scenario,c)
   c.setword(0x7a88,difficulty)
   for a in (0x79de,0x79e0,0x79e2,0x79e4):c.setword(a,0xff)
   c.setword(0x79de,player);seed=0x123456789abc;c.m[0x84:0x8a]=seed.to_bytes(6,'big')
   oldcount=c.count;c.writes=[];c.vmtrace=[]
   execute(c,0x9c52,0x9cf9,[scenario],bank=0)
   after=stockvector(c,scenario);assert before==after
   rows=e.snapshot(scenario,c)
   rec={'scenario':scenario,'player_country_id':player+1,'difficulty':difficulty,'entry_vm':'bank0:$9C52','stop_vm':'bank0:$9CF9','seed':f'{seed:012X}','end_seed':c.m[0x84:0x8a].hex(),'original_cpu_instructions':c.count-oldcount,'all_ten_specialty_stocks_unchanged':True,'country_fields':[{'country_id':r['country_id_display'],'Food_before':b['Food'],'Food_after':r['Food'],'Gold_before':b['Gold'],'Gold_after':r['Gold'],'Arms_before':b['Arms'],'Arms_after':r['Arms'],'Drafted_before':b['Drafted_total'],'Drafted_after':r['Drafted_total'],'Undrafted_before':b['Undrafted_residents'],'Undrafted_after':r['Undrafted_residents']} for b,r in zip(before_fields,rows)],'sram_sha256_after_country_setup':hashlib.sha256(c.m[0x6000:0x8000]).hexdigest()}
   out.append(rec);print(scenario,player+1,difficulty,rec['original_cpu_instructions'],'PASS stockwords invariant',flush=True)
(e.ROOT/'country_setup_replays.json').write_text(json.dumps(out,indent=2)+'\n')
print(f'PASS {len(out)} condition-labelled original country setup replays',flush=True)
