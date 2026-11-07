# Run039 Q08 Friendly Occupied Fire — QA Evidence

Direct source: ROTK II Answer Matrix v1.4 Q08, original ignition `hex:$818A`, `lookup_c99c_field` `$C99C`; target is selected hex. If a live unit exists on a hex, source ignition and casualty path uses that unit's Intelligence/troops. Source description has no side filter. This is a source-backed port correction; original English NES controller/pixel replay is still UNRUN and should independently verify that choosing a friendly occupied hex is legal and which original message appears. Unrestricted 1:1 is OPEN.

Parent immutable Run038 SHA `1a1b193c1bb8f4aa5f5f343bbcc9c5617d4889cd2b550f5a8a03aa2041c3e14f`. New complete Run039 SHA `e2a1581af46f900fe16cc311566a27946320689ffb99e8a0ad21e72278153935`. New ZIP 148793567 bytes, 557 members, 554/556 inherited byte-identical. Production changes only `third_branch/officer_battle_adapter.py`; prior Run037 regression reflecting old incorrect policy updated, original preserved immutable in Run038 parent; added new seven-case regression.

Actual Linux Python3.13.5/Xvfb Tk:
* RED pristine Run038 5 failed / 2 passed on the new seven cases, after corrected fixture setup.
* GREEN Run039 Q08 new+inherited 24 passed.
* Negative mutant old friendly-fire veto 3 expected failures/4 controls passing; source restored 7 passed.
* Selected broad 206 passed; independent Run039 complete-ZIP cold extraction 24 passed.
* Actual Tk software controller Attack > Fire tests selectable occupied friendly hex and B cancel. Correct target selected by coordinate, no unrelated officer fallback; normal melee ally rejection remains. Actual `.ro2` written to disk and loaded from a separate Python process.
* Original ROM 30 emulator event goldens, Windows native and physical Xbox NOT RUN; 41-map full parity NOT VERIFIED.

To build byte-exact source use `build_run039.py <exact_Run038.zip> <output_Run039.zip>` with sibling `run039_changed_source/`; script rejects wrong parent SHA and checks exact output hash/CRC. Every historical source ZIP remains immutable. QA sources are separate from Classic NES and DOS.
