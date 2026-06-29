#ifndef COMMOM_H
#define COMMOM_H

#include <stdint.h>
#include <stddef.h>

#define NUM_REGS    32
#define MEM_SIZE    65536  //64KB memory size
#define PC_START    0x00000000
#define OPCODE_MASK 0x7F

#define EXTRACT_BITS(value,start,end) (((value) >> (start)) & ((1 << ((end) - (start) + 1)) - 1))

#endif