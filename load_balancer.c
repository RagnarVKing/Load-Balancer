/* Copyright 2023 <> */
#include <stdlib.h>
#include <string.h>

#include "load_balancer.h"
#include "utils.h"

unsigned int hash_function_servers(void *a) {
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int hash_function_key(void *a) {
    unsigned char *puchar_a = (unsigned char *)a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

load_balancer *init_load_balancer() {
    load_balancer *main = malloc(sizeof(load_balancer));
    DIE(main == NULL, "main malloc");

    main->size = 0;

    main->vector = malloc(sizeof(server_info*));
    DIE(main->vector == NULL, "main->vector malloc");

    return main;
}

void add_server_empty(server_info **vector, int server_id) {
    unsigned int hash;
    hash = hash_function_servers(&server_id);
    vector[0]->id = server_id;
    vector[0]->hash = hash;

    server_info *aux;
    server_id += 100000;
    hash = hash_function_servers(&server_id);
    if (hash < vector[0]->hash) {
        aux = vector[1];
        vector[1] = vector[0];
        vector[0] = aux;
        vector[0]->hash = hash;
        vector[0]->id = server_id;
    } else {
        vector[1]->hash = hash;
        vector[1]->id = server_id;
    }
    server_id += 100000;
    hash = hash_function_servers(&server_id);
    if (hash < vector[0]->hash) {
        aux = vector[2];
        vector[2] = vector[1];
        vector[1] = vector[0];
        vector[0] = aux;
        vector[0]->hash = hash;
        vector[0]->id = server_id;
    } else if (hash >= vector[1]->hash) {
        vector[2]->hash = hash;
        vector[2]->id = server_id;
    } else {
        aux = vector[2];
        vector[2] = vector[1];
        vector[1] = aux;
        vector[1]->hash = hash;
        vector[1]->id = server_id;
    }
}

void add_server_first(server_info **vector,
    unsigned int hash, int server_id, int server_number, int size) {
    server_info *aux;
    aux = vector[size - server_number];
    // mut toate elementele la dreapta
    for (unsigned int j = size - server_number; j > 0; j--)
        vector[j] = vector[j - 1];

    vector[0] = aux;
    vector[0]->hash = hash;
    vector[0]->id = server_id;

    ll_node_t *current;
    unsigned int key_hash;
    for (unsigned int j = 0; j < vector[1]->server->hmax; j++) {
        current = vector[1]->server->buckets[j]->head;
        for (unsigned int k = 0; k < vector[1]->server->buckets[j]->size; k++) {
            key_hash = hash_function_key(((info*)current->data)->key);
            ll_node_t *aux = current->next;
            if (key_hash < hash || key_hash > vector[1]->hash) {
                server_store(vector[0]->server,
                ((info*)current->data)->key, ((info*)current->data)->value);
                server_remove(vector[1]->server, ((info*)current->data)->key);
            }
            current = aux;
        }
    }
}

void add_server_last(server_info **vector, unsigned int hash,
    int server_id, int server_number, int size) {
    vector[size - server_number]->hash = hash;
    vector[size - server_number]->id = server_id;

    ll_node_t *current;
    unsigned long key_hash;
    for (unsigned int j = 0; j < vector[0]->server->hmax; j++) {
        current = vector[0]->server->buckets[j]->head;
        for (unsigned int k = 0; k < vector[0]->server->buckets[j]->size; k++) {
            key_hash = hash_function_key(((info*)current->data)->key);
            ll_node_t *aux = current->next;
            if (key_hash < hash && key_hash > vector[0]->hash) {
                server_store(vector[size - server_number]->server,
                    ((info*)current->data)->key, ((info*)current->data)->value);
                server_remove(vector[0]->server, ((info*)current->data)->key);
            }
            current = aux;
        }
    }
}

void add_server_middle(server_info **vector, unsigned int hash,
    int server_id, int server_number, int size) {
    server_info *aux;
    unsigned int poz;
    // pe ce pozitie vreau sa adaug
    for (int j = 0; j < size - server_number - 1; j++) {
        if (hash > vector[j]->hash && hash < vector[j + 1]->hash) {
            poz = j + 1;
            break;
        }
    }
    aux = vector[size - server_number];
    // mut tot, dupa pozitia pe care vreau sa pun, la dreapta
    for (unsigned int j = size - server_number; j > poz - 1; j--) {
        vector[j] = vector[j - 1];
    }
    vector[poz] = aux;
    vector[poz]->hash = hash;
    vector[poz]->id = server_id;
    ll_node_t *current;
    unsigned long key_hash;
    for (unsigned int j = 0; j < vector[poz + 1]->server->hmax; j++) {
        current = vector[poz + 1]->server->buckets[j]->head;
        for (unsigned int k = 0;
            k < vector[poz + 1]->server->buckets[j]->size; k++) {
            key_hash = hash_function_key(((info *)current->data)->key);
            ll_node_t *aux = current->next;
            if (key_hash < hash) {
                server_store(vector[poz]->server,
                ((info *)current->data)->key, ((info *)current->data)->value);
                server_remove(vector[poz + 1]->server,
                ((info *)current->data)->key);
            }
            current = aux;
        }
    }
}

void loader_add_server(load_balancer *main, int server_id) {
    main->size += 3;

    main->vector = realloc (main->vector, main->size * sizeof(server_info *));
    DIE(main->vector == NULL, "main->vector realloc");

    main->vector[main->size - 3] = malloc(sizeof(server_info ));
    DIE(main->vector[main->size - 3] == NULL,
        "main->vector[main->size - 3] malloc");
    main->vector[main->size - 3]->server = init_server_memory();

    main->vector[main->size - 2] = malloc(sizeof(server_info ));
    DIE(main->vector[main->size - 2] == NULL,
        "main->vector[main->size - 2] malloc");
    main->vector[main->size - 2]->server = init_server_memory();

    main->vector[main->size - 1] = malloc(sizeof(server_info ));
    DIE(main->vector[main->size - 1] == NULL,
        "main->vector[main->size - 1] malloc");
    main->vector[main->size - 1]->server = init_server_memory();

    unsigned int hash;
    if (main->size == 3) {
        add_server_empty(main->vector, server_id);
        return;
    }

    for (unsigned int i = 3; i > 0; i--) {
        hash = hash_function_servers(&server_id);
        // daca trebuie adaugat inainte de primul
        if (hash < main->vector[0]->hash) {
            add_server_first(main->vector, hash, server_id, i, main->size);
            // daca trebuie adaugat dupa ultimul
        } else if (hash > main->vector[main->size - i - 1]->hash) {
            add_server_last(main->vector, hash, server_id, i, main->size);
        } else {  // daca adaug la mijloc
            add_server_middle(main->vector, hash, server_id, i, main->size);
        }
        server_id += 100000;
    }
}

void loader_remove_server(load_balancer *main, int server_id) {
    unsigned int poz;
    ll_node_t *current;
    for (unsigned int i = 0; i < 3; i++) {
        for (unsigned int j = 0; j < main->size; j++) {
            if (main->vector[j]->id == server_id) {
                poz = j;
                break;
            }
        }
        int next = poz + 1;
        if (poz == main->size - 1)
            next = 0;
        for (unsigned int j = 0; j < main->vector[poz]->server->hmax; j++) {
            current = main->vector[poz]->server->buckets[j]->head;
            if (current == NULL)
                continue;
            for (unsigned int k = 0;
                k < main->vector[poz]->server->buckets[j]->size; k++) {
                server_store(main->vector[next]->server
                , ((info*)current->data)->key, ((info*)current->data)->value);
                current = current->next;
            }
        }
        free_server_memory(main->vector[poz]->server);
        free(main->vector[poz]);
        for (unsigned int j = poz; j < main->size - 1; j++) {
            main->vector[j] = main->vector[j + 1];
        }
        main->size--;
        server_id += 100000;
    }
    main->vector = realloc(main->vector,
        main->size * sizeof(server_info *));
}

void loader_store(load_balancer *main, char *key,
    char *value, int *server_id) {
    unsigned int key_hash = hash_function_key(key);
    if (key_hash < main->vector[0]->hash ||
        key_hash > main->vector[main->size - 1]->hash) {
        (*server_id) = main->vector[0]->id % 100000;
        server_store(main->vector[0]->server, key, value);
    } else {
        for (unsigned int i = 0; i < main->size - 1; i++) {
            if (key_hash > main->vector[i]->hash &&
                key_hash < main->vector[i + 1]->hash) {
                (*server_id) = main->vector[i + 1]->id % 100000;
                server_store(main->vector[i + 1]->server, key, value);
            }
        }
    }
}

char *loader_retrieve(load_balancer *main, char *key, int *server_id) {
    unsigned int key_hash = hash_function_key(key);
    if (key_hash < main->vector[0]->hash ||
        key_hash > main->vector[main->size - 1]->hash) {
        (*server_id) = main->vector[0]->id % 100000;
        return server_retrieve(main->vector[0]->server, key);
    } else {
        for (unsigned int i = 0; i < main->size - 1; i++) {
            if (key_hash > main->vector[i]->hash &&
                key_hash < main->vector[i + 1]->hash) {
                (*server_id) = main->vector[i + 1]->id % 100000;
                return server_retrieve(main->vector[i + 1]->server, key);
            }
        }
    }
    return NULL;
}

void free_load_balancer(load_balancer *main) {
    for (unsigned int i = 0; i < main->size; i++) {
        free_server_memory(main->vector[i]->server);
        free(main->vector[i]);
    }
    free(main->vector);
    free(main);
}
