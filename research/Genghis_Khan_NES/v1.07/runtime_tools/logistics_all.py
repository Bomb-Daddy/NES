exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
RESULTS=[]
def begin(mode):
 menu(7);keys(['down']*mode);press('a',5,500);press('a',5,500);press('up',5,80);press('a',5,600)
def finish(label,q):
 number(q);snap(label+'_prompt');press('a',5,400);press('a',5,300);return snap(label+'_result')
def keep(x):RESULTS.append(x);(OUT/'logistics_all_results.json').write_text(json.dumps(RESULTS,indent=2))
for i in range(11):
 restore();write(0x65fb,[0]);a1=0x6def+2*i;a2=0x6e42+2*i;s1=word(a1);s2=word(a2);snap(f'move_item{i}_before');begin(4);item(i);out=finish(f'move_item{i}',1)
 x={'case':'item'+str(i),'source_before':s1,'destination_before':s2,'source_after':word(a1),'destination_after':word(a2),'orders':out['orders'],'abilities':out['abilities']};keep(x)
 assert (word(a1),word(a2))==(s1-1,s2+1) and out['orders']==2,x
for j,off in enumerate([28,32,62,58,24]):
 restore();write(0x65fb,[0]);q=8 if j==4 else 1;s1=word(0x6dcd+off);s2=word(0x6e20+off);snap(f'move_people{j}_before');begin(1);keys(['down']*j);press('a',5,500);out=finish(f'move_people{j}',q)
 x={'case':'people'+str(j),'q':q,'source_before':s1,'destination_before':s2,'source_after':word(0x6dcd+off),'destination_after':word(0x6e20+off),'destination_quality':[word(0x6e20+o) for o in [20,22,66]],'orders':out['orders'],'abilities':out['abilities']};keep(x)
 assert (word(0x6dcd+off),word(0x6e20+off))==(s1-q,s2+q) and out['orders']==2 and out['abilities']==[79,77,112,51,170,129],x
 assert x['destination_quality']==[(105*v+q*s)//(105+q) for v,s in zip([106,122,76],[110,124,118])],x
restore();write(0x65fb,[0])
for base,counts,quality in [(0x6dcd,[8,0,0,0,0],[110,124,118]),(0x6e20,[10,0,0,0,0],[150,80,100])]:
 for off,v in zip([24,28,32,58,62],counts):wword(base+off,v)
 for off,v in zip([20,22,66],quality):wword(base+off,v)
snap('move_empty_source_before');begin(1);keys(['down']*4);press('a',5,500);out=finish('move_empty_source',8);x={'case':'empty_source','source_quality':[word(0x6dcd+o) for o in [20,22,66]],'destination_quality':[word(0x6e20+o) for o in [20,22,66]],'orders':out['orders']};keep(x)
assert x['source_quality']==[0,0,118] and x['destination_quality']==[83,44,108] and out['orders']==2,x
RESULTS={'cases':len(RESULTS),'passed':True}
