"""Playable, stdlib-only textual third-branch proof-of-concept.
Run `python -m third_branch.console --demo` for an end-to-end deterministic battle;
run `python -m third_branch.console --interactive` for a command-driven campaign.
"""
import argparse,json,shlex
from og.core.fixed import CoreError
from .integrated import new_campaign,load_file,save_file,hash_state
from .ai import take_turn

HELP='''Commands:
  help                           show this guide
  map                            show ownership and borders
  status                         print kingdom ledger
  advice                         view advisor for your faction A
  officer                         list officers and remaining BP
  act <officer> <kind> {JSON}     issue any supported command (use double-quoted JSON)
  attack <officer> <dst> <troops> start a real battle from the officer's province
  battle                         show active tactical grid and armies
  move <q> <r>                   move current attacker's unit (3 cells)
  strike                         strike adjacent defender
  battle-end                     advance battle day and AI counterattack
  battle-auto                    resolve battle automatically, retaining campaign changes
  ai [A|B|C] [style]             have ruler make a legal game command
  month                          run 13 economic phases, upkeep and BP regeneration
  save [path]                    save hash-checked atomic persistent campaign
  load [path]                    cold load saved campaign
  quit                           exit
All numbers are provisional test balance; geography is a synthetic 7-province scenario.
'''

def compact(obj):return json.dumps(obj,indent=2,sort_keys=True)

def show_battle(c):
    b=c.state['battle']
    if not b:return 'No battle is active.'
    rows=[list(x) for x in b['terrain']]
    for k,letter in [('d','D'),('a','A')]:
        x=b[k];rows[x['r']][x['q']]=letter
    return '\n'.join(''.join(row) for row in rows)+f'\nDay {b["day"]}, attackers {b["a"]["troops"]}, defenders {b["d"]["troops"]}'

def play(c,default_path):
    print('KOEI third-branch integrated TEST GAME — terminal edition. Type help.')
    counter=0
    while True:
        try:line=input('koei> ').strip()
        except (EOFError,KeyboardInterrupt):print();return
        if not line:continue
        try:
            counter+=1
            cmd,*rest=line.split(' ',1);tail=rest[0].strip() if rest else ''
            if cmd in ('quit','exit'):return
            if cmd=='help':print(HELP)
            elif cmd=='map':
                for pid,owner in sorted(c.state['owners'].items()):print(pid,owner,'->',','.join(c.state['neighbors'][pid]))
            elif cmd=='status':print(compact(c.advice('A')))
            elif cmd=='advice':print(compact(c.advice('A')))
            elif cmd=='officer':
                for oid,off in sorted(c.state['core']['officers'].items()):print(oid,c.state['officer_faction'][oid],off['province_id'],'BP',off['bp'],'POL',off['politics'],'CHA',off['charisma'])
            elif cmd=='act':
                oid,kind,raw=tail.split(' ',2);print(compact(c.do(kind,oid,json.loads(raw),f'cli:{counter}')))
            elif cmd=='attack':
                oid,dst,n=tail.split();print(compact(c.do('attack',oid,{'target':dst,'troops':int(n)},f'cli:{counter}')))
            elif cmd=='battle':print(show_battle(c))
            elif cmd in ('move','strike','battle-end','battle-auto'):
                b=c.state['battle']
                if not b:print('No active battle.');continue
                actor=next(k for k,v in c.state['officer_faction'].items() if v==b['attacker'])
                kind={'move':'battle_move','strike':'battle_attack','battle-end':'battle_end_turn','battle-auto':'battle_auto'}[cmd]
                params={} if cmd!='move' else dict(zip(['q','r'],[int(v) for v in tail.split()]))
                print(compact(c.do(kind,actor,params,f'cli:{counter}')))
            elif cmd=='ai':
                parts=tail.split();print(compact(take_turn(c,parts[0] if parts else 'A',parts[1] if len(parts)>1 else 'balanced','cli',counter)))
            elif cmd=='month':print(compact(c.advance_month()))
            elif cmd=='save':
                path=tail or default_path;save_file(c,path);print('Saved',path,'sha',c.fingerprint)
            elif cmd=='load':
                path=tail or default_path;c=load_file(path);print('Loaded',path,'sha',c.fingerprint)
            else:print('Unknown command; type help')
        except (CoreError,ValueError,KeyError,IndexError) as e:print('Rejected:',str(e))

def demo(seed,months):
    c=new_campaign(seed)
    events=[]
    for m in range(months):
        if m==0:
            events.append(c.do('distribute_food','A1',{'province_id':'p1','units':10},'demo-food'))
            events.append(c.do('market','A1',{'commodity':'metal','side':'buy','units':2},'demo-trade'))
            events.append(c.do('plan_attack','A3',{'target':'p4'},'demo-plan'))
            events.append(c.do('attack','A3',{'target':'p4','troops':180},'demo-attack'))
            events.append(c.do('battle_auto','A3',{},'demo-resolve'))
        else:
            for faction in ('A','B','C'):
                events.append(take_turn(c,faction,'balanced','demo',m))
        c.advance_month()
    print(compact({'seed':seed,'months':months,'events':events,'battle_history':c.state['history'],
                   'owners':c.state['owners'],'bp_A1':c.state['core']['officers']['A1']['bp'],
                   'fingerprint':c.fingerprint,'advisor':c.advice('A')}))
    return c

def main():
    p=argparse.ArgumentParser();p.add_argument('--demo',action='store_true');p.add_argument('--interactive',action='store_true');p.add_argument('--seed',type=int,default=7);p.add_argument('--months',type=int,default=3);p.add_argument('--save',default='third_branch_save.json')
    a=p.parse_args();c=demo(a.seed,a.months) if a.demo else new_campaign(a.seed)
    if a.interactive or not a.demo:play(c,a.save)
if __name__=='__main__':main()
