#!/usr/bin/env python3
"""Apply Run003 Q14 source-order correction ONLY to exact Run002 Classic46 BPF.

Parent full ZIP: 1JX1s68-TcNZnTBoOj3ob3u2MfLjOqBd-
SHA256: 2c95615d8f1d75f29eae42153be08a31896fb0b0ac879201bdbe7c78cded3851.
Work on a *copy* extracted from the parent; do not touch the parent ZIP.
The original NES runtime was NOT replayed for this port correction.
"""
from pathlib import Path
from hashlib import sha256
import sys

EXPECTED = {
    "src/rotk2/battle/engine.py": ("261f6c170644bafe11d8a1f43f94e9a9ae4cc6689978d5d1ddee9f42cf5eab08",
        "a61435c71f63e67ba0fa0127da6f3569671716681fe18ebffaa94b907c3f5614"),
    "src/rotk2/state/models.py": ("965dd5837df2df940aef035dc745ebdbea547e5c57d68fb2a5389063dc84f09c",
        "7c87426532ef497ea3eebd174129548bd17ece1f09609ecf3295f679d25bf7ff"),
}

def replace_once(s, old, new):
    if s.count(old) != 1:
        raise RuntimeError(f"Expected unique original anchor; found {s.count(old)}: {old[:60]}")
    return s.replace(old, new, 1)

def patch_engine(s):
    begin = s.index("def ruler_release_destination(")
    end = s.index("\ndef cpu_ruler_disposition(", begin)
    function = '''def ruler_release_destination(state: GameState, b: Battle, ruler_id: int):
    """Q14 original source branch order, when the adapter supplies source flags.

    sengo $8436: eligible supplied slot; safe friendly neighbors in original
    directional order; friendly (including attacked) OR unowned neighbors in
    that same order; first linked owned ruler province; zero/no destination.

    No new head order is inferred: when a source head is not imported, the
    historical port fallback remains explicitly PROVISIONAL for multi-province
    ownership. This does not certify a full NES original replay.
    """
    if ruler_id is None or ruler_id not in state.rulers:
        return None
    target = state.provinces[b.target_province]
    ruler = state.rulers[ruler_id]
    head = ruler.q14_owned_head
    if head is not None and (head not in state.provinces or
                             state.provinces[head].owner != ruler_id):
        # Never convert corrupt recovered pointers into a plausible refuge.
        raise ValueError('BROKEN_RULER_OWNED_HEAD')
    slot_id = b.from_province if b.result == 'ATTACKER' else b.target_province
    slot = state.provinces.get(slot_id)
    if slot and slot.id != target.id and slot.owner in (None, ruler_id):
        return slot.id
    neighbors = [pid for pid in target.neighbors if pid in state.provinces]
    for pid in neighbors:
        candidate = state.provinces[pid]
        if candidate.owner == ruler_id and not candidate.under_attack:
            return pid
    for pid in neighbors:
        if state.provinces[pid].owner in (None, ruler_id):
            return pid
    owned = [p.id for p in state.provinces.values()
             if p.owner == ruler_id and p.id != target.id]
    if not owned:
        return None
    if head is not None and head != target.id:
        return head
    if len(owned) == 1:
        return owned[0]  # unique known survivor; linked-order ambiguity absent
    ruler_officer = state.officers.get(ruler_id)
    if ruler_officer is not None and ruler_officer.province_id in owned:
        return ruler_officer.province_id
    # Provisional historical campaign order for unhydrated original list HEAD.
    return owned[0]

'''
    s = s[:begin] + function.rstrip("\n") + "\n" + s[end:]
    old = """    if att_won:
        dst.owner = atk_ruler
        for u in b.units:
"""
    new = """    if att_won:
        old_owner = dst.owner
        dst.owner = atk_ruler
        # The imported NES linked owned-list head is valid only while its
        # province is still owned. On conquest, invalidate the lost head;
        # never guess a successor pointer from numeric province ordering.
        if old_owner != atk_ruler and old_owner in state.rulers:
            old_ruler = state.rulers[old_owner]
            if old_ruler.q14_owned_head == dst.id:
                old_ruler.q14_owned_head = None
        # Only a single active battle is represented by this campaign state.
        dst.under_attack = False
        for u in b.units:
"""
    return replace_once(s, old, new)

def patch_models(s):
    s=replace_once(s, """    rice_price: int = 50
    neighbors: list = field(default_factory=list)
""", """    rice_price: int = 50
    neighbors: list = field(default_factory=list)
    # Original Q14 safe-neighbor predicate; defaults to no OTHER active siege.
    # The single-battle campaign adapter still lacks general multi-battle upkeep.
    under_attack: bool = False
""")
    s=replace_once(s, """    q14_source_guard: Optional[int] = None                  # g_E006 at capture resolution
""", """    q14_source_guard: Optional[int] = None                  # g_E006 at capture resolution
    # Actual original linked province-list HEAD, only when source recovered.
    # None means not recovered; NEVER manufacture a linked order from province ID.
    q14_owned_head: Optional[int] = None
""")
    s=replace_once(s, """            if p.owner is not None and p.owner not in self.rulers:
""", """            if not isinstance(p.under_attack, bool):
                errs.append(f"province {p.id} invalid under_attack flag")
            if p.owner is not None and p.owner not in self.rulers:
""")
    s=replace_once(s, """        for r in self.rulers.values():
            if r.id not in self.officers:
""", """        for r in self.rulers.values():
            if r.q14_owned_head is not None and (
                    r.q14_owned_head not in self.provinces or
                    self.provinces[r.q14_owned_head].owner != r.id):
                errs.append(f"ruler {r.id} invalid Q14 owned-head")
            if r.id not in self.officers:
""")
    return s

def main():
    if len(sys.argv)!=2:
        raise SystemExit("Usage: python run003_apply_q14.py <CLEAN_EXTRACTED_RUN002_COPY>")
    root=Path(sys.argv[1])
    data={}
    for name,(before,after) in EXPECTED.items():
        path=root/name
        content=path.read_bytes()
        found=sha256(content).hexdigest()
        if found!=before:
            raise SystemExit(f"ABORT wrong parent for {name}: {found} != {before}")
        data[name]=content.decode("utf-8")
    changed={
      "src/rotk2/battle/engine.py":patch_engine(data["src/rotk2/battle/engine.py"]),
      "src/rotk2/state/models.py":patch_models(data["src/rotk2/state/models.py"]),
    }
    for name,body in changed.items():
        actual=sha256(body.encode()).hexdigest()
        expected=EXPECTED[name][1]
        if actual!=expected:raise SystemExit(f"ABORT mismatch of generated {name}: {actual} != {expected}")
    for name,body in changed.items():
        (root/name).write_bytes(body.encode())
        print(f"PATCHED {name} SHA256={EXPECTED[name][1]}")
    print("Run003 source edits exact-hash validated. Run the GitHub Run003 regression tests.")

if __name__=="__main__":
    main()
