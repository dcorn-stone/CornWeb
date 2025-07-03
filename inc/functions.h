#ifndef FUNCTIONS
#define FUNCTIONS

#define MAX_ENDPOINTS 64

int load_config();

typedef struct {
  char address[32];
  int port;
  char root_static[256];
} server_config;

typedef struct {
  char endpoint[127];
  char path[383];
} endpoint_t;

extern server_config config;

extern endpoint_t endpoints[MAX_ENDPOINTS];
extern int endpoint_count;

#endif // FUNCTIONS
