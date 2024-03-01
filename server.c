/* Copyright 2023 <> */
#include <stdlib.h>
#include <string.h>

#include "server.h"
#include "utils.h"

int
compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

unsigned int
hash_function_string(void *a)
{
	unsigned char *puchar_a = (unsigned char*) a;
	unsigned long hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c;

	return hash;
}

void key_val_free_function(void *data) {
	free(((info *)data)->key);
    free(((info *)data)->value);
    free(data);
}

server_memory *init_server_memory()
{
	server_memory *server = malloc(sizeof(server_memory));
    DIE(server == NULL, "server malloc");

	server->hmax = 9999;
	server->size = 0;
	server->hash_function = hash_function_string;
	server->compare_function = compare_function_strings;
	server->key_val_free_function = key_val_free_function;

	server->buckets = malloc((server->hmax) * sizeof(linked_list_t*));
	DIE(server->buckets == NULL, "server->buckets malloc");

	for (unsigned int i = 0; i < server->hmax; i++)
		server->buckets[i] = ll_create(sizeof(info));
	return server;
}

void server_store(server_memory *server, char *key, char *value) {
	unsigned int hash = server->hash_function(key);
    char *node = server_retrieve(server, key);

    if (node == NULL) {
        int index = hash % server->hmax;

        info *data = malloc(sizeof(info));
		DIE(data == NULL, "data malloc");

        data->key = malloc(128);
		DIE(data->key == NULL, "data->key malloc");

        memcpy(data->key, key, 128);

        data->value = malloc(65536);
		DIE(data->value == NULL, "data->value malloc");

        memcpy(data->value, value, 65536);
        ll_add_nth_node(server->buckets[index]
        , server->buckets[index]->size, data);
        free(data);
        server->size++;
    } else {
        memcpy(node, value, 65536);
    }
}

char *server_retrieve(server_memory *server, char *key) {
	unsigned int hash = server->hash_function(key);
    int index = hash % server->hmax;
    ll_node_t *current = server->buckets[index]->head;
    while (current != NULL) {
        if (server->compare_function(((info *)current->data)->key, key) == 0)
            return ((info *)current->data)->value;
        current = current->next;
    }

	return NULL;
}

void server_remove(server_memory *server, char *key) {
	unsigned int hash = server->hash_function(key);
    int index = hash % server->hmax;
    ll_node_t *current = server->buckets[index]->head;
    int n = 0;
    while (!current) {
        if (server->compare_function(((info *)current->data)->key, key) == 0)
            break;
        current = current->next;
        n++;
    }
    current = ll_remove_nth_node(server->buckets[index], n);
    server->size--;
    key_val_free_function(current->data);
    free(current);
}

void free_server_memory(server_memory *server) {
	for (unsigned int i = 0; i < server->hmax; i++)
        ll_free(&server->buckets[i], key_val_free_function);
    free(server->buckets);
    free(server);
}

unsigned int server_get_size(server_memory *server)
{
	if (server == NULL)
		return 0;

	return server->size;
}

unsigned int server_get_hmax(server_memory *server)
{
	if (server == NULL)
		return 0;

	return server->hmax;
}
