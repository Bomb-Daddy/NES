# ZL-GK1-EXEC-0014 — Sound driver decoded and implemented

**Date/version:** 2026-11-05 / 1.00. **Author:** Zhuge Liang. **Recipients:** Director; Nobunaga (Windows integration); Cao Cao (independent QA).

**Execution:** completed this bounded research/implementation batch. **Overall audio reconstruction:** PARTIAL. **Windows game execution:** NOT RUN. **Shared publication:** NOT DELIVERED / BLOCKED: current GitHub and Google Drive tools expose only reads. No remote commit, Drive upload or recipient acknowledgement is claimed.

## What changed since EXEC0013

The previous batch established audio options, a 52-entry source cue table, and the troop-training sound. This batch goes further: an **independent, usable sound sequencer now implements the game's channel, duration, instrument, repeat and interruption rules**. It consumes the private sound bank and returns ordered NES APU-register writes. It does not run the original 6502 CPU.

The source module is `sound_driver_model.py`; the developer contract is `SOUND_DRIVER_IMPLEMENTATION_CONTRACT_v1.00.md`. These are usable Windows integration inputs, not merely test counts or a new plan. The Windows renderer must still turn these register writes into actual audio samples and route the remaining game events.

## Actual native execution and comparison

Two successful headless Mesen runs used the approved Shared-BP/Rest ROM:

`4764d46359581be1e7d12bcb85bd375c84cd4497d1084d9d306a6e884afbe7c5`

The corrected runtime's actual emulator and core hashes are in `execution_receipt.json`. The capture bridge adds **read-only** hooks at bank14 sound-driver entry `$8000` and exit `$805F`. Every APU write remains logged with CPU-cycle/frame coordinates. Controlled cue fixtures modify only disclosed sound-request fields; this is not a cartridge patch. The later Gift Food comparisons additionally set the Sound flag and restore a disposable prior research checkpoint. The Director's separate campaign was not touched.

| Result | Verified extent |
|---|---|
| Native cue entries requested | All52 table entries, using explicitly isolated sound requests |
| Native end-state reached | 40 entries, including structurally empty entry0 |
| Longer entries | Twelve captured prefixes; their full sequences also run to completion in the independent model |
| Complete native driver calls compared | **33,419** |
| Register writes inside those calls | **55,814**, exact values and order |
| Total captured register writes | 55,816; two startup `$4015` writes are separately outside the driver |
| Driver state compared per call | All78 zero-page bytes and64 loop-stack bytes |
| Continuous carried-state transitions | 33,414; three explicitly logged checkpoint restores explain the only full-state discontinuities |
| Unmatched terminal driver calls | Zero |
| Complete logged request/response pairs | 865 in first run; 628 in second |
| Negative controls | Altered APU byte, altered native post-state and altered ROM all rejected |

The scenario-selection, strategic-map and title/opening pieces received extended native runs through their endings. New interaction captures exercise repeat, noise over ongoing music, a new effect replacing selected music channels, and explicit stop. **Every command token encountered in full independent model runs of the 52 entries also occurs in the compared native captures.** This does not prove every unvisited gameplay trigger or every arbitrary malformed sound program.

These are native modified-ROM observations and independent high-level reference-code comparisons. They are not 33,419 gameplay features, an audible waveform comparison, or a Windows release acceptance.

## New implementable rules

**Partial replacement:** a cue's zero channel pointer means keep that channel playing, not mute it. A noise-only sound can overlay existing music. A zero timing byte preserves the prior speed; a nonzero one updates global speed. Thus a context-free WAV for every cue would miss observed interactions.

**Repeat:** the source repeat condition considers the three melodic channels, not noise. It restores the melodic streams separately. Cue14 produced two native repeat requests in the controlled loop case. The implementation preserves that exact policy.

**Playback ordering:** the driver resumes after the last voice that consumed a note, and does not decode a new note on all channels indiscriminately each update. Pulse envelopes, triangle gating, noise-register selection, duration wraparound, loops and alternate endings are implemented in the supplied module.

**Data/engine separation:** the public authored code contains no ROM or music payload. The private evidence package contains the pinned 16KiB sound-bank input and source manifests. The same sound-bank hash is present in the two inspected mod variants; current native testing is specifically the Shared-BP variant. An unmodified-stock binary was not newly compared.

