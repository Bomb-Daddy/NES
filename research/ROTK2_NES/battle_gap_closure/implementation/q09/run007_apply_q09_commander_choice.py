#!/usr/bin/env python3
"""Run007 canonical NES Classic Q09 controller prompt patch.

Apply ONLY after rebuilding the exact Run006 engine from verified Run004 and
Run005/Run006 hash-guarded scripts. The complete binary source lives in the
canonical SUP-0016 Drive folder, not GitHub Original_KOEI_Windows_Game.
"""
from pathlib import Path
from hashlib import sha256
import sys

EXPECTED_ENGINE="bdfee773b81aeb0c828251baaad034b79ca8c1e0477d18862f6a2c090b7524eb"
PATCHES={
"src/rotk2/ui/widgets.py":(
"141c46eb5dacec8294089e89f86eea9032e5935244e86e1ce8d892521280d51f",
"4f47609db5479e9548f7fbfd52bb587bdbbda6cac014b8b170d9f008d593a184",
[
("""    def __init__(self, lines, on_yes, on_no=None):
        self.lines, self.on_yes, self.on_no = lines, on_yes, on_no
        self.yes = True
""","""    def __init__(self, lines, on_yes, on_no=None, on_cancel=None):
        self.lines, self.on_yes, self.on_no = lines, on_yes, on_no
        # When the source prompt distinguishes B/cancel from a deliberate
        # No selection, do not treat B as No. Historical users retain default.
        self.on_cancel = on_cancel
        self.yes = True
"""),
("""        elif a == Action.CANCEL:
            app.pop()
            if self.on_no: self.on_no()
""","""        elif a == Action.CANCEL:
            app.pop()
            callback = self.on_cancel if self.on_cancel is not None else self.on_no
            if callback: callback()
""")
]),
"src/rotk2/ui/screens.py":(
"dbed9c43eea983c48d6c3fd6dd7ee7d97c2c8e424b86500f68cacae5555b560c",
"985dee11d1dec42dd0e3f3ef91eadb9496eddd3f7c6ad007a1115d0d625e4055",
[("""        app.push(ListPrompt('Withdraw where?',[(f'Province {pid}',pid) for pid in available],
            lambda dest:self._apply(app,'battle_withdraw',destination=dest,
                                   whole_army=bool(u.commander),officer_id=u.officer_id)))
""","""        # Original Q09 hsovl2 $D905/$D89D: only a slot commander is asked
        # whether to withdraw the *whole army*. A No answer still allows
        # this one commander to flee. B cancels without consuming his action.
        # The destination itself is selected afterwards; its B path also
        # returns without issuing battle_withdraw.
        def choose_destination(whole_army):
            app.push(ListPrompt('Withdraw where?',[(f'Province {pid}',pid) for pid in available],
                lambda dest:self._apply(app,'battle_withdraw',destination=dest,
                                       whole_army=whole_army,officer_id=u.officer_id)))
        if u.commander:
            app.push(YesNo(['Withdraw whole army?'],
                lambda:choose_destination(True), lambda:choose_destination(False),
                on_cancel=lambda:None))
        else:
            choose_destination(False)
""")])
}

def apply(root:Path):
    engine=root/"src/rotk2/battle/engine.py"
    assert sha256(engine.read_bytes()).hexdigest()==EXPECTED_ENGINE,"wrong parent engine; abort"
    staged={}
    for relative,(old_sha,new_sha,replaces) in PATCHES.items():
        path=root/relative
        original=path.read_bytes()
        assert sha256(original).hexdigest()==old_sha,(relative,"parent hash mismatch")
        text=original.decode("utf-8")
        for old,new in replaces:
            assert text.count(old)==1,(relative,"expected splice missing")
            text=text.replace(old,new,1)
        result=text.encode("utf-8")
        assert sha256(result).hexdigest()==new_sha,(relative,"result mismatch")
        staged[path]=result
    for path,result in staged.items():
        path.write_bytes(result)
        print(path,sha256(result).hexdigest())

if __name__=="__main__":
    if len(sys.argv)!=2: raise SystemExit("usage: run007_apply_q09_commander_choice.py EXTRACTED_RECONSTRUCTED_RUN006_SOURCE")
    apply(Path(sys.argv[1]))
