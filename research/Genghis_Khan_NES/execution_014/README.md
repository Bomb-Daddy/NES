# Genghis Khan I NES sound sequencer — execution batch0014

This is authored implementation code and an evidence-backed contract, not another planning memo. It produces ordered APU register writes using a separately supplied private sound bank. It does not include the commercial ROM, music data, emulator or screenshots.

Files:
- `sound_driver_model.py`: independent high-level sequencer and state/cue/stop API.
- `verify_native_trace.py`: compare the module with the private native captures, including continuous state carry.
- `SOUND_DRIVER_IMPLEMENTATION_CONTRACT_v1.00.md`: source semantics, new findings, evidence scope, remaining work and integration instructions.
- `verification_summary.json`: counts/hashes without private trace payload.

Python3.10+; standard library only. No compilation or network access required.

For full trace verification, supply the private modified ROM and private batch evidence:

```text
python verify_native_trace.py --rom /private/Genghis_Khan_Shared_BP_Rest_v1.0.nes --run /private/evidence/driver --run /private/evidence/interactions --out verification.json
```

See the contract for normal module integration. `tick()` emits writes, NOT audio samples. A Windows game still needs an APU renderer and event routing; the module is not an emulator frontend or full game.

Source build hash: `4764d46359581be1e7d12bcb85bd375c84cd4497d1084d9d306a6e884afbe7c5`.

**Publication state:** local GitHub-ready package only. No remote write capability was exposed in this turn; no commit or Drive delivery is claimed.
