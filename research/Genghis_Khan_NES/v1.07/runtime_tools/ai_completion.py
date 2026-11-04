"""Controlled native AI economic observations, reached from normal Pass.

Execute only through research_session_v107.py's batch operation. No PC/VM-PC,
stack, call-frame, mapper, or ROM writes. PRG read breakpoints observe the actual
interpreter fetching mapped function entry/return bytecodes. Conditions and RNG
are explicit RAM interventions at an already-reached original routine.
"""
exec((ROOT/'batches/runtime/runtime_helpers.py').read_text())
exec((ROOT/'batches/runtime/event_helpers.py').read_text())

AI_SEED = 0x123456789ABC
AI_FIELDS = {'arms':0x14,'skill':0x16,'troops':0x18,'economy':0x1a,
             'town':0x1c,'defense':0x1e,'masons':0x20,'food':0x36,
             'commodity_history':0x38,'artisans':0x3a,'food_history':0x3c,
             'foodworkers':0x3e,'gold':0x40,'morale':0x42,'tax':0x44,
             'extra_used':0x46,'residents':0x49}
AI_GROUPS = ['troops','town','masons','artisans','foodworkers']
AI_RESULTS=[]

def ai_rng(seed, n):
    seed = (-((seed|1)*0xE0900E0187C5)) & ((1<<48)-1)
    return seed, ((seed>>32)&0x7FFF)%n

def ai_read_bp(bank, vm):
    return call('breakpoint.create', memoryType='nesPrgRom',
                address=bank*0x4000+vm-0x8000, length=1,
                access='read', cpuType='nes')

def ai_state(base):
    return {k:word(base+offset) for k,offset in AI_FIELDS.items()}

def ai_set(base, fields):
    for k,v in fields.items():
        wword(base+AI_FIELDS[k],v)

def ai_enter(label, bank, entry, checkpoint='spring1175'):
    restore(checkpoint)
    bp=ai_read_bp(bank,entry)
    pass_start()
    until_hit(bp,label+'_entry',True,loops=60)
    observation=call('breakpoint.list')
    frame=word(4)
    stream=word(6)
    # At read callback the stream may name this opcode or its next byte;
    # physical PRG address in the breakpoint is the decisive executed read.
    base=word(frame+11)
    delete_bp(bp)
    return base, {'entry_bank':bank,'entry_vm':hex(entry),
                  'vm_frame':frame,'vm_stream_at_pause':stream,
                  'entry_breakpoints':observation}

def ai_seed():
    write(0x84,AI_SEED.to_bytes(6,'big'))
    write(0x8a,[1])

def ai_finish(label, bank, stops, base, before, expected, provenance,
              interventions):
    bps=[ai_read_bp(bank,p) for p in stops]
    for _ in range(20):
        if any(hit(bp['handle']) for bp in bps):break
        step(60)
    else:raise AssertionError(('AI completion return not reached',label,stops))
    stop_state=call('breakpoint.list')
    after=ai_state(base)
    capture=snap(label+'_after')
    for bp in bps:delete_bp(bp)
    checks={key:after[key]==value for key,value in expected.items()}
    record={'case':label,'scope':'Controlled native bounded AI routine reached by normal Pass; not a complete autonomous turn',
            'country_index':(base-0x6dcd)//83,'country_base':hex(base),
            'before':before,'after':after,'expected':expected,'checks':checks,
            'entry':provenance,'return_breakpoints':stop_state,
            'interventions':interventions,'snapshot':capture}
    AI_RESULTS.append(record)
    (OUT/'ai_completion_results.json').write_text(json.dumps(AI_RESULTS,indent=2))
    assert all(checks.values()),record

def ai_country_arg(base):
    assert 0x6dcd <= base < 0x6dcd+83*word(0x65b9) and (base-0x6dcd)%83==0,hex(base)

