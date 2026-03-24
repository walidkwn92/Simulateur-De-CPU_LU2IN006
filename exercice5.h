#ifndef EXERCICE5_H
#define EXERCICE5_H
#include "hachage.h"
#include "memorymanager.h"
#include "parser.h"
#include "exercice8.h"
#include "data_seg.h"
CPU *cpu_init2(int memory_size);
void cpu_destroy2(CPU *cpu);
void* store(MemoryHandler *handler, const char *segment_name,int pos, void *data);
void* load(MemoryHandler *handler, const char *segment_name,int pos);
void allocate_variables(CPU *cpu,Instruction** data_instructions,int data_count);
void print_data_segment(CPU* cpu);
void *immediate_addressing(CPU *cpu, const char *operand);
void *register_addressing(CPU *cpu, const char *operand);
void *memory_direct_addressing(CPU *cpu, const char *operand);
void* register_indirect_addressing(CPU* cpu, const char* operand);
void handle_MOV(CPU* cpu, void* src, void* dest);
CPU* setup_test_environment();
//void* resolve_addressing(CPU *cpu, const char *operand);
int matches ( const char * pattern , const char * string );
#endif