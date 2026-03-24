#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "exercice7.h"

int main() {
    CPU *cpu = cpu_init4(1024);
    Instruction *instrs[6];
    instrs[0] = malloc(sizeof(Instruction));
    instrs[0]->mnemonic = strdup("MOV");
    instrs[0]->operand1 = strdup("AX");
    instrs[0]->operand2 = strdup("10");
    instrs[1] = malloc(sizeof(Instruction));
    instrs[1]->mnemonic = strdup("MOV");
    instrs[1]->operand1 = strdup("BX");
    instrs[1]->operand2 = strdup("20");
    instrs[2] = malloc(sizeof(Instruction));
    instrs[2]->mnemonic = strdup("PUSH");
    instrs[2]->operand1 = strdup("AX");
    instrs[2]->operand2 = NULL;
    instrs[3] = malloc(sizeof(Instruction));
    instrs[3]->mnemonic = strdup("PUSH");
    instrs[3]->operand1 = strdup("BX");
    instrs[3]->operand2 = NULL;
    instrs[4] = malloc(sizeof(Instruction));
    instrs[4]->mnemonic = strdup("POP");
    instrs[4]->operand1 = strdup("CX");
    instrs[4]->operand2 = NULL;
    instrs[5] = malloc(sizeof(Instruction));
    instrs[5]->mnemonic = strdup("POP");
    instrs[5]->operand1 = strdup("DX");
    instrs[5]->operand2 = NULL;
    for (int i = 0; i < 6; i++) {
        Instruction *instr = instrs[i];
        printf(">> %s %s %s\n", instr->mnemonic,
               instr->operand1 ? instr->operand1 : "",
               instr->operand2 ? instr->operand2 : "");
        execute_instruction(cpu, instr);
    }
    printf("\n=== Registres finaux ===\n");
    char *regs[] = {"AX", "BX", "CX", "DX", "SP"};
    for (int i = 0; i < 5; i++) {
        int *val = hashmap_get(cpu->context, regs[i]);
        if (val) printf("%s = %d\n", regs[i], *val);
    }

    /* Libération des instructions */
    for (int i = 0; i < 6; i++) {
        free(instrs[i]->mnemonic);
        free(instrs[i]->operand1);
        free(instrs[i]->operand2);
        free(instrs[i]);
    }
    cpu_destroy(cpu);
    return 0;
}
