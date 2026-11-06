#!/usr/bin/env python3
"""Rebuild Run021 from verified Run020 using the adjacent GitHub regression module."""
import sys, hashlib, zipfile
from pathlib import Path

PARENT_SHA='964d9ad55c50eaab944bf8f70ff257a230494b1ff6a799e152d88561b21ea345'
NEW_SHA='93a9acb7dee1819bd6f04a761b5b0aacbc315b4d8b92472269135b204b6e9eb3'
ENGINE='src/rotk2/battle/engine.py'
TEST='src/tests/test_run021_q09_middle_officer_unlink.py'
OLD='''        if origin.q09_officer_head==officer.id:
            successor=state.officers.get(officer.q09_officer_next)
            origin.q09_officer_head=(successor.id if successor is not None
                                    and successor.alive and successor.province_id==old
                                    and successor.id!=officer.id else None)
'''
NEW='''        # Q09 Run021: root select_via_69ea $50EF-$5121 unlinks from the
        # SOURCE province's linked officer list before select_officer_5154
        # inserts at destination. Run020 handled only departure of the head.
        # Follow only an explicitly known, resident, acyclic source chain:
        # a dictionary traversal or numeric officer sort is not a source link.
        successor=state.officers.get(officer.q09_officer_next)
        valid_successor=(successor.id if successor is not None
                         and successor.alive and successor.province_id==old
                         and successor.id!=officer.id else None)
        if origin.q09_officer_head==officer.id:
            origin.q09_officer_head=valid_successor
        else:
            current_id=origin.q09_officer_head
            visited=set()
            while current_id is not None and current_id not in visited:
                visited.add(current_id)
                current=state.officers.get(current_id)
                if current is None or not current.alive or current.province_id!=old:
                    break  # ambiguous/corrupt imported head: do not guess
                next_id=current.q09_officer_next
                if next_id==officer.id:
                    current.q09_officer_next=valid_successor
                    break
                current_id=next_id
'''
def sha(path): return hashlib.sha256(path.read_bytes()).hexdigest()
def main():
    parent=Path(sys.argv[1]); output=Path(sys.argv[2]); test=Path(sys.argv[3])
    assert sha(parent)==PARENT_SHA, 'WRONG_PARENT'
    assert test.name==Path(TEST).name, 'WRONG_TEST'
    with zipfile.ZipFile(parent) as source, zipfile.ZipFile(output,'w') as dest:
        dest.comment=source.comment
        for info in source.infolist():
            blob=source.read(info.filename)
            if info.filename==ENGINE:
                decoded=blob.decode('utf-8')
                assert decoded.count(OLD)==1, 'PATCH_PRECONDITION_FAILED'
                blob=decoded.replace(OLD,NEW,1).encode('utf-8')
            dest.writestr(info,blob)
        dest.writestr(zipfile.ZipInfo(TEST,date_time=(2026,11,6,17,0,0)),test.read_bytes(),compress_type=zipfile.ZIP_DEFLATED,compresslevel=9)
    with zipfile.ZipFile(output) as z:
        assert z.testzip() is None and len(z.namelist())==166
    assert sha(output)==NEW_SHA, 'REBUILT_SOURCE_SHA_MISMATCH'
    print('PASS',NEW_SHA)
if __name__=='__main__': main()
