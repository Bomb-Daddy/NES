exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
RESULTS=[]
def begin(mode):
 menu(7);keys(['down']*mode);press('a',5,500);press('a',5,500);press('up',5,80);press('a',5,500)
def finish(label,q):
 number(q);snap(label+'_prompt');press('a',5,400);press('a',5,300);return snap(label+'_result')
for name,mode,addr1,addr2,q in [('gold',2,0x6e0d,0x6e60,10),('food',3,0x6e03,0x6e56,8)]:
 restore();write(0x65fb,[0]);before=snap('move_'+name+'_matrix_before');old1=word(addr1);old2=word(addr2);begin(mode);out=finish('move_'+name+'_matrix',q)
 x={'case':name,'source_before':old1,'destination_before':old2,'source_after':word(addr1),'destination_after':word(addr2),'orders':out['orders'],'abilities':out['abilities']};RESULTS.append(x);(OUT/'logistics_matrix_results.json').write_text(json.dumps(RESULTS,indent=2))
 assert (word(addr1),word(addr2))==(old1-q,old2+q) and out['orders']==2 and out['abilities']==[84,72,107,61,170,129],x
# Inspect the generic Item picker; next continuation is prepared from its actual screen.
restore();write(0x65fb,[0]);snap('move_item_before');begin(4);snap('move_item_picker');RESULTS={'scalar_cases':len(RESULTS),'status':'inspect Item picker'}
