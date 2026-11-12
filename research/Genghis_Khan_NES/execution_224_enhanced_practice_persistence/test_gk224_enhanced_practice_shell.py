#!/usr/bin/env python3
"""Real Linux X11/controller save/restore, non-overwrite, no stock claims."""
import hashlib
import os
from pathlib import Path
import subprocess
import sys
import tempfile

def main():
    if len(sys.argv)!=2:
        return 2
    with tempfile.TemporaryDirectory(prefix="gk224_x11_") as root:
        path=Path(root)/"receipt.gk24"
        env=os.environ.copy()
        env["GK1_ENHANCED_PRACTICE_SAVE_FILE"]=str(path)
        command=["xvfb-run","-a","-s","-screen 0 1280x720x24",
                 sys.argv[1],"--smoke-enhanced-practice-save","120"]
        outcome=subprocess.run(command,env=env,text=True,capture_output=True,timeout=45)
        if outcome.returncode or not path.is_file():
            print("FAIL controller persistence path:",outcome.returncode,outcome.stdout,outcome.stderr)
            return 1
        data=path.read_bytes()
        if len(data)!=80 or data[:8]!=b"GK1P224!" or hashlib.sha256(data[:48]).digest()!=data[48:]:
            print("FAIL saved receipt format",len(data))
            return 1
        again=subprocess.run(command,env=env,text=True,capture_output=True,timeout=45)
        if again.returncode==0 or data!=path.read_bytes():
            print("FAIL: prior valid save overwritten or reuse falsely reported success")
            return 1
        print("EXEC0224 real Xvfb 1280x720 Xbox-mapped X SAVE/Y LOAD PASS; nonoverwrite refusal PASS; physical Xbox/Win32 NOT RUN")
    return 0
if __name__=="__main__":
    raise SystemExit(main())
