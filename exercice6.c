#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "exercice6.h"

int search_and_replace(char **str, HashMap *values) {
    if (!str || !*str || !values) return 0;

    int replaced = 0;
    char *input = *str;

    for (int i = 0; i < values->size; i++) {
        if (values->table[i].key && values->table[i].key != (void *) -1) {
            char *key = values->table[i].key;
            int value = (int)(long)values->table[i].value;

            char *substr = strstr(input, key);
            if (substr) {
                char replacement[64];
                snprintf(replacement, sizeof(replacement), "%d", value);

                int key_len = strlen(key);
                int repl_len = strlen(replacement);

                char *new_str = malloc(strlen(input) - key_len + repl_len + 1);
                strncpy(new_str, input, substr - input);
                new_str[substr - input] = '\0';
                strcat(new_str, replacement);
                strcat(new_str, substr + key_len);

                free(input);
                *str = new_str;
                input = new_str;

                replaced = 1;
            }
        }
    }

    if (replaced) {
        char *trimmed = trim(input);
        if (trimmed != input) memmove(input, trimmed, strlen(trimmed) + 1);
    }

    return replaced;
}

int resolve_constants(ParserResult *result) {
    if (!result || !result->code_instructions) return 0;

    int modified = 0;

    for (int i = 0; i < result->code_count; i++) {
        Instruction *instr = result->code_instructions[i];

        if (instr->operand1) {
            if (search_and_replace(&instr->operand1, result->memory_locations)) modified = 1;
            if (search_and_replace(&instr->operand1, result->labels)) modified = 1;
        }

        if (instr->operand2) {
            if (search_and_replace(&instr->operand2, result->memory_locations)) modified = 1;
            if (search_and_replace(&instr->operand2, result->labels)) modified = 1;
        }
    }

    return modified;
}

CPU* cpu_init3(int memory_size) {
    CPU* cpu = malloc(sizeof(CPU));
    cpu->memory_handler = memory_init(memory_size);
    cpu->context = hashmap_create();
    cpu->constant_pool = hashmap_create();

    const char* registers[] = {"AX", "BX", "CX", "DX", "IP", "ZF", "SF"};
    for (int i = 0; i < 7; i++) {
        int *val = malloc(sizeof(int));
        *val = 0;
        hashmap_insert(cpu->context, registers[i], val);
    }

    return cpu;
}

void allocate_code_segment(CPU *cpu, Instruction **code_instructions, int code_count) {
    if (!cpu || !code_instructions || !cpu->memory_handler || code_count <= 0) return;

    create_segment(cpu->memory_handler, "CS", 0, code_count);
    for (int i = 0; i < code_count; i++) {
        store(cpu->memory_handler, "CS", i, code_instructions[i]);
    }

    int *ip = hashmap_get(cpu->context, "IP");
    if (ip) *ip = 0;
}

int execute_instruction(CPU *cpu, Instruction *instr) {
    if (!cpu || !instr) return -1;
    void *src = NULL, *dest = NULL;
    if (strcmp(instr->mnemonic, "MOV") == 0 || strcmp(instr->mnemonic, "ADD") == 0 || strcmp(instr->mnemonic, "CMP") == 0 || strcmp(instr->mnemonic, "PUSH") == 0 || strcmp(instr->mnemonic, "POP") == 0) {
        dest = resolve_addressing(cpu, instr->operand1);
        src = resolve_addressing(cpu, instr->operand2);
        return handle_instruction(cpu, instr, dest, src);
    }
    if (strcmp(instr->mnemonic, "JMP") == 0 || strcmp(instr->mnemonic, "JZ") == 0 || strcmp(instr->mnemonic, "JNZ") == 0) {
        src = resolve_addressing(cpu, instr->operand1);
        if (!src) return -1;
        return handle_instruction(cpu, instr, NULL, src);
    }
    if (strcmp(instr->mnemonic, "HALT") == 0 || strcmp(instr->mnemonic, "ALLOC") == 0 || strcmp(instr->mnemonic, "FREE") == 0) {
        return handle_instruction(cpu, instr, NULL, NULL);
    }

    return 0;
}

Instruction* fetch_next_instruction(CPU *cpu) {
    if (!cpu || !cpu->memory_handler) return NULL;
    Segment* cs = hashmap_get(cpu->memory_handler->allocated, "CS");
    int *ip = hashmap_get(cpu->context, "IP");
    if (!cs || !ip || *ip >= cs->size) return NULL;

    Instruction *instr = (Instruction*)load(cpu->memory_handler,"CS",*ip);
    (*ip)++;
    return instr;
}

int run_program(CPU *cpu) {
    if (!cpu) return -1;
    print_data_segment(cpu);
    char input;
    while (1) {
        printf("\nAppuyez sur 'Entrée' pour exécuter l'instruction suivante, ou 'q' pour quitter : ");
        input = getchar();
        getchar();

        if (input == 'q') {
            printf("Exécution terminée par l'utilisateur.\n");
            break;
        }

        Instruction* instr = fetch_next_instruction(cpu);
        if (!instr) {
            printf("Fin du programme ou erreur lors du fetch.\n");
            break;
        }

        printf("Instruction exécutée : %s %s %s\n",
               instr->mnemonic,
               instr->operand1 ? instr->operand1 : "",
               instr->operand2 ? instr->operand2 : "");

        int result = execute_instruction(cpu, instr);
        if (result == 0) {
            printf("HALT ou saut terminé, fin de l'exécution.\n");
            break;
        } else if (result < 0) {
            printf("Erreur lors de l'exécution de l'instruction.\n");
            break;
        }
        print_data_segment(cpu);
    }
    printf("\nÉtat final du CPU:\n");
    print_data_segment(cpu);
    return 0;
}
