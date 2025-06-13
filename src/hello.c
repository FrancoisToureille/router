#include "hello.h"
#include "neighbors.h"
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define HELLO_PORT 5000
#define HELLO_INTERVAL 2

static volatile int running = 0;
static pthread_t hello_thread;
static char local_router_id[MAX_NAME_LEN];
static char local_interfaces[MAX_INTERFACES][MAX_NAME_LEN];
static int local_interface_count = 0;

static void* hello_sender(void* arg) {
    while (running) {
        for (int i = 0; i < local_interface_count; ++i) {
            // Envoyer un paquet hello multicast ou broadcast sur chaque interface
            // Pour simplification, on envoie sur localhost en UDP vers HELLO_PORT
            int sock = socket(AF_INET, SOCK_DGRAM, 0);
            if (sock < 0) continue;

            struct sockaddr_in addr = {0};
            addr.sin_family = AF_INET;
            addr.sin_port = htons(HELLO_PORT);
            addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Remplacer par multicast ou interface

            char msg[128];
            snprintf(msg, sizeof(msg), "HELLO %s", local_router_id);

            sendto(sock, msg, strlen(msg), 0, (struct sockaddr*)&addr, sizeof(addr));
            close(sock);
        }
        sleep(HELLO_INTERVAL);
    }
    return NULL;
}

void start_hello(const char* router_id, const char* interfaces[], int interface_count) {
    if (running) return;
    running = 1;
    strncpy(local_router_id, router_id, MAX_NAME_LEN - 1);
    local_router_id[MAX_NAME_LEN - 1] = 0;
    local_interface_count = interface_count;
    for (int i = 0; i < interface_count; ++i) {
        strncpy(local_interfaces[i], interfaces[i], MAX_NAME_LEN - 1);
        local_interfaces[i][MAX_NAME_LEN - 1] = 0;
    }
    pthread_create(&hello_thread, NULL, hello_sender, NULL);
}

void stop_hello() {
    if (!running) return;
    running = 0;
    pthread_join(hello_thread, NULL);
}
