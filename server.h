/* Copyright 2023 <> */
#ifndef SERVER_H_
#define SERVER_H_

#include "linkedlist.h"


typedef struct info info;
struct info {
	void *key;
	void *value;
};

typedef struct server_memory server_memory;
struct server_memory {
	linked_list_t **buckets; /* Array de liste simplu-inlantuite. */
	/* Nr. total de noduri existente curent in toate bucket-urile. */
	unsigned int size;
	unsigned int hmax; /* Nr. de bucket-uri. */
	/* (Pointer la) Functie pentru a calcula valoarea hash asociata cheilor. */
	unsigned int (*hash_function)(void*);
	/* (Pointer la) Functie pentru a compara doua chei. */
	int (*compare_function)(void*, void*);
	/* (*) Functie pentru a elibera memoria ocupata de cheie si valoare. */
	void (*key_val_free_function)(void*);
};

/*
 * Functie de comparare a cheilor:
 */
int
compare_function_strings(void *a, void *b);

/*
 * Functie de hashing:
 */
unsigned int
hash_function_string(void *a);

/*
 * Functie apelata pentru a elibera memoria ocupata de cheia si valoarea unei
 * perechi din hashtable. Daca cheia sau valoarea contin tipuri de date complexe
 * aveti grija sa eliberati memoria luand in considerare acest aspect.
 */
void key_val_free_function(void *data);

/** init_server_memory() -  Initializes the memory for a new server struct.
 * 							Make sure to check what is returned by malloc using DIE.
 * 							Use the linked list implementation from the lab.
 *
 * Return: pointer to the allocated server_memory struct.
 */
server_memory *init_server_memory();

/** free_server_memory() - Free the memory used by the server.
 * 						   Make sure to also free the pointer to the server struct.
 * 						   You can use the server_remove() function for this.
 *
 * @arg1: Server to free
 */
void free_server_memory(server_memory *server);

/**
 * server_store() - Stores a key-value pair to the server.
 *
 * @arg1: Server which performs the task.
 * @arg2: Key represented as a string.
 * @arg3: Value represented as a string.
 */
void server_store(server_memory *server, char *key, char *value);

/**
 * server_remove() - Removes a key-pair value from the server.
 *					 Make sure to free the memory of everything that is related to the entry removed.
 *
 * @arg1: Server which performs the task.
 * @arg2: Key represented as a string.
 */
void server_remove(server_memory *server, char *key);

/**
 * server_retrieve() - Gets the value associated with the key.
 * @arg1: Server which performs the task.
 * @arg2: Key represented as a string.
 *
 * Return: String value associated with the key
 *         or NULL (in case the key does not exist).
 */
char *server_retrieve(server_memory *server, char *key);

unsigned int server_get_size(server_memory *server);

unsigned int server_get_hmax(server_memory *server);

#endif /* SERVER_H_ */