## One more cue identified through actual gameplay

**Cue30 (`$94F8`, source file `0x39508`) is the success/cheering sound for the tested Give Food1 action.** This association came from real controller navigation, not a sound-table injection.

Matched checkpoint cases:

| Observation | Sound on | Sound off |
|---|---:|---:|
| APU writes during the route | 157 | 0 |
| Food | 208→207 | 208→207 |
| Morale | 118→119 | 118→119 |
| Shared BP | 100→95 | 100→95 |
| Remaining orders | 3→2 | 3→2 |
| RNG bytes | `2212b730fc13` unchanged | Same unchanged bytes |

The controlled setting was written to `$7BB8` after loading the same checkpoint; no new claim is made about manually navigating the options menu (covered by EXEC0013). The gameplay route itself was controller-driven.

**Timing correction worth implementing:** the gift finishes after its timed feedback without another acknowledgement. At the snapshot after480 frames, before the subsequent A press, BP and order costs were already charged. An earlier feedback snapshot had the food/morale change while BP/orders were still unchanged. Do not apply the earlier training-acknowledgement interpretation as a global rule for all commands.

Five cue entries now have documented gameplay associations (four carried forward and one newly identified); **47 remain event-unmapped**. Other gift types or quantities are not claimed verified from this one-unit Food case.

## Failures and exclusions retained

A native WAV-export probe failed because the pinned Mesen Lua build has dynamic-library loading disabled. The diagnostic records the actual message. No WAV was produced, and the probe does not alter the successful register captures. The failed sessions' request logs and native stdout/stderr are preserved, along with initial failing module/API tests and corrected runs.

The sequencer emits ordered writes per driver invocation. It does **not** generate exact CPU-cycle offsets within the invocation or emulate the APU waveform/mixer. There is no claim of audible comparison, full original-ROM parity, platform sound-device validation, a complete Windows build, or completion of combat/AI research.

All successful and failed emulator processes started for this batch were stopped before final packaging. No background continuation is claimed.

## Completion register and receiving implementation

| Item | Status | Windows consumer / remaining work |
|---|---|---|
| Sound bytecode/channel driver | PASS VERIFIED for captured cases | Reuse `SoundDriver.tick`, cue/stop/state APIs; matches33,419 native calls |
| All52 cue structures and usable source data | PASS VERIFIED, bounded source/native coverage | Private bank + cue manifest; no duplicate decoder investigation needed |
| All52 full native endings | PARTIAL:40 ended,12 prefix-only | Extend only where a new branch or mismatch warrants it; not a gate to using the proven decoder |
| Give Food1 cue association | PASS VERIFIED | `GiveFood` success feedback → cue30, respect Sound toggle, keep gameplay calculations independent |
| Complete gameplay-event audio mapping | PARTIAL:5 identified,47 unknown | Battle, diplomacy, succession and ending call sites remain priority |
| Native PCM capture / audible equivalence | NOT RUN successfully | Failed method disclosed; integrate supported APU recording/rendering path |
| Windows game integration | NOT RUN | Nobunaga consumes module, routes events and tests renderer on Windows |
| Remote GitHub/Drive publication | BLOCKED / NOT DELIVERED | Public import and private evidence packages exist locally; read-only actions exposed in this turn |

**Next engineering target:** wire this completed sequencing layer into the Windows audio module while investigating only missing event routes and renderer fidelity. Do not repeat already solved command grammar or invalidate native modified-ROM findings because the ROM is modified.

## Reproduction

From the private evidence root, provide the exact named ROM separately:

```text
python scripts/verify_native_trace.py --rom /private/Genghis_Khan_Shared_BP_Rest_v1.0.nes --run driver --run interactions --out verification.json
```

For normal module usage and dependencies, see the included implementation contract. Native capture reproduction needs the separately preserved Mesen runtime, matching bridge and checkpoint, with paths adapted from each run's `environment.json`; the original execution scripts/logs remain unedited as evidence. No ROM is bundled in these output ZIPs.

Evidence files are hashed in the archive manifest. The final receipt records runtime/source hashes, comparisons, scope and delivery limitation.

**End of EXEC0014.**
