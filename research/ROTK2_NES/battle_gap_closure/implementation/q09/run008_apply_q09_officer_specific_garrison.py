#!/usr/bin/env python3
"""RUN008 hash-pinned patch on exact verified Run007 canonical SUP-0016 source.
Run once on a clean extraction of Run007. Refuses any different parent.
Original-source class VERIFIED_STATIC; port tests TESTED_PORT, release OPEN.
"""
from pathlib import Path
from hashlib import sha256

def sha(b): return sha256(b).hexdigest()

def apply(root):
    path=root/'src/rotk2/battle/engine.py'
    before=path.read_bytes()
    if sha(before)!='bdfee773b81aeb0c828251baaad034b79ca8c1e0477d18862f6a2c090b7524eb':
        raise ValueError('RUN008_HASH_GUARD: '+str(path)+' wrong original Run007 source sha='+sha(before))
    old='def retreat_destinations(state:GameState,b:Battle,side:str):\n    ruler=b.attacker_ruler if side==\'A\' else b.defender_ruler\n    return [n for n in state.provinces[b.target_province].neighbors\n            if state.provinces[n].owner in (None,ruler)]\n\n\ndef withdraw(state:GameState,b:Battle,side:str,destination:int,whole_army:bool=True,officer_id:int|None=None):\n    """Q09 destination eligibility; precise original after-flee relocation remains OPEN."""\n    if side!=b.side_to_move or destination not in retreat_destinations(state,b,side):\n        raise ValueError(\'ILLEGAL_WITHDRAW_DESTINATION\')\n    units=active_units(b,side) if whole_army else [u for u in active_units(b,side) if u.officer_id==officer_id]\n'
    new='def retreat_destinations(state:GameState,b:Battle,side:str,officer_id:int|None=None):\n    """Q09 hsovl2 $D905 -> hex $7FED: neighboring flee destinations.\n\n    The original accepts an open province, or a populated province whose\n    officer-list head belongs to the *fleeing officer\'s* liege. The port\'s\n    campaign model lacks the original linked-list head order, so the populated\n    case tests membership in the same-faction serving garrison; mixed-head\n    order remains a separate parity gap. Do not infer a garrison from owner.\n    """\n    members=active_units(b,side)\n    subject=(next((u for u in members if u.officer_id==officer_id),None)\n             if officer_id is not None else\n             next((u for u in members if u.commander),members[0] if members else None))\n    if subject is None:\n        return []\n    officer=state.officers.get(subject.officer_id)\n    ruler=officer.ruler_id if officer is not None else (b.attacker_ruler if side==\'A\' else b.defender_ruler)\n    allowed=[]\n    for n in state.provinces[b.target_province].neighbors:\n        province=state.provinces[n]\n        if province.owner is None:\n            allowed.append(n)\n        elif (ruler is not None and province.owner==ruler and\n              state.officers_in(n,ruler)):\n            allowed.append(n)\n    return allowed\n\n\ndef withdraw(state:GameState,b:Battle,side:str,destination:int,whole_army:bool=True,officer_id:int|None=None):\n    """Q09 destination eligibility; officer escape chance/capture still OPEN."""\n    if side!=b.side_to_move or destination not in retreat_destinations(state,b,side,\n            officer_id=None if whole_army else officer_id):\n        raise ValueError(\'ILLEGAL_WITHDRAW_DESTINATION\')\n    units=active_units(b,side) if whole_army else [u for u in active_units(b,side) if u.officer_id==officer_id]\n'
    data=before.decode('utf-8')
    if data.count(old)!=1: raise ValueError('RUN008_ANCHOR_MISMATCH: '+str(path))
    updated=data.replace(old,new).encode('utf-8')
    if sha(updated)!='ba082324e775020e942d157ed211e396f1e615634c209cc01124574e3fb554db': raise ValueError('RUN008_AFTER_HASH_MISMATCH: '+str(path))
    path.write_bytes(updated)
    path=root/'src/rotk2/ui/screens.py'
    before=path.read_bytes()
    if sha(before)!='985dee11d1dec42dd0e3f3ef91eadb9496eddd3f7c6ad007a1115d0d625e4055':
        raise ValueError('RUN008_HASH_GUARD: '+str(path)+' wrong original Run007 source sha='+sha(before))
    old='available=bt.retreat_destinations(app.engine.state,b,u.side)'
    new='available=bt.retreat_destinations(app.engine.state,b,u.side,officer_id=u.officer_id)'
    data=before.decode('utf-8')
    if data.count(old)!=1: raise ValueError('RUN008_ANCHOR_MISMATCH: '+str(path))
    updated=data.replace(old,new).encode('utf-8')
    if sha(updated)!='ba8a0ce994da372fdb1635e5c4d57d2ab9d316b422cd70b7b40603e9dd083478': raise ValueError('RUN008_AFTER_HASH_MISMATCH: '+str(path))
    path.write_bytes(updated)
    print('RUN008 APPLIED exact source-pinned changes')

if __name__=='__main__':
    import sys
    if len(sys.argv)!=2: raise SystemExit('Usage: python run008_apply_q09_officer_specific_garrison.py /path/to/extracted/Run007')
    apply(Path(sys.argv[1]))
