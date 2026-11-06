#!/usr/bin/env python3
"""Hash-guarded reproduction from *immutable* canonical Run012 full source.

Input: Run012 full ZIP path, directory with RUN013_test.py, destination ZIP path.
The only production modification is src/rotk2/battle/engine.py; the only
additional member is src/tests/test_run013_q09_native_bfbb_sign.py.
All legacy members and metadata are copied directly from the parent ZIP.
"""
import hashlib, pathlib, sys, zipfile

PARENT_SHA='5214b118fd350544fa677394233d3a0c1d1db7e9c8d8eec5c64e55a7bd63ad9e'
ORIG_ENGINE_SHA='8d35e169be9e28c46ffdfdbe54e4e33b7c1bf35b0cc7b291decea98e2ba1a832'
CHILD_SHA='28e4d7c0296621941b274ffcbc5d41651a1a76b0053ee45d35086fddb4b51c68'
FILE='src/rotk2/battle/engine.py'
NEW_TEST='src/tests/test_run013_q09_native_bfbb_sign.py'
ANCHOR='def q09_escape_score(movement:int, war:int, training:int, troops100:int,'
INSERT='''def q09_effective_movement_byte(movement11:int, training:int)->int:
    """Native hex $BFBB (hsovl2 $D582): 8-bit subtraction and BMI.

    Original movement byte +11 is compared against the training-derived
    floor by an 8-bit subtraction whose NEGATIVE flag, not unsigned carry or
    Python max(), governs whether to replace it. This matters for anomalous
    but representable original byte values >= 130. Not the UI's current MP.
    """
    if (type(movement11) is not int or not 0 <= movement11 <= 255 or
            type(training) is not int or not 0 <= training <= 255):
        raise ValueError('INVALID_Q09_SOURCE_BYTES')
    floor=(max(training-1, 0)//20)+2
    return floor if ((movement11-floor)&0x80) else movement11


'''
OLD='''    # Original native $BFBB raises ordinary movement byte to the trained floor.
    # Exact 8-bit signed subtraction behavior for arbitrary injected high bytes
    # is a distinct source-execution fixture still outstanding.
    movement=max(movement11, (max(officer.training-1,0)//20)+2)
'''
NEW='''    # Native $BFBB uses the 8-bit subtraction negative flag; unsigned max()
    # agrees for normal low values but differs for valid high source bytes.
    movement=q09_effective_movement_byte(movement11,officer.training)
'''
def sha(b):return hashlib.sha256(b).hexdigest()
def main(parent, testpath, output):
    parent,testpath,output=map(pathlib.Path,(parent,testpath,output))
    if sha(parent.read_bytes())!=PARENT_SHA:raise RuntimeError('BLOCKED: actual parent differs; do NOT patch')
    test=testpath.read_bytes()
    with zipfile.ZipFile(parent) as z:
        if z.testzip() is not None:raise RuntimeError('parent CRC fail')
        if NEW_TEST in z.namelist():raise RuntimeError('test already present; new parent?')
        orig=z.read(FILE)
        if sha(orig)!=ORIG_ENGINE_SHA:raise RuntimeError('engine blob mismatch; abort')
        source=orig.decode('utf-8')
        if source.count(ANCHOR)!=1 or source.count(OLD)!=1:raise RuntimeError('patch context mismatch')
        updated=source.replace(ANCHOR,INSERT+ANCHOR,1).replace(OLD,NEW,1).encode('utf-8')
        with zipfile.ZipFile(output,'w') as out:
            for info in z.infolist():
                out.writestr(info,updated if info.filename==FILE else z.read(info.filename))
            test_info=zipfile.ZipInfo(NEW_TEST,date_time=(2026,11,6,10,20,0))
            test_info.compress_type=zipfile.ZIP_DEFLATED
            test_info.external_attr=0o600 << 16
            out.writestr(test_info,test)
    with zipfile.ZipFile(parent) as original, zipfile.ZipFile(output) as result:
        assert result.testzip() is None
        assert len(result.namelist())==len(original.namelist())+1
        assert all(result.read(i.filename)==original.read(i.filename)
                   for i in original.infolist() if i.filename!=FILE)
    actual=sha(output.read_bytes())
    if CHILD_SHA!='REPLACE_CHILD_SHA' and actual!=CHILD_SHA:raise RuntimeError('child hash differs '+actual)
    print('verified parent',PARENT_SHA,'\nchild',actual,'\nentries',len(original.namelist())+1)
if __name__=='__main__':
    if len(sys.argv)!=4:raise SystemExit('usage: python RUN013_REBUILD.py Run012FullSource.zip RUN013_test.py OUT.zip')
    main(*sys.argv[1:])
