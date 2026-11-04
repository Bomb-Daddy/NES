exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
import math,traceback
TABLE=[[10,16,32,18,14,11,13,8,20,18,10,20,80,100,40,60,150],[4,37,28,22,8,14,15,5,15,29,11,15,75,110,40,70,200],[8,25,25,10,15,7,18,20,31,38,11,25,82,90,35,65,250]]
RESULTS=[]
path=OUT/'trade_matrix_results.json'
for m in range(3):
 for mode,count in [(1,17),(0,11)]:
  for i in range(count):
   label=f'matrix_m{m}_{"buy" if mode else "sell"}_i{i:02d}'
   restore();all_offers();g=word(0x6e0d);stock=word(0x6def+2*i) if i<11 else None;ability=word(0x65cf)
   shop(m,mode)
   if i==0:snap(label+'_quotes')
   item(i);number(1);snap(label+'_prompt');press('a',5,180);press('a',5,180);out=snap(label+'_result')
   quote=TABLE[m][i]*201//100
   if not mode:quote=max(1,quote*90//100)
   expected_gold=g-(quote+9)//10 if mode else g+quote//10
   expected_stock=stock+(1 if mode else -1) if i<11 else None
   expected_arms=110+18*TABLE[0][i]//103 if mode and i>=11 else 110
   result=dict(label=label,merchant=m,item=i,mode='buy' if mode else 'sell',quantity=1,quote_tenths=quote,gold_before=g,gold_after=out['gold'],stock_before=stock,stock_after=word(0x6def+2*i) if i<11 else None,arms_after=word(0x6de1),judgment_before=ability,judgment_after=word(0x65cf),orders_after=out['orders'],interventions='restore fullmachine; all84 offer words1; three country merchant flags1')
   result['checks']={'gold':out['gold']==expected_gold,'stock':result['stock_after']==expected_stock,'arms':result['arms_after']==expected_arms,'orders':out['orders']==2,'ability':result['judgment_after']==ability-5}
   RESULTS.append(result);path.write_text(json.dumps(RESULTS,indent=2))
   assert all(result['checks'].values()),result
print('Trade matrix executed',len(RESULTS),'completed transactions',flush=True)
RESULTS={'cases':len(RESULTS),'passed':all(all(r['checks'].values()) for r in RESULTS),'details':str(path)}
