# Original KOEI-inspired Enhanced PC game — Build40R3 (music separated)

This folder is for the Director's **separate Original KOEI-inspired Windows/Linux game**. Do not mix its source into the canonical ROTK II NES restoration simply because this repository is called NES.

## Repackaged artifacts (locally verified; NOT yet stored in this repository)

- **Game:** `KOEI_ENHANCED_BUILD40R3_NO_MUSIC_20261102.zip`; 68,112,121 bytes; SHA-256 `ad62439cdeead6769d022c79e2940957af268ab8e5a67c77b2b96f2fb782c8c1`. 512 entries, ZIP CRC PASS. Contains source, graphics, 14 generated sound effects, tests, and the game launcher. Excludes the 15 privately supplied MP3 tracks.
- **Music Assets:** `Music Assets.zip`; 80,632,011 bytes; SHA-256 `ee7078303874ccd44c1cc3737a9414dd8962727109435ed63b30dbb80fdf70fa`. 15 MP3s plus install instructions, ZIP CRC PASS. Its recording rights are **NOT cleared for public distribution**, so do not upload this archive to the public repository without appropriate rights.
- **Original:** `KOEI_ENHANCED_BUILD40R3_HELP_TROOP_UI_CANDIDATE_20261102.zip`; 148,742,521 bytes; SHA-256 `dc51379520ec55765d983507724b2fe884f195b48d85b86ecaa4115b6191c4f4`, preserved unchanged outside GitHub.

The two new ZIPs are each below GitHub's **100 MB per-file limit**. **This status document is not a claim that the binaries have been uploaded.** The current GitHub connector supports text writes and inline blob data, but does not provide a tool to stream container-local 68 MB / 80 MB files to GitHub. Accordingly both ZIP binaries remain local downloadable artifacts and are **NOT PUBLISHED IN GITHUB**. The provisional 77-row command matrix IS committed and read back here:

[Command verification matrix](./Build40R3/ALL_COMMANDS_VERIFICATION_MATRIX_PROVISIONAL_20261102.csv)

## Verified source-only and optional-music tests (Linux)

- **No music pack:** `KOEI_DISABLE_AUDIO=1 PYTHONPATH=. xvfb-run -a python -m pytest -q` in clean extracted core: **657 passed, 7 skipped, 2 xfailed, 3 subtests passed, exit 0** (48.08s).
- **With private music overlaid:** `KOEI_DISABLE_AUDIO=1 PYTHONPATH=. python -m pytest -q tests/test_private_music_and_sfx_v113.py tests/test_events_and_bundled_audio_v115.py tests/test_windows_port_v116.py`: **26 passed, exit 0**. `verify_private_assets()` returned `ok=True`, 15 MP3s, 14 WAV SFX, no hash errors.
- Missing music is nonfatal. `python -m third_branch.windows_doctor` returned `ok=True` in the no-music install, with a warning about 15 absent optional tracks. These are Linux tests, not Windows or actual audio-speaker validation.
- ZIP CRCs pass and the split manifest proves all 15 original MP3 members were moved and 507 other original members remained byte-identical; four test/preflight files changed to support optional music.

## Installation and regression

Unzip game archive first. Music is OPTIONAL; the game reports missing private tracks and still keeps sound effects. For authorized private music installation, unzip `Music Assets.zip` into the same extracted project root. Re-run the game after overlay. All 15 MP3s are SHA-checked against the retained music manifest.

This source candidate is **not a released game**: geographically accurate China overview, complete province art, all real commands, 300 matched simulations and full combat parity remain open. The Director already accepted Windows installation and Xbox controller support as standing baselines; do not reopen those baselines.

This source/audio split changes only optional-audio preflight logic and three corresponding tests; no game mechanics or existing art or launcher entries were intentionally changed.
