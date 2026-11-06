import hashlib,io,json,os,pathlib,unittest,zipfile
from gk1_join_timeline import *

PRIVATE=pathlib.Path(os.getenv('GK1_EXEC0037_PRIVATE','/nonexistent'))
class AuditTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.raw=PRIVATE.read_bytes() if PRIVATE.is_file() else None
    def require(self):
        if self.raw is None: self.skipTest('private licensed source archive unavailable')
    def mutate(self,path,fn):
        self.require()
        src=zipfile.ZipFile(io.BytesIO(self.raw))
        dst=io.BytesIO()
        with zipfile.ZipFile(dst,'w',compression=zipfile.ZIP_DEFLATED) as z:
            for name in src.namelist():
                content=src.read(name)
                if name==path: content=fn(content)
                z.writestr(name,content)
        return dst.getvalue()
    def test_original_full_audit(self):
        self.require(); r=inspect_archive(self.raw)
        self.assertEqual((r['cases'],r['snapshots']),(4,28))
        self.assertEqual(r['units_first_observed']['mover_soldiers'],74)
        self.assertEqual(r['word_613e_first_observed'],2)
    def test_sha_guard(self):
        self.require()
        with self.assertRaisesRegex(EvidenceError,'ZIP SHA256'):
            inspect_archive(self.raw+b'garbage')
    def test_bad_crc_or_snapshot_hash(self):
        self.require()
        target='private/friendly_occupied/after_attempt.sram'
        out=self.mutate(target,lambda b:b[:0x60]+bytes([b[0x60]^1])+b[0x61:])
        with self.assertRaisesRegex(EvidenceError,'hash mismatch'):
            inspect_archive(out,False)
    def test_wrong_rom_profile(self):
        self.require(); p='private/friendly_occupied/after_attempt.json'
        def m(x):
            j=json.loads(x);j['rom_sha256']='0'*64;return json.dumps(j).encode()
        with self.assertRaisesRegex(EvidenceError,'oracle'):
            inspect_archive(self.mutate(p,m),False)
    def test_wrong_phase_label(self):
        self.require();p='private/friendly_occupied/after_idle.json'
        def m(x):
            j=json.loads(x);j['phase']='after_attempt';return json.dumps(j).encode()
        with self.assertRaisesRegex(EvidenceError,'phase or case'):
            inspect_archive(self.mutate(p,m),False)
    def test_late_scratch_negative(self):
        self.require();p='private/friendly_occupied/after_idle.sram'
        def m(x): return x[:0x14c]+bytes([14])+x[0x14d:]
        # Recompute the metadata checksum to ensure timing logic independently fails.
        replacement=m(zipfile.ZipFile(io.BytesIO(self.raw)).read(p))
        dst=io.BytesIO()
        src=zipfile.ZipFile(io.BytesIO(self.raw))
        with zipfile.ZipFile(dst,'w') as z:
            for n in src.namelist():
                value=src.read(n)
                if n==p:value=replacement
                if n==p[:-5]+'.json':
                    j=json.loads(value);j['sram_sha256']=sha(replacement);value=json.dumps(j).encode()
                z.writestr(n,value)
        with self.assertRaisesRegex(EvidenceError,'late scratch614c'):
            inspect_archive(dst.getvalue(),False)
    def test_missing_stage(self):
        self.require()
        target='private/friendly_occupied/after_attempt.zp'
        src=zipfile.ZipFile(io.BytesIO(self.raw)); buf=io.BytesIO()
        with zipfile.ZipFile(buf,'w') as z:
            for n in src.namelist():
                if n!=target:z.writestr(n,src.read(n))
        with self.assertRaisesRegex(EvidenceError,'missing/invalid stage'):
            inspect_archive(buf.getvalue(),False)
    def test_hash_constant(self):
        self.assertEqual(len(ROM_SHA),64); self.assertEqual(len(CHECKPOINT_SHA),64)
        self.assertEqual(len(PRIVATE_ZIP_SHA),64)
    def test_unit_reader(self):
        raw=bytearray(8192);raw[0x60:0x66]=bytes([0x12,0x34,7,8,9,10])
        self.assertEqual(unit(raw,0),{'soldiers':0x3412,'x':7,'y':8,'mobility':10})
    def test_word_little_endian(self):
        raw=bytearray(8192);raw[0x13e:0x140]=bytes([1,2]);self.assertEqual(word(raw),513)
    def test_source_trace_script_has_original_pc_and_vm(self):
        content=(pathlib.Path(__file__).parent/'mesen_join_watch.lua').read_text()
        for token in ('0xF1C8','0xF1CD','emu.convertAddress','0x6060','0x6075','0x613e','0x614c','0x614e','vm_source_prg_offset','unit1'):
            self.assertIn(token,content)
        self.assertNotIn('TESTED_PORT',content)
if __name__=='__main__':unittest.main()
