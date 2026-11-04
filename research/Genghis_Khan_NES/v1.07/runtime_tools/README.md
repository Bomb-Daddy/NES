# Native original-ROM research tools

These files are authored controller drivers, independent capture verification, event recipes and a patch against the restored Mesen-for-AI bridge. They contain no ROM, screenshots, SRAM or full-machine checkpoints. The private evidence archive preserves the full working path layout, inputs, saved states and historical scripts as executed; this public folder is a readable source mirror, not a replacement standalone emulator installation.

For native reproduction, extract the private evidence archive into one root, restore the hash-pinned Mesen/Xvfb bundle, and privately provide `private/Genghis Khan (USA).nes`. Use the directory layout in the completion/reproduction index. Apply the bridge patch only to the specified restored bridge baseline; the private archive also contains the exact resulting bridge. Verify binary, ROM and checkpoint hashes first.

The driver reads JSON actions on stdin. Start with180 frames so its state-restore callback is active, then run a documented batch. Every controller request, memory intervention, breakpoint and response is logged. Save outputs to a NEW directory; copy only the required checkpoint inputs. Never overwrite the historical captures. Standard `runtime_helpers` restores `OUT/<name>.mss`; the48-case domestic driver additionally pins the original baseline checkpoint by absolute path relative to the extracted root.

Example after paths and environment are prepared:

```bash
/usr/bin/python3 -u batches/runtime/research_session_v107.py batches/runtime/new_replay
```

```json
{"op":"wait","frames":180}
{"op":"batch","path":"batches/runtime/trade_matrix.py"}
{"op":"close"}
```

`GK_ROM` selects the private ROM. `GK_TRACE_OUTPUT` selects a new trace JSONL path; `GK_EVENT_LOG` selects a log filename within the new output directory. The research driver allows14400 seconds per emulator session. Controller input is released before restoring; if an old original-game message prevents the next VM callback, the helper acknowledges it and re-restores the intended checkpoint with released input.

The native bridge adds full-machine checkpoint/restore at an allowed CPU callback, read-only watchpoints/traces, and capture access. Controlled cases write valid economic state/RNG operands explicitly. They do not patch the ROM or override the native CPU/VM PC to force an outcome. Source-only suites do use synthetic execution and are labelled separately.

The legacy trace field `country` records word7B66 and is NOT the active country. The later bridge additionally emits `active_country` from7B34. Determine actor identity from that field or the matching captured SRAM. Mesen's bridge-frame counter and the restored emulated CPU clock have different lifetimes; use log order and case IDs, not frame number alone, to identify evidence.

`verify_native.py` is standard-library offline verification. It independently recomputes expected state changes and binds captures to recorded8192-byte reads. Its three roots deliberately separate fresh complete logs, historical annual context and the additional boundary experiments. The completion index gives the final command and receipt. Source-pass flags alone, missing cases and successful process exit without expected completed counts are not acceptance.
