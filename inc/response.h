#ifndef RESPONSE
#define RESPONSE

void handle_get(int client_fd);

typedef struct {
  char method[8];
  char path[512];
  char http_version[16];
  struct {
    char name[64];
    char value[256];
  } headers[32];
  int header_count;
} http_request_t;

typedef struct {
  int status_code;
  char status_text[32];
  char headers[16][128];
  int header_count;
} http_response_t;

extern http_request_t request_s;

#endif // !RESPONSE
