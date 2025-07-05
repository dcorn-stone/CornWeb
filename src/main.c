// importing libraries
#include "functions.h"
#include "response.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// buffer chunk size
#define BUFFER_SIZE 4096

// functions declaration
int serve_file(char *buf_ptr, int client_fd);

// structure definitions
server_config config;
http_request_t request_s;

// main function
int main() {

  // read configuration
  load_config();

  // creating connection
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in server_addr;

  if (server_fd < 0) {
    perror("Failed to create socket");
    return -1;
  }

  // using ipv4, listening all connections
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(config.port);

  inet_pton(AF_INET, config.address, &server_addr.sin_addr);

  // binding to a port and an address
  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    perror("Bind failed");
    close(server_fd);
    return -2;
  }

  // listen to the connection
  if (listen(server_fd, 3) < 0) {
    perror("Failed to listen");
    close(server_fd);
    return -3;
  }

  // console
  printf("Server started on port %d\n", config.port);
  printf("Listening from %s\n\n", config.address);

  // preparing for client connections
  int client_fd;
  struct sockaddr_in client_addr;
  socklen_t addrlen = sizeof(client_addr);

  // buffer to store requests
  char *buffer = malloc(BUFFER_SIZE);
  if (!buffer) {
    perror("Failed to allocate memory");
    return -4;
  }

  // respond to client
  while (1) {

    // connect to client
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
    if (client_fd < 0) {
      perror("Failed to accept connection");
      continue;
    }

    // recieve requests and respond
    memset(buffer, 0, BUFFER_SIZE);
    size_t bytes_read = read(client_fd, buffer, BUFFER_SIZE);
    if (bytes_read > 0) {
      printf("Request recieved: \n%s\n", buffer);
      buffer[bytes_read] = '\0';

      // responses
      if (handle_request(buffer, client_fd) < 0) {
        perror("Error encountered when preparing for response");
      }
    }
  }

  close(server_fd);
  free(buffer);
  return 0;
}
