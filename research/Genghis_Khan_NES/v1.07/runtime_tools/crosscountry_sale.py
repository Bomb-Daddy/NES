exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
restore('crosscountry_newbase');before=snap('crosscountry_sale_before');before_temujin=[word(0x65cd+2*i) for i in range(6)]
shop(1,0);snap('crosscountry_islam_quotes');item(2);number(1);snap('crosscountry_sale_prompt');press('a',5,400);press('a',5,300);after=snap('crosscountry_sale_after')
RESULTS={'active_country':word(0x7b34),'season':word(0x65bd),'rate':word(0x7a8a),'source_jewels':word(0x6df3),'destination_jewels':word(0x6e46),'source_gold':word(0x6e0d),'destination_gold':word(0x6e60),'orders':word(0x7b36),'temujin_before':before_temujin,'temujin_after':[word(0x65cd+2*i) for i in range(6)]}
(OUT/'crosscountry_sale_results.json').write_text(json.dumps(RESULTS,indent=2))
assert RESULTS['active_country']==1 and RESULTS['destination_jewels']==29 and RESULTS['destination_gold']==71 and RESULTS['orders']==2 and RESULTS['temujin_after']==[84,67,107,61,150,129],RESULTS
