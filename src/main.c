// importing libraries
#include "functions.h"
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

// structure declaration
server_config config;

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

    // detect file type
    char file_type[128];
    const char *ext = strrchr(config.root_static, '.');
    if (!ext) {
      strcpy(file_type, "application/octet-stream"); // fall-back MIME type
    }

    if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0)
      strcpy(file_type, "text/html");
    else if (strcmp(ext, ".css") == 0)
      strcpy(file_type, "text/css");
    else if (strcmp(ext, ".js") == 0)
      strcpy(file_type, "application/javascript");
    else if (strcmp(ext, ".json") == 0)
      strcpy(file_type, "application/json");
    else if (strcmp(ext, ".png") == 0)
      strcpy(file_type, "image/png");
    else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0)
      strcpy(file_type, "image/jpeg");
    else if (strcmp(ext, ".gif") == 0)
      strcpy(file_type, "image/gif");
    else
      strcpy(file_type, "application/octet-stream"); // default MIME type

    // serve the file
    FILE *fptr = fopen(config.root_static, "rb");
    if (fptr) {

      // Calculate file size for Content-Length
      fseek(fptr, 0, SEEK_END);
      long filesize = ftell(fptr);
      rewind(fptr);

      // send header
      char header[256];
      snprintf(header, sizeof(header),
               "HTTP/1.1 200 OK\r\n"
               "Content-Type: %s\r\n"
               "Content-Length: %ld\r\n"
               "\r\n",
               file_type, filesize);

      write(client_fd, header, strlen(header));

      char buf[4096];
      size_t n;

      // send the file
      while ((n = fread(buf, 1, sizeof(buf), fptr)) > 0) {
        size_t total_sent = 0;
        while (total_sent < n) {
          ssize_t sent = write(client_fd, buf + total_sent, n - total_sent);
          if (sent <= 0) {
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
  } else if (strcmp(method, "GET") == 0) {

    // search for file
    for (int i = 0; i < endpoint_count; i++) {

      // parse the corresponding static file
      if (strcmp(path, endpoints[i].endpoint) == 0) {

        // detect file type
        char file_type[128];
        const char *ext = strrchr(endpoints[i].path, '.');
        if (!ext) {
          strcpy(file_type, "application/octet-stream"); // fall-back MIME type
        }

        if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0)
          strcpy(file_type, "text/html");
        else if (strcmp(ext, ".css") == 0)
          strcpy(file_type, "text/css");
        else if (strcmp(ext, ".js") == 0)
          strcpy(file_type, "application/javascript");
        else if (strcmp(ext, ".json") == 0)
          strcpy(file_type, "application/json");
        else if (strcmp(ext, ".png") == 0)
          strcpy(file_type, "image/png");
        else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0)
          strcpy(file_type, "image/jpeg");
        else if (strcmp(ext, ".gif") == 0)
          strcpy(file_type, "image/gif");
        else
          strcpy(file_type, "application/octet-stream"); // default MIME type

        // serve file
        FILE *fptr = fopen(endpoints[i].path, "rb");
        if (fptr) {

          // Calculate file size for Content-Length
          fseek(fptr, 0, SEEK_END);
          long filesize = ftell(fptr);
          rewind(fptr);

          // send header
          char header[256];
          snprintf(header, sizeof(header),
                   "HTTP/1.1 200 OK\r\n"
                   "Content-Type: %s\r\n"
                   "Content-Length: %ld\r\n"
                   "\r\n",
                   file_type, filesize);

          write(client_fd, header, strlen(header));

          char buf[4096];
          size_t n;

          // send file
          while ((n = fread(buf, 1, sizeof(buf), fptr)) > 0) {
            size_t total_sent = 0;
            while (total_sent < n) {
              ssize_t sent = write(client_fd, buf + total_sent, n - total_sent);
              if (sent <= 0) {
                break;
              }
              total_sent += sent;
            }
          } // send file

          fclose(fptr);
        }
      } // parse the corresponding static file
    } // search for file
  }

  shutdown(client_fd, SHUT_WR);
  close(client_fd);
  return 0;
}
