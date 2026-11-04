exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
RESULTS=[]
for name,occupation,q,confirm in [('artisans16',3,16,True),('town0',0,0,True),('town66',0,66,True),('town67_invalid',0,67,False),('artisans16_cancel',3,16,False),('town5_noop',0,5,True)]:
 restore();snap('assign_'+name+'_before');menu(1);press('a',5,600);snap('assign_'+name+'_edit');keys(['down']*occupation);press('a',5,500);number(q);snap('assign_'+name+'_prompt');press('a',5,400);snap('assign_'+name+'_staged');press('b',5,300);snap('assign_'+name+'_confirm')
 if not confirm:press('down',5,80)
 press('a',5,600);out=snap('assign_'+name+'_result');RESULTS.append({'case':name,'state':out,'town':word(0x6de9),'artisans':word(0x6e07),'morale':word(0x6e0f)})
 (OUT/'assign_probe_results.json').write_text(json.dumps(RESULTS,indent=2))
