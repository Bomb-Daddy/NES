exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
RESULTS=[]
for case,changes in [('control',{}),('town10',{0x6de9:15,0x6e0b:17}),('masons10',{0x6ded:14,0x6e0b:17}),('artisans10',{0x6e07:16,0x6e0b:17})]:
 restore('winter1175_menu')
 for a,v in changes.items():wword(a,v)
 snap('annual_match_'+case+'_before');keys(['down']*12,50);press('a',5,180);press('a',5,600);step(1800);out=snap('annual_match_'+case+'_spring')
 fields={n:word(a) for n,a in {'economy':0x6de7,'defense':0x6deb,'morale':0x6e0f,'food_history':0x6e09,'item_history':0x6e05,'town':0x6de9,'masons':0x6ded,'artisans':0x6e07,'farmers':0x6e0b}.items()}
 RESULTS.append({'case':case,'interventions':{hex(a):v for a,v in changes.items()},'result':out,'fields':fields});(OUT/'matched_annual_results.json').write_text(json.dumps(RESULTS,indent=2))
 assert out['season']==0 and out['year']==1176
 assert out['food']==238 and out['gold']==(453 if case=='town10' else 449)
 assert fields['economy']==(187 if case=='town10' else 177)
 assert fields['defense']==(125 if case=='masons10' else 115)
 assert fields['item_history']==(22 if case=='artisans10' else 12)
 assert fields['food_history']==(54 if case=='control' else 44)
RESULTS={'cases':4,'passed':True}
