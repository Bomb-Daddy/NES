exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
RESULTS=[]
def save(case,out,extra=None):
 x={'case':case,**{k:out[k] for k in ['gold','food','fur','troops','orders','abilities','reserve','eligible_percent']},'morale':word(0x6e0f),'skill':word(0x6de3),'arms':word(0x6de1)}
 x.update(extra or {});RESULTS.append(x);(OUT/'give_train_results.json').write_text(json.dumps(RESULTS,indent=2))
for mode,name in [(0,'gold'),(1,'food'),(2,'native_item')]:
 for q in [0,1,8]:
  restore();snap(f'give_{name}_{q}_before');menu(2);keys(['down']*mode);press('a',5,500);number(q);snap(f'give_{name}_{q}_prompt');press('a',5,400);press('a',5,300);save(f'give_{name}_{q}',snap(f'give_{name}_{q}_result'))
for case,elig,res,skill in [('people_baseline',32,34,124),('people49',49,34,124),('people50',50,34,124),('people_zero_residents',32,0,124),('troops_baseline',32,34,124),('troops999',32,34,999)]:
 restore();write(0x6e15,[elig]);wword(0x6e16,res);wword(0x6de3,skill);snap('train_'+case+'_before');menu(3)
 if case.startswith('people'):keys(['down']*3)
 press('a',5,500);snap('train_'+case+'_commit');press('a',5,300);save(case,snap('train_'+case+'_result'))
