exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
exec((ROOT/'batches/runtime/event_helpers.py').read_text())
RESULTS=[]
def keep(label):
 s=snap(label);d={k:s[k] for k in ['label','year','season','gold','food','troops','reserve','eligible_percent','orders','abilities']};d.update({n:word(a) for n,a in {'town':0x6de9,'masons':0x6ded,'artisans':0x6e07,'farmers':0x6e0b,'morale':0x6e0f,'economy':0x6de7,'tax':0x6e11,'tax_used':0x6e13,'food_history':0x6e09,'item_history':0x6e05}.items()});RESULTS.append(d);(OUT/'levy_year_results.json').write_text(json.dumps(RESULTS,indent=2));return d
def levy(label):
 write(0x8a,[1]);write(0x84,(8).to_bytes(6,'big'));menu(0);press('down',5,80);press('a',5,500);number(1);snap(label+'_prompt');press('a',5,400);press('a',5,300);write(0x8a,[0]);s=keep(label);assert s['tax_used']==1 and s['orders']==2
restore();keep('levy_year_start');levy('levy_year_first_levy')
for season,name in [(1,'summer1175'),(2,'fall1175'),(3,'winter1175'),(0,'spring1176')]:
 pass_start();step(3600);s=keep('levy_year_'+name)
 call('state.checkpoint',path=str(OUT/('levy_year_'+name+'.mss')));step(2)
 if season==3 and s['season']==2 and s['orders']==0:
  snap('levy_year_country2_treaty_before');press('down',5,80);press('a',5,300);step(3600);s=keep('levy_year_winter1175_player')
 assert s['season']==season
 if s['orders']!=3:raise RuntimeError('UI event pending '+name)
 if season==2:
  assert s['tax_used']==0
  levy('levy_year_autumn_reuse')
 if season==0:
  assert s['tax_used']==1
  menu(0);press('down',5,80);bp=create_bp({'memoryType':'nesPrgRom','address':0x3eb5d,'length':1,'access':'read','cpuType':'nes'});call('input.set',buttons={'a':True});until_hit(bp,'levy_final_spring_denial',False);call('input.set',buttons={});assert word(word(4)+11)==0x9dc0;delete_bp(bp);step(31);snap('levy_year_spring_denial_message');step(300);press('a',5,300);after=keep('levy_year_spring_repeat_denied')
  assert after['orders']==3 and after['gold']==s['gold'] and after['abilities']==s['abilities'] and after['tax_used']==1
RESULTS={'seasons':4,'passed':True}