# Gold100 and P100 exclude the military-heavy special allocation row without
# forcing a policy flag. Exact source threshold boundaries remain native.
for economy,row in [(299,[20,5,30,5]),(300,[18,7,30,5]),(500,[15,8,30,7])]:
    label='ai_labor_e'+str(economy)
    base,provenance=ai_enter(label,2,0x8177)
    ai_country_arg(base)
    assert word(0x7b70)==0,('Expected natural AI call',label)
    fields=dict(troops=100,town=0,masons=0,artisans=0,foodworkers=0,
                gold=100,economy=economy,arms=110,skill=124,morale=118)
    ai_set(base,fields);ai_seed();before=ai_state(base);snap(label+'_before')
    expected=dict(fields,town=row[0],masons=row[1],foodworkers=row[2],
                  artisans=row[3],troops=100-sum(row))
    ai_finish(label,2,[0x8226],base,before,expected,provenance,
              {'country_fields':fields,'rng_seed_hex':hex(AI_SEED),'nmi_rng_guard':1})

# The initial winter menu is a natural saved player turn. No season write.
for population in [600,601]:
    label='ai_winter_tax_p'+str(population)
    base,provenance=ai_enter(label,2,0x8294,'winter1175_menu')
    ai_country_arg(base)
    assert word(0x65bd)==3 and word(0x7b70)==0,('Expected natural Winter AI call',label)
    fields=dict(troops=population,town=0,masons=0,artisans=0,foodworkers=0,
                tax=35,morale=100)
    ai_set(base,fields);ai_seed();before=ai_state(base);snap(label+'_before')
    expected=dict(fields,tax=35 if population==600 else 50+ai_rng(AI_SEED,6)[1])
    ai_finish(label,2,[0x82BA,0x82EB],base,before,expected,provenance,
              {'country_fields':fields,'rng_seed_hex':hex(AI_SEED),'nmi_rng_guard':1})

# Seasonal assistance is an all-country loop. Its real loop count stays14;
# only country1 is measured. Difficulty5 makes its chance gate100%, allowing
# ownership and available-Gold negative controls without seeded chance luck.
for player,gold in [(False,100),(True,100),(False,30)]:
    label='ai_food_'+('human' if player else 'ai')+'_gold'+str(gold)
    unused,provenance=ai_enter(label,1,0x82B6)
    base=0x6dcd
    lord=word(base+0x51)
    player_owner=word(0x79de)
    assert player_owner<255
    player_slots=[word(0x79de+2*i) for i in range(4)]
    ai_owner=next(i for i in range(14) if i not in player_slots)
    write(lord+0x1d,[player_owner if player else ai_owner])
    wword(0x7a88,5)
    fields=dict(troops=61,food=0,gold=gold)
    ai_set(base,fields);ai_seed();before=ai_state(base);snap(label+'_before')
    q=min(91,2*(gold-30)) if not player and gold>30 else 0
    expected=dict(fields,food=q,gold=gold-q//2)
    ai_finish(label,1,[0x832B],base,before,expected,provenance,
              {'country_fields':fields,'country1_owner_byte':player_owner if player else ai_owner,
               'difficulty':5,'rng_seed_hex':hex(AI_SEED),'nmi_rng_guard':1,
               'note':'Original all-country loop and player-owner list retained'})

# The native helper is reached through the AI caller, then only its threshold
# operands and qualities are controlled; no forced call or rewritten PC.
for gold in [200,201]:
    label='ai_hire_gold'+str(gold)
    base,provenance=ai_enter(label,2,0x8A9D)
    ai_country_arg(base)
    fields=dict(troops=100,town=0,masons=0,artisans=0,foodworkers=0,
                gold=gold,arms=110,skill=124,morale=118)
    ai_set(base,fields);wword(0x7a8a,101);ai_seed()
    before=ai_state(base);snap(label+'_before')
    q=min(899,gold*10//120)//2 if gold>200 else 0
    expected=dict(fields,troops=100+q,gold=(10*gold-120*q)//10)
    if q:
        expected.update(arms=(100*110+100*q)//(100+q),
                        skill=(100*124+100*q)//(100+q),
                        morale=(100*118+q*(50+ai_rng(AI_SEED,11)[1]))//(100+q))
    ai_finish(label,2,[0x8AE3],base,before,expected,provenance,
              {'country_fields':fields,'rate':101,'rng_seed_hex':hex(AI_SEED),'nmi_rng_guard':1})

RESULTS={'cases':len(AI_RESULTS),'passed':all(all(x['checks'].values()) for x in AI_RESULTS),
         'scope':'10 controlled native AI economic routine observations; no complete AI-turn/campaign closure'}
