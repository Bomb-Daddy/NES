#!/usr/bin/env python3
"""EXEC0303 Win32 artifact acceptance gate (copyright-clean; no original ROM).

This is an artifact authenticity/QA gate, NOT original NES runtime parity,
physical Xbox acceptance, a first playable Spring, or a release approval.
Example:
  python verify_exec303_ci_artifact.py GK1_EXEC0303_WIN64_CI_QA_ARTIFACT.zip \
    --run-id 43710756716 --head-sha 13757389b5dde4a1194888022d4138b76aaecbc4 \
    --artifact-sha256 625622483b7e6dd4f225b8ba5a9ac1dcb39615952050c0e974c845bb0dcaf201
"""
import argparse
import hashlib
import io
import json
from pathlib import Path
import re
import sys
import unittest
import zipfile

MANIFEST_NAME = "GK1_EXEC0263_QA_ONLY_manifest.json"  # inherited Windows artifact naming
EXE_NAME = "GK1-Native-Research-1.99.0-win64.exe"
EXPECTED_MEMBERS = {MANIFEST_NAME, EXE_NAME}
SHA256_RE = re.compile(r"[0-9a-f]{64}\Z")
SHA40_RE = re.compile(r"[0-9a-f]{40}\Z")

class ArtifactError(ValueError):
    """Source/QA authenticity failure; caller must not report PASS."""


def _sha(raw):
    return hashlib.sha256(raw).hexdigest()


def verify_zip(raw, *, run_id, head_sha, artifact_sha256):
    """All-or-nothing independently pinned archive and manifest validation."""
    if not SHA40_RE.fullmatch(head_sha) or not SHA256_RE.fullmatch(artifact_sha256):
        raise ArtifactError("invalid caller-provided identity")
    if type(run_id) is not int or run_id < 1:
        raise ArtifactError("invalid run ID")
    if _sha(raw) != artifact_sha256:
        raise ArtifactError("artifact SHA256 mismatch")
    try:
        with zipfile.ZipFile(io.BytesIO(raw), "r") as z:
            info = z.infolist()
            if len(info) != 2 or {i.filename for i in info} != EXPECTED_MEMBERS:
                raise ArtifactError("unexpected, missing or duplicate artifact members")
            if any(i.is_dir() or i.flag_bits & 1 or i.file_size > 16_000_000
                   or i.compress_size > 16_000_000 for i in info):
                raise ArtifactError("unsafe or oversized artifact member")
            # zipfile.read also checks each member CRC, including the executable.
            meta_raw = z.read(MANIFEST_NAME)
            exe = z.read(EXE_NAME)
    except (zipfile.BadZipFile, OSError, RuntimeError, ValueError) as e:
        if isinstance(e, ArtifactError):
            raise
        raise ArtifactError("invalid or corrupted ZIP: " + str(e)) from e
    try:
        m = json.loads(meta_raw)
    except (ValueError, UnicodeDecodeError) as e:
        raise ArtifactError("invalid JSON acceptance manifest") from e
    if not isinstance(m, dict):
        raise ArtifactError("invalid manifest structure")
    required = {
        "schema": "GK1_EXEC0263_QA_ONLY",
        "github_sha": head_sha,
        "github_run_id": str(run_id),
        "installer_name": EXE_NAME,
        "tested_port_win32_ci": True,
        "native_ctest_passed": True,
        "winmain_software_controller_passed": True,
        "nsis_installed_executable_passed": True,
        "release_accepted": False,
        "physical_xbox_tested": False,
        "full_campaign_tested": False,
    }
    for name, value in required.items():
        if name not in m or type(m[name]) is not type(value) or m[name] != value:
            raise ArtifactError("untrusted manifest field: " + name)
    for field in ("installer_sha256", "built_exe_sha256", "installed_exe_sha256", "country_csv_sha256"):
        if not isinstance(m.get(field), str) or not SHA256_RE.fullmatch(m[field]):
            raise ArtifactError("missing/invalid SHA256: " + field)
    if _sha(exe) != m["installer_sha256"]:
        raise ArtifactError("installer hash mismatch")
    if m["installed_exe_sha256"] != m["built_exe_sha256"]:
        raise ArtifactError("installed executable differs from built executable")
    if not exe.startswith(b"MZ"):
        raise ArtifactError("installer missing Windows executable header")
    return {"status": "VERIFIED_STATIC_ARTIFACT_ONLY", "run_id": run_id,
            "github_sha": head_sha, "artifact_sha256": artifact_sha256,
            "installer_sha256": _sha(exe), "installer_bytes": len(exe),
            "win32_ctest_passed": True, "qa_only": True,
            "not_tested": ["original NES Mesen parity", "physical Xbox", "complete campaigns"]}


