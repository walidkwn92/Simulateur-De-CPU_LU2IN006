#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hachage.h"

unsigned long simple_hash(const char *str){
    unsigned long hash=0;
    int i=0;
    while(str[i]){
        hash=hash*13+str[i];
        i++;
    }
    return (hash%TABLE_SIZE);
}

unsigned long probing_lineaire(unsigned long gk,int i){
    return (gk + i) % TABLE_SIZE;
}

HashMap *hashmap_create() {
    HashMap *hash = (HashMap *)malloc(sizeof(HashMap));
    if (!hash) return NULL;
    hash->size = TABLE_SIZE;
    hash->table = (HashEntry *)calloc(TABLE_SIZE, sizeof(HashEntry));
    if (!hash->table) {
        free(hash);
        return NULL;
    }
    return hash;
}


int hashmap_insert(HashMap *map, const char *key, void *value){ // Insertion par probing
    unsigned long cleHachee = simple_hash(key);
    for(int i=0;i<map->size;i++){
        unsigned long curIndex =probing_lineaire(cleHachee,i);
        if(map->table[curIndex].value == NULL || map->table[curIndex].value == TOMBSTONE){
            map->table[curIndex].key=strdup(key);
            map->table[curIndex].value=value;
            return 0;
        }
    }
    return -1;
}


void *hashmap_get(HashMap *map, const char *key){
    unsigned long cleHachee = simple_hash(key);
    for(int i=0;i<map->size;i++){
        unsigned long curIndex =probing_lineaire(cleHachee,i);
        // Case vide (jamais utilisée) : on arrête le probing
        if(map->table[curIndex].value == NULL && map->table[curIndex].key == NULL){return NULL;}
        if(map->table[curIndex].value == TOMBSTONE){
            continue; // On continue le probing
        }
        if(map->table[curIndex].key && strcmp(map->table[curIndex].key,key)==0){return map->table[curIndex].value;}
    }
    return NULL;
}


int hashmap_remove(HashMap *map, const char *key){
    unsigned long cleHachee = simple_hash(key);
    for(int i=0;i<map->size;i++){
        unsigned long curIndex =probing_lineaire(cleHachee,i);
        // Case vide (jamais utilisée) : la clé n'est pas dans la table
        if(map->table[curIndex].value == NULL && map->table[curIndex].key == NULL){return -1;}
        if(map->table[curIndex].value == TOMBSTONE){
            continue; // On continue le probing
        }
        if(map->table[curIndex].key && strcmp(map->table[curIndex].key,key)==0){
            free(map->table[curIndex].key);
            map->table[curIndex].key = NULL;   // Évite le dangling pointer
            map->table[curIndex].value=TOMBSTONE;
            return 0;
        }
    }
    return -1;
}

void hashmap_destroy(HashMap *map){
    if(!map){return;}
    for(int i=0;i<map->size;i++){
        if (map->table[i].key != NULL && map->table[i].value != TOMBSTONE){
            free(map->table[i].key);
        }
    }
    free(map->table);
    free(map);
}