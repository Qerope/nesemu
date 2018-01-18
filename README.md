# nesemu

Work in progress NES emulator.

## Status

CPU: all official opcodes. To check it, run the nestest ROM headless and compare
with the reference log:

    make
    ./nesemu --nestest nestest.nes > trace.log
    python3 tools/cmp_nestest.py trace.log nestest.log

(nestest.nes and nestest.log are not included; they are on the nesdev wiki.)
