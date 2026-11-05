# NES battle white marker — source and original runtime findings

Version 1.00 · 2026-11-05 · Author: Cao Cao / ChatGPT independent research

## Disposition

**The fresh filled white square over Yuan Shu at battle hex C2 is the blinking battle-cursor overlay.** The original NES turn loop initializes the cursor coordinates from the acting officer, then enables a four-sprite white overlay. The NES frame routine shows and hides that overlay in ten-frame phases. This is **VERIFIED_STATIC + OBSERVED_RUNTIME** for the original ROM and the fixtures identified below.

**The exact identity of the older November 3 S-02 image remains INFERRED.** That matrix describes a white filled tile at the top right and cites `_sheet_attack.png`; the actual sheet is absent from the recovered local package. The fresh observation supplies a concrete implementation reference, but does not retroactively identify an unavailable picture. S-02, the enemy active-hex flash F-03, and the attack-direction arrow C-04 were separate historical observations.

The accepted Director-unified NES/SNES battle mechanics remain the authority. This investigation resolves an original NES rendering path; it introduces no gameplay rule or platform-specific mechanics fork. Windows implementation and controller acceptance were not run.

## Source identity

The inspected original NES ROM is 524,304 bytes with SHA-256:

`11e312691b516e5e29dade3c8ce5434df3561cc69e1aecad9c948acdfddc517a`

Read-only listings were regenerated locally using `chrisaacson69/emu-harness` commit `9a436f493bf20f760c5eac49014163c2570e77bd`. NES VM bank listings used the original `$E4D1` entry stub and 8 KiB bank geometry. No title-specific names were supplied. Semantic claims below were read from the instruction listings and original bytes, rather than inferred from SNES sprite names. `white_marker_audit.json` records each bounded source range, file offset, length and SHA-256. The four reviewed VM ranges are compared byte for byte with their original ROM spans, including routine stubs and with no omissions. Native instructions were independently inspected at the listed addresses.

All bank numbers below mean **MMC5 8 KiB PRG banks**. Some generic native-listing output uses a combined 16 KiB view named bank 15; the actual kernel bank here is 31. File offsets include the 16-byte iNES header.

## What the original code does

| Layer | Exact source | Established behavior |
|---|---|---|
| Acting officer | Bank 8 `$8128–$8147`, file `0x10138–0x10157` | Writes the current officer pointer to `$7D2C`. Extracts the high and low nibbles of officer position byte `+0x12` into battle cursor x/y at `$7D1A/$7D1B`. |
| Enable during command selection | Bank 8 `$815F–$816A`, file `0x1016F–0x1017A` | Calls bank 14 `$B014` with argument 1 before the command-selection call. |
| Disable after the command | Bank 8 `$81C0–$81CB`, file `0x101D0–0x101DB` | Calls the same helper with argument 0. Other callers also exist; this is not an exhaustive inventory of every UI context. |
| Cursor coordinates | Bank 14 `$AFF6–$B013`, `$AFD2–$AFF5`, and `$AEBC–$AED4` | Reads the current x/y and computes the sprite position from the staggered battle grid. For valid battle coordinates, X is `8*(2*x+3)`; sprite Y is `8*(2*y+(x&1)+3)-5`. NES sprite display starts one scanline below that Y value. |
| White template | Bank 14 `$B014–$B02B` → `$AFBE–$AFD1` → `$AF98–$AFBD` | Passes four tile IDs `$E0`, attributes `$03`, pixel coordinates, and sprite slot `$20` plus the enable value to native syscall `$19` through `$EEE6`. |
| Native configuration | Kernel `$FD77–$FDCE`; syscall table word at `$F976`, file `0x3F986` | Syscall 25 resolves to `$FD77`. It writes the 16-byte sprite template at `$071A–$0729`, the enable byte `$072A`, sprite slot `$072C`, and initial countdown 1 at `$072D`. |
| Blink update | Kernel `$F8A5–$F8FA`, file `0x3F8B5–0x3F90A` | While enabled, decrements `$072D`; on expiry resets it to 10 and toggles `$072B` with XOR `$FF`. One phase copies the 16-byte template to shadow OAM at `$0600 + 4*$072C`; the other writes Y=`$F8` to each of its four sprites. When disabled, it hides a still-visible template and clears the phase. |
| Render timing | Kernel `$F671–$F6BA` | NMI performs OAM DMA from page `$06` at `$F683`, then later calls the blink updater at `$F6A5`. A shadow-buffer write and the exported picture are therefore not instantaneous observations of the same stage of rendering. |

For the tested C2 position, x=12 and y=2. The resulting white rectangle is at native screen **x=216…231, y=52…67**, exactly 16×16 pixels. The template is four sprites at X `$D8/$E0`, Y `$33/$3B`, each using tile `$E0` and attributes `$03`. Slot `$20` maps to shadow OAM `$0680–$068F`.

The blink updater reads its UI state at `$072A–$072D`. It does not read the occupancy byte for the hex. The runtime data independently supports this distinction: the C2 occupancy value remained `$04` while the white square alternated, and a natural C4 fixture uses the same white template with occupancy `$94`. In the C2 fixture the terrain byte is 0, so the white block in these frames is also not the separately documented code-6 castle tile.

## Original runtime evidence

The unmodified `battle_probe.py` and `run_mesen.py` produced isolated runs using the original ROM and the pinned historical Mesen/core identity. The wrapper checked ROM, executable and core hashes, native checkpoint restoration, completion frame, product validity, stdout/stderr, and process exit. The three runs below applied no controller input and no RAM interventions. All raw output and native artifacts remain in the private evidence workspace; this package contains authored analysis, hashes and reproducible audit code.

