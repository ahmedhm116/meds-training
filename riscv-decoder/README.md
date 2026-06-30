# RISC-V RV32I Instruction Decoder

A command-line tool that reads a hex file containing RISC-V RV32I machine code,
decodes each 32-bit instruction, and prints the corresponding human-readable
assembly. This project is the front-end of a CPU simulator and models the
fetch/decode stage of a real RISC-V processor.

## Build Instructions

```bash
make            # build the decoder (bin/riscv-decoder)
make debug      # build with -DDEBUG and -O0 (debug logging, no optimisation)
make release    # build with -O2 and -DNDEBUG (optimised build)
make clean      # remove build/ and bin/ directories
```

## Usage

```bash
./bin/riscv-decoder <hex_file>
```

Example:

```bash
./bin/riscv-decoder test/programs/mixed.hex
```

The hex file must contain one 32-bit instruction per line, written as 8 hex
digits (no `0x` prefix), for example:

```
00500113
00A00193
003100B3
```

## Sample Output

```
RISC-V RV32I Instruction Decoder
================================
Loaded 8 instructions from test/programs/mixed.hex

Addr         Hex        Assembly
---------- ---------- -------------------------
0x00000000 00500113   addi x2, x0, 5
0x00000004 00A00193   addi x3, x0, 10
0x00000008 003100B3   add  x1, x2, x3
0x0000000C 40310133   sub  x2, x2, x3
0x00000010 0020A023   sw   x2, 0(x1)
0x00000014 0000A103   lw   x2, 0(x1)
0x00000018 FE209CE3   bne  x1, x2, -8
0x0000001C 004000EF   jal  x1, 4

Decoded 8 instructions (8 valid, 0 unknown)
```

## Project Structure

```
riscv-decoder/
├── include/        # header files (declarations only)
│   ├── common.h    # shared constants, macros, fixed-width type includes
│   ├── decoder.h   # opcode enum, decoded_instr_t struct, decoder prototype
│   └── memory.h    # hex file loader prototype
├── src/            # implementation files
│   ├── main.c      # CLI entry point, decode/print loop
│   ├── decoder.c   # instruction decoding logic
│   └── memory.c    # hex file loading into simulated memory
├── test/
│   ├── test_decoder.c    # unit tests for the decoder
│   └── programs/         # sample .hex programs for end-to-end testing
└── docs/
    └── DESIGN.md   # design rationale and decoding strategy
```

## Testing

```bash
make test       # builds and runs the decoder on test/programs/mixed.hex
make valgrind   # builds and runs under Valgrind to check for memory errors
```

Four test programs are provided under `test/programs/`, covering R-type,
I-type, branch, and a mixed instruction sequence matching the assignment's
expected output.

## Supported Instructions

- **R-type:** ADD, SUB, AND, OR, XOR, SLL, SRL, SRA, SLT, SLTU
- **I-type (arithmetic):** ADDI, ANDI, ORI, XORI, SLTI, SLTIU, SLLI, SRLI, SRAI
- **I-type (load):** LB, LH, LW, LBU, LHU
- **S-type (store):** SB, SH, SW
- **B-type (branch):** BEQ, BNE, BLT, BGE, BLTU, BGEU
- **U-type:** LUI, AUIPC
- **J-type:** JAL
- **I-type (jump):** JALR

Any instruction with an unrecognised opcode/funct3/funct7 combination prints
as `UNKNOWN` rather than crashing.