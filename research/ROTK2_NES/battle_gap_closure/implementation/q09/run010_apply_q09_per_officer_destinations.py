"""Run010 Q09 per-officer destination guard. Only use on hash-pinned Run009.
Run the source regression on Run009 first: 5 failures, 1 pass; then apply.
"""
from pathlib import Path
from hashlib import sha256
import argparse

SOURCE_SHA = {
    'src/rotk2/battle/engine.py': '8cebe4f4cf6a754d6efe92faaa7727ad215a09a1b624c8567fe77ec78d5b92f0',
    'src/rotk2/rules/battle_commands.py': 'c8d395bcaba6d346e140d2e981f5be58a5261d6f48ec242ab6b58ee0602c21e8',
    'src/rotk2/ui/screens.py': '1642e1727a86fdc172658734a1bdb05676c6b2b557626f6d2c4b06454e5ab355',
}

def change_one(path, before, after):
    content=path.read_text()
    count=content.count(before)
    if count!=1:
        raise SystemExit(f'EXACT_ANCHOR_FAILURE {path} count={count}')
    path.write_text(content.replace(before,after),encoding='utf-8')

parser=argparse.ArgumentParser(description='Apply Run010 only to extracted canonical Run009 source')
parser.add_argument('source_root',type=Path)
root=parser.parse_args().source_root.resolve()
for rel, expected in SOURCE_SHA.items():
    path=root/rel
    digest=sha256(path.read_bytes()).hexdigest()
    if expected and digest!=expected:
        raise SystemExit(f'PARENT_SHA_MISMATCH {rel}: {digest} != {expected}')

engine=root/'src/rotk2/battle/engine.py'
change_one(engine,
    'def withdraw(state:GameState,b:Battle,side:str,destination:int,whole_army:bool=True,officer_id:int|None=None):\n    """Q09 destination eligibility; officer escape chance/capture still OPEN."""',
    'def withdraw(state:GameState,b:Battle,side:str,destination:int,whole_army:bool=True,officer_id:int|None=None,officer_destinations:dict|None=None):\n    """Q09 original $D89D selects a legal destination independently for each officer.\n\n    Escape chance/capture remains OPEN: this fixes destination selection and\n    transaction validation without claiming the complete original withdrawal.\n    """')
change_one(engine,
    "    # Q09: a whole-army retreat must cancel pending slot-1/3 officers too.\n",
    """    # $D89D iterates officers with separate destinations. Do not guess that
    # all have the primary commander's liege or the same eligible neighbors.
    # Validate the complete explicit choice set BEFORE changing ANY battle state.
    individual_destinations={}
    if officer_destinations is not None:
        if (not whole_army or not isinstance(officer_destinations,dict) or
                any(type(k) is not int or type(v) is not int
                    for k,v in officer_destinations.items()) or
                set(officer_destinations)!={u.officer_id for u in units} or
                officer_destinations.get(primary.officer_id)!=destination):
            raise ValueError('INVALID_WITHDRAW_DESTINATIONS')
        for u in units:
            chosen=officer_destinations[u.officer_id]
            if chosen not in retreat_destinations(state,b,side,officer_id=u.officer_id):
                raise ValueError('ILLEGAL_WITHDRAW_DESTINATION')
            individual_destinations[u.officer_id]=chosen
    # Q09: a whole-army retreat must cancel pending slot-1/3 officers too.
""")
change_one(engine,
    "        b.retreat_destinations[str(u.officer_id)]=homes.get(u.officer_id,destination)\n",
    "        b.retreat_destinations[str(u.officer_id)]=homes.get(u.officer_id,individual_destinations.get(u.officer_id,destination))\n")

commands=root/'src/rotk2/rules/battle_commands.py'
change_one(commands,
    "    oid=cmd.params.get('officer_id')\n    try:bt.withdraw(s,b,b.side_to_move,dest,whole,oid)\n",
    "    oid=cmd.params.get('officer_id')\n    choices=cmd.params.get('officer_destinations')\n    try:bt.withdraw(s,b,b.side_to_move,dest,whole,oid,choices)\n")

screens=root/'src/rotk2/ui/screens.py'
change_one(screens,
    """        def choose_destination(whole_army):
            app.push(ListPrompt('Withdraw where?',[(f'Province {pid}',pid) for pid in available],
                lambda dest:self._apply(app,'battle_withdraw',destination=dest,
                                       whole_army=whole_army,officer_id=u.officer_id)))
""",
    """        def choose_destination(whole_army):
            if not whole_army:
                app.push(ListPrompt('Withdraw where?',[(f'Province {pid}',pid) for pid in available],
                    lambda dest:self._apply(app,'battle_withdraw',destination=dest,
                                           whole_army=False,officer_id=u.officer_id)))
                return
            # Original hsovl2 $D89D: each active officer chooses a separate
            # eligible province. First B aborts; later B repeats that prompt.
            # Selection is staged until complete, so malformed selections
            # cannot partially concede an army. Capture rolls remain OPEN.
            army=bt.active_units(b,u.side)
            primary_id=u.officer_id
            order=[u]+[member for member in army if member.officer_id!=primary_id]
            def choose_one(index,picks):
                member=order[index]
                destinations=bt.retreat_destinations(app.engine.state,b,member.side,
                                                      officer_id=member.officer_id)
                if not destinations:
                    app.push(MessageBox(['Cannot flee: no destination']))
                    return
                def selected(pid):
                    new=dict(picks);new[member.officer_id]=pid
                    if index+1<len(order):
                        choose_one(index+1,new)
                    else:
                        self._apply(app,'battle_withdraw',destination=new[primary_id],
                                    officer_destinations=new,whole_army=True,
                                    officer_id=primary_id)
                app.push(ListPrompt('Flee '+app.engine.state.officers[member.officer_id].name[:11],
                    [(f'Province {pid}',pid) for pid in destinations],selected,
                    on_cancel=(lambda:choose_one(index,picks)) if index else None))
            choose_one(0,{})
""")
print('Run010: exact original source hashes and anchors verified; three production members updated')
for rel in SOURCE_SHA:
    print(rel,sha256((root/rel).read_bytes()).hexdigest())