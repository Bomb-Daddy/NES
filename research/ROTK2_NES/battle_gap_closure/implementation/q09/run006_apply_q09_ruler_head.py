#!/usr/bin/env python3
"""Run006 Q09/Q14 ruler-owned linked-head update; canonical NES Classic ONLY.

Input must be the exact immutable Run005 Classic source engine bytes.
No DOS, Enhanced or original KOEI branch writes. Fail closed on mismatch.
"""
from pathlib import Path
from hashlib import sha256
import sys

OLD_ENGINE_SHA = "015b09d61c4d2392270f8e8eca6bfd5e186d0504a119e76703eb6cbbf9a57090"
NEW_ENGINE_SHA = "bdfee773b81aeb0c828251baaad034b79ca8c1e0477d18862f6a2c090b7524eb"
OLD = "            if province.owner is None and officer.ruler_id in state.rulers:\n                province.owner=officer.ruler_id\n"
NEW = "            if province.owner is None and officer.ruler_id in state.rulers:\n                province.owner=officer.ruler_id\n            # Q09 uses root $5154 for province insertion. For the actual\n            # ruler-officer, root $51BD moves that province to the front of\n            # the ruler's owned-province linked list. The Q14 release helper\n            # subsequently reads precisely that head, not the numeric minimum.\n            if (officer.is_ruler and officer.ruler_id in state.rulers\n                    and province.owner == officer.ruler_id):\n                state.rulers[officer.ruler_id].q14_owned_head=destination\n"

def apply(root):
    path=Path(root)/"src/rotk2/battle/engine.py"
    b=path.read_bytes()
    if sha256(b).hexdigest()!=OLD_ENGINE_SHA:
        raise SystemExit("PARENT_MISMATCH: source must be immutable Run005")
    t=b.decode("utf-8")
    if t.count(OLD)!=1:
        raise SystemExit("SPLICE_MISMATCH")
    output=t.replace(OLD,NEW,1).encode("utf-8")
    if sha256(output).hexdigest()!=NEW_ENGINE_SHA:
        raise SystemExit("RESULT_HASH_MISMATCH")
    path.write_bytes(output)
    print("Run006 Q09 head patch", NEW_ENGINE_SHA)

if __name__=="__main__":
    if len(sys.argv)!=2: raise SystemExit("usage: run006_apply_q09_ruler_head.py EXTRACTED_RUN005_ROOT")
    apply(sys.argv[1])
