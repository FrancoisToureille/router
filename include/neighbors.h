#ifndef NEIGHBORS_H
#define NEIGHBORS_H

#include <time.h>
#include <netinet/in.h>

#define MAX_NAME_LEN 64
#define MAX_NEIGHBORS 50
#define TIMEOUT_NEIGHBOR 10

typedef struct {
    char router_id[MAX_NAME_LEN];
    char ip[INET_ADDRSTRLEN];
    time_t last_seen;
    int link_cost;
} Neighbor;

void init_neighbors();
void add_or_update_neighbor(const char* id, const char* ip);
int get_neighbors(Neighbor* list, int max);
void cleanup_neighbors();

#endif
