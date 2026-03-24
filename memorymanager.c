#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memorymanager.h"
#include "hachage.h"
MemoryHandler *memory_init(int size) {
    MemoryHandler* handler = (MemoryHandler*)malloc(sizeof(MemoryHandler));
    if (!handler) return NULL;
    
    handler->total_size = size;
    handler->memory = (void**)calloc(size,sizeof(void*));
    handler->allocated = hashmap_create();
    handler->free_list = (Segment *)malloc(sizeof(Segment));
    
    if (!handler->free_list) {
        free(handler);
        return NULL;
    }

    handler->free_list->start = 0;
    handler->free_list->size = size;
    handler->free_list->next = NULL;
    
    return handler;
}
Segment* find_free_segment(MemoryHandler *handler, int start, int size, Segment **prev) {
    Segment *current = handler->free_list;
    *prev = NULL;

    while (current != NULL) {
        if (current->start <= start && (current->start + current->size) >= (start + size)) {
            return current;
        }
        *prev = current;
        current = current->next;
    }
    return NULL;
}
int create_segment(MemoryHandler *handler, const char *name, int start, int size) {
    Segment *prev = NULL;
    Segment *free_seg = find_free_segment(handler, start, size, &prev);
    
    if (!free_seg) {
        return -1;
    }

    Segment *new_seg = (Segment *)malloc(sizeof(Segment));
    if (!new_seg) {
        return -1;
    }

    new_seg->start = start;
    new_seg->size = size;
    new_seg->next = NULL;

    hashmap_insert(handler->allocated, name, new_seg);
    if (free_seg->start == start && free_seg->size == size) {
        if (prev == NULL) {
            handler->free_list = free_seg->next;
        } else {
            prev->next = free_seg->next;
        }
        free(free_seg);
    } else if (free_seg->start == start && free_seg->size >= size) {
        free_seg->start += size;
        free_seg->size -= size;
    } else if (free_seg->start + free_seg->size == start + size) {
        free_seg->size -= size;
    } else {
        Segment *new_free_seg = (Segment *)malloc(sizeof(Segment));
        new_free_seg->start = start + size;
        new_free_seg->size = free_seg->start + free_seg->size - start - size;
        new_free_seg->next = free_seg->next;
        free_seg->size = start - free_seg->start;
        free_seg->next = new_free_seg;
    }

    return 0;
}

int remove_segment(MemoryHandler *handler, const char *name) {
    Segment* seg = (Segment*)hashmap_get(handler->allocated, name);
    if (!seg) {
        return -1;
    }

    hashmap_remove(handler->allocated, name);

    Segment* cur = handler->free_list;
    Segment* prec = NULL;

    while (cur && cur->start < seg->start) {
        prec = cur;
        cur = cur->next;
    }

    seg->next = cur;
    if (prec) {
        prec->next = seg;
    } else {
        handler->free_list = seg;
    }
    if (seg->next && (seg->start + seg->size) == seg->next->start) {
        seg->size += seg->next->size;
        Segment *to_free = seg->next;
        seg->next = seg->next->next;
        free(to_free);
    }

    if (prec && (prec->start + prec->size) == seg->start) {
        prec->size += seg->size;
        prec->next = seg->next;
        free(seg);
    }

    return 0;
}
void memory_destroy(MemoryHandler *handler) {
    if (!handler) return;
    if (handler->memory) {
        free(handler->memory);
    }
    if (handler->allocated) {
        hashmap_destroy(handler->allocated);
    }
    Segment *cur = handler->free_list;
    while (cur) {
        Segment *next = cur->next;
        free(cur);
        cur = next;
    }
    free(handler);
}
