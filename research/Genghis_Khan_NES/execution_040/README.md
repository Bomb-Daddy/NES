# GK1 EXEC0040: portable tactical research snapshot v1.00 (C11)

**Purpose:** NEW executable C11 serializer and CLI for one **partial** Genghis Khan I NES tactical research capture. Works with the already verified `execution_035` read-only NES SRAM decoder, not ROTK II or a new inferred gameplay rule. The format is Windows-portable C11 and does not assume a C ABI, pointer size, struct padding, ROM version, or byte order. **It is not the complete Windows game, a valid NES savestate, a full player campaign save, or proof that a movement command is complete.**

It stores: all 20×11 raw battlefield unit fields including opaque bytes, the 16×10 raw map including high-bit bytes, `$613E:$613F` scratch u16, externally supplied exact 32-byte source ROM SHA-256, separate source profile (stock USA or approved combined modified ROM), explicitly recorded capture phase, optional VM *data-source pointer/bank* and optional RNG48 bytes. The raw 8192-byte NES SRAM input is NOT serialized. No native CPU PC is invented. An unknown phase, VM, or RNG stays *unknown*; they are never filled in from other captures.

## Layout (440 bytes, v1)

- `0..7` ASCII `GK1BTL01`
- `8` format version = `1`; `9` source profile (1 stock English USA, 2 approved combined Shared BP/Rest + Demobilize/Fonzarelli)
- `10` phase 0 unknown, 1 destination selected, 2 after attempt, 3 after idle, 4 observed finish-control
- `11` bit0 VM pointer available, bit1 RNG bytes available, no unknown flags
- `12..43` 32-byte source-ROM SHA-256 supplied by caller; must be nonzero
- `44..45` VM source pointer little-endian; `46` VM bank; `47` reserved 0
- `48..53` six RNG bytes, exactly zero if unknown/unavailable
- `54..55` scratch word little-endian
- `56..275` 20 unit records × 11 raw bytes, zero-soldier/opaque records retained
- `276..435` 160 raw terrain bytes, 16×10 row-major
- `436..439` CRC32 ISO-HDLC little-endian of bytes `0..435`

**Source identity is a caller-supplied label, not cryptographic verification of the associated NES bytes.** With a genuine ROM, the caller must calculate its SHA-256 first and separately verify that the capture belongs to it. `decode_for_rom` refuses a different profile or hash even with correct CRC. CRC32 detects accidental corruption, not malicious tampering.

## Build and executable tests

From an extracted archive root:

```
cmake -S execution_040 -B build040 -DCMAKE_BUILD_TYPE=Debug
cmake --build build040
ctest --test-dir build040 --output-on-failure
./build040/test_gk1_snapshot
cmake -S execution_040 -B build040san -DCMAKE_C_FLAGS='-fsanitize=address,undefined -fno-omit-frame-pointer' -DCMAKE_EXE_LINKER_FLAGS='-fsanitize=address,undefined'
cmake --build build040san
ASAN_OPTIONS=detect_leaks=1 ./build040san/test_gk1_snapshot
```

Test-only CLI (creates a clearly synthetic partial snapshot and refuses overwrite):

```
SHA=4764d46359581be1e7d12bcb85bd375c84cd4497d1084d9d306a6e884afbe7c5
./build040/gk1_snapshot_cli synthetic-write /tmp/gk1_SyntheticOnly_040.gk1b combined "$SHA"
./build040/gk1_snapshot_cli inspect /tmp/gk1_SyntheticOnly_040.gk1b combined "$SHA"
```

The `SHA` CLI argument labels a test fixture only; no actual ROM or native capture is used in this CLI scenario. The CLI's output is not a Nintendo save or a gameplay result.

## Hostile negative coverage

Exactly 440-byte length, magic, format version, source profile, legal phase, reserved flags, nonzero source hash, optional VM/RNG zero-on-absence invariants, every bit flip across 436 bytes, recomputed-CRC malformed inputs, identity mismatch, wrong expected profile, zero-soldier occupancy, all raw 0..255 tile values, preservation of every opaque unit byte, unchanged outputs on rejected inputs, non-battle SRAM noninterference, short/null pointers and CLI overwrite refusal and wrong-ROM read rejection. No authentic ROM, SRAM, checkpoint, graphics, audio or screenshots are in the public source.

## Evidence boundary / next work

Actual QA in EXEC0040 is Linux GCC C11 on **synthetic SRAM** only. This implements a portable battlefield capture and validation codec, **not movement legality, Entire/Divide, merging, terminal battle, AI, complete campaign saves or a 1280×720 GUI**. It can preserve and compare phase-labeled real captures once authorized Mesen input is available. No new actual NES, Windows executable, or physical Xbox controller runs occurred. Matching developer Mesen executable/core and private in-battle full-machine restore are not present in this execution's container; see EXEC0030 and EXEC0039 for identity pins. Re-establish exact Mesen/Core and the pending→loaded checkpoint before making natural gameplay/source-branch claims.
