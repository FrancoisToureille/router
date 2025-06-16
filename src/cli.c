#include "cli.h"
#include "neighbors.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern volatile int keep_running;

void cli_run() {
    char cmd[64];
    while (keep_running) {
        printf("> ");
        if (!fgets(cmd, sizeof(cmd), stdin)) break;
        cmd[strcspn(cmd, "\r\n")] = 0;

        if (strcmp(cmd, "show neighbors") == 0) {
            Neighbor list[MAX_NEIGHBORS];
            int n = get_neighbors(list, MAX_NEIGHBORS);
            printf("Voisins (%d):\n", n);
            for (int i = 0; i < n; i++) {
                printf("  %s - %s - last seen %ld sec ago\n", list[i].router_id, list[i].ip, time(NULL) - list[i].last_seen);
            }
        } else if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "quit") == 0) {
            keep_running = 0;
        } else if (strcmp(cmd, "disable protocol") == 0) {
            // TODO : désactiver protocole, arrêter hello etc.
            printf("Protocole désactivé (non implémenté)\n");
        } else if (strcmp(cmd, "enable protocol") == 0) {
            // TODO : activer protocole, lancer hello etc.
            printf("Protocole activé (non implémenté)\n");
        } else {
            printf("Commandes disponibles: show neighbors, enable protocol, disable protocol, exit\n");
        }
    }
}
