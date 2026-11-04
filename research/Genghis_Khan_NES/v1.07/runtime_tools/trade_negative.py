exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
RESULTS=[]
cases=[('judgment4',{0x65cf:4},'entry'),('china_absent',{0x6e18:0},'entry'),('hire_no_gold',{0x6e0d:0},'hire'),('hire_population999',{0x6de5:957},'hire'),('buy_stock999',{0x6def:999},'buy'),('sell_gold999',{0x6e0d:999},'sell'),('buy_cancel',{},'cancel'),('buy_zero',{},'zero')]
for name,changes,kind in cases:
 restore();all_offers()
 for a,v in changes.items():wword(a,v)
 before=snap('trade_negative_'+name+'_before');menu(4)
 if kind!='entry':
  press('a',5,180)
  if kind=='hire':keys(['down']*2);press('a',5,500)
  else:
   if kind in ('buy','cancel','zero'):press('down',5,60)
   press('a',5,600);item(0)
   if kind=='cancel':press('b',5,300)
   elif kind=='zero':press('a',5,300)
 elif name=='china_absent':press('a',5,500)
 out=snap('trade_negative_'+name+'_result')
 x={'case':name,'before':before,'after':out,'stock':word(0x6def)};RESULTS.append(x);(OUT/'trade_negative_results.json').write_text(json.dumps(RESULTS,indent=2))
 assert out['gold']==before['gold'] and out['orders']==before['orders'] and out['abilities']==before['abilities'],name
RESULTS={'cases':len(RESULTS),'passed':True}
