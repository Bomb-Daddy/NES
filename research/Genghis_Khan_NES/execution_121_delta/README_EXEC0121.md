# EXEC0121: Source-bounded NES 2A03 BIT/overflow branch support

Incremental implementation in the complete inherited EXEC0120 native application, not a replacement.

Adds documented BIT zero-page/absolute, CLV, BVC and BVS semantics to the existing original-ROM CPU source tracer. BIT reads are subjected to the pre-existing known-RAM/peripheral gates, with no assumption about PPU register values. This code does not solve RNG, first playable Spring or real AI. 524288 exhaustive BIT flag cases and cycle/page test, plus full inherited regression suite.

All Classic/Enhanced app screens, worlds, saves and previous history are preserved.

Evidence: VERIFIED_STATIC for ISA behavior and source code; TESTED_PORT_LINUX when CTest/real 1280x720 graphical smoke passes; OBSERVED_RUNTIME_STOCK previous Mesen captures preserved but not rerun; Win32 native build and physical Xbox NOT RUN.
