#ifndef EXERCICE7_H
#define EXERCICE7_H
#include "data_seg.h"
#include "hachage.h"
#include "memorymanager.h"
#include "parser.h"
#include "exercice5.h"
#include "exercice6.h"
#include "exercice8.h"
CPU *cpu_init4(int memory_size);
void cpu_destroy(CPU *cpu);
int push_value(CPU *cpu, int value);
int pop_value(CPU *cpu, int *dest);
#endif
