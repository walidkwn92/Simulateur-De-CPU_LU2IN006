#include "exercice7.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
CPU* cpu_init4(int memory_size) {
    CPU* cpu = malloc(sizeof(CPU));
    cpu->memory_handler = memory_init(memory_size);
    cpu->context = hashmap_create();
    cpu->constant_pool = hashmap_create();
    create_segment(cpu->memory_handler,"SS",0,128);
    const char* registers[] = {"AX", "BX", "CX", "DX", "IP", "ZF", "SF","SP","BP"};
    Segment* seg = (Segment*)hashmap_get(cpu->memory_handler->allocated, "SS");
    int *sp = malloc(sizeof(int));
    int* bp = malloc(sizeof(int));
    *sp= seg->size;
    *bp=seg->size-1;
    for (int i = 0; i < 7; i++) {
        int *val = malloc(sizeof(int));
        *val = 0;
        hashmap_insert(cpu->context, registers[i], val);
    }
    hashmap_insert(cpu->context,"SP",sp);
    hashmap_insert(cpu->context,"BP",bp);

    return cpu;
}
int push_value(CPU *cpu, int value) {
    if (!cpu) return -1;
    Segment *ss =(Segment*) hashmap_get(cpu->memory_handler->allocated, "SS");
    int *sp = (int*)hashmap_get(cpu->context, "SP");
    if (!ss || !sp) return -1;
    if (*sp <= 0) {
        printf("pile pleine.\n");
        return -1;
    }
    (*sp)--;
    int *val = malloc(sizeof(int));
    if (!val) {
        printf("Erreur allocation mémoire.\n");
        return -1;
    }
    *val = value;
    cpu->memory_handler->memory[ss->start + *sp] = val;
    return 0;
}

int pop_value(CPU *cpu, int *dest) {
    if (!cpu || !dest) return -1;
    Segment *ss =(Segment*)hashmap_get(cpu->memory_handler->allocated, "SS");
    int *sp = (int*)hashmap_get(cpu->context, "SP");
    if(!ss || !sp){return -1;}
    if (*sp >= ss->size) {
        printf("pile vide.\n");
        return -1;
    }

    void *val = cpu->memory_handler->memory[ss->start + *sp];
    if (!val) {
        printf("Aucun élément à dépiler.\n");
        return -1;
    }
    *dest = *((int *)val);
    free(val);
    cpu->memory_handler->memory[ss->start + *sp] = NULL;
    (*sp)++;

    return 0;
}
