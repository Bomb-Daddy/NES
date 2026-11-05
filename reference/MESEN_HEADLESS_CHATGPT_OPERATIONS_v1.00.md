# Mesen Headless Emulator — ChatGPT Operations & Recovery Reference

**Edition:** v1.00 • 2026-11-05 • Project: Zhuge Liang / KOEI ROM research  
**Owner:** Zhuge Liang / ChatGPT; Director: human Project Director  
**Destination:** `Bomb-Daddy/NES/reference/` (public procedural reference, **not** ROM storage)  
**Execution status:** original NES ROM bridge **live-tested** on Linux with native Spring 1186 restoration and controller D-pad probe; full 120-season autonomous strategy controller **NOT built or tested**.  
**Fastest entry point:** [the public test harness](../research/Genghis_Khan_NES/tools/headless_mesen_campaign_test.py) and its [offline regression tests](../research/Genghis_Khan_NES/tools/test_headless_mesen_campaign_test.py).

> **For another ChatGPT session:** Read this document, identify the exact game and current Director-approved strategy, locate **authorized private** ROM/savestate/runtime, run the preflight/unit tests and then the real Mesen test. Do not substitute a hypothetical simulation for executing the ROM. No need to request the ROM again until existing authorized copies are actually unavailable. A reference file does not automatically install a native ChatGPT skill, unlock a cloud computer, start automation, or authorize rewriting a user's game state.

## 1. Architecture in one minute

The tested original-ROM path is:

```text
Human instruction / ChatGPT
        |
        v
private ROM .nes + matching original save .mss + Mesen executable
        |
        v
headless_mesen_campaign_test.py  [this repository; safe diagnostic test]
        |
        v
mesen_client.py   [upstream mesen-for-ai checkout]
        |
        v
mesen_mcp.daemon  -- MCP JSON-RPC over private stdio
        |
        v
run_headless.sh -> xvfb-run -a Mesen --testrunner ROM bridge.lua
        |
        v
Lua/TCP bridge (per-session localhost port and session handle)
        |
        v
Mesen original emulated CPU, PPU, controller, graphics, memory and state
        |
        +--> native .mss save/restore [ONLY campaign-patched bridge]
        +--> actual P6 PPM screenshot (256x240 for the verified NES case)
        +--> read-only CPU/RAM inspection and state hashes
        +--> JSON receipt and action log (private local evidence)
```

`mesen-for-ai` exposes **stateful original-game execution**, not a strategic plan by itself. The repo's Python client is the safest transport wrapper; do not hand-write the JSON-RPC framing or bypass its session-handle tracking. The `--testrunner` mode is not the same thing as a Mesen desktop GUI window. `xvfb-run` provides a virtual X server, but actual emulation and screenshot generation happen inside Mesen.

The test harness published here **does not autoplay** Genghis Khan; it cold-loads a private ROM and optionally a private checkpoint, checks source hashes and in-game state, optionally pulses a noncommitting direction input, and captures an actual image. To run 10/40/120 turns, use a separately validated controller operator/driver, while applying the Director's current operating script and the gates below. Do not represent the diagnostic harness as a complete turn-taking AI.

## 2. Select the right project and game before running

| Scope | Source of truth | What headless Mesen evidence proves |
|---|---|---|
| **Genghis Khan I (USA NES)** | private original-ROM SHA-256, current GK1 economic mapping and native `.mss` | that original ROM's behavior at a precise state under precise inputs |
| **GK1 demobilization / Fonzarelli / shared-BP modified ROMs** | separately hashed and approved modified ROMs | only the matching modified ROM; not original Classic behavior |
| **ROTK II NES Windows** | Google Drive `Retro Gaming/ROTK2_NES_Windows`, per SUP-0016 | NES original observations; does **not** by itself prove the Windows executable |
| **ROTK II DOS Windows** | `ROTK2_DOS_Windows` | DOS evidence is separate from NES evidence |
| **Original Koei-inspired Windows game** | separate original-game source and approved design | emulator data is reference/mechanics research, not implementation acceptance |
| **SNES / PCE / GBA** | per-system evidence and upstream `mesen-for-ai` skills | possible with correct tool memory type/ROM, **not** with GK1-specific SRAM field offsets in this harness |

Current GK1 original USA NES ROM SHA-256: `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`, size **262,160 bytes**, NES mapper **MMC1 / mapper 1**. This pin is for the exact archived research variant. If the hash differs, **stop** and identify the variant; do not make the test accept a different ROM by deleting the expected hash.

