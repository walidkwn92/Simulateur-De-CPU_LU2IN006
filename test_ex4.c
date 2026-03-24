#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_seg.h"
#include "memorymanager.h"
#include "hachage.h" 
#include "parser.h"

int main() {
    Instruction* d1 = malloc(sizeof(Instruction));
    d1->mnemonic = strdup("x");
    d1->operand1 = strdup("DW");
    d1->operand2 = strdup("3");

    Instruction* d2 = malloc(sizeof(Instruction));
    d2->mnemonic = strdup("y");
    d2->operand1 = strdup("DB");
    d2->operand2 = strdup("4");

    Instruction* d3 = malloc(sizeof(Instruction));
    d3->mnemonic = strdup("arr");
    d3->operand1 = strdup("DB");
    d3->operand2 = strdup("5,6,7,8");

    Instruction* d4 = malloc(sizeof(Instruction));
    d4->mnemonic = strdup("z");
    d4->operand1 = strdup("DB");
    d4->operand2 = strdup("9");

    Instruction* instructions[] = { d1, d2, d3, d4 };
    int count = 4;
    CPU* cpu = cpu_init(1024);
    printf("\n=== Début de l'allocation des variables ===\n");
    allocate_variables(cpu, instructions, count);
    printf("\n=== Contenu du segment DS ===\n");
    print_data_segment(cpu);
    for (int i = 0; i < count; i++) {
        free(instructions[i]->mnemonic);
        free(instructions[i]->operand1);
        free(instructions[i]->operand2);
        free(instructions[i]);
    }
    cpu_destroy(cpu);
    return 0;
}
