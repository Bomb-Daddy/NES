# Original-KOEI Enhanced Run045 Q04 partial-deployment save guard (research-only evidence)

**Not canonical NES Classic source; not DOS.** Canonical Original-KOEI full source remains in Google Drive `Retro Gaming/Original_KOEI_Windows_Game`. This GitHub folder holds test/patch/manifest only and must not be promoted as complete game's source.

Latest fully independently DRIVE-verified parent: Enhanced Run044 complete ZIP SHA-256 `20fe5a3ed964d4cd4854419ba887dadca2df9f7cfee420ae3041cbd206eb14e1`, 148815052 bytes, 567 ZIP entries, Drive folder https://drive.google.com/drive/folders/158Sfjh_9nvGEutRisW8eyvJj2GDeLn76.

Run045 **local tested candidate**: full ZIP SHA-256 `e6a171c5a7b1a1e042d18f3998d7e7e2fb8e9910183ff88ac9896684c6f9d4dd`, 148818054 bytes, 569 CRC-clean ZIP entries, 566/567 inherited unchanged. Exact modified module `third_branch/q04_manual_deployment.py`; one new five-case test module and README. No economic/Body Points changes.

Q04 English NES source basis: source-derived 41 province region data SHA-256 `9bc5a0c945920e0d6fa50c4fcca3cd4c75ff2b986cd147c912f59aba6edb5aa1`, original hsovl6 $D0CF/$CE5C. Run045 additionally refuses *partially deployed saved battles* whose already committed human officers have illegal source-zone coordinates, enemy overlap, or fire; pending officer preview remains nonoccupying; guard is disabled after final deployment to allow ordinary movement. A source-backed port/save invariant, not a new original NES emulator runtime golden.

Actual Linux Python3.13.5 Xvfb/Tk QA:
- Untouched Run044 + regressions: 3 intended RED failures, 2 controls pass.
- New Run045 source: 5 green; related focused: 8 pass.
- Negative switched-off mutant: 3 intended failures, 2 controls pass; restored 5 pass.
- Actual six-command Tk software controller and related QA: 18 pass.
- Broad selected 335 pass; fresh cold extracted ZIP GUI scope 14 pass.
- Four independent Python processes real .ro2 disk save/quit/cold-load, fingerprint `b19a1e3ff5e523757a5c40d048238dea07c25e705e1015a6aab7f6a3ce08408a`, disk file SHA256 `4fd354a6d987ef3f4a77bbb2ec5d7e3b3b6e6c0595bb8664294b135574a50924`, 38000 troop hundredths conserved.
- Unrestricted collection: 911 collected /2 inherited errors (missing original ROM fixture and rollback dotted module), not green. Original 30 event goldens, current Windows, physical Xbox not run.

**BINARY EGRESS BLOCKED:** New correct original-game folder https://drive.google.com/drive/folders/18ASNq454OzIpoxGxjuh8jUSQL73ggMY5 exists, but attempted QA and first 8MiB source part uploads through `container_path` both returned `container_session_unavailable`. All 18 local verified source parts and QA exist, but NOT independently Drive read back. Therefore do NOT promote Run045: Run044 remains latest verified Drive full source.

Collaboration handoff 105: https://docs.google.com/document/d/196Kf3PW7goDOIZPLW_BGS3NVJksGCmwA8M1zqH06Wx8/edit ; source register https://docs.google.com/document/d/1UXDXguwS0EGKKTInRWZraSFWabRD6pSuYHq9LKTBjZE/edit ; local Completion Register Google Doc https://docs.google.com/document/d/1QzpRyUn5iY9uCOa5kspeccoXbrr6JqLF1mTUAGUNYrs/edit .

Next: use authentic exported file snapshot refs to publish exact 18 parts and QA to Run045 Drive folder; independently fetch all SHA/CRC and run Drive-cold tests before promotion. Continue original NES source/emulator-supported Q04 comparisons; no invented second tactical battle scheduler. External agent acknowledgment unknown.
