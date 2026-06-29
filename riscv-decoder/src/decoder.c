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

    switch (decoded.opcode){
        case OP_R_TYPE:
            decoded.imm = 0; // R-type instructions do not have an immediate value
            switch(decoded.funct3){
                case 0x0:
                    if(decoded.funct7 == 0x00){
                        strncpy(decoded.mnemonic, "ADD", MAX_MNEMONIC_LEN - 1);
                    } else if(decoded.funct7 == 0x20){
                        strncpy(decoded.mnemonic, "SUB", MAX_MNEMONIC_LEN - 1);
                    }
                    break;
                case 0x1:
                    strncpy(decoded.mnemonic, "SLL", MAX_MNEMONIC_LEN - 1);
                    break;
                case 0x2:
                    strncpy(decoded.mnemonic, "SLT", MAX_MNEMONIC_LEN - 1);
                    break;
                case 0x3:
                    strncpy(decoded.mnemonic, "SLTU", MAX_MNEMONIC_LEN - 1);
                    break;
                case 0x4:
                    strncpy(decoded.mnemonic, "XOR", MAX_MNEMONIC_LEN - 1);
                    break;
                case 0x5:
                    if(decoded.funct7 == 0x00){
                        strncpy(decoded.mnemonic, "SRL", MAX_MNEMONIC_LEN - 1);
                    } else if(decoded.funct7 == 0x20){
                        strncpy(decoded.mnemonic, "SRA", MAX_MNEMONIC_LEN - 1);
                    }
                    break;
                case 0x6:
                    strncpy(decoded.mnemonic, "OR", MAX_MNEMONIC_LEN - 1);
                    break;
                case 0x7:
                    strncpy(decoded.mnemonic, "AND", MAX_MNEMONIC_LEN - 1);
                    break;
                default:
                    strncpy(decoded.mnemonic, "UNKNOWN", MAX_MNEMONIC_LEN - 1);
                    break;
            }
            break;
        case OP_I_ARI_TYPE:
            decoded.imm = get_imm_i(instruction);
            switch(decoded.funct3){
                case 0x0:
                    strncpy(decoded.mnemonic, "ADDI", MAX_MNEMONIC_LEN - 1);
                    break;
                case 0x1:
                    strncpy(decoded.mnemonic, "SLLI", MAX_MNEMONIC_LEN - 1);
                    break;
                case 0x2:
                    strncpy(decoded.mnemonic, "SLTI", MAX_MNEMONIC_LEN - 1);
                    break;
                case 0x3:
                    strncpy(decoded.mnemonic, "SLTIU", MAX_MNEMONIC_LEN - 1);
                    break;
                case 0x4:
                    strncpy(decoded.mnemonic, "XORI", MAX_MNEMONIC_LEN - 1);
                    break;
                case 0x5:
                    if(decoded.funct7 == 0x00){
                        strncpy(decoded.mnemonic, "SRLI", MAX_MNEMONIC_LEN - 1);
                    } else if(decoded.funct7 == 0x20){
                        strncpy(decoded.mnemonic, "SRAI", MAX_MNEMONIC_LEN - 1);
                    }
                    break;
                case 0x6:
                    strncpy(decoded.mnemonic, "ORI", MAX_MNEMONIC_LEN - 1);
                    break;
                case 0x7:
                    strncpy(decoded.mnemonic, "ANDI", MAX_MNEMONIC_LEN - 1);
                    break;
                default:
                    strncpy(decoded.mnemonic, "UNKNOWN", MAX_MNEMONIC_LEN - 1);
                    break;
            }
            break;
        case OP_I_LOAD_TYPE:
            decoded.imm = get_imm_i(instruction);
            switch(decoded.funct3){
                case 0x0:
                    strncpy(decoded.mnemonic, "LB", MAX_MNEMONIC_LEN - 1);
                    break;
                case 0x1:
                    strncpy(decoded.mnemonic, "LH", MAX_MNEMONIC_LEN - 1);
                    break;
                case 0x2:
                    strncpy(decoded.mnemonic, "LW", MAX_MNEMONIC_LEN - 1);
                    break;
                case 0x4:
                    strncpy(decoded.mnemonic, "LBU", MAX_MNEMONIC_LEN - 1);
                    break;
                case 0x5:
                    strncpy(decoded.mnemonic, "LHU", MAX_MNEMONIC_LEN - 1);
                    break;
                default:
                    strncpy(decoded.mnemonic, "UNKNOWN", MAX_MNEMONIC_LEN - 1);
                    break;
            }
            break;
        case OP_S_TYPE:
            decoded.imm = get_imm_s(instruction);
            switch(decoded.funct3){
                case 0x0:
                    strncpy(decoded.mnemonic, "SB", MAX_MNEMONIC_LEN - 1);
                    break;
                case 0x1:
                    strncpy(decoded.mnemonic, "SH", MAX_MNEMONIC_LEN - 1);
                    break;
                case 0x2:
                    strncpy(decoded.mnemonic, "SW", MAX_MNEMONIC_LEN - 1);
                    break;
                default:
                    strncpy(decoded.mnemonic, "UNKNOWN", MAX_MNEMONIC_LEN - 1);
                    break;
            }
            break;
        case OP_B_TYPE:
            decoded.imm = get_imm_b(instruction);
            switch(decoded.funct3){
                case 0x0:
                    strncpy(decoded.mnemonic, "BEQ", MAX_MNEMONIC_LEN - 1);
                    break;
                case 0x1:
                    strncpy(decoded.mnemonic, "BNE", MAX_MNEMONIC_LEN - 1);
                    break;
                case 0x4:
                    strncpy(decoded.mnemonic, "BLT", MAX_MNEMONIC_LEN - 1);
                    break;
                case 0x5:
                    strncpy(decoded.mnemonic, "BGE", MAX_MNEMONIC_LEN - 1);
                    break;
                case 0x6:
                    strncpy(decoded.mnemonic, "BLTU", MAX_MNEMONIC_LEN - 1);
                    break;
                case 0x7:
                    strncpy(decoded.mnemonic, "BGEU", MAX_MNEMONIC_LEN - 1);
                    break;
                default:
                    strncpy(decoded.mnemonic, "UNKNOWN", MAX_MNEMONIC_LEN - 1);
                    break;
            }
            break;
        case OP_U_LUI_TYPE:
            decoded.imm = get_imm_u(instruction);
            strncpy(decoded.mnemonic, "LUI", MAX_MNEMONIC_LEN - 1);
            break;
        case OP_U_AUIPC_TYPE:
            decoded.imm = get_imm_u(instruction);
            strncpy(decoded.mnemonic, "AUIPC", MAX_MNEMONIC_LEN - 1);
            break;
        case OP_J_JAL_TYPE:
            decoded.imm = get_imm_j(instruction);
            strncpy(decoded.mnemonic, "JAL", MAX_MNEMONIC_LEN - 1);
            break;
        case OP_I_JALR_TYPE:
            decoded.imm = get_imm_i(instruction);
            strncpy(decoded.mnemonic, "JALR", MAX_MNEMONIC_LEN - 1);
            break;
        default:
            strncpy(decoded.mnemonic, "UNKNOWN", MAX_MNEMONIC_LEN - 1);
            break;    
    }

    return decoded;
}