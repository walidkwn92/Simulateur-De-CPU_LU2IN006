#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hachage.h"

int main() {
    HashMap *map = hashmap_create();
    int value1 = 42;
    hashmap_insert(map, "cle1", &value1);
    char *value2 = "Bonjour";
    hashmap_insert(map, "cle2", value2);
    double value3 = 3.14;
    hashmap_insert(map, "cle3", &value3);
    char *value4 = strdup("Valeur Dynamique");
    hashmap_insert(map, "cle4", value4);
    int *v1 = (int *)hashmap_get(map, "cle1");
    printf("clé1: %d\n", *v1);
    char *v2 = (char *)hashmap_get(map, "cle2");
    printf("clé2: %s\n", v2);
    double *v3 = (double *)hashmap_get(map, "cle3");
    printf("clé3: %f\n", *v3);
    char *v4 = (char *)hashmap_get(map, "cle4");
    printf("clé4: %s\n", v4);
    int value5 = 99;
    hashmap_insert(map, "cle_collision", &value5);
    int *v5 = (int *)hashmap_get(map, "cle_collision");
    printf("clé_collision: %d\n", *v5);
    if (hashmap_remove(map, "cle2") == 0)
        printf("clé2 supprimée avec succès\n");
    if (hashmap_get(map, "cle2") == NULL)
        printf("cle2 est bien supprimée\n");

    if (hashmap_remove(map, "cle1") == 0)
        printf("clé1 supprimée avec succès\n");
    if (hashmap_get(map, "cle1") == NULL)
        printf("cle1 est bien supprimée\n");

    if (hashmap_remove(map, "cle3") == 0)
        printf("clé3 supprimée avec succès\n");
    if (hashmap_get(map, "cle3") == NULL)
        printf("cle3 est bien supprimée\n");

    if (hashmap_remove(map, "cle4") == 0) {
        printf("clé4 supprimée avec succès\n");
        free(value4);
    }
    if (hashmap_get(map, "cle4") == NULL)
        printf("cle4 est bien supprimée\n");

    if (hashmap_remove(map, "cle_inexistante") == -1)
        printf("clé inexistante n'a pas été supprimée\n");
    hashmap_destroy(map);
    return 0;
}
