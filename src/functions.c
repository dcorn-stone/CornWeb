#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NORMAL_SECTION 0
#define ENDPOINTS 1

int load_config() {
  FILE *conf = fopen("../server.conf", "r");
  if (!conf) {
    return -1;
  }

  char line[512];
  int section = NORMAL_SECTION;
  while (fgets(line, sizeof(line), conf)) {
    char key[128], value[384];
    if (sscanf(line, "%127[^=]=%383[^\n]", key, value) == 2) {
      if (section == NORMAL_SECTION) {
        if (strcmp(key, "port") == 0) {
          config.port = atoi(value);
        } else if (strcmp(key, "listening_address") == 0) {
          strncpy(config.address, value, sizeof(config.address));

          config.address[sizeof(config.address) - 1] = '\0';
        } else if (strcmp(key, "root_static") == 0) {

          strncpy(config.root_static, value, sizeof(config.root_static));
        } else if (strcmp(key, "[/endpoint") == 0 &&
                   strcmp(value, "/path/to/file]") == 0) {
          section = ENDPOINTS;
        }
      } else if (section == ENDPOINTS) {
        strncpy(endpoints[endpoint_count].endpoint, key, 127);
        strncpy(endpoints[endpoint_count].path, value, 383);
        endpoint_count++;
      }
    }
  }

  return 0;
}