For the completed ten-year experiment, the verified native checkpoint is **Spring 1186, start of turn**: [private Drive save](https://drive.google.com/file/d/1LgKX5HschZ1uLHYDgS0kEesgpiZuGgUw/view). Expected Gold **832**, Food **443**, **146** soldiers, **154** civilians, Morale **129**, 3 orders, Market Rate **55**, and ruler Leadership **137**. The Director's later [unified AI operating script v2.00](https://docs.google.com/document/d/1Tn4xeiCFIUCl6eDgu4DLZKuby65mT_AdMRlV8IQDeko/edit) is a **policy document** awaiting implementation/testing, not evidence that Leadership 451 or 250 soldiers were achieved.

## 3. Authoritative materials and exact locations

**Public source and tools:**

- [This repository's GK1 research hub](../research/Genghis_Khan_NES/README.md) and [economic mapping v1.07](../research/Genghis_Khan_NES/v1.07/GK1_ECONOMIC_MAPPING_v1.07_2026-11-04.md), including [runtime tool README](../research/Genghis_Khan_NES/v1.07/runtime_tools/README.md) and [independent QA](../research/Genghis_Khan_NES/v1.07/QA/INDEPENDENT_REVIEW.md).
- [Command crosswalk](../research/Genghis_Khan_NES/GK1_COMMAND_CROSSWALK_v1.04_2026-11-03.md); useful for original menu semantics and known corrected assumptions. Do not elevate old v1.04 uncertain market formula claims over the later v1.07 mapping.
- [Upstream `paulomanrique/mesen-for-ai`](https://github.com/paulomanrique/mesen-for-ai), particularly [`README.md`](https://github.com/paulomanrique/mesen-for-ai/blob/main/README.md), [`AGENTS.md`](https://github.com/paulomanrique/mesen-for-ai/blob/main/AGENTS.md), [`skills/mesen-emulator/SKILL.md`](https://github.com/paulomanrique/mesen-for-ai/blob/main/skills/mesen-emulator/SKILL.md) and [`scripts/run_headless.sh`](https://github.com/paulomanrique/mesen-for-ai/blob/main/scripts/run_headless.sh). This supplies the daemon/client and stock bridge, **not** our campaign's custom checkpoint commands.
- [Mesen2 upstream emulator](https://github.com/SourMesen/Mesen2) (the tested campaign used a particular **MesenCE native executable**, not an arbitrary unverified Mesen2 release).
- [SecondPlayer's independent headless MesenCE adapter testing](https://github.com/Daxiongmao87/SecondPlayer/blob/main/docs/HEADLESS_TESTING.md). Its Xvfb, Lua/input callback, screenshot, and settings-isolation gates are useful **cross-project practices**, not proof this GK1 bridge was executed there.

**Private assets (do NOT commit):**

- [Temujin run folder](https://drive.google.com/drive/folders/1pWASkSQBTPwihklnGZJbetrRZCqyp2g9): original `.mss` files, latest run journal, and evidence archive.
- [GK1 30-year recovery package v1.00](https://drive.google.com/file/d/1FZAVIJ3MEk8WucgfCtQsqT_LhKkCrFEv/view): historical exact Mesen executable (SHA-256 `f02685dafe559addf8dd8d769621a89777137d0c4592436aed65694a7e740676`), `mesen-for-ai` checkout + research-only patches, controller driver, logs, captures and first campaign saves. **No commercial ROM is included.**
- [Temujin save-state journal](https://docs.google.com/document/d/1TPKEQ_6E4zDBrdWkY47HBqNqcfgOeg98M_tAqWXw_OU/edit): later append-only correction records verified Spring 1186 milestone; older "latest Summer 1175/1177" paragraphs are historical.
- [Original v1.00 strategy & controller document](https://docs.google.com/document/d/1a9doDD78SPCGZrlJnJqIYTmSzVDU4WQ5-i66sGZZxj0/edit): partial controller and commands, **not** a finished unattended driver.
- [KOEI authority register](https://docs.google.com/document/d/1qEVhDdiG2oH8LvPz0jan5DCtFFVxHhQWdWRUpduTllQ/edit): read current register **and later supersessions** before changing project source or treating past handoffs as active.

Historical GK1 data and source files in GitHub are public **authored text / code only**. Never publish a copyrighted `.nes` / `.sfc`, firmware, private `.mss`, SRAM dumps, actual screenshots extracted from licensed games, or research ZIP containing them. Put private evidence in the authorized Drive folder, preserving access limits.

## 4. Tested platform and nonnegotiable prerequisites

The verified 2026-11-05 smoke case used a temporary Linux x86-64 cloud container, Python **3.13.5**, installed `xvfb-run`/Xvfb, private original ROM, pinned native Mesen executable, and the customized GK1 bridge included in the recovery package. It was **not** a Windows launcher, physical Xbox controller, nor the separate Windows game.

Minimum host software: Python 3.11+, an executable MesenCE build supporting `--testrunner` and Lua networking, Xvfb (`xvfb-run` plus `xauth`), necessary SDL2 dependencies, and the upstream `mesen-for-ai` checkout. Optional Python Pillow or ImageMagick is useful for converting P6 `.ppm` to `.png`, but is **not** needed by the published test (it stores/validates P6 directly). Use a fresh writable evidence directory per run.

```bash
python3 --version
command -v xvfb-run
command -v Xvfb
file /absolute/path/to/Mesen
sha256sum /absolute/path/to/Mesen
sha256sum '/private/Genghis Khan (USA).nes'
```

Place the private original ROM wherever the *current* authorized runtime can read it. Never assume cloud filesystem paths from an earlier ChatGPT session persist. The recorded `/workspace/scratch/.../private/` or `F:` paths are historical, not public GitHub resources or proof that another environment can access them. Obtain private files from an existing authorized Drive/Library mapping when available, then verify bytes and parent links.

### 4.1 Install/locate the bridge

Use a checked-out upstream `mesen-for-ai` checkout, or the **exact version in the private recovery package** when restoring GK1 research savestates. Required source paths inside the checkout:

```text
mesen-for-ai/
  bridge.lua
  src/mesen_mcp/{__init__,daemon,session,bridge_client,validation}.py
  scripts/run_headless.sh
  skills/mesen-emulator/scripts/mesen_client.py
```

The script uses `MESEN_FOR_AI_REPO` internally and adds the checkout's Python `mesen_client.py` to its module path. You do not need an installed MCP connector when running this *direct Python client*. Conversely, merely creating a Markdown guide or adding a plugin reference does **not** install a first-party ChatGPT skill. A ChatGPT session needs a real Python/shell/cloud runtime to execute these commands.

### 4.2 Important campaign checkpoint difference from stock upstream

The privately preserved GK1 `bridge.lua` and `mesen_mcp.daemon` implement:

- `state.checkpoint`: request `saveResearchState(path=...)`
- `state.restore`: request `loadResearchState(path=...)`
- `state.status`: query `researchStateStatus`

In the archived customized Lua bridge, the actual save/restore is executed at the **NES VM callback `$EFC9`**. A call to `state.restore` returns `{"operation":"pending"}` first; this is **NOT SUCCESS**. The game must have reached its original VM loop, and the driver must verify a subsequent `{"operation":"loaded"}` from `state.status`. The saved year/season/cash/orders must then match the expected input state. The public upstream README lists other tools but does not guarantee this custom `state.*` trio. Never send those commands blindly to a stock upstream bridge.

`headless_mesen_campaign_test.py --mode checkpoint` includes the historically recorded **initial GK1 menu/bootstrap input prefix** before restoring, then waits for the VM callback to complete. This is temporary setup on an isolated emulator session; it does not replay lost decisions or alter the source `.mss` file. It is game-specific; do not apply that input prefix to ROTK II or an unrelated NES game.

## 5. Mesen first-run setup wizard — the recurring real failure

**Symptom:** `MesenToolError: session.load_rom: bridge did not become ready` after ~15–20 seconds, with a live Mesen window and no bridge connection; `mesen.stdout.log` and `.stderr.log` can be empty. A timed-out process is not proof that the emulator was unavailable or that the ROM failed.

**Observed cause:** Native MesenCE displayed a **Configuration / Please take a moment to perform Mesen's initial setup** window instead of running the bridge. On 2026-11-05 the visible window advertised a data folder at `/home/oai/.config/MesenCE`, despite a surrounding shell with uid=0 (`root`) and a different `HOME`/`getpwuid` profile assumption. Preparing only `/root/.config/MesenCE/settings.json` failed to eliminate the wizard. Preparing the directory **actually advertised by that Mesen window** fixed startup; the subsequent bridge connected and the original ROM ran.

**Corrective procedure:** determine the effective settings directory for the **actual Mesen binary and Linux user/profile**, not merely what the caller's `HOME` says. The main upstream `run_headless.sh` makes an isolated session `home/.config/MesenCE/settings.json`; in this recorded environment Mesen still used a different profile. Preserve existing user settings, create a **backup** before edits, and seed the wizard's directory only when confirmed. A permissive, known-working MesenCE 2.2.1 test profile in the recovery package contains:

```json
{
  "Version": "2.2.1", "ConfigUpgrade": 1, "EnableTestMode": true,
  "Debug": {"ScriptWindow": {
    "AllowIoOsAccess": true,
    "AllowNetworkAccess": true,
    "ScriptTimeout": 60
  }},
  "Nes": {
    "RamPowerOnState": "AllZeros",
    "RandomizeMapperPowerOnState": false,
    "RandomizeCpuPpuAlignment": false
  }
}
```

**Version warning:** upstream's `run_headless.sh` currently emits a numeric `"RamPowerOnState": 1`, while the restored research helper used the string `"AllZeros"`. Both are source-specific input formats, **not a license to assume either setting works on all Mesen releases**. The live 2026-11-05 run used the recovery helper's text-format profile at the actual Mesen path. Pin the emulator version, inspect its settings behavior, and verify deterministic observations, not just the JSON value.

**No silent side effects:** do not overwrite the Director's personal Mesen settings on a desktop. This guide describes a temporary cloud runtime. When changing a shared profile is inappropriate, create a dedicated host account/isolated environment and configure it before running Mesen. Capture the settings path and before/after checksums. Disable update checks/desktop shortcut wizard options if a manual first-run confirmation is unavoidable. `xvfb-run` still creates an X display; it cannot click the configuration wizard for you.

#### Visual proof of the wizard when stderr is empty

For controlled debugging, use a dedicated Xvfb display and screenshot. For example:

```bash
Xvfb :95 -screen 0 1280x720x24 -nolisten tcp > /tmp/xvfb.log 2>&1 & XVFB_PID=$!
export DISPLAY=:95 SDL_AUDIODRIVER=dummy
# Launch a diagnostic Mesen process (not an active campaign save) here.
xwininfo -root -tree
ffmpeg -hide_banner -loglevel error -y -f x11grab -video_size 1280x720 \
  -i :95 -frames:v 1 /tmp/mesen-window.png
kill "$XVFB_PID"
```

This is a **host-GUI diagnostic screenshot**, distinct from a rendered NES game frame obtained via `video.export_frame`. In the actual failed run the window said *Configuration*, proving the issue was a setup wizard. Do not attempt to solve such a case by sending blind NES controller inputs; first fix the emulator host setup.

## 6. One-command preflight and unit tests

Clone/access this repository through the connected GitHub integration or a permitted git working checkout. The following examples assume you are in the repository root and have locally extracted the **private, correctly patched** bridge and Mesen executable. Set these to actual absolute paths; do **not** literally type the bracketed placeholders.

```bash
python3 research/Genghis_Khan_NES/tools/headless_mesen_campaign_test.py --mode doctor
python3 -m unittest -v \
  research/Genghis_Khan_NES/tools/test_headless_mesen_campaign_test.py
```

The first command is **DOCTOR ONLY**, not an emulator test. Unit tests verify the decoder with synthetic SRAM, the current annual army-target formula, Winter reserve constants, state mismatch rejection, invalid officer pointers, PPM validation, output overwrite protection and argument validation. They do **not** create or advance a game.

Recommended environment variables for the actual run:

```bash
export GK_ROM='/absolute/private/path/Genghis Khan (USA).nes'
export GK_STATE='/absolute/private/path/GK1_1186_Spring_START_10YEAR_TARGET_VERIFIED.mss'
export MESEN_BIN='/absolute/path/to/Mesen'
export MESEN_FOR_AI_REPO='/absolute/path/to/mesen-for-ai'
```

The original ROM SHA above is required in the example; the saved checkpoint hash is **optional but recommended** when downloaded. If the restored file is a `.mss.bin`, that extension alone is not corruption: verify its size/hash and pass its exact local path. Do not rename a private binary simply to satisfy a tool argument.

## 7. Test the actual original game / restore a known save

```bash
OUT="$(mktemp -d -t gk1-probe-XXXXXX)"
python3 -u research/Genghis_Khan_NES/tools/headless_mesen_campaign_test.py \
  --mode checkpoint \
  --rom "$GK_ROM" \
  --expected-rom-sha256 ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3 \
  --restore "$GK_STATE" \
  --mesen-bin "$MESEN_BIN" \
  --bridge-repo "$MESEN_FOR_AI_REPO" \
  --expect-year 1186 --expect-season 0 --expect-gold 832 --expect-orders 3 \
  --frames 2 --session-timeout 90 --out "$OUT"
cat "$OUT/RUN_RECEIPT.json"
```

**Expected from the pinned Spring 1186 checkpoint** (2026-11-05 live-tested): `RUN_RECEIPT.json` contains `"status": "PASS"`, `"restore_completion": {"state_status": {"operation": "loaded", ...}}`, `gk1_state` with the exact expected year/Gold/orders and a frame of **256×240**. It may report a bridge frame counter inherited from the save; do not equate that counter with the local wall-clock elapsed time or a complete new season.

Optional **noncommitting D-pad input** proof (new output folder required each time):

```bash
OUT2="$(mktemp -d -t gk1-input-probe-XXXXXX)"
python3 -u research/Genghis_Khan_NES/tools/headless_mesen_campaign_test.py \
  --mode checkpoint --rom "$GK_ROM" \
  --expected-rom-sha256 ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3 \
  --restore "$GK_STATE" --mesen-bin "$MESEN_BIN" \
  --bridge-repo "$MESEN_FOR_AI_REPO" \
  --expect-year 1186 --expect-season 0 --expect-gold 832 --expect-orders 3 \
  --probe-button up --hold-frames 5 --release-frames 25 \
  --frames 2 --session-timeout 90 --out "$OUT2"
```

This verifies the bridge accepted a directional button pulse and that Year, Season, Orders and Gold did not change. It **does not** prove every menu works, physical Xbox connectivity, or that a user can complete a campaign from that frame. An A/Start button isn't automatically issued by this diagnostic harness because either could commit a consequential order.

For **ROM-only smoke** without a save, use `--mode smoke` and omit `--restore`/`--expect-*`. `run.step_frames(frames=N,reset=True)` validates a cold deterministic boot and screenshot but GK1 ruler-country state may not yet be initialized at the title screen; don't invent a meaningful 1175 kingdom state from uninitialized bytes.

To explicitly write a NEW private `.mss` checkpoint as test output, add `--checkpoint-copy`. The harness refuses to overwrite a nonempty output directory or the source state, verifies a nontrivial saved-file size, and hashes the new file. **This is not necessary for a read-only checkpoint smoke test.**

### 7.1 How to read the receipt

| Field | Meaning / acceptance |
|---|---|
| `preflight.rom_sha256` | Exact ROM bytes actually loaded; compare to authoritative variant |
| `preflight.mesen_sha256` / `bridge_sha256` | Exact emulator and bridge version that made the result |
| `restore_response.operation=pending` | Merely acknowledged; **never** success by itself |
| `restore_completion.state_status.operation=loaded` | Actual original-game VM callback accepted the state |
| `gk1_state.year/season_index/orders/gold` | Exact original NES SRAM fields, independently asserted |
| `gk1_policy_v2_readonly` | Planning arithmetic only; not a performed order |
| `frame.width/height/sha256` | Real Mesen rendered PPM frame, preserved locally |
| `probe_button` | A D-pad pulse was requested; resource/order invariants checked |
| `status=PASS` | This bounded harness passed; **not** a complete AI campaign |
| `status=FAIL` | A supported check failed; preserve receipt and troubleshoot |

Inspect `events.jsonl`, which opens, appends and closes on each event, and `frame.ppm`. The handler intentionally does **not** copy the entire ROM, copyrighted images into public GitHub, or full 8,192-byte SRAM into public logs. Private test receipts may contain absolute local paths; redact them if sharing publicly.

## 8. Direct headless controller workflow for real campaigns

The simulator is not an idle background process; each assistant session must either actually execute with tools or set a verified scheduled automation when asked. For a real gameplay campaign, attach an input planner/controller to this verified bridge. **Use the current Director strategy** and original in-game UI as the authority for command choices.

1. **Cold-load with proof:** confirm ROM SHA, bridge/Mesen versions, exact `.mss` hash; drive only enough temporary title inputs to reach the GK1 VM callback, call `state.restore`, step until `state.status.operation=loaded`, and compare ruler/country/year/orders with the journal. Use a new local working directory. Do not reset after restoration.
2. **View before acting:** inspect the actual screen and ruler stats; read current PPU/CPU state only to observe (not edit). Resolve the player's active country from the documented GK1 word at `$7B34`; legacy trace field `country` was `$7B66` and does **not** identify the ability actor.
3. **Protect orders:** the NES season typically grants only three commands. Calculate mandatory self-training, eligible hire, labor allocation, and native Fur morale gift costs *before* optional speculative buys or troop training. View is normally free; actual changing actions consume commands. Record any task deferred due to real scarcity.
4. **Controller input:** from the custom bridge use `input.set(port=0, subport=0, buttons={'down':True})`, run bounded frames (e.g., 5), release `buttons={}`, then advance ~25–100 frames depending on screen. Each press should have a clear start/end; input belongs in `inputPolled`, not an arbitrary CPU callback. Inspect selected text/pixel cursor before pressing A to confirm.
5. **Numeric entry:** the original game has its own digit/cursor rules. Do not reuse a generic `number(q)` routine without testing that menu and its place-value direction. Confirm the displayed quantity visually before committing; screenshots and read-only RAM both matter.
6. **Labor screen:** Soldiers are the **remainder** after civilian job allocations; they may not appear as a directly editable field. In the actual campaign, entering a quantity resets the selection to the first job, defeating scripts that assume the previous cursor index is retained. Recompute each new selection from the actual current screen. Verify civilian job totals + Soldiers preserve drafted P and reserve stays unchanged.
7. **Trade screen:** verify merchant mode (Buy/Sell/Hire), item icon, actual quote and quantity before A. Rapid directional presses can fail to register; do not blindly send 6×Down and assume it selected the sixth item. A cancelled or failed selection is *not* a trade. Verify Gold, stock, Judgment and orders after confirming.
8. **Adverse game messages:** during the historical 1177 replay, a `Sickness has risen` notification appeared. Such alerts require inspection and possibly response, not abandonment. Incoming alliance prompts should be accepted under the standing Director policy. If a battle/revolt/end condition appears, inspect the actual event instead of discarding it or rewinding the state.
9. **Save every real seasonal turn:** create checkpoint, wait for `state.status.operation=saved`, verify file bytes/size/hash, and record the actual season and player-turn status. Name saves according to **in-game** season (not script expectation). Publish private checkpoints to the authorized Drive folder and read metadata to verify parent and MIME. Journal the before/after stats and exact input actions; note when a Winter checkpoint was taken during Spring AI rotation.
10. **No outcome selection:** restore only to recover a session timeout/input defect from the last verified causal state. Never reroll a bad trade, random event, diplomatic choice or illness to improve the strategy result. Label reconstructed replay branches separately from the original run.

### 8.1 Reference raw bridge calls (illustrative, not a second driver)

```python
from mesen_client import Mesen
with Mesen(repo='/absolute/path/to/mesen-for-ai') as m:
    loaded = m.load_rom('/private/Genghis Khan (USA).nes', timeout=90)
    # For a brand new ROM session, first reach the game VM callback, as in
    # headless_mesen_campaign_test.py -> bootstrap_gk1().
    m.tool('input.set', port=0, subport=0, buttons={})
    pending = m.tool('state.restore', path='/private/GK1_1186_Spring.mss')
    # pending.operation may be 'pending'; step and check state.status.
    # Never reset=True after loading a checkpoint.
    m.tool('run.step_frames', frames=2, reset=False)
    status = m.tool('state.status')
    assert status['operation'] == 'loaded', status
    ram = bytes(m.tool('cpu.read_memory', memoryType='nesMemory', address=0x6000, length=8192)['bytes'])
    m.tool('video.export_frame', path='/new/private/evidence/frame.ppm')
```

For the actual tested path use the wrapper script rather than copy-pasting this short illustration, because it has strict preflight, additional restore polling, output protection, and screenshot verification.

## 9. Problems actually encountered and proven corrections

| Symptom | Observed cause | Correct recovery / proof gate |
|---|---|---|
| Bridge never ready, blank stdout/stderr | Initial MesenCE **Configuration** wizard hidden under Xvfb | Screenshot real host window, find advertised config path, seed/backup correct profile, relaunch; then require MCP `session.load_rom` success |
| `state.restore` returns `pending`; subsequent RAM reads all zero | Original GK1 VM `$EFC9` callback not running at title screen | Execute historical GK1 title/bootstrap prefix on a **temporary** session before `state.restore`; poll `state.status` until `loaded`, then assert save fields |
| Unexpected menu after Down sequence | Press too short/fast; cursor didn't move as assumed | Pulse/releases with frame gaps; verify actual selected item and menu mode before A |
| Town/Food/Artisan labor quantity ends in wrong field | Labor entry resets menu selection; Soldiers are calculated remainder | Navigate anew from actual screen, confirm full workforce conservation after each committed allocation |
| AI turn/session exceeds execution time | Long frame-step/interaction and many AI prompts exceed task or bridge time budget | Run smaller bounded chunks, store local native `.mss` at every calendar boundary; resume from same causal save, not fabricated later state |
| Old bridge has no `state.restore` or `state.status` | Stock upstream bridge differs from private GK1 patched bridge | Use exact authorized private recovery checkout or forward-port feature with explicit source QA; never claim a native restore was done |
| Screenshot shown but no successful action | Screenshot alone does not prove a command committed | Cross-check Orders, abilities, Gold, inventory, seasonal state; retain failed attempts in raw event log |
| `.mss` filename says Winter but year shows Spring | Saved after Winter's last order during automatic Spring AI rotation | Separate **player season completed** from exact active VM year; label in journal |
| Historical log/report says more turns than current folder | Saves were unpublished, omitted, or placed in another branch | Search actual saved bytes, not just titles/chat; compare hashes and in-game state; reconcile journal as a later correction without deleting older evidence |
| Old process killed by Ctrl-C / code 1 | Director Stop or timeout | Preserve raw logs, last native save and status. Only resume after explicit authorization (or user continuation request), not by guessing post-stop actions |
| Wrong ROM variant | Name matches but source hash differs | Fail before boot; do not test modified/Fonzarelli ROM with the stock original SHA pin |
| `cpu.read_memory` gives uninitialized values | A cold title screen is not an active strategy turn | Use a saved game or enter scenario via real inputs; don't infer country statistics from zero-filled SRAM |
| `emu.log` absent from stdout | Mesen Lua logging does not feed testrunner stdout reliably | Write append-only JSONL through the bridge/test tool; inspect Xvfb process and native emulator frame |

Do **not** conceal earlier unsuccessful runs. In this exact guide's development, two real `session.load_rom` attempts failed because of the wizard/profile mismatch; one subsequent checkpoint call returned `pending` and correctly failed the state assertion; a revised bootstrap + status check **passed**. These are evidence of the repair process, not fictitious all-pass initial tests.

## 10. Hostile QA for every change

Separate evidence classes and acceptance:

- `VERIFIED_STATIC`: known ROM/SHA, script bytes, Python syntax, offline unit results, and inspected .mss fields without game execution.
- `OBSERVED_RUNTIME`: running original ROM in Mesen, screen/frame capture, actual `state.status.loaded`, and bounded input mutation tests with commands, inputs, environment and output.
- `TESTED_PORT`: separately launching a real Windows build with actual environment and controller inputs. **Not** achieved by this headless reference.
- `REPORTED_UNREPRODUCED`: a prior assistant/agent claim or old log not independently reproduced on this run.
- `INFERRED`: proposed strategy outcome or formula use outside tested bounds.
- `UNKNOWN / CONTRADICTED`: missing or conflicting evidence; do not fill gaps with plausible guesses.

**Required gates for a new session:**

1. Pin authority and project, ROM game/region/mapping, Mesen and bridge hashes, save hash, source revision and whether original or modified variant.
2. Run `--mode doctor`, offline unittests and the real `--mode checkpoint` smoke. Each must have exact command, stdout/stderr, exit code and JSON receipt. A green source-only unit test is not runtime proof.
3. Validate real restored **year/season/active ruler/orders** and rendered frame. Use a negative control: wrong expected Year or ROM hash **must FAIL** rather than silently falling back to a new game.
4. Validate controller pulse and release only when the current screen is safe; label its scope. For a full controller plan, test A/B/Start and numeric entry on disposable copies of a safe checkpoint, verifying no unintended hidden actions.
5. Execute ordinary gameplay only with the user-authorized strategy; save each turn and test a **cold reload** of a newly saved state. A saved file by itself is not proof it can reload.
6. Record skips, failed opportunities, unavailable offers, hidden startup wizard, terminated sessions and any manual input; do not trim raw log failures from a purported complete test log.
7. Check actual new artifacts in the expected Drive folder; published GitHub reference text is **not** the same as publication of private Mesen saves.

### 10.1 Tests performed while preparing this reference (November 5, 2026)

- **PASS — source/static:** `python -m py_compile .../headless_mesen_campaign_test.py` and the accompanying standard-library `unittest` suite (**13 PASS**, including negative asynchronous-restore controls). These do not run a ROM.
- **FAIL, retained — runtime startup:** native MesenCE opened the first-run setup wizard and did not start the bridge when only the wrong host profile was seeded.
- **FAIL, retained — restore path:** after repairing the profile, `state.restore` acknowledged `pending` at a cold title screen; year/cash/orders remained zero. This ruled out declaring success on a pending operation.
- **PASS — actual original ROM:** after performing GK1 bootstrap, the custom bridge returned `state.status.operation=loaded`, restored Spring 1186, Gold 832, Orders 3, Leadership 137, and rendered 256×240 P6 frame. No strategic order committed.
- **PASS — controller input bounded:** same original ROM and checkpoint, D-pad Up for 5 frames then released 25 frames; native game Year/Season/Orders/Gold remained unchanged, with an actual frame captured.
- **PASS — new native checkpoint cold reload:** `--checkpoint-copy` saved a NEW 14,580-byte native `.mss` and reported `state.status.operation=saved`; a new emulator session successfully loaded this generated copy and independently rechecked Spring 1186, Gold 832, Orders 3. Source `.mss` was not overwritten.
- **NOT RUN:** autonomously playing 10 or 120 seasons with this new diagnostic harness; any Windows port/hardware controller/parity claims.

Reference original ROM SHA is `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`; native emulator SHA is `f02685dafe559addf8dd8d769621a89777137d0c4592436aed65694a7e740676`. Public test outputs include no private state or copyrighted game assets; actual generated receipts, ROM and PPM remain local/private to the authorized execution environment.

## 11. Other-project lessons: what to reuse and what not to equate

- **Upstream `mesen-for-ai`**: tested design for per-session TCP bridge, MCP stdio, held frames, deterministic reset, `inputPolled` injection and CPU/PPU memory names. Its host and library setup requirements are **source-derived**, not proof of this project's campaign strategy.
- **SecondPlayer's MesenCE headless adapter**: useful independent example of verifying real frame capture, explicit controller port/mask and config isolation before allowing a learning agent to play. Its Laya/model inference path and SNES Player 2 topology are irrelevant to this single-player NES campaign; do not copy them into GK1 testing.
- **Mesen2 upstream**: source of emulator behavior and releases, but native MesenCE binary + bridge API compatibility must be established for each new version; a successful old hash does not certify an arbitrary new build.
- **KOEI original-ROM research v1.07**: provides actual VM state offsets, commodity formula checks and native-evidence QA. Synthetic economic vector replay and CPU instruction verification are **not** a substitute for newly played turns, battles or controller visuals.
- **ROTK II NES Windows**: original NES/SNES decoded battle mechanics and Windows/physical Xbox controller acceptance are separate milestones; a GK1 save-state PASS cannot settle either one.
- **KOEI Bridge QA**: a long raw input log is not a substitute for state-change assertions. Compare controlled negative cases and restore/rollback, and classify the evidence precisely.

## 12. What to tell ChatGPT next time

Copy this short task into a future ChatGPT/Work session **after connecting it to real code/Drive access**:

> "Read `Bomb-Daddy/NES/reference/MESEN_HEADLESS_CHATGPT_OPERATIONS_v1.00.md` and the public `research/Genghis_Khan_NES/tools/headless_mesen_campaign_test.py`. Confirm the current project authority and my latest private ROM/Mesen/.mss assets in the Temujin Drive folder. Execute the offline tests, then cold-load the *latest hash-matched native checkpoint* in real headless Mesen. Confirm the screenshot, country statistics and controller probe. Preserve the source save, create a new evidence folder, and only then continue the authorized season-by-season strategy in small batches with actual private saves and journal updates. Do not stop because of ordinary input timing or tool timeouts; do not claim a run happened without receipts. If no execution tool or compatible private bridge is actually available, report that precise blocker instead of pretending to play." 

For a different game, substitute its canonical ROM, emulator, source authority, savestate, menu controls, memory layout and success conditions; **do not run GK1's bootstrap sequence or decoder on another game**. A public guide can tell ChatGPT what to do, but cannot by itself make a specific session own a cloud VM or run code.

---

**Document status:** READABLE REFERENCE COMPLETE; source code and tests in the same GitHub repository. Proof of actual delivery requires GitHub commit SHA + file readback, not this local draft.
