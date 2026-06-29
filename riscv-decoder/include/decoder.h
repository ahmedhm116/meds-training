#ifndef DECODER_H
#define DECODER_H

#include "common.h"

typedef enum{
    OP_R_TYPE = 0x33,
    OP_I_ARI_TYPE = 0x13,
    OP_I_LOAD_TYPE = 0x03,
    OP_S_TYPE = 0x23,
    OP_B_TYPE = 0x63,
    OP_U_LUI_TYPE = 0x37,
    OP_U_AUIPC_TYPE = 0x17,
    OP_J_JAL_TYPE = 0x6F,
    OP_I_JALR_TYPE = 0x67,
}opcode_t;

typedef struct{
    uint32_t instr;
    uint32_t opcode;
    uint32_t rd;
    uint32_t funct3;
    uint32_t rs1;
    uint32_t rs2;
    uint32_t funct7;
    int32_t imm;
    char mnemonic[MAX_MNEMONIC_LEN];
}decoded_instr_t;

decoded_instr_t decode_instruction(uint32_t instruction);

#endif