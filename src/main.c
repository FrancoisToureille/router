#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "config.h"
#include "neighbors.h"
#include "routing.h"
#include "hello.h"
#include "dijkstra.h"
#include "cli.h"
#include <string.h>
#include <time.h>

volatile int keep_running = 1;

void int_handler(int dummy) {
    keep_running = 0;
}

int main() {
    signal(SIGINT, int_handler);
    signal(SIGTERM, int_handler);

    RouterConfig config;
    if (load_router_config("router.conf", &config) != 0) {
        fprintf(stderr, "Erreur lecture config\n");
        return 1;
    }

    printf("ID Routeur: %s\n", config.router_id);
    printf("Interfaces:\n");
    for (int i = 0; i < config.interface_count; ++i)
        printf("  %s\n", config.interface_names[i]);

    init_neighbors();
    start_hello(config.router_id, (const char**)config.interface_names, config.interface_count);

    while (keep_running) {
        sleep(1);
        cleanup_neighbors();

        Neighbor current_neighbors[MAX_NEIGHBORS];
        int n = get_neighbors(current_neighbors, MAX_NEIGHBORS);

        RouteEntry routing_table[MAX_NEIGHBORS];
        int routes = compute_shortest_paths(current_neighbors, n, config.router_id, routing_table, MAX_NEIGHBORS);

        // Mise à jour table de routage (simple suppression et réajout)
        // Pour demo on supprime toutes et on réajoute
        for (int i = 0; i < routes; i++) {
            add_route(routing_table[i].dest_id, routing_table[i].next_hop_ip, config.interface_names[0], routing_table[i].metric);
        }

        // CLI en mode non bloquant ? Ou lancer dans thread à part. Ici on fait simple:
        // Pour test, on affiche voisins automatiquement:
        printf("Voisins actifs: %d\n", n);
    }

    stop_hello();

    printf("Arrêt du programme.\n");
    return 0;
}
