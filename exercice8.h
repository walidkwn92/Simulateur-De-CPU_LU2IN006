#ifndef EXERCICE8_H
#define EXERCICE8_H
#include "data_seg.h"
#include "hachage.h"
#include "memorymanager.h"
#include "parser.h"
#include "exercice7.h"
#include "exercice5.h"
CPU *cpu_init5(int memory_size);
void *resolve_addressing(CPU *cpu, const char *operand);
int handle_instruction(CPU *cpu, Instruction *instr, void *dest, void *src);
int find_free_address_strategy(MemoryHandler *handler, int size, int strategy);
void* segment_override_addressing(CPU* cpu, const char* operand);
int alloc_es_segment(CPU* cpu);
int free_es_segment(CPU* cpu);
#endif