def _fixture(manifest=None, exe=b"MZ" + b"WindowsQA" * 31, extra=None):
    meta = {"schema":"GK1_EXEC0263_QA_ONLY", "github_sha":"a"*40,
            "github_run_id":"123", "installer_name":EXE_NAME,
            "installer_sha256":_sha(exe), "installed_exe_sha256":"b"*64,
            "built_exe_sha256":"b"*64, "country_csv_sha256":"c"*64,
            "tested_port_win32_ci":True, "native_ctest_passed":True,
            "winmain_software_controller_passed":True,
            "nsis_installed_executable_passed":True, "release_accepted":False,
            "physical_xbox_tested":False,"full_campaign_tested":False}
    if manifest:
        meta.update(manifest)
    bio = io.BytesIO()
    with zipfile.ZipFile(bio, "w", zipfile.ZIP_DEFLATED) as z:
        z.writestr(MANIFEST_NAME, json.dumps(meta).encode("utf-8"))
        z.writestr(EXE_NAME, exe)
        if extra:
            z.writestr(extra, b"unsafe")
    raw = bio.getvalue()
    return raw, {"run_id":123,"head_sha":"a"*40,"artifact_sha256":_sha(raw)}

class HostileArtifactTests(unittest.TestCase):
    def expect_reject(self, raw, kw):
        with self.assertRaises(ArtifactError):
            verify_zip(raw, **kw)

    def test_exact_positive(self):
        raw,kw = _fixture()
        self.assertEqual(verify_zip(raw, **kw)["status"],"VERIFIED_STATIC_ARTIFACT_ONLY")

    def test_wrong_archive_hash(self):
        raw,kw = _fixture(); kw["artifact_sha256"]="f"*64
        self.expect_reject(raw,kw)

    def test_wrong_head(self):
        raw,kw = _fixture(); kw["head_sha"]="9"*40
        self.expect_reject(raw,kw)

    def test_wrong_run(self):
        raw,kw = _fixture(); kw["run_id"]=124
        self.expect_reject(raw,kw)

    def test_missing_inner_hash(self):
        raw,kw = _fixture({"installer_sha256":"0"*64})
        self.expect_reject(raw,kw)

    def test_false_ci_claim(self):
        raw,kw = _fixture({"native_ctest_passed":False})
        self.expect_reject(raw,kw)

    def test_release_true_forbidden(self):
        raw,kw = _fixture({"release_accepted":True})
        self.expect_reject(raw,kw)

    def test_physical_xbox_true_forbidden(self):
        raw,kw = _fixture({"physical_xbox_tested":True})
        self.expect_reject(raw,kw)

    def test_campaign_true_forbidden(self):
        raw,kw = _fixture({"full_campaign_tested":True})
        self.expect_reject(raw,kw)

    def test_extra_zip_member(self):
        raw,kw = _fixture(extra="../unexpected")
        self.expect_reject(raw,kw)

    def test_duplicate_zip_member(self):
        raw,kw = _fixture()
        b=io.BytesIO(raw)
        with zipfile.ZipFile(b,"a") as z:
            import warnings
            with warnings.catch_warnings():
                warnings.simplefilter("ignore",UserWarning)
                z.writestr(EXE_NAME,b"MZspoof")
        raw=b.getvalue(); kw["artifact_sha256"]=_sha(raw)
        self.expect_reject(raw,kw)

    def test_bad_exe_header(self):
        raw,kw = _fixture(exe=b"No Windows executable here")
        self.expect_reject(raw,kw)

    def test_installed_differs(self):
        raw,kw = _fixture({"installed_exe_sha256":"e"*64})
        self.expect_reject(raw,kw)

    def test_invalid_json(self):
        raw,kw = _fixture()
        b=io.BytesIO()
        with zipfile.ZipFile(b,"w") as z:
            z.writestr(MANIFEST_NAME,b"{not json}")
            z.writestr(EXE_NAME,b"MZ")
        raw=b.getvalue(); kw["artifact_sha256"]=_sha(raw)
        self.expect_reject(raw,kw)

    def test_invalid_run_type(self):
        raw,kw = _fixture(); kw["run_id"]="123"
        self.expect_reject(raw,kw)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("artifact",nargs="?",type=Path)
    parser.add_argument("--run-id",type=int)
    parser.add_argument("--head-sha")
    parser.add_argument("--artifact-sha256")
    parser.add_argument("--self-test",action="store_true")
    args = parser.parse_args()
    if args.self_test:
        suite=unittest.defaultTestLoader.loadTestsFromTestCase(HostileArtifactTests)
        return 0 if unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful() else 1
    if not all([args.artifact,args.run_id,args.head_sha,args.artifact_sha256]):
        parser.error("artifact, --run-id, --head-sha, --artifact-sha256 required")
    try:
        print(json.dumps(verify_zip(args.artifact.read_bytes(),run_id=args.run_id,
          head_sha=args.head_sha,artifact_sha256=args.artifact_sha256),sort_keys=True,indent=2))
        return 0
    except (ArtifactError,OSError) as e:
        print("FAIL-CLOSED: "+str(e),file=sys.stderr)
        return 1

if __name__=="__main__":
    sys.exit(main())
