# Genghis Khan I EXEC0033 — Four new native movement legality cases

This package contains copyright-clean executable Python *bounded behavioral contracts* and tests, not a general tactical engine. All four observations used real MesenCE NES controller inputs, the Director-approved **combined Shared BP + Rest + Demobilize + Fonzarelli** NES image (SHA-256 `4764d46359581be1e7d12bcb85bd375c84cd4497d1084d9d306a6e884afbe7c5`) and preserved original in-battle checkpoint (SHA-256 `e1b6266c963a48d736627812d024910b6d600c2ad8571813eb3b0999e0f2938b`). Private source snapshots never belong on public GitHub.

All tests used unit0 40 cavalry, source `(7,7)` destination `(8,7)` (natural terrain-byte 3, cost 4), Gold403, Food198, seed `c7c3f28f2363`. Controlled emulator RAM interventions were deliberately applied **after checkpoint restore, before controller movement**, without changing the ROM/checkpoint or saved campaign:

| Case | SRAM intervention | Native decision at `after_idle` | Troops/unit records | Completed-path word `$613E:$613F` | RNG |
|---|---|---|---|---|---|
| insufficient_mobility | `$6065`: 10→2 | rejected | unit0 stays 40 at (7,7), Mobility2 | 0 | unchanged |
| exact_mobility | `$6065`: 10→4 | accepted, auto finished | unit0 40→34 at (8,7), Mobility refilled10 | 1 | `a31e4ecd8fd1` |
| friendly_occupied | slot1 `$606D..$606E`: (6,9)→(8,7) | not blocked; combine-like outcome | unit0 40→74, slot1 40→0 with position retained; only slots0/1 changed | 2 | `a31e4ecd8fd1` |
| enemy_occupied | enemy slot10 `$60D0..$60D1`: origin→(8,7) | rejected | unit0 remains40 at (7,7), Mobility10; all 20 unit records unchanged | 0 | unchanged |

The friendly case could be a **merge/join, officer transfer, or some other route**. No source caller has been proved: do NOT program a universal friendly-merge formula, nor infer generic occupant and mobility rules outside these exact states. Specifically, the completion word increments *two* in the friendly case. For a future implementation, keep the existing movement costs and source casualty formula from EXEC0032 separate from *eligibility/occupancy dispatch*, and use this package as a fail-closed four-case parity oracle. The 4-case fixture explicitly rejects any unknown inputs.

## Verification

```bash
cd <extracted-public-source>
PYTHONWARNINGS=error GK1_EXEC0033_PRIVATE=/none python -m unittest discover -v -p test_movement_legality_contract.py
# 7 PASS + 4 expected SKIP because private licensed SRAM is absent.

PYTHONWARNINGS=error GK1_EXEC0033_PRIVATE=/path/to/private python audit_private_evidence.py --private /path/to/private --out /tmp/exec0033_receipt.json
PYTHONWARNINGS=error GK1_EXEC0033_PRIVATE=/path/to/private python -m unittest discover -v -p test_movement_legality_contract.py
# 11 PASS / 0 SKIP with exact private snapshots
```

Native local reproducibility on a working matched developer MesenCE with licensed ROM/checkpoint (separate; not in this package):

```bash
bash run.sh insufficient_mobility
bash run.sh exact_mobility
bash run.sh friendly_occupied
bash run.sh enemy_occupied
```

`run.sh` uses established cloud paths, so it is a **replay recipe**, not a portable launcher or Windows game installer. Four original runs exited 0 with `CASE_DONE`; all four produced seven source snapshot stages (28 total). The private ZIP contains SRAM/ZP and emulator logs; the public ZIP includes no ROM, save-state, SRAM or screenshot. Other NES stock-version behavior, Windows implementation, physical Xbox hardware, multi-step pathing, Divide, battle ends, AI and packaging were **NOT TESTED**.
