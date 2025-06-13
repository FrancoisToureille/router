#ifndef CONFIG_H
#define CONFIG_H

#define MAX_INTERFACES 10
#define MAX_NAME_LEN 64
#define MAX_NEIGHBORS 50
#define TIMEOUT_NEIGHBOR 10   // secondes avant timeout voisin

typedef struct {
    char router_id[MAX_NAME_LEN];
    char interface_names[MAX_INTERFACES][MAX_NAME_LEN];
    int interface_count;
} RouterConfig;

int load_router_config(const char* filename, RouterConfig* config);

#endif
