#!/usr/bin/env python3
"""EXEC0273: replay the exact EXEC0272 production app + linked test on Win32.
Stock first-Spring generation remains unimplemented; this is source provenance.
"""
import hashlib
import sys
from pathlib import Path

APP_OLD = "ee00ef0fc03676d09edfef9ca630acb9e605f128e5af5a66b1eafb2e86c2dc68"
APP_NEW = "e7c7e02ed35760234bf7ddc8738f3a0ac87699fb2f1b46ca40f3071df7207372"
TEST_OLD = "bc92f911454cf3479698c21b88bbd6ea306a052fcac9cff0e9fc4e0617272b1b"
TEST_NEW = "ca52f9386bef4600e93fcecdb55aa59f706660599ebb4787a37089ee382898ad"
APP_ANCHOR = b"    app->worlds[world_index].scenario_id!=(Gk1ScenarioId)(world_index+1u))\n"
APP_REPLACE = b"""    app->worlds[world_index].scenario_id!=(Gk1ScenarioId)(world_index+1u)||
    /* EXEC0272: the public batch entry is an ORIGINAL-TEMPLATE audit.
     * Controller labor planning and Enhanced preview can edit the mutable
     * PRE_SETUP world without performing genuine NES setup. Do not then
     * label its projected finances as source-original. The independent
     * finance-comparison screen already handles draft what-ifs. */
    memcmp(&app->worlds[world_index],
           &app->finance_source_baseline[world_index],
           sizeof app->worlds[world_index])!=0)
"""
TEST_ANCHOR = b" game->worlds[0].phase=(Gk1ScenarioPhase)2;\n"
TEST_INSERT = b""" /* EXEC0272 regression: the old game mislabeled port-edited PRE_SETUP
  * labor/Gold as an original-source finance batch. A real source identity
  * read must reject the drift without touching output or the game. */
 memcpy(backup,out,sizeof *out);
 game->worlds[0].countries[0].gold++;
 *saved=*game;
 CHECK(gk1_app_spring_batch_projection(game,0,out)==GK214_BAD_INPUT);
 CHECK(!memcmp(out,backup,sizeof *out));
 CHECK(!memcmp(game,saved,sizeof *game));
 game->worlds[0].countries[0].gold--;
 game->worlds[1].countries[26].food_workers--;
 *saved=*game;
 CHECK(gk1_app_spring_batch_projection(game,1,out)==GK214_BAD_INPUT);
 CHECK(!memcmp(out,backup,sizeof *out));
 CHECK(!memcmp(game,saved,sizeof *game));
 game->worlds[1].countries[26].food_workers++;
 /* Only canonical installed data can enter source-bounded projection. */
 CHECK(gk1_app_spring_batch_projection(game,0,out)==GK214_OK);
 CHECK(gk1_app_spring_batch_projection(game,1,out)==GK214_OK);
"""
def sha(b): return hashlib.sha256(b).hexdigest()
def apply(data, kind):
    old, new, anchor, insert = ((APP_OLD, APP_NEW, APP_ANCHOR, APP_REPLACE)
                                if kind == "app" else
                                (TEST_OLD, TEST_NEW, TEST_ANCHOR, TEST_INSERT+TEST_ANCHOR))
    if sha(data) != old or data.count(anchor) != 1 or insert in data:
        raise ValueError("EXEC0273 wrong inherited source/preimage or duplicate anchor")
    result = data.replace(anchor, insert, 1)
    if sha(result) != new:
        raise ValueError("EXEC0273 output does not match reviewed EXEC0272 C11 source")
    return result
def main():
    if len(sys.argv) < 3 or sys.argv[1] not in ("app", "test"):
        raise SystemExit("usage: gk273_stage_source.py app|test PATH [--self-test]")
    kind = sys.argv[1]
    path = Path(sys.argv[2])
    original = path.read_bytes()
    if "--self-test" in sys.argv:
        changed = apply(original,kind)
        rejected = 0
        for bad in (b"",changed,original+b"\n",original[:-1],
                    original.replace(b"\n",b"\r\n"),
                    original+b"\0",original+b"\xff"):
            try: apply(bad,kind)
            except ValueError: rejected += 1
            else: raise AssertionError("malformed source was accepted")
        assert rejected == 7
        print("EXEC0273 source and 7 negative controls PASS:", kind)
    else:
        changed = apply(original,kind)
        path.write_bytes(changed)
        if path.read_bytes() != changed: raise AssertionError("source readback mismatch")
        print("EXEC0273 exact source compiled candidate ready:", kind, sha(changed))
if __name__ == "__main__": main()
