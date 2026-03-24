#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <regex.h>
#include "exercice5.h"

CPU *cpu_init2(int memory_size){
    CPU* cpu = (CPU*)malloc(sizeof(CPU));
    cpu->memory_handler=memory_init(memory_size);
    cpu->context=hashmap_create();
    cpu->constant_pool=hashmap_create();
    int* ax=(int*)malloc(sizeof(int));
    int* bx=(int*)malloc(sizeof(int));
    int* cx=(int*)malloc(sizeof(int));
    int* dx=(int*)malloc(sizeof(int));
    *ax=0;*bx=0;*cx=0;*dx=0;
    hashmap_insert(cpu->context,"AX",ax);
    hashmap_insert(cpu->context,"BX",bx);
    hashmap_insert(cpu->context,"CX",cx);
    hashmap_insert(cpu->context,"DX",dx);
    return cpu;
}

void cpu_destroy2(CPU *cpu) {
    if (!cpu) return;

    for (int i = 0; i < cpu->memory_handler->total_size; i++) {
        if (cpu->memory_handler->memory[i] != NULL) {
            free(cpu->memory_handler->memory[i]);
        }
    }

    Segment *seg = cpu->memory_handler->free_list;
    while (seg) {
        Segment *tmp = seg->next;
        free(seg);
        seg = tmp;
    }

    for (int i = 0; i < cpu->memory_handler->allocated->size; i++) {
        if (cpu->memory_handler->allocated->table[i].key != NULL &&
            cpu->memory_handler->allocated->table[i].value != NULL &&
            cpu->memory_handler->allocated->table[i].value != TOMBSTONE) {
            free(cpu->memory_handler->allocated->table[i].value);
        }
    }

    hashmap_destroy(cpu->memory_handler->allocated);
    free(cpu->memory_handler->memory);
    free(cpu->memory_handler);

    for (int i = 0; i < cpu->context->size; i++) {
        if (cpu->context->table[i].key != NULL &&
            cpu->context->table[i].value != NULL &&
            cpu->context->table[i].value != TOMBSTONE) {
            free(cpu->context->table[i].value);
        }
    }
    hashmap_destroy(cpu->context);

    for (int i = 0; i < cpu->constant_pool->size; i++) {
        if (cpu->constant_pool->table[i].key != NULL &&
            cpu->constant_pool->table[i].value != TOMBSTONE) {
            free(cpu->constant_pool->table[i].value);
        }
    }
    hashmap_destroy(cpu->constant_pool);

    free(cpu);
}

void *immediate_addressing(CPU *cpu, const char *operand){
    if(!cpu || !operand){return NULL;}
    const char* pattern = "^-?[0-9]+$";
    if(!matches(pattern,operand)){
        return NULL;
    }
    void* deja_dans_constant_pool = hashmap_get(cpu->constant_pool,operand);
    if(deja_dans_constant_pool){
        return deja_dans_constant_pool;
    }
    int* value = malloc(sizeof(int));
    *value=atoi(operand);
    hashmap_insert(cpu->constant_pool,operand,value);
    return value;
}

void *register_addressing(CPU *cpu, const char *operand){
    if(!cpu ||!operand){return NULL;}
    const char* pattern = "^(AX|BX|CX|DX)$";
    if(!matches(pattern,operand)){
        return NULL;
    }
    void* deja_la=hashmap_get(cpu->context,operand);
    return deja_la;
}

void *memory_direct_addressing(CPU *cpu, const char *operand){
    if(!cpu ||!operand){return NULL;}
    const char* pattern = "^\\[[0-9]+\\]$";
    if(!matches(pattern,operand)){
        return NULL;
    }
    int adresse;
    sscanf(operand,"[%d]",&adresse);
    void* valeur = load(cpu->memory_handler,"DS",adresse);
    return valeur;
}

void* register_indirect_addressing(CPU* cpu, const char* operand) {
    const char* pattern = "^\\[([A-Z]{2})\\]$";
    if(!matches(pattern,operand)){
        return NULL;
    }
    char reg[3];
    strncpy(reg,operand+1,2);
    reg[2] ='\0';
    int* reg_ptr=(int*)hashmap_get(cpu->context,reg);
    int adresse = *reg_ptr;
    if(!reg_ptr){return NULL;}
    Segment* ds = hashmap_get(cpu->memory_handler->allocated, "DS");
    if (!ds || adresse < ds->start || adresse >= ds->start + ds->size) {
        return NULL;
    }

    return cpu->memory_handler->memory[adresse];
}

void handle_MOV(CPU* cpu, void* src, void* dest) {
    if (!src || !dest) {
        return;
    }
    *((int*)dest) = *((int*)src);
}

int matches(const char *pattern, const char *string) {
    regex_t regex;
    int result = regcomp(&regex, pattern, REG_EXTENDED);
    if (result) {
        fprintf(stderr, "Regex compilation failed for pattern: %s\n", pattern);
        return 0;
    }
    result = regexec(&regex, string, 0, NULL, 0);
    regfree(&regex);
    return result == 0;
}

CPU *setup_test_environment() {
    CPU *cpu = cpu_init2(1024);
    if (!cpu) {
        printf("Error: CPU initialization failed\n");
        return NULL;
    }

    int *ax = (int *) hashmap_get(cpu->context, "AX");
    int *bx = (int *) hashmap_get(cpu->context, "BX");
    int *cx = (int *) hashmap_get(cpu->context, "CX");
    int *dx = (int *) hashmap_get(cpu->context, "DX");

    *ax = 3;
    *bx = 6;
    *cx = 100;
    *dx = 0;

    if (!hashmap_get(cpu->memory_handler->allocated, "DS")) {
        create_segment(cpu->memory_handler, "DS", 0, 20);

        for (int i = 0; i < 10; i++) {
            int *value = (int *) malloc(sizeof(int));
            *value = i * 10 + 5;
            store(cpu->memory_handler, "DS", i, value);
        }
    }

    printf("Test environment initialized.\n");
    return cpu;
}
