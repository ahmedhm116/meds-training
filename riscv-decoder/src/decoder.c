#include "decoder.h"
#include <stdio.h>
#include <string.h>

static uint32_t get_opcode(uint32_t instruction) {
    return instruction & OPCODE_MASK;
}

static int32_t get_imm_i(uint32_t instruction) {
    int32_t imm = EXTRACT_BITS(instruction, 20, 31);
    if (imm & 0x800) { // Check if the sign bit is set
        imm |= 0xFFFFF000; // Sign-extend to 32 bits   
    }
    return imm;
}

static int32_t get_imm_s(uint32_t instruction) {
    int32_t imm = (EXTRACT_BITS(instruction, 7, 11) | (EXTRACT_BITS(instruction, 25, 31) << 5));
    if (imm & 0x800) { // Check if the sign bit is set
        imm |= 0xFFFFF000; // Sign-extend to 32 bits
    }
    return imm;
}

static int32_t get_imm_b(uint32_t instruction) {
    int32_t imm = (EXTRACT_BITS(instruction, 8, 11) << 1) |
                  (EXTRACT_BITS(instruction, 25, 30) << 5) |
                  (EXTRACT_BITS(instruction, 7, 7) << 11) |
                  (EXTRACT_BITS(instruction, 31, 31) << 12);
    if (imm & 0x1000) { // Check if the sign bit is set
        imm |= 0xFFFFE000; // Sign-extend to 32 bits
    }
    return imm;
}

static int32_t get_imm_u(uint32_t instruction) {
    return (int32_t)(EXTRACT_BITS(instruction, 12, 31) << 12);
}

static int32_t get_imm_j(uint32_t instruction) {
    int32_t imm = (EXTRACT_BITS(instruction, 21, 30) << 1) |
                  (EXTRACT_BITS(instruction, 20, 20) << 11) |
                  (EXTRACT_BITS(instruction, 12, 19) << 12) |
                  (EXTRACT_BITS(instruction, 31, 31) << 20);
    if (imm & 0x100000) { // Check if the sign bit is set
        imm |= 0xFFE00000; // Sign-extend to 32 bits
    }
    return imm;
}

decoded_instr_t decode_instruction(uint32_t instruction){
    decoded_instr_t decoded;
    memset(&decoded, 0, sizeof(decoded_instr_t));
    decoded.instr = instruction;
    decoded.opcode = get_opcode(instruction);

    decoded.rd = EXTRACT_BITS(instruction, 7, 11);
    decoded.funct3 = EXTRACT_BITS(instruction, 12, 14);
    decoded.rs1 = EXTRACT_BITS(instruction, 15, 19);
    decoded.rs2 = EXTRACT_BITS(instruction, 20, 24);
    decoded.funct7 = EXTRACT_BITS(instruction, 25, 31);

    return decoded;
}
