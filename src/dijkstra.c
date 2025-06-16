#include "dijkstra.h"
#include <string.h>
#include <limits.h>
#include <stdio.h>

// Un graphe simple représenté par la liste des voisins, chaque lien coût 1 (peut être étendu)

int compute_shortest_paths(const Neighbor neighbors[], int n, const char* local_id, RouteEntry* routing_table, int max_routes) {
    // Pour simplicité on considère neighbors comme graphe (à étendre dans vrai graphe)
    // Ici on fait un calcul très simplifié: chaque voisin direct a coût 1

    // On stocke les routes directes vers voisins
    int route_count = 0;
    for (int i = 0; i < n && route_count < max_routes; i++) {
        if (strcmp(neighbors[i].router_id, local_id) == 0) continue;
        strncpy(routing_table[route_count].dest_id, neighbors[i].router_id, MAX_NAME_LEN - 1);
        routing_table[route_count].dest_id[MAX_NAME_LEN - 1] = 0;
        strncpy(routing_table[route_count].next_hop_ip, neighbors[i].ip, INET_ADDRSTRLEN - 1);
        routing_table[route_count].next_hop_ip[INET_ADDRSTRLEN - 1] = 0;
        routing_table[route_count].metric = neighbors[i].link_cost;
        route_count++;
    }

    // TODO: Implémenter l'algorithme Dijkstra complet pour plus de 1 saut et meilleurs chemins.
    // Pour l'instant, on considère uniquement les voisins directs.

    return route_count;
}
