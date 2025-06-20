// importing libraries
#include "functions.h"
#include <errno.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


// buffer chunk size
#define BUFFER_SIZE 4096

// functions declaration
int serve_file(char *buf_ptr, int client_fd);

// structure declaration
server_config config;

// main function
int main() {
    

  //read configuration
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

  // tracking used space in the buffer
  unsigned int allocated = BUFFER_SIZE;

  size_t total_read = 0;

  // respond to client
  while (1) {
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
    if (client_fd < 0) {
      perror("Failed to accept connection");
      continue;
    }

    // preserve chunk for one more read
    if (allocated + 2048 + 1 > allocated) {
      allocated += BUFFER_SIZE;
      char *tmp_ptr = realloc(buffer, allocated);
      if (!tmp_ptr) {
        perror("Failed to reallocate memory");
        close(client_fd);
        close(server_fd);
        free(buffer);
        return -5;
      }
      buffer = tmp_ptr;
    }

    // recieve requests and respond
    size_t bytes_read = read(client_fd, buffer + total_read, sizeof(buffer));
    total_read += bytes_read;
    if (bytes_read > 0) {
      buffer[bytes_read] = '\0';
      printf("Request recieved: \n%s\n", buffer);

      // parsing files if needed
      serve_file(buffer, client_fd);
    }
  }

  close(server_fd);
  free(buffer);
  return 0;
}



// serve supported file types to client
int serve_file(char *buf_ptr, int client_fd) {

  // stores the requested method and the path
  char method[8];
  char path[256];

  // read the method and path into the variables
  sscanf(buf_ptr, "%s %s", method, path);


  // if using GET to the root domain
  if (strcmp(method, "GET") == 0 && strcmp(path, "/") == 0) {

    // serve the index.html file
    FILE *fptr = fopen(config.document_root, "rb");
    if (fptr) {

      // Calculate file size for Content-Length
      fseek(fptr, 0, SEEK_END);
      long filesize = ftell(fptr);
      rewind(fptr);

      // send header
      char header[256];
      snprintf(header, sizeof(header),
          "HTTP/1.1 200 OK\r\n"
          "Content-Type: text/html\r\n"
          "Content-Length: %ld\r\n"
          "\r\n", filesize);

      write(client_fd, header, strlen(header));

      char buf[4096];
      size_t n;

      // send the file
      while ((n = fread(buf, 1, sizeof(buf), fptr)) > 0) {
        size_t total_sent = 0;
        while (total_sent < n) {
          ssize_t sent = write(client_fd, buf + total_sent, n - total_sent);
          if (sent <= 0) {
            // Handle write error (e.g., client disconnected)
            break;
          }
        total_sent += sent;
        }
      }

      fclose(fptr);
    } else {

      // file not found
      const char *not_found = "HTTP/1.1 404 Not Found\r\nContent-Type: "
                              "text/html\r\n\r\n<h1>404 Not Found</h1>";
      write(client_fd, not_found, strlen(not_found));
      printf("File not found\n");
    }

  } else {
    // not GET /, send 404
    const char *not_found = "HTTP/1.1 404 Not Found\r\nContent-Type: "
                            "text/html\r\n\r\n<h1>404 Not Found</h1>";
    write(client_fd, not_found, strlen(not_found));
  }

  shutdown(client_fd, SHUT_WR);
  close(client_fd);
  return 0;
}
