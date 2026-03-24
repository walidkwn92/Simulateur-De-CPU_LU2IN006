# LU2IN006 — Simulateur de CPU en C

> **Auteur :** Kaouane Walid — `21314924`  
> **Formation :** Licence Informatique L2 — Sorbonne Université  
> **Module :** LU2IN006 Structures de données

---

## Présentation

Ce projet implémente un simulateur de CPU simplifié en C. Il repose sur un ensemble de structures de données (table de hachage, gestionnaire de mémoire segmentée, pile) et supporte un jeu d'instructions pseudo-assembleur avec plusieurs modes d'adressage.

---

## Structure du projet

```
.
├── hachage.h / hachage.c          # Table de hachage (open addressing + sondage linéaire)
├── memorymanager.h / memorymanager.c  # Gestionnaire de mémoire segmentée
├── parser.h / parser.c            # Parseur de fichiers pseudo-assembleur
├── data_seg.h / data_seg.c        # CPU v1 + segment de données DS
├── exercice5.h / exercice5.c      # CPU v2 + modes d'adressage
├── exercice6.h / exercice6.c      # CPU v3 + exécution d'instructions
├── exercice7.h / exercice7.c      # CPU v4 + gestion de la pile (PUSH/POP)
├── exercice8.h / exercice8.c      # CPU v5 + segment ES + stratégies d'allocation
├── test_ex1.c                     # Tests : table de hachage
├── test_ex2.c                     # Tests : gestionnaire de mémoire
├── test_ex4.c                     # Tests : segment de données
├── test_ex5.c                     # Tests : modes d'adressage
└── test_ex7.c                     # Tests : pile PUSH/POP
```

---

## Compilation

### Compiler tous les tests d'un coup

```bash
gcc -Wall -g hachage.c memorymanager.c parser.c data_seg.c \
    exercice5.c exercice6.c exercice7.c exercice8.c \
    test_ex1.c -o test_ex1

gcc -Wall -g hachage.c memorymanager.c parser.c data_seg.c \
    exercice5.c exercice6.c exercice7.c exercice8.c \
    test_ex2.c -o test_ex2

gcc -Wall -g hachage.c memorymanager.c parser.c data_seg.c \
    exercice5.c exercice6.c exercice7.c exercice8.c \
    test_ex4.c -o test_ex4

gcc -Wall -g hachage.c memorymanager.c parser.c data_seg.c \
    exercice5.c exercice6.c exercice7.c exercice8.c \
    test_ex5.c -o test_ex5

gcc -Wall -g hachage.c memorymanager.c parser.c data_seg.c \
    exercice5.c exercice6.c exercice7.c exercice8.c \
    test_ex7.c -o test_ex7
```
## Exécution

```bash
./test_ex1   # Table de hachage
./test_ex2   # Gestionnaire de mémoire
./test_ex4   # Segment de données DS
./test_ex5   # Modes d'adressage (immédiat, registre, direct, indirect)
./test_ex7   # Pile : MOV → PUSH → POP
```

---

## Détail des modules

### `hachage` — Table de hachage

Table de hachage à adressage ouvert avec **sondage linéaire** et marqueurs **TOMBSTONE** pour les suppressions.

| Fonction | Description |
|---|---|
| `hashmap_create()` | Crée une table vide de taille `TABLE_SIZE` (128) |
| `hashmap_insert(map, key, value)` | Insère un couple clé/valeur |
| `hashmap_get(map, key)` | Recherche une valeur par clé |
| `hashmap_remove(map, key)` | Supprime une entrée (pose un TOMBSTONE) |
| `hashmap_destroy(map)` | Libère toute la mémoire |

---

### `memorymanager` — Gestionnaire de mémoire

Gère un tableau de mémoire physique découpé en **segments nommés**. Les segments libres sont maintenus dans une liste chaînée triée, avec **fusion automatique** pour éviter la fragmentation.

| Fonction | Description |
|---|---|
| `memory_init(size)` | Initialise un handler avec `size` cases |
| `create_segment(handler, name, start, size)` | Alloue un segment nommé |
| `remove_segment(handler, name)` | Libère un segment et fusionne les voisins |
| `find_free_segment(handler, start, size, prev)` | Cherche un bloc libre adapté |

---

### `parser` — Parseur pseudo-assembleur

Lit un fichier source avec deux sections :

