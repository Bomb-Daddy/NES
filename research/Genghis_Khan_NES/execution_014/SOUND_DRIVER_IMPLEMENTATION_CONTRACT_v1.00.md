# GK1 sound-driver implementation contract — EXEC-0014 v1.00

Date: 2026-11-05. Producer/research owner: Zhuge Liang. Consumers: Nobunaga (Windows implementation), Cao Cao (independent QA).

## Deliverable, not another research plan

`sound_driver_model.py` is an executable, independent Python implementation of the game's four-channel sound sequencer. It interprets the private sound data and emits ordered NES APU-register writes. It does **not** execute 6502 instructions. It is suitable as a deterministic reference module for the Windows game, or as the specification for an equivalent implementation in the selected engine language.

**Implemented layer:** cue/channel stream selection → note/duration/loop/instrument state → ordered register writes per sound-driver update. **Not implemented:** NES APU oscillator/mixer/filters/PCM device output, original CPU-cycle timing within an update, or a complete Windows game.

Authoritative tested ROM: shared-BP/Rest NES SHA-256 `4764d46359581be1e7d12bcb85bd375c84cd4497d1084d9d306a6e884afbe7c5`. Private bank14 SHA-256 `36d2110ac1b67f11c6dfcb4685e8baed2d0d37de6c7e74185fb2c80e9d9cc5a8`. The same bank bytes exist in the inspected Demobilize-only mod, but the current native captures ran the Shared-BP build. No pristine-stock binary comparison was newly performed.

## Calling interface

```python
from sound_driver_model import SoundDriver

# Private input only. The public source package contains no ROM or sound-bank payload.
driver = SoundDriver.from_rom(private_rom_bytes)
# Or: driver = SoundDriver.from_sound_bank(private_16384_byte_bank)

# Emulated zero-power-on sound state is intentional. The application must preserve
# cadence and carry state; do not reset the driver every time a cue changes.
driver.tick()
driver.queue_cue(6, repeat=True)

# Invoke at each matching native sound-driver update. These values are not PCM.
for address, value in driver.tick():
    apu.write_register(address, value)  # existing/chosen APU renderer integration

saved_sequencer_state = driver.export_state()
driver.import_state(saved_sequencer_state)
driver.request_stop()
stop_writes = driver.tick()
```

The package's `queue_cue` API rejects invalid indices and ROM/bank identities. Its state snapshot covers the sequencer, **not the complete APU phase/envelope/mixer state**. A new Windows save format must explicitly distinguish those two layers. Original native save semantics do not automatically imply sample-continuous audio after load.

## Decoded state and scheduling

The four voices are pulse1, pulse2, triangle and noise, indexed internally by x=0,2,4,6. The source records use 16-bit little-endian pointers and arithmetic, with deliberate 8-/16-bit wraparound:

| State | Original address | Meaning |
|---|---|---|
| Repeat enable | `$B2` | Requests melody reactivation when the three melodic voices end |
| Requested cue header | `$B3:$B4` | Pointer selected from 52-entry table at bank14 `$8608` |
| Live voice pointers | `$B5..$BC` | One word per voice; a zero high byte marks inactive |
| Cached start pointers | `$BD..$C4` | Used by repeat operation |
| Time to next note | `$C5..$CC` | One 16-bit accumulator per voice |
| Current duration | `$CD..$D4` | Values loaded from the duration table |
| Speed | `$D5:$D6` | Subtracted from the accumulators each applicable update |
| Load/repeat request | `$F5` | 1=new header; 2=repeat melodic voices |
| Stop request | `$F6` | Silence voices when no higher-priority load request exists |
| Loop stack positions | `$F7/$F9/$FB/$FD` | Separate 16-byte regions of `$0731..$0770` |
| Last consumed voice | `$FF` | Determines where next update resumes |

The driver does not blindly advance all four note streams every frame. It resumes after the last channel that consumed a note and returns after the next consumed note event. Pulse envelopes and triangle gating are updated in the source's channel visitation order. The module preserves that order. Register-write order is significant, especially pitch/control writes.

## Cue replacement, zero header fields, repeat and stop

A cue header contains four channel pointers followed by one timing byte. **A zero channel pointer preserves that channel's existing stream; it does not stop it.** A nonzero pointer replaces that voice and caches its starting position. A nonzero timing byte replaces global speed and initializes melodic loop stacks. A zero timing byte preserves existing speed and those melodic loop stacks. Noise stack handling is separate.

A repeat request restores the three melodic streams, not the noise stream. The condition checks whether pulse1, pulse2 and triangle are inactive; noise does not prevent repeat. This was explicitly exercised with short cue14 and two native repeat requests at captured frames1112/1311. Do not implement an indiscriminate restart of all four voices.

The normal stop branch silences the channels and clears pointers. Queueing cue0, whose structure is empty, is **not interchangeable with the explicit stop request**: empty fields preserve live state. A cue's observed duration and write sequence may depend on inherited timing/channel state. Do not replace this driver with isolated WAVs and assume all overlaps behave correctly.

## Stream grammar

