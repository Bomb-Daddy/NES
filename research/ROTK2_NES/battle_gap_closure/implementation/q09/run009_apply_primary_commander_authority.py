"""Reproducible Run009 patch, pinned to exact immutable Run008 production inputs.

Usage: python run009_apply_primary_commander_authority.py /path/to/extracted/Run008
Fails closed on any changed parent or second application. Modifies only two files.
"""
from pathlib import Path
from hashlib import sha256
import sys

ENGINE_SHA='ba082324e775020e942d157ed211e396f1e615634c209cc01124574e3fb554db'
SCREENS_SHA='ba8a0ce994da372fdb1635e5c4d57d2ab9d316b422cd70b7b40603e9dd083478'

def patch(root:Path):
    engine=root/'src/rotk2/battle/engine.py'
    screens=root/'src/rotk2/ui/screens.py'
    sources={engine:engine.read_bytes(), screens:screens.read_bytes()}
    for p,want in [(engine,ENGINE_SHA),(screens,SCREENS_SHA)]:
        got=sha256(sources[p]).hexdigest()
        if got!=want:raise RuntimeError(f'wrong or mutated parent: {p}: {got} != {want}')
    old_engine=b"    if whole_army and not any(u.commander for u in units):raise ValueError('ONLY_COMMANDER_WHOLE_WITHDRAW')"
    new_engine=(
        b"    # Q09: hsovl2 D905/D89D offers army withdrawal only to the primary\n"
        b"    # slot commander. A direct/loaded command cannot borrow their authority.\n"
        b"    if whole_army:\n"
        b"        main_slot=2 if side=='A' else 0\n"
        b"        primary=next((u for u in units if u.commander and u.slot==main_slot),None)\n"
        b"        if primary is None or (officer_id is not None and officer_id!=primary.officer_id):\n"
        b"            raise ValueError('ONLY_COMMANDER_WHOLE_WITHDRAW')"
    )
    old_screen=(
        b"        if u.commander:\n"
        b"            app.push(YesNo(['Withdraw whole army?'],"
    )
    new_screen=(
        b"        # Reinforcement-slot commanders cannot trigger the primary army prompt.\n"
        b"        if u.commander and u.slot==(2 if u.side=='A' else 0):\n"
        b"            app.push(YesNo(['Withdraw whole army?'],"
    )
    for p,old in [(engine,old_engine),(screens,old_screen)]:
        if sources[p].count(old)!=1:raise RuntimeError(f'unexpected context for {p}')
    # All guards run before any mutation; both files are all-or-nothing on source mismatch.
    engine.write_bytes(sources[engine].replace(old_engine,new_engine))
    screens.write_bytes(sources[screens].replace(old_screen,new_screen))
    print('patch applied',sha256(engine.read_bytes()).hexdigest(),sha256(screens.read_bytes()).hexdigest())

if __name__=='__main__':
    if len(sys.argv)!=2:raise SystemExit('usage: script /source/root')
    patch(Path(sys.argv[1]))
