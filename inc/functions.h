#ifndef FUNCTIONS
#define FUNCTIONS

int load_config();

int add_endpoint(const char endpoint, const char static_path);

typedef struct {
  char address[32];
  int port;
  char root_static[256];
} server_config;

extern server_config config;

#endif // FUNCTIONS
