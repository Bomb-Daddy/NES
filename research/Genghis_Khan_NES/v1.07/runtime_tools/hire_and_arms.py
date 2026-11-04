exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
RESULTS=[]
for q in [1,36]:
 restore();all_offers();snap(f'hire{q}_before');shop(0,2);number(q);snap(f'hire{q}_prompt');press('a',5,400);press('a',5,300);out=snap(f'hire{q}_result');x={'q':q,'state':out,'arms':word(0x6de1),'skill':word(0x6de3),'morale':word(0x6e0f)};RESULTS.append(x)
 assert out['gold']==439-12*q and out['troops']==61+q and out['reserve']==34 and out['orders']==2
 assert x['arms']==(103*110+q*100)//(103+q) and x['skill']==(103*124+q*100)//(103+q)
 assert (103*118+q*50)//(103+q)<=x['morale']<=(103*118+q*60)//(103+q)
restore();all_offers();shop(0,1);item(11);number(2);snap('dagger2_prompt');press('a',5,400);press('a',5,300);out=snap('dagger2_result');assert out['gold']==431 and word(0x6de1)==115 and out['orders']==2
RESULTS.append({'q':2,'item':'Daggers','gold':out['gold'],'arms':word(0x6de1),'orders':out['orders']});(OUT/'hire_arms_results.json').write_text(json.dumps(RESULTS,indent=2))
