# Additive Move Base source suite

This is a separate19-method suite, added after the existing262-method package was validated. It does not change `run_package.py`, `PACKAGE_MANIFEST.json`, the original input manifest, or the original integration receipt. Run it independently from the extracted `tools` tree:

```sh
export GK_ROM='/private/Genghis Khan (USA).nes'
export GK_SRAM='/private/baseline.sram'
python3 batches/base_relocation/run_base.py --output ./base_relocation_results
```

Only those two private files are required. Their exact byte sizes and SHA-256 values are recorded in `SOURCE_PROVENANCE.json`; the runner verifies them before executing tests. It also verifies hashes of the nine executed source/dependency files. Missing inputs, wrong private hashes or altered source files return2. The suite passes only with process exit0, exactly one completed19-method footer and the plain unittest `OK` line. Truncated or skipped completion does not pass. Raw stdout/stderr and a receipt go to the selected output directory.

No new arithmetic adaptation was needed: `replay_base.py` and `test_base.py` are byte-identical to their authored originals. Their relative imports resolve to the existing portable logistics/trade/CPU dependencies, which already require `GK_ROM` and `GK_SRAM`. Originals are preserved under `tools/originals/batches/base_relocation`. `SOURCE_PROVENANCE.json` records both hashes and attribution. The exact `adaptations.patch` contains only the runtime-evidence report addendum; no executable diff is omitted.

The authored tests remain **VERIFIED_STATIC** bounded source replay. The report separately records the parent's subsequently captured **OBSERVED_RUNTIME** controlled transfer→Move Base→later country2 turn→Islam sale. The runtime captures are not inputs to these19 source methods and are not bundled as SRAM/images. No natural-conquest, Windows-port or physical-controller claim is implied.

`VALIDATION/` records an independent run from a clean temporary copy with only the two private-input environment variables. It also records rejection controls for a missing input, incorrect input hash, altered source hash and incomplete/failing unittest output. `PRIOR_PACKAGE_GUARD.json` fingerprints all119 files that existed before this addition; validation checks they remain unchanged. The original262-method receipt is preserved as historical evidence for its own exact package.
