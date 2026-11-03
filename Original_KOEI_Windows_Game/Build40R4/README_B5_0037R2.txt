Fork4 Enhanced B37R2 candidate — new-game China-map/historical-ruler first-run integration.

Local candidate only; NOT a release and NOT Director-accepted.

Parent B37R1 complete source SHA256: 24c79651666a2c08ad83839ebe555a714195cc63d625dd5264cca0db3c222ccb

Linux source test: KOEI_DISABLE_AUDIO=1 xvfb-run -a env PYTHONPATH=. pytest -q tests/test_first_run_china_map_setup_b5.py
Full CI: KOEI_DISABLE_AUDIO=1 xvfb-run -a env PYTHONPATH=. pytest -vv
Run game: PYTHONPATH=. python -m launcher

New games require an occupied China-map province selection, followed by its historical ruler confirmation; no R0/P9 auto-start. A saved campaign resumes directly. The custom-ruler creation feature is still NOT implemented, and is not replaced by a fake custom ruler. Province-specific art, historic save migration, complete battle parity and 300 paired campaigns remain OPEN. Windows installation and Xbox controller support remain Director-accepted baseline requirements; no new hardware test is claimed.
