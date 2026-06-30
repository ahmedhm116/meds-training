#include "common.h"
#include "decoder.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_instruction(uint32_t addr, decoded_instr_t *d);

int main(int argc, char *argv[]) {
    if (argc < 2){
        fprintf(stderr, "Usage: %s <binary_file>\n", argv[0]);
        return 1;
    }

    uint8_t *memory = calloc(MEM_SIZE, sizeof(uint8_t));
    if (!memory) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    int count = load_hex_file(argv[1], memory, MEM_SIZE);
    if (count < 0) {
        fprintf(stderr, "Failed to load hex file\n");
        free(memory);
        return 1;
    }

    printf("RISC-V RV32I Instruction Decoder\n");
    printf("================================\n");
    printf("Loaded %d instructions from %s\n\n", count, argv[1]);
    printf("%-12s %-10s %s\n", "Addr", "Hex", "Assembly");
    printf("---------- ---------- -------------------------\n");

    int unknown = 0;
    for (int i = 0; i < count; i++) {
        uint32_t addr = i * 4;
        uint32_t word = memory[addr] | (memory[addr + 1] << 8) | (memory[addr + 2] << 16) | (memory[addr + 3] << 24);
        decoded_instr_t d = decode_instruction(word);
        if (strcmp(d.mnemonic, "UNKNOWN") == 0) unknown++;
        print_instruction(addr, &d);
    }

    printf("\nDecoded %d instructions (%d valid, %d unknown)\n",
           count, count - unknown, unknown);

    free(memory);
    memory = NULL;
    return 0;
}

static void print_instruction(uint32_t addr, decoded_instr_t *d){
    switch (d->opcode)
    {
    case OP_R_TYPE:
        printf("0x%08X %08X %-8s x%u, x%u, x%u\n", addr, d->instr, d->mnemonic, d->rd, d->rs1, d->rs2);
        break;
    case OP_I_ARI_TYPE:
        printf("0x%08X %08X %-8s x%u, x%u, %d\n", addr, d->instr, d->mnemonic, d->rd, d->rs1, d->imm);
        break;
    case OP_I_LOAD_TYPE:
        printf("0x%08X %08X %-8s x%u, %d(x%u)\n", addr, d->instr, d->mnemonic, d->rd, d->imm, d->rs1);
        break;
    case OP_S_TYPE:
        printf("0x%08X %08X %-8s x%u, %d(x%u)\n", addr, d->instr, d->mnemonic, d->rs2, d->imm, d->rs1);
        break;
    case OP_B_TYPE:
        printf("0x%08X %08X %-8s x%u, x%u, %d\n", addr, d->instr, d->mnemonic, d->rs1, d->rs2, d->imm);
        break;
    case OP_U_LUI_TYPE:
        printf("0x%08X %08X %-8s x%u, %d\n", addr, d->instr, d->mnemonic, d->rd, d->imm);
        break;
    case OP_U_AUIPC_TYPE:
        printf("0x%08X %08X %-8s x%u, %d\n", addr, d->instr, d->mnemonic, d->rd, d->imm);
        break;
    case OP_J_JAL_TYPE:
        printf("0x%08X %08X %-8s x%u, %d\n", addr, d->instr, d->mnemonic, d->rd, d->imm);
        break;
    case OP_I_JALR_TYPE:
        printf("0x%08X %08X %-8s x%u, x%u, %d\n", addr, d->instr, d->mnemonic, d->rd, d->rs1, d->imm);
        break;
    default:
        printf("0x%08X %08X %-8s\n", addr, d->instr, d->mnemonic);
        break;
    }
}
