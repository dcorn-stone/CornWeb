#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int load_config() {
  FILE *conf = fopen("../server.conf", "r");
  if (!conf) {
    return -1;
  }

  char line[512];
  while (fgets(line, sizeof(line), conf)) {
    char key[128], value[384];
    if (sscanf(line, "%127[^=]=%383[^\n]", key, value) == 2) {
      if (strcmp(key, "port") == 0) {
        config.port = atoi(value);
      } else if (strcmp(key, "listening_address") == 0) {
        strncpy(config.address, value, sizeof(config.address));
        config.address[sizeof(config.address) - 1] = '\0';
      } else if (strcmp(key, "root_static") == 0) {
        strncpy(config.root_static, value, sizeof(config.root_static));
      }
    }
  }

  return 0;
}
