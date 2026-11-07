#!/usr/bin/env python3
"""Rebuild exact Enhanced Run040 from immutable Drive Run039 + GitHub regression.

Usage: python rebuild_run040_from_github.py <run039_complete.zip> <output_run040.zip>
Test file `test_run040_q11_former_subject_duel.py` must be beside this script.
Only test and one production module are changed; it fails closed on ancestry.
"""
import hashlib
import sys
import zipfile
from pathlib import Path

PARENT='e2a1581af46f900fe16cc311566a27946320689ffb99e8a0ad21e72278153935'
EXPECTED='19bd16739b8a168b3c5b5a0d5fbcf9f7032777c9967f4815ea1852e1f5e7308a'
MODULE='third_branch/officer_battle_adapter.py'
TEST='tests/test_run040_q11_former_subject_duel.py'
OLD="        if challenger.get('protected') or opponent.get('protected'):\n"
NEW='''        # Q11 hsovl52 / ai_eval_odds_b: the non-lethal duel ceilings also
        # apply to a former subject of the OTHER duelist's current ruler,
        # not solely to an original +3 protected officer.  Only explicit
        # source-backed switched/previous-liege metadata can activate this
        # branch; unknown officer history must not be invented from side.
        former_subordinate=(
            (challenger.get('switched') is True and
             challenger.get('original_faction')==(
                 b['defender'] if opponent['side']=='D' else b['attacker'])) or
            (opponent.get('switched') is True and
             opponent.get('original_faction')==(
                 b['defender'] if challenger['side']=='D' else b['attacker']))
        )
        if challenger.get('protected') or opponent.get('protected') or former_subordinate:
'''


def digest(p):
    h=hashlib.sha256()
    with p.open('rb') as f:
        for part in iter(lambda:f.read(1024*1024),b''):h.update(part)
    return h.hexdigest()


def main(src,dst):
    if digest(src)!=PARENT:raise ValueError('WRONG IMMUTABLE RUN039 PARENT')
    if dst.exists():raise FileExistsError('refusing to overwrite '+str(dst))
    test=Path(__file__).with_name(Path(TEST).name).read_bytes()
    with zipfile.ZipFile(src) as old,zipfile.ZipFile(dst,'w') as new:
        if old.testzip() is not None or len(old.namelist())!=557 or MODULE not in old.namelist() or TEST in old.namelist():
            raise ValueError('RUN039 CRC/MEMBERSHIP WRONG')
        for info in old.infolist():
            data=old.read(info.filename)
            if info.filename==MODULE:
                text=data.decode('utf-8')
                if text.count(OLD)!=1:raise ValueError('SOURCE GUARD MISSING/AMBIGUOUS')
                data=text.replace(OLD,NEW).encode('utf-8')
            new.writestr(info,data)
        info=zipfile.ZipInfo(TEST,(2026,11,7,20,0,0))
        info.compress_type=zipfile.ZIP_DEFLATED
        info.external_attr=(0o100644<<16)
        new.writestr(info,test)
    with zipfile.ZipFile(dst) as z:
        if len(z.namelist())!=558 or z.testzip() is not None:raise ValueError('ZIP CRC BAD')
    actual=digest(dst)
    if actual!=EXPECTED:raise ValueError('WRONG RUN040 HASH '+actual)
    print('RUN040 VERIFIED SHA256:',actual)


if __name__=='__main__':
    if len(sys.argv)!=3:sys.exit(__doc__)
    main(Path(sys.argv[1]),Path(sys.argv[2]))
