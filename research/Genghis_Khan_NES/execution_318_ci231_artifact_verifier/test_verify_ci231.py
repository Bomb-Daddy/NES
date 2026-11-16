#!/usr/bin/env python3
"""Executable positive + hostile regression tests on actual EXEC0317 GitHub artifact."""
import io
import json
from pathlib import Path
import unittest
import zipfile
import verify_ci231 as v

ARTIFACT=Path(__file__).resolve().parent/'GK1_EXEC0317_WIN64_CI231_QA_ONLY_NOT_RELEASE.zip'

class ArtifactTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.good=ARTIFACT.read_bytes()
        with zipfile.ZipFile(io.BytesIO(cls.good)) as z:
            cls.installer=z.read(v.INSTALLER_NAME)
            cls.manifest=json.loads(z.read(v.MANIFEST_NAME))

    def repack(self,manifest=None,installer=None,name=None,extra=None,duplicate=False):
        bio=io.BytesIO()
        with zipfile.ZipFile(bio,'w',compression=zipfile.ZIP_DEFLATED) as z:
            z.writestr(name or v.INSTALLER_NAME, self.installer if installer is None else installer)
            z.writestr(v.MANIFEST_NAME, json.dumps(self.manifest if manifest is None else manifest).encode())
            if extra:
                z.writestr(extra,b'REJECT')
            if duplicate:
                z.writestr(v.MANIFEST_NAME,b'{}')
        return bio.getvalue()

    def test_01_actual_verified(self):
        evidence=v.validate_archive_bytes(self.good)
        self.assertEqual(evidence['verification'],'PASS')
        self.assertFalse(evidence['release_accepted'])
        self.assertFalse(evidence['physical_xbox_tested'])

    def test_02_zip_byte_corruption(self):
        raw=bytearray(self.good);raw[len(raw)//3]^=1
        with self.assertRaises(v.Rejected):v.validate_archive_bytes(raw)

    def test_03_repacked_valid_zip_rejected_by_pin(self):
        with self.assertRaises(v.Rejected):v.validate_archive_bytes(self.repack())

    def test_04_altered_installer_even_without_zip_pin(self):
        with self.assertRaises(v.Rejected):
            v.validate_archive_bytes(self.repack(installer=self.installer+b'X'),enforce_artifact_pin=False)

    def test_05_wrong_git_commit(self):
        m=self.manifest.copy();m['github_sha']='0'*40
        with self.assertRaises(v.Rejected):v.validate_archive_bytes(self.repack(m),enforce_artifact_pin=False)

    def test_06_wrong_run(self):
        m=self.manifest.copy();m['github_run_id']='43767261827'
        with self.assertRaises(v.Rejected):v.validate_archive_bytes(self.repack(m),enforce_artifact_pin=False)

    def test_07_false_release_acceptance(self):
        m=self.manifest.copy();m['release_accepted']=True
        with self.assertRaises(v.Rejected):v.validate_archive_bytes(self.repack(m),enforce_artifact_pin=False)

    def test_08_false_physical_xbox_claim(self):
        m=self.manifest.copy();m['physical_xbox_tested']=True
        with self.assertRaises(v.Rejected):v.validate_archive_bytes(self.repack(m),enforce_artifact_pin=False)

    def test_09_false_full_campaign_claim(self):
        m=self.manifest.copy();m['full_campaign_tested']=True
        with self.assertRaises(v.Rejected):v.validate_archive_bytes(self.repack(m),enforce_artifact_pin=False)

    def test_10_wrong_flag_type(self):
        m=self.manifest.copy();m['native_ctest_passed']='true'
        with self.assertRaises(v.Rejected):v.validate_archive_bytes(self.repack(m),enforce_artifact_pin=False)

    def test_11_missing_installer(self):
        with self.assertRaises(v.Rejected):v.validate_archive_bytes(self.repack(name='wrong.exe'),enforce_artifact_pin=False)

    def test_12_extra_private_rom_disallowed(self):
        with self.assertRaises(v.Rejected):v.validate_archive_bytes(self.repack(extra='private.nes'),enforce_artifact_pin=False)

    def test_13_duplicate_member_disallowed(self):
        with self.assertWarns(UserWarning):bad=self.repack(duplicate=True)
        with self.assertRaises(v.Rejected):v.validate_archive_bytes(bad,enforce_artifact_pin=False)

    def test_14_duplicate_manifest_key_disallowed(self):
        with self.assertRaises(v.Rejected):v.strict_json(b'{"x":0,"x":1}')

    def test_15_wrong_installed_exe_identity(self):
        m=self.manifest.copy();m['installed_exe_sha256']='f'*64
        with self.assertRaises(v.Rejected):v.validate_archive_bytes(self.repack(m),enforce_artifact_pin=False)

    def test_16_missing_manifest_gate(self):
        m=self.manifest.copy();m.pop('nsis_installed_executable_passed')
        with self.assertRaises(v.Rejected):v.validate_archive_bytes(self.repack(m),enforce_artifact_pin=False)

    def test_17_unexpected_manifest_gate(self):
        m=self.manifest.copy();m['full_campaign_verified']=True
        with self.assertRaises(v.Rejected):v.validate_archive_bytes(self.repack(m),enforce_artifact_pin=False)

    def test_18_non_object_manifest_rejected(self):
        with self.assertRaises(v.Rejected):v.strict_json(b'[]')

if __name__=='__main__':unittest.main(verbosity=2)
