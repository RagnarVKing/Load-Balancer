**Nume:Eftenie Andrei-Vlad**

## Load Balancer

### Descriere:

* Scurtă descriere a funcționalității programului

Programul foloseste 9 functii principale si este implementat cu ajutorul listelor simplu inlantuite.
Acesta aloca memorie pentru un hash_ring pe care se pun servere, care au atribuite chei cu valori, in functie de hash-uri.

CLIENT are 2 functii principale :

- client_store -> stocheaza o valoare si o atribuie unei chei

- client_retrieve -> intoarce valoarea un chei

LOAD BALANCER are 4 functii principale :

- loader_store -> stocheaza o cheie pe serverul potrivit, in functie de hash

- loader_retrieve -> intoarce valoarea un chei si id-ul serverului pe care se afla

- loader_add_server -> adauga un server pe hash_ring, in functie de hash, s

- loader_remove_server -> sterge un server de pe hash_ring, redistribuind cheile de pe el urmatorului server

SERVER are 3 functii principale :

- server_store -> adauaga nodul cheii pe serverul potrivit

- server_retrieve -> intoarce valoarea atribuita unei chei

- server_remove -> sterge nodul cheii de pe serverul potrivit

* Eventuale explicații suplimentare pentru anumite părți din temă ce crezi că nu sunt suficient de clare doar urmărind codul

- Functiile init_load_balancer si init_server_memory aloca si initializeaza hash_ring-ul si serverele.

- Functiile free_load_balancer si free_server_memory elibereaza toata memoria utilizata de hash_ring si de servere.

- Implementarea listei simplu inlantuite este in "linkedlist.c" si "linkedlist.h".
