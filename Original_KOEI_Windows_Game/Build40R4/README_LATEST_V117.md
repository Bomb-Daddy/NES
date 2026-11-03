# Original KOEI-inspired Third Branch v1.17 — Han AI direct integration

This private Windows/Linux PC candidate preserves all original v1.16 media, historical campaign data, economy, UI, saved-game interfaces and platform bootstrap. It integrates the 12 Han-named deterministic simulated player policies v3.2 in actual historical-scenario PC month advancement. The previous v1.16 build remains available and unchanged. `PLAY_WINDOWS.bat` and `PLAY_LINUX.sh` are the entry points.

Source ancestry: verified v1.16 ZIP SHA256 d03696c5ebd3059c3c28877b7205c345749288bce6fa314f4fa2efa8ed151fe3.

Proof: `QA_V117_FULL_PYTEST.log` (354 passed, 2 expected failures, 3 subtests; exit 0), `QA_V117_12_MONTHS.json` (12 real historical months, 153 AI actions, 12 save/cold-load validations), and direct integration tests in `tests/test_han_ai_v117_integration.py`.

Historical ruler identities remain distinct from Han AI playstyle names. This is not a native-Windows-certified executable or human-playability acceptance. See `VERSION_DIRECT_AI_v117.md` for limitations and method.
