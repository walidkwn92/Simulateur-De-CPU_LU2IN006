#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "data_seg.h"

CPU *cpu_init(int memory_size) {
    CPU *cpu = (CPU *)malloc(sizeof(CPU));
    if (!cpu) return NULL;

    cpu->memory_handler = memory_init(memory_size);
    cpu->context = hashmap_create();
    cpu->constant_pool = NULL;  /* non utilisé dans cpu_init v1 */

    int *ax = malloc(sizeof(int));
    int *bx = malloc(sizeof(int));
    int *cx = malloc(sizeof(int));
    int *dx = malloc(sizeof(int));

    *ax = 0; *bx = 0; *cx = 0; *dx = 0;

    hashmap_insert(cpu->context, "AX", ax);
    hashmap_insert(cpu->context, "BX", bx);
    hashmap_insert(cpu->context, "CX", cx);
    hashmap_insert(cpu->context, "DX", dx);

    return cpu;
}

void cpu_destroy(CPU *cpu) {
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

    if (cpu->constant_pool) {
        for (int i = 0; i < cpu->constant_pool->size; i++) {
            if (cpu->constant_pool->table[i].key != NULL &&
                cpu->constant_pool->table[i].value != NULL &&
                cpu->constant_pool->table[i].value != TOMBSTONE) {
                free(cpu->constant_pool->table[i].value);
            }
        }
        hashmap_destroy(cpu->constant_pool);
    }

    free(cpu);
}

void *store(MemoryHandler *handler, const char *segment_name, int pos, void *data) {
    if (!handler || !segment_name || !data) return NULL;
    Segment *seg = hashmap_get(handler->allocated, segment_name);
    if (!seg || pos >= seg->size) return NULL;
    int absolute = seg->start + pos;
    handler->memory[absolute] = data;
    return data;
}

void *load(MemoryHandler *handler, const char *segment_name, int pos) {
    if (!handler || !segment_name) return NULL;

    Segment *seg = hashmap_get(handler->allocated, segment_name);
    if (!seg || pos >= seg->size) return NULL;

    int absolute = seg->start + pos;
    return handler->memory[absolute];
}

void allocate_variables(CPU *cpu, Instruction **data_instructions, int data_count) {
    if (!cpu || !cpu->memory_handler || !data_instructions) return;

    int total_size = 0;
    for (int i = 0; i < data_count; i++) {
        char *copy = strdup(data_instructions[i]->operand2);
        if (!copy) continue;

        char *token = strtok(copy, ",");
        while (token) {
            total_size++;
            token = strtok(NULL, ",");
        }
        free(copy);
    }

    if (create_segment(cpu->memory_handler, "DS", 0, total_size) != 0) {
        printf("Erreur lors de la création du segment DS.\n");
        return;
    }

    int pos = 0;
    for (int i = 0; i < data_count; i++) {
        char *copy = strdup(data_instructions[i]->operand2);
        if (!copy) continue;

        char *token = strtok(copy, ",");
        while (token) {
            int *val = malloc(sizeof(int));
            if (!val) break;
            *val = atoi(token);
            store(cpu->memory_handler, "DS", pos++, val);
            token = strtok(NULL, ",");
        }
        free(copy);
    }
}

void print_data_segment(CPU *cpu) {
    if (!cpu || !cpu->memory_handler) return;

    Segment *ds = hashmap_get(cpu->memory_handler->allocated, "DS");
    if (!ds) return;

    for (int i = 0; i < ds->size; i++) {
        int addr = ds->start + i;
        void *data = cpu->memory_handler->memory[addr];
        if (data) {
            printf("Adresse %d : Valeur %d\n", addr, *((int *)data));
        } else {
            printf("Adresse %d : Valeur NULL\n", addr);
        }
    }
}
