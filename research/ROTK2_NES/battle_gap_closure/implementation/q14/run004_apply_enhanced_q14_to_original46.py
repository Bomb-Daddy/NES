#!/usr/bin/env python3
"""Run004 Enhanced46 Q14 source-order correction. Separate original KOEI branch.

Apply only to the extracted exact 18-part Enhanced46 parent (not NES Classic).
Parent ZIP SHA256 7eca762acd26410969f26d1964032c853a7eba792e020bb7c8144698b091181e.
This script patches ONE battle source file; reconciled legacy tests and the
new real-game regression are preserved in the Run004 candidate full ZIP.
"""
from pathlib import Path
from hashlib import sha256
import sys

BEFORE='235a0bea2aee0fedb095e7e07e41dc963336b6654fe9123950f8e318747f37ba'
AFTER='6b002d397eac834f881b39f5aeb89ca059754bcc52029f1a6adab0038be2da4d'
FUNCTION='''def ruler_release_destination(s, b, officer_id):
    """Q14 $8436 original legal refuge order for a captured defending ruler.

    Supplied eligible original slot (only if imported), ordered neighboring
    friendly provinces not under attack, then ordered friendly OR unowned
    neighbors, then the original ruler-owned linked HEAD. With no imported
    HEAD only a *unique* surviving distant province is unambiguous. Multiple
    remote holdings without recovered HEAD remain PARITY-OPEN rather than
    selecting a fabricated numeric/lexicographic destination.

    This pure query is shared by the button menu, CPU fate, and settlement;
    provenance for source attack flags is s['q14_source_attacked'] when present.
    """
    defender=b['defender']
    if s['factions'][defender].get('ruler_officer')!=officer_id:
        return None
    dst=b['dst']; owner=s['owners']
    slot=b.get('q14_source_supplied_slot')
    if slot is not None and slot!=dst and slot in owner and owner[slot] in (None,defender):
        return slot
    neighbors=s['neighbors'][dst]
    attacked=s.get('q14_source_attacked',{})
    for pid in neighbors:
        if owner.get(pid)==defender and not attacked.get(pid,False):
            return pid
    for pid in neighbors:
        if pid in owner and owner[pid] in (None,defender):
            return pid
    owned=[pid for pid,which in owner.items() if which==defender and pid!=dst]
    head=s['factions'][defender].get('q14_owned_head')
    if head is not None:
        if head not in owned:fail('BROKEN_Q14_OWNED_HEAD')
        return head
    if len(owned)==1:
        return owned[0]  # no linked-order ambiguity when only one survives
    return None  # Unknown list head; no false 'original' Release target.
'''

def apply(root):
    p=root/'third_branch/officer_battle_adapter.py'
    raw=p.read_bytes()
    if sha256(raw).hexdigest()!=BEFORE:
        raise SystemExit('ABORT: Enhanced46 member SHA mismatch')
    text=raw.decode('utf8')
    start=text.index('def ruler_release_destination(')
    end=text.index('\n\n\ndef cpu_ruler_disposition(',start)
    patched=(text[:start]+FUNCTION+text[end:]).encode('utf8')
    if sha256(patched).hexdigest()!=AFTER:
        raise SystemExit('ABORT: Enhanced Q14 output hash mismatch')
    p.write_bytes(patched)
    print('Enhanced46 Run004 Q14 source SHA256='+AFTER)

if __name__=='__main__':
    if len(sys.argv)!=2:
        raise SystemExit('Usage: run004_apply_enhanced_q14_to_original46.py <CLEAN_ENHANCED46_EXTRACTED_TREE>')
    apply(Path(sys.argv[1]))
