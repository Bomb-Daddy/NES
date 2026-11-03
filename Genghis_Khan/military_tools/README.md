# Genghis Khan (NES) military-system tools

Decoding and verification tools used for `Genghis_Khan_NES_Military_System_Guide_v1.0.md`.

- `gkcore.c` / `gk.py`: emulator core and Python wrapper. Build the core first: `gcc -O2 -shared -fPIC gkcore.c -o libgk.so`.
- `vmdis.py`, `d6502.py`, `cmdtrace.py`: bytecode-VM disassembler, 6502 disassembler, VM tracer.
- `nav.py`, `wartools.py`: menu navigation and battle helpers.
- `*verify.py`: each checks one decoded rule against the game, with RAM values set by the script.
- `state_*.bin`: save-states the verify scripts load (run scripts from this folder).
- `vm_opcode_table.txt`, `vm_handlers.txt`, `train_troops.json`: reference tables.

The ROM is not included. Set `GK_ROM` to the path of `Genghis Khan (USA).nes`, or put the ROM in this folder.

```
GK_ROM="/path/to/Genghis Khan (USA).nes" python3 combatverify.py
```

`targetverify.py` and `warverify.py` take an optional argument (player Troops, e.g. `3`).
