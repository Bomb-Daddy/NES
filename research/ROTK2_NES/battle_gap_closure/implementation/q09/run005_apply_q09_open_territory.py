#!/usr/bin/env python3
"""Run005 Q09 canonical Classic ONLY; apply to extracted full Run004 candidate.

Reproduces source-observed open-territory capture on successful Flee. This
does NOT modify the independent Original KOEI Enhanced / DOS projects.
"""
from pathlib import Path
from hashlib import sha256
import sys

OLD_ENGINE_SHA = "2a2b50fb3b57fedd4ca6105d3785aeee3dd79860731a2616a1ff3f8009a7c532"
NEW_ENGINE_SHA = "015b09d61c4d2392270f8e8eca6bfd5e186d0504a119e76703eb6cbbf9a57090"
OLD = "    for key,destination in b.retreat_destinations.items():\n        officer=state.officers.get(int(key))\n        if officer is not None and officer.alive:\n            officer.province_id=destination\n            if state.provinces[destination].owner is None:\n                officer.ruler_id=None\n"
NEW = "    for key,destination in b.retreat_destinations.items():\n        officer=state.officers.get(int(key))\n        if officer is not None and officer.alive:\n            officer.province_id=destination\n            # Q09 hsovl2 $D648 -> root $5154 / $507C: a successful\n            # escape into peaceful open territory inserts the departing officer\n            # AND establishes ownership for the officer's existing faction.\n            # Merely moving the officer and clearing his ruler, as this port\n            # previously did, loses the observed Yuan Shu P20->P29 outcome.\n            # This port still defers insertion to settlement; immediate\n            # mid-battle officer-list transfer remains a separate open gap.\n            province=state.provinces[destination]\n            if province.owner is None and officer.ruler_id in state.rulers:\n                province.owner=officer.ruler_id\n"

def apply(root):
    path = Path(root) / 'src/rotk2/battle/engine.py'
    original = path.read_bytes()
    if sha256(original).hexdigest() != OLD_ENGINE_SHA:
        raise SystemExit('PARENT_MISMATCH: refusing to patch')
    source = original.decode('utf-8')
    if source.count(OLD) != 1:
        raise SystemExit('SPLICE_MISMATCH: refusing to patch')
    patched = source.replace(OLD, NEW, 1).encode('utf-8')
    if sha256(patched).hexdigest() != NEW_ENGINE_SHA:
        raise SystemExit('RESULT_HASH_MISMATCH: refusing to patch')
    path.write_bytes(patched)
    print('Q09 Run005 engine SHA-256:', NEW_ENGINE_SHA)

if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit('usage: run005_apply_q09_open_territory.py PATH_TO_EXTRACTED_RUN004')
    apply(sys.argv[1])
