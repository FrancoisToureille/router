#include "neighbors.h"
#include "routing.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

static Neighbor neighbors[MAX_NEIGHBORS];
static int neighbor_count = 0;

void init_neighbors() {
    neighbor_count = 0;
    memset(neighbors, 0, sizeof(neighbors));
}

void add_or_update_neighbor(const char* id, const char* ip) {
    time_t now = time(NULL);
    for (int i = 0; i < neighbor_count; ++i) {
        if (strcmp(neighbors[i].router_id, id) == 0) {
            neighbors[i].last_seen = now;
            return;
        }
    }
    if (neighbor_count < MAX_NEIGHBORS) {
        strncpy(neighbors[neighbor_count].router_id, id, MAX_NAME_LEN - 1);
        neighbors[neighbor_count].router_id[MAX_NAME_LEN - 1] = '\0';
        strncpy(neighbors[neighbor_count].ip, ip, INET_ADDRSTRLEN - 1);
        neighbors[neighbor_count].ip[INET_ADDRSTRLEN - 1] = '\0';
        neighbors[neighbor_count].last_seen = now;
        neighbors[neighbor_count].link_cost = 1;
        neighbor_count++;
    }
}

int get_neighbors(Neighbor* list, int max) {
    int count = 0;
    time_t now = time(NULL);
    for (int i = 0; i < neighbor_count && count < max; ++i) {
        if (difftime(now, neighbors[i].last_seen) <= TIMEOUT_NEIGHBOR) {
            list[count++] = neighbors[i];
        }
    }
    return count;
}

void cleanup_neighbors() {
    time_t now = time(NULL);
    for (int i = 0; i < neighbor_count; ++i) {
        if (difftime(now, neighbors[i].last_seen) > TIMEOUT_NEIGHBOR) {
            printf("[INFO] Voisin %s timeout, suppression...\n", neighbors[i].router_id);
            del_route(neighbors[i].ip);
            for (int j = i; j < neighbor_count - 1; ++j) {
                neighbors[j] = neighbors[j + 1];
            }
            neighbor_count--;
            i--;
        }
    }
}
