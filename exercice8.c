#include "exercice8.h"
#include "memorymanager.h"
#include "hachage.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

CPU *cpu_init5(int memory_size){
    CPU* cpu = malloc(sizeof(CPU));
    cpu->memory_handler = memory_init(memory_size);
    cpu->context = hashmap_create();
    cpu->constant_pool = hashmap_create();
    create_segment(cpu->memory_handler,"SS",0,128);
    const char* registers[] = {"AX", "BX", "CX", "DX", "IP", "ZF", "SF","SP","BP"};
    Segment* seg = (Segment*)hashmap_get(cpu->memory_handler->allocated, "SS");
    int *sp = malloc(sizeof(int));
    int* bp = malloc(sizeof(int));
    int* es = malloc(sizeof(int));
    *sp= seg->size;
    *bp=seg->size-1;
    *es=-1;
    for (int i = 0; i < 7; i++) {
        int *val = malloc(sizeof(int));
        *val = 0;
        hashmap_insert(cpu->context, registers[i], val);
    }
    hashmap_insert(cpu->context,"SP",sp);
    hashmap_insert(cpu->context,"BP",bp);
    hashmap_insert(cpu->context,"ES",es);

    return cpu;
}

int handle_instruction(CPU *cpu, Instruction *instr, void *dest, void *src) {
    if (!cpu || !instr) return -1;

    int *zf = hashmap_get(cpu->context, "ZF");
    int *sf = hashmap_get(cpu->context, "SF");
    int *ip = hashmap_get(cpu->context, "IP");

    if (strcmp(instr->mnemonic, "MOV") == 0 && src && dest) {
        *((int*) dest) = *((int*)src);
    } 
    else if (strcmp(instr->mnemonic, "ADD") == 0 && src && dest) {
        *((int*) dest) += *((int*)src);
    } 
    else if (strcmp(instr->mnemonic, "CMP") == 0 && src && dest) {
        *zf = (*((int*)dest) == *((int*)src)) ? 1 : 0;
        *sf = (*((int*)dest) < *((int*)src)) ? 1 : 0;
    } 
    else if (strcmp(instr->mnemonic, "JMP") == 0 && src) {
        *ip = *((int*)src);
    } 
    else if (strcmp(instr->mnemonic, "JZ") == 0 && *zf == 1 && src) {
        *ip = *((int*)src);
        return 0;
    } 
    else if (strcmp(instr->mnemonic, "JNZ") == 0 && *zf == 0 && src) {
        *ip = *((int*)src);
        return 0;
    } 
    else if (strcmp(instr->mnemonic, "HALT") == 0) {
        Segment* cs = hashmap_get(cpu->memory_handler->allocated, "CS");
        if (cs) *ip = cs->start + cs->size;
        return 0;
    } 
    else if (strcmp(instr->mnemonic, "PUSH") == 0) {
        int *reg = instr->operand1 ? hashmap_get(cpu->context, instr->operand1) : hashmap_get(cpu->context, "AX");
        if (!reg) return -1;
        return push_value(cpu, *reg);
    } 
    else if (strcmp(instr->mnemonic, "POP") == 0) {
        int *reg = instr->operand1 ? hashmap_get(cpu->context, instr->operand1) : hashmap_get(cpu->context, "AX");
        if (!reg) return -1;
        return pop_value(cpu, reg);
    }
    if(strcmp(instr->mnemonic,"ALLOC")==0){
        return alloc_es_segment(cpu);
    }
    if(strcmp(instr->mnemonic,"FREE")==0){
        return free_es_segment(cpu);
    }
    return 1;
}

void *resolve_addressing(CPU *cpu, const char *operand){
    if (!operand) return NULL;
    void *result = immediate_addressing(cpu, operand);
    if (result != NULL) return result;
    result = register_addressing(cpu, operand);
    if (result != NULL) return result;
    result = memory_direct_addressing(cpu, operand);
    if (result != NULL) return result;
    result = register_indirect_addressing(cpu, operand);
    if (result != NULL) return result;
    result = segment_override_addressing(cpu, operand);
    if (result != NULL) return result;
    return NULL;
}

void* segment_override_addressing(CPU* cpu, const char* operand) {
    if (cpu == NULL || operand == NULL) return NULL;
    const char* pattern = "^\\[([A-Z]{2}):([A-Z]{2})\\]$";
    if (!matches(pattern, operand)) return NULL;
    char segment[3];
    char reg[3];
    sscanf(operand, "[%2[A-Z]:%2[A-Z]]", segment, reg);
    segment[2] = '\0';
    reg[2] = '\0';
    int* reg_val = hashmap_get(cpu->context, reg);
    if (!reg_val) return NULL;
    Segment* seg = hashmap_get(cpu->memory_handler->allocated, segment);
    if (!seg) return NULL;
    if (*reg_val < 0 || *reg_val >= seg->size) return NULL;
    return load(cpu->memory_handler, segment, *reg_val);
}

int find_free_address_strategy(MemoryHandler *handler, int size, int strategy) {
    if (!handler|| size <= 0) return -1;
    int adresse_selectionnee = -1;
    Segment *courant = handler->free_list;
    int meilleure_taille = (strategy == 1) ? __INT_MAX__ : -1;
    while (courant) {
        if (courant->size >= size) {
            if (strategy == 0) return courant->start;
            else if (strategy == 1 && courant->size < meilleure_taille) {
                meilleure_taille = courant->size;
                adresse_selectionnee = courant->start;
            }
            else if (strategy == 2 && courant->size > meilleure_taille) {
                meilleure_taille = courant->size;
                adresse_selectionnee = courant->start;
            }
        }
        courant = courant->next;
    }
    return adresse_selectionnee;
}

int alloc_es_segment(CPU* cpu) {
    if (cpu == NULL) return -1;
    if (hashmap_get(cpu->memory_handler->allocated, "ES")) {
        free_es_segment(cpu);
    }
    int *AX = hashmap_get(cpu->context, "AX");
    int *BX = hashmap_get(cpu->context, "BX");
    int *ZF = hashmap_get(cpu->context, "ZF");
    int *ES = hashmap_get(cpu->context, "ES");
    if (!AX || !BX || !ZF || !ES) return -1;
    int taille = *AX;
    int strategie = *BX;
    int adresse = find_free_address_strategy(cpu->memory_handler, taille, strategie);
    if (adresse == -1) {
        *ZF = 1;
        return -1;
    }
    if (create_segment(cpu->memory_handler, "ES", adresse, taille) == -1) {
        *ZF = 1;
        return -1;
    }
    int zero = 0;
    for (int i = 0; i < taille; i++) {
        int *val = malloc(sizeof(int));
        *val = zero;
        store(cpu->memory_handler, "ES", i, val);
    }
    *ES = adresse;
    *ZF = 0;
    return 0;
}

int free_es_segment(CPU* cpu) {
    if (!cpu) return -1;
    if (!hashmap_get(cpu->memory_handler->allocated, "ES")) return -1;
    remove_segment(cpu->memory_handler, "ES");
    int* es_reg = hashmap_get(cpu->context, "ES");
    if (es_reg) *es_reg = -1;
    return 0;
}
