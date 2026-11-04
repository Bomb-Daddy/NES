exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
RESULTS=[]
for q in [1,9]:
 restore();write(0x8a,[1]);write(0x84,(8).to_bytes(6,'big'));snap(f'levy_{q}_before');menu(0);press('down',5,80);press('a',5,500);number(q);snap(f'levy_{q}_prompt');press('a',5,400);press('a',5,300);out=snap(f'levy_{q}_result')
 x={'case':f'levy{q}','gold':out['gold'],'food':out['food'],'troops':out['troops'],'morale':word(0x6e0f),'economy':word(0x6de7),'used':word(0x6e13),'reserve':out['reserve'],'orders':out['orders'],'abilities':out['abilities']};RESULTS.append(x)
 assert out['gold']==439+q and out['orders']==2 and word(0x6e13)==1 and word(0x6de7)==185-q
 (OUT/'extra_tax_results.json').write_text(json.dumps(RESULTS,indent=2))
RESULTS={'cases':len(RESULTS),'passed':True}
