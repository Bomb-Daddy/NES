exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
exec((ROOT/'batches/runtime/event_helpers.py').read_text())
RESULTS=json.loads((OUT/'levy_year_results.json').read_text())
def keep(label):
 s=snap(label);d={k:s[k] for k in ['label','year','season','gold','food','troops','reserve','eligible_percent','orders','abilities']};d.update({n:word(a) for n,a in {'town':0x6de9,'masons':0x6ded,'artisans':0x6e07,'farmers':0x6e0b,'morale':0x6e0f,'economy':0x6de7,'tax':0x6e11,'tax_used':0x6e13,'food_history':0x6e09,'item_history':0x6e05}.items()});RESULTS.append(d);(OUT/'levy_year_completed_ledger.json').write_text(json.dumps(RESULTS,indent=2));return d
restore('levy_year_winter1175');snap('levy_year_fall_country2_treaty');press('down',5,80);press('a',5,300);step(3600);s=keep('levy_year_winter1175_player');assert s['season']==3 and s['orders']==3 and s['tax_used']==1
pass_start();step(3600);s=keep('levy_year_spring1176_player');call('state.checkpoint',path=str(OUT/'levy_year_spring1176_player.mss'));step(2)
assert s['season']==0 and s['year']==1176 and s['orders']==3 and s['tax_used']==1
menu(0);press('down',5,80);bp=create_bp({'memoryType':'nesPrgRom','address':0x3eb5d,'length':1,'access':'read','cpuType':'nes'});call('input.set',buttons={'a':True});until_hit(bp,'levy_spring_deny_call',False);call('input.set',buttons={});message=word(word(4)+11);delete_bp(bp);step(180);snap('levy_year_spring_denial_message');press('a',5,400);after=keep('levy_year_spring_repeat_denied')
assert message==0x9dc0 and after['orders']==3 and after['gold']==s['gold'] and after['abilities']==s['abilities'] and after['tax_used']==1
RESULTS={'seasons':4,'autumn_reuse':True,'spring_does_not_reset':True,'passed':True};(OUT/'levy_year_final_result.json').write_text(json.dumps(RESULTS,indent=2))
