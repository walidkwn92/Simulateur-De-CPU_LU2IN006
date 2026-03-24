#ifndef EXERCICE6_H
#define EXERCICE6_H
#include "data_seg.h"
#include "hachage.h"
#include "memorymanager.h"
#include "parser.h"
#include "exercice5.h"
#include "exercice7.h"
#include "exercice8.h"
int search_and_replace(char **str, HashMap *values);
int resolve_constants(ParserResult *result);
CPU *cpu_init3(int memory_size);
void allocate_code_segment(CPU *cpu, Instruction **code_instructions,int code_count);
//int handle_instruction(CPU *cpu, Instruction *instr, void *src, void *dest);
int execute_instruction(CPU *cpu, Instruction *instr);
Instruction* fetch_next_instruction(CPU *cpu);
int run_program(CPU *cpu);
#endif