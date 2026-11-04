exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
restore();write(0x65fb,[0]);write(0x65fc,[101]);snap('crosscountry_before')
menu(7);keys(['down']*4);press('a',5,500);press('a',5,500);press('up',5,80);press('a',5,600);item(2);number(1);snap('crosscountry_jewels_prompt');press('a',5,400);press('a',5,300);out=snap('crosscountry_transferred')
assert word(0x6df3)==25 and word(0x6e46)==30 and out['orders']==2
menu(7);press('a',5,500);snap('crosscountry_base_destination');press('up',5,80);press('a',5,500);snap('crosscountry_relocated_commit');step(3000);out=snap('crosscountry_newbase_turn')
call('state.checkpoint',path=str(OUT/'crosscountry_newbase.mss'));step(2)
RESULTS={'active_country':word(0x7b34),'orders':word(0x7b36),'season':word(0x65bd),'country1_officer':word(0x6e1e),'country2_officer':word(0x6e71),'temujin_body':word(0x65d5),'destination_jewels':word(0x6e46),'destination_gold':word(0x6e60)}
(OUT/'transfer_relocation_results.json').write_text(json.dumps(RESULTS,indent=2))
