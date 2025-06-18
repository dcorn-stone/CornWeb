#include <errno.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 4096

int serve_file(char *buf_ptr, int client_fd);

int main() {

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in server_addr;

  if (server_fd < 0) {
    perror("Failed to create socket");
    return -1;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    perror("Bind failed");
    close(server_fd);
    return -2;
  }

  if (listen(server_fd, 3) < 0) {
    perror("Failed to listen");
    close(server_fd);
    return -3;
  }

  printf("Server started on port %d\n", PORT);

  int client_fd;
  struct sockaddr_in client_addr;
  socklen_t addrlen = sizeof(client_addr);

  char *buffer = malloc(BUFFER_SIZE);
  if (!buffer) {
    perror("Failed to allocate memory");
    return -4;
  }

  unsigned int allocated = BUFFER_SIZE;

  size_t total_read = 0;

  while (1) {
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
    if (client_fd < 0) {
      perror("Failed to accept connection");
      continue;
    }

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

    size_t bytes_read = read(client_fd, buffer + total_read, sizeof(buffer));
    total_read += bytes_read;
    if (bytes_read > 0) {
      buffer[bytes_read] = '\0';
      printf("Request recieved: \n%s\n", buffer);
      serve_file(buffer, client_fd);
    }
  }

  close(server_fd);
  close(client_fd);
  free(buffer);
  return 0;
}

int serve_file(char *buf_ptr, int client_fd) {

  char method[8];
  char path[256];

  sscanf(buf_ptr, "%s %s", method, path);

  if (strcmp(method, "GET") == 0 && strcmp(path, "/") == 0) {

    FILE *fptr = fopen("../static/index.html", "r");
    if (fptr) {

      const char *header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

      write(client_fd, header, strlen(header));

      char buf[4096];
      size_t n;

      while ((n = fread(buf, 1, sizeof(buf), fptr)) > 0) {
        write(client_fd, &buf, n);
      }

      fclose(fptr);
    } else {
      const char *not_found = "HTTP/1.1 404 Not Found\r\nContent-Type: "
                              "text/html\r\n\r\n<h1>404 Not Found</h1>";
      write(client_fd, not_found, strlen(not_found));
      printf("File not found");
    }

  } else {
    // Not GET /, send 404
    const char *not_found = "HTTP/1.1 404 Not Found\r\nContent-Type: "
                            "text/html\r\n\r\n<h1>404 Not Found</h1>";
    write(client_fd, not_found, strlen(not_found));
  }

  return 0;
}
