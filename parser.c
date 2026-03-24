#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "parser.h"

static int mem_adress = 0;

char *trim(char *str) {
    while (isspace(*str)) str++;
    if (*str == '\0') return str;

    char *end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) end--;
    *(end + 1) = '\0';
    return str;
}

Instruction *parse_data_instruction(const char *line, HashMap* memory_locations) {
    if (!line || !memory_locations) return NULL;

    char *linecopy = strdup(line);
    if (!linecopy) return NULL;

    Instruction* instr = malloc(sizeof(Instruction));
    if (!instr) {
        free(linecopy);
        return NULL;
    }

    char *token = strtok(linecopy, " ");
    if (!token) { free(instr); free(linecopy); return NULL; }
    instr->mnemonic = strdup(token);

    token = strtok(NULL, " ");
    instr->operand1 = token ? strdup(token) : strdup("");

    token = strtok(NULL, "\0");
    instr->operand2 = token ? strdup(trim(token)) : strdup("");
    int nb_elem = 0;
    char *opcopy = strdup(instr->operand2);
    char *valtok = strtok(opcopy, ",");
    while (valtok) {
        nb_elem++;
        valtok = strtok(NULL, ",");
    }
    free(opcopy);

    hashmap_insert(memory_locations, instr->mnemonic, (void *)(intptr_t)mem_adress);
    mem_adress += nb_elem;

    free(linecopy);
    return instr;
}

Instruction *parse_code_instruction(const char *line, HashMap* labels, int code_count) {
    if (!line || !labels) return NULL;

    char *linecopy = strdup(line);
    if (!linecopy) return NULL;

    Instruction* instr = malloc(sizeof(Instruction));
    if (!instr) { free(linecopy); return NULL; }

    instr->mnemonic = NULL;
    instr->operand1 = NULL;
    instr->operand2 = NULL;

    char *token = strtok(linecopy, " ");
    if (token && token[strlen(token) - 1] == ':') {
        token[strlen(token) - 1] = '\0';
        hashmap_insert(labels, token, (void *)(intptr_t)code_count);
        token = strtok(NULL, " ");
    }

    if (token) instr->mnemonic = strdup(token);

    token = strtok(NULL, ",");
    if (token) instr->operand1 = strdup(trim(token));

    token = strtok(NULL, ",");
    if (token) instr->operand2 = strdup(trim(token));
    else instr->operand2 = strdup("");

    free(linecopy);
    return instr;
}

ParserResult *parse(const char *filename) {
    FILE* pf = fopen(filename, "r");
    if (!pf) return NULL;

    ParserResult* res = malloc(sizeof(ParserResult));
    if (!res) {
        fclose(pf);
        return NULL;
    }

    res->data_instructions = NULL;
    res->code_instructions = NULL;
    res->data_count = 0;
    res->code_count = 0;
    res->memory_locations = hashmap_create();
    res->labels = hashmap_create();

    char buffer[256];
    int code = 0, data = 0;

    while (fgets(buffer, sizeof(buffer), pf)) {
        char *line = trim(buffer);
        if (line[0] == '\0' || line[0] == ';') continue;

        if (strcmp(line, ".DATA") == 0) {
            data = 1; code = 0;
            continue;
        }
        if (strcmp(line, ".CODE") == 0) {
            code = 1; data = 0;
            continue;
        }

        if (data) {
            Instruction **tmp = realloc(res->data_instructions, (res->data_count + 1) * sizeof(Instruction*));
            if (!tmp) { fclose(pf); free_parser_result(res); return NULL; }
            res->data_instructions = tmp;
            res->data_instructions[res->data_count] = parse_data_instruction(line, res->memory_locations);
            res->data_count++;
        }

        if (code) {
            Instruction **tmp = realloc(res->code_instructions, (res->code_count + 1) * sizeof(Instruction*));
            if (!tmp) { fclose(pf); free_parser_result(res); return NULL; }
            res->code_instructions = tmp;
            res->code_instructions[res->code_count] = parse_code_instruction(line, res->labels, res->code_count);
            res->code_count++;
        }
    }

    fclose(pf);
    return res;
}

void free_parser_result(ParserResult *result) {
    if (!result) return;

    for (int i = 0; i < result->data_count; i++) {
        free(result->data_instructions[i]->mnemonic);
        free(result->data_instructions[i]->operand1);
        free(result->data_instructions[i]->operand2);
        free(result->data_instructions[i]);
    }
    free(result->data_instructions);

    for (int i = 0; i < result->code_count; i++) {
        free(result->code_instructions[i]->mnemonic);
        free(result->code_instructions[i]->operand1);
        free(result->code_instructions[i]->operand2);
        free(result->code_instructions[i]);
    }
    free(result->code_instructions);

    hashmap_destroy(result->labels);
    hashmap_destroy(result->memory_locations);
    free(result);
}
