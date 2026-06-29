#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int load_hex_file(const char *filename, uint8_t *memory, size_t mem_size) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return -1;
    }

    char line[32];
    uint32_t addr = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        if (line[0] == '\n' || line[0] == '\0') continue;

        if (addr + 4 > mem_size) {
            fprintf(stderr, "Memory full at address 0x%08X\n", addr);
            fclose(file);
            return -1;
        }

        uint32_t word = (uint32_t)strtoul(line, NULL, 16);

        memory[addr + 0] = (word >> 0)  & 0xFF;
        memory[addr + 1] = (word >> 8)  & 0xFF;
        memory[addr + 2] = (word >> 16) & 0xFF;
        memory[addr + 3] = (word >> 24) & 0xFF;

        addr += 4;
    }

    fclose(file);
    return (int)(addr / 4); 
}