```asm
.DATA
x  DW 3
arr DB 5,6,7,8

.CODE
MOV AX, 10
PUSH AX
```

| Fonction | Description |
|---|---|
| `parse(filename)` | Analyse le fichier complet, retourne un `ParserResult` |
| `parse_data_instruction(line, map)` | Parse une ligne `.DATA` |
| `parse_code_instruction(line, labels, n)` | Parse une ligne `.CODE` |
| `free_parser_result(result)` | Libère le résultat du parsing |

---

### `data_seg` — CPU v1 + segment DS

Premier niveau du CPU : registres `AX`, `BX`, `CX`, `DX` et segment de données `DS`.

| Fonction | Description |
|---|---|
| `cpu_init(size)` | Crée le CPU avec 4 registres à 0 |
| `cpu_destroy(cpu)` | Libère toutes les ressources |
| `store(handler, segment, pos, data)` | Écrit dans un segment mémoire |
| `load(handler, segment, pos)` | Lit depuis un segment mémoire |
| `allocate_variables(cpu, instrs, n)` | Crée le segment DS à partir des instructions `.DATA` |
| `print_data_segment(cpu)` | Affiche le contenu du segment DS |

---

### `exercice5` — Modes d'adressage

Détection des opérandes via expressions régulières POSIX.

| Mode | Format | Exemple |
|---|---|---|
| Immédiat | entier | `42`, `-3` |
| Registre | `REG` | `AX`, `BX` |
| Mémoire directe | `[N]` | `[5]` |
| Indirect registre | `[REG]` | `[AX]` |

| Fonction | Description |
|---|---|
| `immediate_addressing(cpu, op)` | Résout une constante, la met en cache dans `constant_pool` |
| `register_addressing(cpu, op)` | Retourne le pointeur vers le registre |
| `memory_direct_addressing(cpu, op)` | Charge depuis `DS[N]` |
| `register_indirect_addressing(cpu, op)` | Utilise la valeur du registre comme adresse |
| `handle_MOV(cpu, src, dest)` | Copie `*src` dans `*dest` |

---

### `exercice6` — Exécution d'instructions

Cycle fetch / decode / execute complet.

| Fonction | Description |
|---|---|
| `cpu_init3(size)` | CPU avec registres `IP`, `ZF`, `SF` en plus |
| `allocate_code_segment(cpu, instrs, n)` | Crée le segment `CS` et y stocke les instructions |
| `fetch_next_instruction(cpu)` | Récupère l'instruction à `CS[IP]` et incrémente `IP` |
| `execute_instruction(cpu, instr)` | Exécute une instruction (résolution d'opérandes + dispatch) |
| `run_program(cpu)` | Boucle interactive pas-à-pas |
| `resolve_constants(result)` | Remplace les noms de variables/labels par leurs adresses |

---

### `exercice7` — Pile (PUSH / POP)

Segment de pile `SS` de 128 cases, croissant vers le bas.

| Fonction | Description |
|---|---|
| `cpu_init4(size)` | CPU avec segment `SS`, registres `SP` et `BP` |
| `push_value(cpu, value)` | Empile une valeur (`SP--`) |
| `pop_value(cpu, dest)` | Dépile dans `*dest` (`SP++`) |

---

### `exercice8` — Segment ES + stratégies d'allocation

Allocation dynamique d'un segment extra `ES` selon trois stratégies.

| Stratégie | Code | Description |
|---|---|---|
| First Fit | `0` | Premier bloc suffisamment grand |
| Best Fit  | `1` | Plus petit bloc qui convient |
| Worst Fit | `2` | Plus grand bloc disponible |

| Fonction | Description |
|---|---|
| `cpu_init5(size)` | CPU complet avec registre `ES` initialisé à `-1` |
| `find_free_address_strategy(handler, size, strat)` | Cherche un bloc selon la stratégie |
| `alloc_es_segment(cpu)` | Alloue `ES` (taille dans `AX`, stratégie dans `BX`) |
| `free_es_segment(cpu)` | Libère `ES`, remet le registre à `-1` |
| `segment_override_addressing(cpu, op)` | Résout `[SEG:REG]` (ex. `[DS:BX]`) |
| `handle_instruction(cpu, instr, dest, src)` | Dispatcher complet de toutes les instructions |

---
--KW--

- **GCC** ≥ 9
- **libc** standard (pas de dépendances externes)
- `regex.h` (POSIX, inclus dans libc sur Linux/macOS)
