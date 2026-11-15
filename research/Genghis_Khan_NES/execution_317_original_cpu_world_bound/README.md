# GK1 EXEC0317 â World-bound original stock CPU claim admission

## Defect and correction
The inherited EXEC0316 `gk1_app_original_cpu_claims_parse` checked Classic PRE_SETUP and source-ready state but admitted an otherwise well-formed **World**-scenario CPU claim transcript against a **Mongolian** active campaign, and vice versa. This is not source-authentic association. Source research receipts are claims until correlated with private original NES/Mesen evidence.

This additive patch stages after the exact current EXEC0316 code. It changes only the C11 wrapper to parse into an isolated heap object, check the claim's scenario against the current active scenario, atomically reject wrong-campaign claims (GK316_STAGE, untouched caller output), and preserve all previous source and fail-closed behavior. No fabricated first Spring, no new orders, no modification of 14/27 source worlds or user saves. `test_gk317_world_bound.c` exercises two-world acceptance/refusal and profile refusal on the actual Gk1App. `stage317.py`, `test_stage317.py`, `repair317_provenance.py`, `test_repair317_provenance.py` provide source-hash-pinned additive integration, rollback, and strict preservation of inherited Win32 source-provenance checks.

## Local QA
- `python test_stage317.py`: **9/9 PASS** against controlled source fixture, duplicate/missing inherited source and rollback.
- `python test_repair317_provenance.py`: **7/7 PASS** against positive and tampered historical-gate cases.
- Full C11 app Linux/Win32 test outcomes are NOT claimed until actual compiled build evidence exists.
- No fresh or archived Mesen run; original ROM and 83-byte records remain private.
- Physical Xbox controller and full two campaigns NOT RUN.

## Reproduce on complete staged application
1. Source-stage current complete C11 application through EXEC0315, then apply verified `stage316.py` in `execution_316_stock_cpu_trace_ingest`.
2. Run `python stage317.py path/to/current-gk1/execution_199`, then `python repair317_provenance.py path/to/current-gk1/execution_199` on Win32 build path after the prior hostile provenance repair.
3. Build full app; CTest test `GK1_317_OriginalCpuClaimsAreWorldBoundWithoutOrders`; independently inspect 14/27 source and save invariants. Keep source-verified original first-Spring generation OPEN.

## Publication
All files here are copyright-clean original C11/Python source and QA. No source ROM, private trace, raw NES data, Mesen captures or licensed assets. Publishing or passing tests does not constitute acceptance of a playable game or any other agent's execution acknowledgement.
