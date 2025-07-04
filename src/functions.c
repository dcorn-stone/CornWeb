#include "functions.h"
#include "response.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// reading flags
#define NORMAL_SECTION 0
#define ENDPOINTS 1

// global scope definitions
int endpoint_count = 0;
endpoint_t endpoints[MAX_ENDPOINTS];

// read configuration
int load_config() {

  FILE *conf = fopen("../server.conf", "r");
  if (!conf) {
    return -1;
  }

  // buffer for each line
  char line[512];
  int section = NORMAL_SECTION;

  while (fgets(line, sizeof(line), conf)) {

    char key[128], value[384];

    if (sscanf(line, "%127[^=]=%383[^\n]", key, value) == 2) {

      if (section == NORMAL_SECTION) {
        // read from normal section
        if (strcmp(key, "port") == 0) {
          config.port = atoi(value);

        } else if (strcmp(key, "listening_from") == 0) {
          strncpy(config.address, value, sizeof(config.address));
          config.address[sizeof(config.address) - 1] = '\0';

        } else if (strcmp(key, "root_static") == 0) {
          strncpy(config.root_static, value, sizeof(config.root_static));

        } else if (strcmp(key, "[/endpoint") == 0 &&
                   strcmp(value, "/path/to/file]") == 0) {
          section = ENDPOINTS;
        }

      } else if (section == ENDPOINTS) {
        // read from endpoints section and add endpoints
        strncpy(endpoints[endpoint_count].endpoint, key, 127);
        strncpy(endpoints[endpoint_count].path, value, 383);
        endpoint_count++;
      }
    }
  }

  return 0;
}

int handle_request(char *buf_ptr, int client_fd) {
  // stored request method, request path and http version
  char method[8];
  char path[512];
  char http_version[16];

  // parse informations
  sscanf(buf_ptr, "%s %255s %15s", method, path, http_version);
  strncpy(request_s.method, method, sizeof(method));
  strncpy(request_s.path, path, sizeof(path));
  strncpy(request_s.http_version, http_version, sizeof(http_version));

  if (strcmp(method, "GET") == 0) {
    handle_get(client_fd);
  }

  return 0;
}
