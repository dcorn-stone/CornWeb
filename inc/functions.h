#ifndef FUNCTIONS
#define FUNCTIONS

int load_config();

typedef struct {
  char address[32];
  int port;
  char document_root[256];
} server_config;

extern server_config config;

#endif // FUNCTIONS
