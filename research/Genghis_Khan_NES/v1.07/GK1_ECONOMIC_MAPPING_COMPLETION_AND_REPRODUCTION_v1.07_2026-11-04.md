# GK1 economic mapping — completion and reproduction v1.07

Canonical rendered copy: [GitHub](https://github.com/Bomb-Daddy/NES/blob/main/research/Genghis_Khan_NES/v1.07/GK1_ECONOMIC_MAPPING_COMPLETION_AND_REPRODUCTION_v1.07_2026-11-04.md). Relative document links resolve there or in a local checkout.

**Record:** GK1-ECON-CLOSE-0007. **Author/owner:** Zhuge Liang / Codex. **Recipients:** Director; shared reference for Nobunaga and Cao Cao. **Date:** 2026-11-04 UTC. **Execution status: PASS VERIFIED. Publication readback is recorded separately below.**

## Delivered research scope

This edition completes the domestic, Trade, seasonal, logistics, scenario-economic data, yearly ROI and identified AI-economic mapping batches listed in v1.06 section9. It provides original formulas, integer behavior, state addresses, data and observed-game acceptance evidence. It does not turn the separate167-obligation full-game register into a claim of completed campaigns, battles, dynasty, espionage, tribute or a Windows port. Mixed-scope rows preserve those additional obligations.

The maintained specification is [Economic Mapping v1.07](GK1_ECONOMIC_MAPPING_v1.07_2026-11-04.md). [Economic batch register](GK1_ECONOMIC_BATCH_COMPLETION_REGISTER_v1.07.csv) records owner/evidence/status. [All167 original requirements](GK1_COMPLETE_167_PROOF_MATRIX_v1.07_2026-11-04.csv) retain all19 earlier columns and add the current evidence and separate economic-scope status.

## Authority and original inputs

Director instructions control. The current authority/supersession register was re-read2026-11-04; its latest stored modification was2026-11-02 and introduced no conflicting Genghis economic instruction. Keep this research separate from both ROTK II ports and the custom KOEI economy.

- Private USA NES ROM:262160 bytes; SHA256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`.
- Native Linux Mesen binary: SHA256 `f02685dafe559addf8dd8d769621a89777137d0c4592436aed65694a7e740676`.
- Standardized Spring1175 SRAM:8192 bytes; SHA256 `dd22847140cc0397ab8f42e29f1832681628ed52bfa17e95041c288afc4ef773`.
- Original Spring1175 full-machine checkpoint: SHA256 `a3838705c58010a11cac37ae2d8f446cccf29aa7b904f41b199a604a98245cce`.
- USA manual used for names: SHA256 `ecc2150b0ef43861b3590fa8d1671b99fe5f486894b1289836eb77265f49d0ae`. Printedp21/PDFp24 identifies the item icons.

The public repository contains authored text, numeric findings, test source and hashes. ROM, saved memory, checkpoint files, original screens and manual pages remain private.

## Reproduction

### Original-instruction package

Read [tools/README.md](tools/README.md) and supply the exact private inputs in its fingerprint manifest. Run from that tools directory:

```bash
/usr/bin/python3 run_package.py --output ./results
/usr/bin/python3 batches/base_relocation/run_base.py --output ./base_results
```

The first expects262 completed unittest methods and15 setup cases; the additive Base suite expects19 methods. Both require exact input hashes and proper completed output/exit. These checks execute selected original instructions or compare actual captures; they are not281 complete original-game playthroughs. Root and independent reviewer each reran them from clean copies.

### Native Mesen

The [restored emulator bundle](https://drive.google.com/file/d/10a4CW22GYVbOOwAYx4MWPZ1ED496HzjD/view) and [setup guide](https://drive.google.com/file/d/1iu5Vpd1A0p2n6zeY0VIqUYnivx_M1H5d/view) establish the executable environment. The new evidence archive preserves the exact patched bridge and controller scripts. Its root layout retains `batches/runtime`, `recovery/static_dependencies`, archived v1.02 captures and the v1.06 baseline evidence. Restore Mesen/Xvfb separately and privately provide the ROM. Use fresh output paths and copy required checkpoint inputs; never overwrite earlier evidence.

```bash
export PATH="$PWD/recovery/xvfb/usr/bin:$PATH"
export LD_LIBRARY_PATH="$PWD/recovery/xvfb/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
export GK_ROM="$PWD/private/Genghis Khan (USA).nes"
export GK_TRACE_OUTPUT="$PWD/batches/runtime/new_trace.jsonl"
export GK_EVENT_LOG=events_new.jsonl
/usr/bin/python3 -u batches/runtime/research_session_v107.py batches/runtime/new_replay
```

Send JSON actions from the source scripts/readable action log. First step180 frames, then restore/run a batch. Replay either the top-level actions with the exact case-script revision, or its low-level recorded requests; **never both**. The evidence archive preserves original and corrected scripts. A later corrected script is not falsely described as the exact script behind an earlier failed attempt. Native step/frame counters and restored CPU clocks are different timelines; identify cases by log ordinal and capture label.

### Independent native evidence audit

From the extracted evidence root:

```bash
/usr/bin/python3 batches/native_verification/verify_native.py \
  --evidence-root batches/runtime/final_replay \
  --historic-root batches/runtime/live \
  --boundary-root batches/runtime/boundary_completion \
  --require-complete-log
```

Fresh main cases require their own raw logged memory bindings. Historic annual context remains separately labelled; it cannot silently fill a missing fresh case. Boundary cases select the latest explicit rerun for each unique case, retaining superseded attempts. The verifier checks formulas and byte hashes independently and deliberately rejects both a modified capture and a semantically wrong capture whose digest was recomputed.

## Failure recovery and acceptance

The final report preserves initial navigation errors, a misentered Tax100, incomplete earlier logs, a timed-out emulator session, pending original treaty screens, restore waits and the incorrect initial message breakpoint. The repeated-gift continuation initially entered Assign Labor; correcting menu exit then passed the exact repeated-gift case. One boundary prompt lacked a historical log binding; its explicit rerun has matching state and complete provenance. These are recovered investigation failures, not erased records or open mechanic defects.

The original Extra Tax refusal was reached at bytecodeEB5D with argument9DC0 and was visually captured at the short-frame endpoint. The full-year levy ledger confirms Autumn resets the flag, actual reuse succeeds then, and following Spring does not reset it. The second natural Mongol year's Food346→214 loss was observed inside the original theft path and independently replayed with the same final RNG. Other treaty income is separately accounted.

Final evidence hashes, verification counts and actual publication links are recorded in the publication receipt. Publication proves the files were delivered and read back; it does not claim Nobunaga/Cao Cao read or implemented them. No new implementation order or partial economic handoff is created by this closeout.

## Verified private evidence delivery

The large single-file upload timed out; the same closed archive was delivered as12 valid ZIP volumes. All12 were downloaded from Drive, their wrapper and payload hashes were checked, then the complete archive was reconstructed and every one of9,229 manifest files was verified. No evidence was removed to fit an upload limit.

Archive: `GK1_Economic_Evidence_v1.07_2026-11-04.zip`;97,949,707 bytes; SHA256 `d8b8290fa9195c9b99137e8c1f0fe6cf09e0494b728553a5236217a27682e0c6`.

| Volume | Bytes | Verified Drive file |
|---:|---:|---|
|1|8388804|[GK1_Evidence_v1.07_volume01.zip](https://drive.google.com/file/d/1t2ysBjVGecdwuLndUMujKNc4edagHXJs/view)|
|2|8388804|[GK1_Evidence_v1.07_volume02.zip](https://drive.google.com/file/d/1n7sGut9QzDAunRDEf35dl1Ki4rZNrguZ/view)|
|3|8388804|[GK1_Evidence_v1.07_volume03.zip](https://drive.google.com/file/d/1W2asGbC7gD4pQFrvxCc8MTuw-4aDeige/view)|
|4|8388804|[GK1_Evidence_v1.07_volume04.zip](https://drive.google.com/file/d/1F1vUjuQgw-ed_Sf2BPt6l_NVlw_7vLHW/view)|
|5|8388804|[GK1_Evidence_v1.07_volume05.zip](https://drive.google.com/file/d/19gk0H6l8c6y2KcH3M7Ol8KA2KqIR3m8l/view)|
|6|8388804|[GK1_Evidence_v1.07_volume06.zip](https://drive.google.com/file/d/1O5hZEia2TMnoZk1Qn9o8-g1v8vDW4KPs/view)|
|7|8388804|[GK1_Evidence_v1.07_volume07.zip](https://drive.google.com/file/d/1bjEBj69-3_k_5p8uoS7l4UbYKfe0IsjF/view)|
|8|8388804|[GK1_Evidence_v1.07_volume08.zip](https://drive.google.com/file/d/1S1NxU4N6Pj2zalIR03pjM1aPreNlrAdq/view)|
|9|8388804|[GK1_Evidence_v1.07_volume09.zip](https://drive.google.com/file/d/1LTYXP32v6DPPdJxBdepmyMEud_1da3pi/view)|
|10|8388804|[GK1_Evidence_v1.07_volume10.zip](https://drive.google.com/file/d/1tp1mEeQ8axof8fHY0UCoYk9qjbM9Xm17/view)|
|11|8388804|[GK1_Evidence_v1.07_volume11.zip](https://drive.google.com/file/d/1ok1QctCWG_kqy5fCDD35kvH3gvpGS5cM/view)|
|12|5675215|[GK1_Evidence_v1.07_volume12.zip](https://drive.google.com/file/d/1U-bu2N2aAejN5FzrNgUtx_oOB0ou7MJn/view)|

Download all12 volumes, [parts manifest](GK1_Evidence_Parts_v1.07.json) and [reassembly script](reassemble_evidence.py) into one directory; run `python3 reassemble_evidence.py`. The script checks each volume, the reconstructed archive and all inner files before reporting success. Then extract the verified archive. The ROM and emulator executable are supplied separately as described above.

## Canonical document delivery

The following raw files were saved in their actual project folders and downloaded again for full-byte comparison. The Markdown files are raw Markdown, not Google Docs renamed with a `.md` extension.

- [Economic Mapping v1.07 on Drive](https://drive.google.com/file/d/1AC7h5seTDE4-KdAK7rP3Nv_g-bMJO5xk/view) — Documentation.
- [167-obligation register v1.07 on Drive](https://drive.google.com/file/d/1IDPGgaFVjw9_kPdAWKVSVWS2v3dzlH9H/view) — Documentation.
- [This completion and reproduction record on Drive](https://drive.google.com/file/d/15HCTLAv2JiYGio94ICXBn5CjnYBiTfx3/view) — Collaboration.

The [publication receipt](PUBLICATION_RECEIPT_v1.07.json) records the exact verified revisions and GitHub delivery state. The [public file manifest](PUBLIC_FILE_MANIFEST_v1.07.json) identifies the versioned authored files by SHA-256 and Git blob hash. It explicitly excludes itself to avoid a circular hash. Outside-agent read acknowledgment and implementation are not claimed.
