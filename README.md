LC-3 CPU Emulator

A clean, lightweight LC-3 (Little Computer 3) architecture emulator written in C99.

This project implements a virtual machine executing the LC-3 instruction set specification, complete with dynamic .obj image file loading and memory-mapped I/O.

Features
- Instruction Set Implementation: Supports core arithmetic, logic, PC-relative, base-offset, indirect memory operations, and control flow.
- Condition Flags: Status flag updates (N, Z, P) synchronized with register writes.
- TRAP Routines: Implementation of system call routines (GETC, OUT, PUTS, IN, PUTSP, HALT).
- Memory-Mapped I/O: Keyboard Status Register (KBSR at 0xFE00) and Keyboard Data Register (KBDR at 0xFE02) via non-blocking I/O.
- Binary Image Loader: Cross-platform .obj loader handling Big-Endian streams with boundary validation.

Building

Compile using make:

make

To build with Address and Undefined Behavior Sanitizers enabled:

make sanitize

Usage

Run the emulator by passing a compiled LC-3 .obj binary file as an argument:

./lc3_emu program.obj

Testing

Run the automated test suite:

make test

Limitations

- Interrupts & Privilege Modes: Real hardware interrupts, Supervisor/User execution modes, and the RTI instruction are not supported; executing RTI halts the emulator with an illegal instruction fault.
- Display Registers: Console output is stream-buffered via standard I/O rather than memory-mapped framebuffers.