| Run | Start state and extent | Result |
|---|---|---|
| `white_marker_noinput_001` | `wait_001/state_002200.b64`; stop frame 181; requested screenshots 1–180 and 13 full-memory snapshots | **INCOMPLETE_RUNTIME**, retained. The immediately post-restore frame-1 screenshot was empty/invalid. Frames 2–180 supplied 179 valid pictures. The original receipt records both the invalid screenshot and missing valid shot 1. No clean-pass claim is made for this run. |
| `white_marker_writer_001` | Same native state; stop frame 45; screenshots and full-memory snapshots 2–44; watches on `$0680/$0684/$0688/$068C` and `$7E1A` | **OBSERVED_RUNTIME / PASS**. All 43 screenshots and 43 full-memory snapshots passed the harness gates. There were exactly 20 expected sprite-Y writes and zero writes to watched C2 occupancy. |
| `white_marker_initializer_001` | Earlier `wait_001/state_001000.b64`; stop frame 600; six screenshots/snapshots; watches on `$071A–$072D` | **OBSERVED_RUNTIME / PASS**. The existing enemy-turn sequence completed with no new input. Configuration enabled the overlay at screen (152,132) at frame 101, disabled it at 142, and enabled the C2 player cursor at 180. |

The first two runs agree on every comparable screenshot phase. The C2 rectangle is entirely white in frames 4–13, 24–33, 44–53 and so on; the unit and its troop number are visible in frames 14–23, 34–43 and so on. The observed full cycle is 20 frames. The clean repeat includes two complete visible transitions in each direction. At frames 4 and 24, all 256 pixels of the rectangle are RGB `(255,254,255)` under the pinned Mesen palette. Frames 14 and 34 contain the normal multicolor unit/number image. Their ROI hashes are in the audit JSON.

Every clean-repeat memory snapshot retains cursor `(12,2)`, officer `$672A`, occupancy `$04`, terrain 0, enabled slot `$20`, and the same four-sprite template. The changing values are the blink phase/countdown and the expected shadow Y values. Original callback events record:

| Visible operation | Actual store instruction | Callback-reported PC after store | Observed values |
|---|---|---|---|
| Copy template into shadow OAM | `$F8DA`, indexed `STA $0600,X` | `$F8DD` | Four watched Y locations receive `$33,$33,$3B,$3B`. |
| Hide the four sprites | `$F8EE/$F8F1/$F8F4/$F8F7` | `$F8F1/$F8F4/$F8F7/$F8FA` | All four watched Y locations receive `$F8`. |

The callbacks fire at local frames 1,11,21,31,41. The screenshots show the resulting phases later in the callback/export sequence. The evidence preserves that timing distinction; it does not pretend that a CPU shadow-memory sample is the already-rendered picture.

During initialization, all writes to enable `$072A` report post-store PC `$FDCE`. The then-current VM instruction pointer is `$AFBC`, mapped by the original emulator to PRG offset `0x1CFBC`: bank 14, at the tail of the native-syscall call in `$AF98`. This joins the observed native writes to the decoded NES configuration helper.

## Separate appearances that share the blink facility

The existing original `fire_prompt_001` frame 700 and matching memory were inspected. The selected direction arrow uses enable=1 and **sprite slot `$24`**, with template tile IDs `$9E/$9F/$9A/$9B` and attributes `$80`. Its old slot-$20 white-square entries are hidden. It shares the kernel blink facility but uses a different template and sprite slot. Calling the arrow the white filled-tile marker would conflate two appearances.

The earlier initialization replay also configures the white `$E0` template at screen (152,132) during the preceding enemy-turn sequence before returning it to Yuan Shu. This establishes reuse of the template in that sequence. No screenshot was requested at that exact setup event, so it does not independently identify the particular enemy or prove that historical F-03 names this same flash.

The normal white backgrounds within troop-number rendering, the solid castle tile, the white cursor square, and directional-arrow sprites must remain separate visual references. The current result identifies the white cursor square and its writer.

## Acceptance and remaining boundary

For a port implementation of this observed indicator, a decisive comparison is: select a battle actor; place a 16×16 white overlay at the current staggered hex; show and hide it in ten-original-frame phases; retain the underlying unit/count image during the hidden phase; disable or replace the template as the original command UI does. Use the original frame order and the recorded enable/disable call sites for integration. The source does not authorize using occupancy bit `$80` as this blink's trigger.

The fresh indicator's writer, coordinates and phase behavior are established. Remaining work is narrow:

1. Recover `_sheet_attack.png` or an exact November 3 frame to decide whether historical S-02 names this same cursor. Until then, keep that historical correspondence **INFERRED**.
2. Apply and test this reference on the canonical Windows implementation; no `TESTED_PORT` claim exists in this package.
3. If a different white visual is reported, obtain its exact frame plus current cursor, template, sprite-slot and phase state. Do not apply this identification to a visually similar tile by name alone.

## Reproducing the audit

Run from a workspace containing the private original-ROM evidence and regenerated bounded listings:

```sh
python audit_white_marker.py --workspace /path/to/workspace \
  --rom /path/to/authorized-original.nes \
  --output /path/to/white_marker_audit.json
```

The script verifies the complete original ROM hash, reviewed VM listing bytes, every recorded runtime product hash, unchanged state fields, twenty native writer events, initialization events, and the observed screenshot phases. It preserves the first run's failure status. It does not run an emulator or modify a ROM, saved state, source listing, or game implementation. `audit_attempt01_receipt.json` and its stdout/stderr preserve the actual audit invocation and result.

Authored package: this report, `audit_white_marker.py`, `white_marker_audit.json`, the audit execution receipt/stdout/stderr, and `delivery_manifest_v1.00.json`. Commercial inputs and full decoded source are excluded.
