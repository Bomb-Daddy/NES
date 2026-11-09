# EXEC0085 honest hostile QA checklist

**Project:** English NES Genghis Khan I native Windows/Linux recreation, not ROTK II or original KOEI-inspired game. Inherited full EXEC0084 source and historical files; original 83-byte country records are read only from authenticated private owner data at runtime.

**VERIFIED_STATIC:** gk77 exact profile SHA256, original offset 0x0DCD source buffer and 83-byte country record constraints unchanged. gk77_decode copies all 83 bytes after checking input length; original known field offsets remain preserved. Gk79 retains all 14/27 raw records and SHA256; tamper-before-Pass rejected. Gk80 existing legacy numeric save now additionally requires in-memory raw integrity when saving/loading. Controller VIEW toggles raw/typed view, only in authenticated checkpoint. No unknown byte is interpreted as water, marriage, original skill formula or first-Spring RNG. Nothing added to public ZIP hardcodes a stock Gold/Rate outcome.

**TESTED_PORT_LINUX:** compile/test results, clean ZIP readback, Xvfb, Debug, sanitizer and package acceptance to be recorded in actual run receipt. No tests or screenshots containing privately recovered raw stock 83-byte bytes are to be publicized. The always-on test uses synthetic records and is explicitly labeled synthetic; private authentic record smoke requires the actual raw record export files, not inferred from screenshots/numeric receipts.

**OBSERVED_RUNTIME_STOCK (older authenticated captures, NOT rerun here):** fresh 1175 Gold430 Food208 Troops61 Rate121 orders3, archived 1175 Gold439 Food208 Troops61 Rate101 orders3, archived World1206 Gold193 Food332 Troops116 Rate114 orders3; source bank nesWorkRam fresh versus nesSaveRam archived. Modified ROM Rest case and 16x10 tactical map are *not* original stock.

**NOT RUN:** original-stock Mesen fresh reset in this increment; original first-Spring RNG; native Windows binary and installer execution; physical Xbox device; entire two-campaign end-to-end game. Game is **INCOMPLETE**. New read-only UI does not mean an authentic game-world turn has been generated.
