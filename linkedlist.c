/* Copyright 2023 <> */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linkedlist.h"
#include "utils.h"

linked_list_t *
ll_create(unsigned int data_size)
{
    linked_list_t* list = malloc(sizeof(linked_list_t));
    DIE(list == NULL, "linked_list malloc");

    list->head = NULL;
    list->data_size = data_size;
    list->size = 0;

    return list;
}

ll_node_t*
create_node(void* new_data, unsigned int data_size)
{
    ll_node_t* node = malloc(sizeof(ll_node_t));
    DIE(node == NULL, "node malloc");

    node->data = malloc(data_size);
    DIE(node->data == NULL, "node->data malloc");

    memcpy(node->data, new_data, data_size);

    return node;
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se creeaza un nou nod care e
 * adaugat pe pozitia n a listei reprezentata de pointerul list. Pozitiile din
 * lista sunt indexate incepand cu 0 (i.e. primul nod din lista se afla pe
 * pozitia n=0). Daca n >= nr_noduri, noul nod se adauga la finalul listei. Daca
 * n < 0, eroare.
 */

void
ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
    ll_node_t* node = create_node((void*) new_data, list->data_size);

    if (list == NULL) {
        return;
    }

    if (n > list->size) {
        n = list->size;
    }

    if (n == 0) {
		node->next = list->head;
		list->head = node;
		list->size++;
		return;
	}

    ll_node_t* current = list->head;

	for(unsigned int i = 0; i < n - 1; i++) {
		current = current->next;
	}

	node->next = current->next;
	current->next = node;
	list->size++;
	return;
}

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Daca n >= nr_noduri - 1, se elimina nodul de
 * la finalul listei. Daca n < 0, eroare. Functia intoarce un pointer spre acest
 * nod proaspat eliminat din lista. Este responsabilitatea apelantului sa
 * elibereze memoria acestui nod.
 */
ll_node_t *
ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
    if (list == NULL) {
        return NULL;
    }

    if (list->head == NULL) { /* Lista este goala. */
        return NULL;
    }

    if (n == 0)
    {
        ll_node_t* current = list->head;
        list->head = current->next;
        list->size--;
        return current;
    }

    /* n >= list->size - 1 inseamna eliminarea nodului de la finalul listei. */
    if (n >= list->size - 1) {
        n = list->size - 1;
        ll_node_t* current = list->head;

		for(unsigned int i = 0; i < n - 2; i++) {
			current = current->next;
		}

		ll_node_t* aux = current->next;
		current->next = NULL;
		list->size--;
		return aux;
    }

    ll_node_t* current = list->head;
	for(unsigned int i = 0; i < n - 1; i ++)
	{
		current = current->next;
	}
	list->size--;
	ll_node_t* aux = current->next;
	current->next = current->next->next;
	return aux;
}

/*
 * Functia intoarce numarul de noduri din lista al carei pointer este trimis ca
 * parametru.
 */
unsigned int
ll_get_size(linked_list_t* list)
{
    if (list == NULL) {
        return -1;
    }

    return list->size;
}

/*
 * Procedura elibereaza memoria folosita de toate nodurile din lista, iar la
 * sfarsit, elibereaza memoria folosita de structura lista si actualizeaza la
 * NULL valoarea pointerului la care pointeaza argumentul (argumentul este un
 * pointer la un pointer).
 */
void
ll_free(linked_list_t** pp_list, void (*free_function)(void*))
{
    linked_list_t* list = *pp_list;

	if(list != NULL || list->size > 0) {
		ll_node_t* current = list->head;
		ll_node_t* aux;

		for(unsigned int i = 0; i < list->size; i++)
		{
			aux = current->next;
			free_function(current->data);
			free(current);
			current = aux;
		}
	}

    free(list);
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza int-uri. Functia afiseaza toate valorile int stocate in nodurile
 * din lista inlantuita separate printr-un spatiu.
 */
void
ll_print_int(linked_list_t* list)
{
    if (list == NULL) {
        return;
    }

    ll_node_t* current = list->head;

	for(unsigned int i = 0; i < list->size; i++)
	{
		printf("%d ", *(int*)current->data);
		current = current->next;
	}
    printf("\n");
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza string-uri. Functia afiseaza toate string-urile stocate in
 * nodurile din lista inlantuita, separate printr-un spatiu.
 */
void
ll_print_string(linked_list_t* list)
{
    if (list == NULL) {
        return;
    }

    ll_node_t* current = list->head;

	for(unsigned int i = 0; i < list->size; i++)
	{
		printf("%s ", (char*)current->data);
		current = current->next;
	}
    printf("\n");
}
