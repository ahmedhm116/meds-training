# Design Document — RISC-V RV32I Instruction Decoder

## Architecture Overview

The project is split into three independent modules, each with a single
responsibility:

- **memory.c / memory.h** — owns everything related to loading a `.hex` file
  into a simulated byte-addressable memory array. It knows nothing about
  instructions or decoding; it only deals with file I/O and little-endian
  byte ordering.
- **decoder.c / decoder.h** — owns everything related to interpreting a raw
  32-bit value as a RISC-V instruction. It has no knowledge of files, memory,
  or where the instruction came from. It is a pure function: given a
  `uint32_t`, it returns a `decoded_instr_t`.
- **main.c** — the only module that knows about both. It coordinates the
  two: load memory, then loop over it calling the decoder and printing
  results.

This separation means each module can be reasoned about, tested, and
debugged independently. If a decoded value is wrong, the bug is in
`decoder.c`. If the wrong bytes are loaded, the bug is in `memory.c`. The
decoder can be unit-tested with hardcoded instruction words without needing
a `.hex` file at all, which is exactly how `test_decoder.c` is structured.

## Decoded Instruction Representation

`decoded_instr_t` (defined in `decoder.h`) stores both the raw 32-bit
instruction word and every individually extracted field (`opcode`, `rd`,
`rs1`, `rs2`, `funct3`, `funct7`, sign-extended `imm`, and the resolved
`mnemonic` string).

Storing the raw instruction alongside the parsed fields — rather than only
the parsed fields — serves two purposes. First, the output table prints the
original hex word (e.g. `00500113`) directly from `raw`, without needing to
reconstruct it from the separated fields. Second, it makes debugging easier:
if an extracted field looks wrong, the raw value can be manually re-checked
against the bit-extraction logic.

## Decoding Strategy

Decoding happens in two levels, mirroring how the RISC-V ISA itself is
structured:

1. **Opcode → instruction format.** The 7-bit opcode (bits [6:0]) is
   extracted first and identifies the instruction's format: R-type, I-type,
   S-type, B-type, U-type, or J-type. This is implemented as the outer
   `switch` in `decode_instruction`.
2. **funct3 (and funct7 where needed) → exact mnemonic.** Knowing the format
   alone is not enough — opcode `0x33` (R-type) covers ten different
   instructions. A nested `switch` on `funct3` narrows this further, and for
   the cases where funct3 alone is ambiguous (ADD vs SUB, SRL vs SRA), an
   additional check on `funct7` resolves the exact mnemonic.

Any opcode/funct3/funct7 combination that doesn't match a known instruction
falls through to a `default` case that sets the mnemonic to `"UNKNOWN"`
rather than crashing or leaving the field empty.

## Immediate Sign Extension

Each instruction format encodes its immediate value differently, both in
which bits are used and in how those bits are laid out within the 32-bit
word:

- **I-type:** a contiguous 12-bit field at bits [31:20].
- **S-type:** split into two pieces — bits [31:25] and [11:7] — because
  bits [11:7] is the same physical position used for `rd` in other formats.
- **B-type:** split into four pieces across bits [31], [7], [30:25], and
  [11:8], with the lowest immediate bit implicitly zero (branch targets are
  always 2-byte aligned).
- **U-type:** a 20-bit field at bits [31:12], left-shifted into the upper
  bits of the result with the lower 12 bits implicitly zero.
- **J-type:** split into four pieces across bits [31], [19:12], [20], and
  [30:21], again with the lowest bit implicitly zero.

The reason the bits are non-contiguous for S, B, and J types is that RISC-V
keeps the *register fields* (`rd`, `rs1`, `rs2`) in the same bit positions
across every instruction format. This allows hardware to begin reading the
register file before it has even finished determining the instruction type.
The immediate bits are placed in whatever positions are left over once the
register fields are fixed, which is why they appear scrambled. The
decoder's job is to reassemble these scattered bits back into the correct
numerical order using shifts and ORs.

After reassembly, each immediate is sign-extended by checking its sign bit
(the most significant bit of its original encoded width) and, if set, OR-ing
in 1s through the remaining upper bits of the 32-bit `int32_t` result.

## Known Limitations

System instructions under opcode `0x73` (ECALL, EBREAK, and CSR
read/modify/write instructions) and FENCE are not implemented, as they were
outside the scope of the assignment's required instruction set. Any such
instruction encountered will print as `UNKNOWN`.

## Testing Approach

Four `.hex` test programs are provided under `test/programs/`:

- `r_type.hex` — exercises R-type arithmetic and logic instructions.
- `i_type.hex` — exercises I-type arithmetic instructions, including
  negative immediates to verify sign extension.
- `branch.hex` — exercises B-type branch instructions, including a
  backward (negative offset) branch.
- `mixed.hex` — a short instruction sequence spanning R, I, S, B, and J
  types, matching the expected output given in the assignment description,
  used as the primary correctness check.

`test_decoder.c` additionally unit-tests `decode_instruction()` directly
against known instruction words and their expected decoded fields, without
going through the file-loading path.