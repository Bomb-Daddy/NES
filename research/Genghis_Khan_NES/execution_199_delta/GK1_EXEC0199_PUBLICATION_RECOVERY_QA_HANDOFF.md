# GK1 EXEC0199 — Public source publication recovery and negative-control QA

Date: 2026-11-11. **Not a new game build.** This is a reproducibility repair on top of the currently published native C11 execution_199 source handoff. It does not supersede that game's tested logic, predecessor, or private stock evidence.

## Verified defect and direct repair

The two public GitHub files under `research/Genghis_Khan_NES/execution_199_delta/` were present as empty 0-byte Git blobs before this execution:

- `GK1_EXEC0199_Complete_Integration_From0198.patch`
- `GK1_EXEC0199_Source_SHA256_Manifest_v1.00.json`

The independently published Drive text copies have 35,247 and 62,569 bytes respectively. The exact content has now been written into the original GitHub paths using the existing-file SHA guard, followed by an exact readback. Git blob hashes now match local raw-byte `git hash-object` output:

- Patch blob `ff5250f34866f6072881caabcfd59b82693f7afa` — commit `871653d772553ddeea3304690ab0e91ff5b40011`
- Manifest blob `7a7481b01757f17c3c03260e242683b94b865030` — commit `62452d4d5eb8ca46215067a65a0e2b359c028bd3`

## Executable QA added

`verify_public_release.py` validates an existing patch and the exact EXEC0199 631-file SHA-256 manifest; optionally validates every source file against a provided complete current source root. `test_public_release.py` includes seven tests, positive and adversarial. Invoking `python test_public_release.py` passed **7/7**, including empty patch, truncated patch, invalid manifest, hash tampering, path traversal, and absent full source-tree rejection.

The real public patch and manifest pass the artifact-only mode: **631 declared source digests; 35,247 byte patch; zero errors**. No attempt was made to claim a full source-tree parity pass when the current complete EXEC0198/0199 native source tree is not locally recoverable. The last completely mounted historical archive in this workspace is EXEC0196, not EXEC0199, and MUST NOT be used as a replacement game release.

The EXEC0198 integration patch was independently recovered from GitHub and its exact Git blob `406948e1015a7d93149987cee2c57c8979ff1e7a` verified. **EXEC0197 complete native source / exact integration patch is not currently published in the accessible Drive folder or GitHub tree.** Reconstruct it or recover its exact source before asserting independent rebuild of EXEC0199 and before issuing an EXEC0200 full native implementation based on it. Prior reports of 162/162 build tests for EXEC0199 are inherited, not newly rerun here.

## Fidelity labels

- **VERIFIED_STATIC:** GitHub publication defect, hash-exact repair, 631-file manifest schema and negative controls.
- **OBSERVED_RUNTIME_STOCK:** no new original Mesen gameplay; previous fresh reset and archived-restored claims remain separate.
- **TESTED_PORT_LINUX:** QA Python verifier only. No native C11 game build in this recovery run.
- **NOT RUN:** native Win32/NSIS, physical Xbox, source-authentic first Spring, campaigns.

Original stock ROM, screenshots, Mesen states, private 83-byte country captures, and other licensed assets are intentionally excluded. Recurring development must remain active.