The implementation resolves the source grammar using bank14 `$8000..$8508`; native parity covers every complete captured invocation rather than a claim about malformed bytecode.

| Token | Source behavior |
|---|---|
| `00..7D` | Note index; duration is accumulated and the stream advances |
| `7E` | Extend the time accumulator and continue reading |
| `7F` | Rest/silence for the current duration, while retaining the live stream |
| `80..BF` | Duration-table selector (low5 bits) |
| `C0..CE` | Repetition entry; four-byte stack records track count and loop address |
| `CF` | Repetition end, update count and branch or pop |
| `Dn` | Alternate ending selected by loop iteration |
| `E0/E1` | Pulse instrument data pointer: control/sweep/length/envelope parameters |
| `E2` | Triangle instrument/gate data pointer |
| `E3` | Noise-register tuple data pointer |
| `FF` | End the current channel |

Timers subtract speed with 16-bit wrapping and decode when the high-byte sign bit is set. Duration contribution is eight times the duration word. Pulse software-envelope index is limited to63; the low volume bits combine with source control bits. The triangle has source-specific gated and ungated modes. Noise uses a modulo12 mapping into three-register tuples. Exact implementation and raw native comparisons are supplied; do not substitute generic assumptions about MIDI tempo/envelopes.

## Evidence and independent verification

Two completed successful Mesen capture runs provide **33,419 complete native driver invocations**. The independent sequencer matches all **55,814 writes produced inside those calls**, in exact order and value, as well as all78 captured zero-page bytes and64 loop-stack bytes after each call. There are55,816 total register writes: two separately accounted startup `$4015` writes occur outside these driver calls.

Continuous replay carries the prior computed state through33,414 transitions, updating only explicitly observed sound-request fields. The three full-state discontinuities in the second run coincide with three logged restores of our disposable research checkpoint. There is no unexplained state reset and no incomplete terminal driver call. These are not33,419 separate gameplay scenarios.

All52 cue-table entries were requested in native Mesen via disclosed audio-field writes. Forty reached their inactive end state in the captured window (including empty entry0); twelve long pieces have native prefixes only. Additional complete native runs cover cue1 (scenario), cue6 (strategic) and cue17 (opening/title). An independent full model run reaches the end for all52 entries; **model-only endings do not replace native coverage**.

The native isolated requests prove playback behavior, not each cue's gameplay trigger. Five table entries now have identified gameplay associations: prior1/6/17/32 and newly30. Forty-seven remain unassociated with a specific game event in the maintained manifest.

Three deliberate corruptions—an APU value, a native post-state byte and the ROM identity—were rejected. Public API checks cover source validation, state round-trip, pending cue behavior, preserving music channels for a noise-only cue, and stop.

## Newly closed gameplay route: Give Food one unit

Actual controller route: main → Give → Food → quantity1 → feedback. Cue30 (`$94F8`, file`0x39508`) is requested. In the matched Sound-on case157 register writes occur; Sound-off produces zero. Both result in Food208→207, Morale118→119, BP100→95 and Orders3→2, with identical preserved RNG bytes `2212b730fc13`.

The audio setting was explicitly set through a logged memory write after restoring the same own research checkpoint; this is a controlled on/off comparison, not another proof of navigating the Music/Sound option menus. Those menus were covered in EXEC0013.

The gift can finish after its timed feedback without an extra acknowledgement: the snapshot after480 frames, before the subsequent A press, already has BP95 and Orders2. Earlier intermediate feedback showed Food207 but BP100/Orders3. Thus do not globally copy the training acknowledgement interpretation onto every command. The event-to-audio connection and timing are part of the receiving `GiveFood` implementation contract.

## Remaining work, not concealed

1. Associate the other47 cue entries with actual gameplay events and callers; most urgent are combat, diplomacy, succession, victory and defeat.
2. Extend native ending coverage for the twelve remaining long pieces where it adds a materially new branch, rather than merely increasing replay counts.
3. Integrate this sequencer with a faithful NES APU renderer and verify audible PCM, mixer, pitch, loop/interruption timing and Windows output. Exact within-call CPU-cycle emission is not generated by this high-level module.
4. Execute the Windows implementation and actual platform/controller tests. Linux native-ROM execution plus Python sequence parity is not Windows acceptance.

A native WAV-capture attempt was preserved. The pinned Mesen Lua build reports `dynamic libraries not enabled; check your Lua installation` when attempting its supported Lua load-library mechanism. No WAV was produced; no synthesized output is labeled a native recording. Use a supported frontend/audio capture route or a separately reviewed runtime extension for this remaining task.

## Scope and delivery

Evidence label: **OBSERVED_RUNTIME_MOD** for native actions/fixtures; **TESTED reference implementation** for the authored sequencer. The modified ROM is authoritative for its own tested behavior. No original-stock or full-game statuses were silently promoted. Native ROM and sound bank remain private.

Remote publication: **NOT DELIVERED / BLOCKED in this turn**. GitHub/Drive tool discovery exposed read operations but no create/update/upload action. Public import package and private evidence are provided locally; no remote commit, folder publication or recipient execution is claimed.
