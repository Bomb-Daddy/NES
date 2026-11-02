# Build40R3 optional music package split — 2026-11-02

The original 148,742,521-byte Build40R3 ZIP was split **without modifying campaign or battle code**.

| Component | Bytes | SHA-256 | ZIP CRC |
|---|---:|---|---|
| Core no-music ZIP | 68,112,121 | `ad62439cdeead6769d022c79e2940957af268ab8e5a67c77b2b96f2fb782c8c1` | PASS |
| Optional Music Assets ZIP | 80,632,011 | `ee7078303874ccd44c1cc3737a9414dd8962727109435ed63b30dbb80fdf70fa` | PASS |

**Source integrity:** all 15 originally bundled arranged MP3s were extracted to the music archive. The main game ZIP contains zero music-format files. 14 generated WAV sound effects remain in the game. The core retains 507 unchanged source assets and modifies 4 narrowly related optional-audio test/preflight files.

**Installation:** Extract the core ZIP; start game. To restore music for private use, extract `Music Assets.zip` into the same directory preserving `third_branch/assets/audio/soundware_1989/`. No executable modification is needed.

**Copyright:** Original Soundware 1989 recordings are private Director-supplied music; public distribution rights are not verified. This GitHub document does **not** include those recordings or either ZIP binary. Game is a CANDIDATE, not accepted release.
