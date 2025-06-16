#include "config.h"
#include <stdio.h>
#include <string.h>

int load_router_config(const char* filename, RouterConfig* config) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("fopen router.conf");
        return -1;
    }
    char line[128];
    config->interface_count = 0;
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        if (strncmp(line, "router_id=", 10) == 0) {
            strncpy(config->router_id, line + 10, MAX_NAME_LEN - 1);
            config->router_id[strcspn(config->router_id, "\r\n")] = 0;
        } else if (strncmp(line, "interface=", 10) == 0) {
            if (config->interface_count < MAX_INTERFACES) {
                strncpy(config->interface_names[config->interface_count], line + 10, MAX_NAME_LEN - 1);
                config->interface_names[config->interface_count][strcspn(config->interface_names[config->interface_count], "\r\n")] = 0;
                config->interface_count++;
            }
        }
    }
    fclose(f);
    return 0;
}
