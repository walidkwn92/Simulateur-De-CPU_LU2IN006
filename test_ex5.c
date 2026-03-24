#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "exercice5.h"
int main() {
    CPU *cpu = setup_test_environment();
    if (!cpu) return -1;
    const char* src_imm = "10";
    const char* dest_imm = "0";
    void* src_ptr = immediate_addressing(cpu, src_imm);
    void* dest_ptr = immediate_addressing(cpu, dest_imm);
    handle_MOV(cpu, src_ptr, dest_ptr);
    printf("Valeur après MOV immédiat : %d\n", *((int*)dest_ptr));
    const char* src_reg = "AX";
    const char* dest_reg = "BX";
    src_ptr = register_addressing(cpu, src_reg);
    dest_ptr = register_addressing(cpu, dest_reg);
    handle_MOV(cpu, src_ptr, dest_ptr);
    printf("Valeur après MOV registre : %d\n", *((int*)dest_ptr));
    const char* src_mem = "[2]";
    const char* dest_mem = "[5]";
    src_ptr = memory_direct_addressing(cpu, src_mem);
    dest_ptr = memory_direct_addressing(cpu, dest_mem);
    handle_MOV(cpu, src_ptr, dest_ptr);
    printf("Valeur après MOV mémoire directe : %d\n", *((int*)dest_ptr));
    const char* src_indirect = "[AX]";
    const char* dest_indirect = "[BX]";
    src_ptr = register_indirect_addressing(cpu, src_indirect);
    dest_ptr = register_indirect_addressing(cpu, dest_indirect);
    handle_MOV(cpu, src_ptr, dest_ptr);
    printf("Valeur après MOV indirect : %d\n", *((int*)dest_ptr));
    cpu_destroy2(cpu);
    return 0;
}
