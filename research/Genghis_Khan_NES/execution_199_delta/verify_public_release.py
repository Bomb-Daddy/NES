#!/usr/bin/env python3
"""Fail-closed offline verification of GK1 numbered public source handoffs.

Copyright-clean: no ROM bytes, NES RAM, screenshots or private checkpoints.
Checks recovered unified patch, SHA-256 manifest, optional complete source tree.
"""
import argparse
import hashlib
import json
import pathlib
import re
import sys

SHA256 = re.compile(r"[0-9a-f]{64}\Z")


def audit(patch_path, manifest_path, source_root=None):
    errors = []
    patch = pathlib.Path(patch_path).read_bytes()
    if not patch or len(patch) < 200 or not patch.startswith(b"--- a/"):
        errors.append("patch: missing, truncated, or not a unified diff")
    elif (b"+++ b/gk199_classic_tax_undo.c" not in patch or
          b"+++ b/gk1_native_shell.c" not in patch):
        errors.append("patch: essential source/controller changes missing")
    try:
        m = json.loads(pathlib.Path(manifest_path).read_text(encoding="utf-8"))
    except (ValueError, UnicodeError) as e:
        return {"ok": False, "errors": errors + [f"manifest invalid: {e}"]}
    if not isinstance(m, dict):
        errors.append("manifest: expected object")
        return {"ok": False, "errors": errors}
    if str(m.get("execution")) != "0199" or m.get("version") != "1.99.0-dev1":
        errors.append("manifest: wrong release/version")
    entries = m.get("file_sha256")
    if not isinstance(entries, dict) or len(entries) != 631 or m.get("source_files") != len(entries):
        errors.append("manifest: expected exactly 631 declared source files")
    else:
        for name, digest in entries.items():
            path = pathlib.PurePosixPath(name)
            if (not name or path.is_absolute() or ".." in path.parts or
                not isinstance(digest, str) or SHA256.fullmatch(digest) is None):
                errors.append(f"manifest: invalid path/digest {name!r}")
                continue
            if source_root:
                f = pathlib.Path(source_root).joinpath(*path.parts)
                if not f.is_file():
                    errors.append(f"source absent: {name}")
                elif hashlib.sha256(f.read_bytes()).hexdigest() != digest:
                    errors.append(f"source hash mismatch: {name}")
    changed = m.get("changed_paths")
    if (not isinstance(changed, list) or len(changed) != 15 or
        not {"gk199_classic_tax_undo.c", "gk1_app_state.c", "gk1_native_shell.c"}.issubset(set(changed))):
        errors.append("manifest: invalid changed-source inventory")
    return {"ok": not errors, "errors": errors,
            "release": m.get("execution"), "manifest_files": len(entries) if isinstance(entries, dict) else 0,
            "patch_bytes": len(patch), "patch_sha256": hashlib.sha256(patch).hexdigest(),
            "source_tree_verified": source_root is not None}


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--patch", required=True, type=pathlib.Path)
    p.add_argument("--manifest", required=True, type=pathlib.Path)
    p.add_argument("--source-root", type=pathlib.Path)
    args = p.parse_args(argv)
    result = audit(args.patch, args.manifest, args.source_root)
    print(json.dumps(result, indent=2, sort_keys=True))
    return 0 if result["ok"] else 1


if __name__ == "__main__":
    sys.exit(main())
