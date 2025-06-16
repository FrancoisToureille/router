#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "neighbors.h"

typedef struct {
    char dest_id[MAX_NAME_LEN];
    char next_hop_ip[INET_ADDRSTRLEN];
    int metric;
} RouteEntry;

int compute_shortest_paths(const Neighbor neighbors[], int n, const char* local_id, RouteEntry* routing_table, int max_routes);

#endif
