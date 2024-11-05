## Load Balancer

### Description:

This program is a load balancer implemented using singly linked lists, with nine main functions that collectively allocate and manage memory for a hash ring containing servers. Each server is assigned keys with associated values based on hash values.

**CLIENT** has two main functions:

- `client_store`: Stores a value and assigns it to a key.
- `client_retrieve`: Retrieves the value associated with a key.

**LOAD BALANCER** has four main functions:

- `loader_store`: Stores a key on the appropriate server, determined by the hash function.
- `loader_retrieve`: Retrieves the value of a key and the server ID where it’s located.
- `loader_add_server`: Adds a server to the hash ring based on its hash.
- `loader_remove_server`: Removes a server from the hash ring, redistributing its keys to the next server in the ring.

**SERVER** has three main functions:

- `server_store`: Adds a key node to the appropriate server.
- `server_retrieve`: Returns the value associated with a key.
- `server_remove`: Removes the key node from the appropriate server.

#### Additional Explanations:

- `init_load_balancer` and `init_server_memory` allocate and initialize the hash ring and servers.
- `free_load_balancer` and `free_server_memory` release all memory used by the hash ring and servers.
- The implementation of the singly linked list is located in `linkedlist.c` and `linkedlist.h`.
