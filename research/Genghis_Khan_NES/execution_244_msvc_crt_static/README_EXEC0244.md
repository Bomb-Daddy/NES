# EXEC0244 — native Windows static-zlib CRT contract

Based **only** on exact complete EXEC0243 C11 source. Do not replace it with an isolated prototype. The archived World importer must remain enabled and link with vcpkg's `x64-windows-static` zlib.

Observed EXEC0243 Windows LNK4098: MSVC's application/runtime objects defaulted to /MD, while the approved static vcpkg triplet uses /MT. This changes the CMake target CRT setting **only** when MSVC, required archived import and the static triplet are all selected. CMake policy CMP0091 is set NEW before project(), preserving proper target runtime property semantics.

Exact predecessor CMakeLists SHA256: `2bda9246ca69ba5b5ff36ad5a3f8f4091625b7482d91404bdbae1c4310ce2ff2`. Postimage: `b288693a8e83056d2e35295792bf9dd487c6e6d0286308b697013b029bc6bdd2`. Apply `python apply244.py ROOT`, where ROOT contains the inherited `execution_199/` directory. A wrong or already changed predecessor is rejected before mutation.

`python test244.py EXEC0243_COMPLETE_SOURCE.zip` checks positive replay, source/policy order, rollback for two malicious changes, repeat refusal, and four CMake-evaluated condition combinations.

This is a Win32 packaging/runtime correctness increment, **not** actual original Spring initialization. The original ROM (SHA256 ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3) and original Mesen evidence remain private and unchanged. Original ruler/difficulty/character RNG, all three original strategic orders across both campaigns, AI, trade merchant scheduling, battle aftermath, diplomacy/dynasty, sound/art, physical Xbox and full end-to-end campaigns are not certified.

Release proof must include *actual* Windows MSVC build, check the generated WinMain and archived importer .vcxproj RuntimeLibrary is MultiThreaded for Release, and verify no LNK4098, not merely this source predicate. Linux source unchanged functionally. Keep the existing hourly run until accepted completed release or explicit Director stop.
