#include <stdio.h>
#include <stdlib.h>
#include "memorymanager.h"

int main() {
    MemoryHandler *handler = memory_init(1024);
    if (!handler) {
        printf("Erreur d'initialisation de la mémoire.\n");
        return -1;
    }

    if (create_segment(handler, "data", 200, 100) == 0) {
        printf("Segment 'data' alloué de [200, 100]\n");
    } else {
        printf("Erreur d'allocation de 'data'\n");
    }

    if (remove_segment(handler, "data") == 0) {
        printf("Segment 'data' libéré\n");
    } else {
        printf("Erreur lors de la libération de 'data'\n");
    }
    Segment *cur = handler->free_list;
    printf("Liste des segments libres :\n");
    while (cur) {
        printf("  [start = %d, size = %d]\n", cur->start, cur->size);
        cur = cur->next;
    }
    memory_destroy(handler);
    return 0;
}
