#!/usr/bin/env python3
"""SHA-guarded Classic Run004 settlement correction; apply ONLY after Run003.

The parent must be a fresh Run002 source tree patched by the published
Run003 script. Original ZIPs are never edited. This is Linux-tested Python,
NOT native Windows/physical Xbox or fresh original Mesen evidence.
"""
from pathlib import Path
from hashlib import sha256
import sys

BEFORE='a61435c71f63e67ba0fa0127da6f3569671716681fe18ebffaa94b907c3f5614'
AFTER='2a2b50fb3b57fedd4ca6105d3785aeee3dd79860731a2616a1ff3f8009a7c532'
MODELS_SHA='7c87426532ef497ea3eebd174129548bd17ece1f09609ecf3295f679d25bf7ff'
OLD='''        # Only a single active battle is represented by this campaign state.
        dst.under_attack = False
'''
NEW='''    # Q14: an ended siege is no longer active whether the attacker captured
    # the province or the defender successfully repelled the attack.  Retain
    # every unrelated province's attack flag; Q14's next refuge search must
    # see the true current siege status after either battle result.
    dst.under_attack = False
    b.committed = True
'''

def apply(root):
    engine=root/'src/rotk2/battle/engine.py'
    model=root/'src/rotk2/state/models.py'
    raw=engine.read_bytes()
    if sha256(raw).hexdigest()!=BEFORE or sha256(model.read_bytes()).hexdigest()!=MODELS_SHA:
        raise SystemExit('ABORT: wrong Run003 parent source or model SHA')
    text=raw.decode('utf8')
    commit='    b.committed = True\n'
    if text.count(OLD)!=1 or text.count(commit)!=1:
        raise SystemExit('ABORT: ambiguous source anchors')
    changed=text.replace(OLD,'',1).replace(commit,NEW,1).encode()
    if sha256(changed).hexdigest()!=AFTER:
        raise SystemExit('ABORT: output source did not match tested SHA')
    engine.write_bytes(changed)
    print('Run004 Classic source SHA256='+AFTER)

if __name__=='__main__':
    if len(sys.argv)!=2:
        raise SystemExit('Usage: run004_apply_classic_after_run003.py <CLEAN_RUN003_EXTRACTED_TREE>')
    apply(Path(sys.argv[1]))
