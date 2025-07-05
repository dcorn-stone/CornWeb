// importing libraries
#include "response.h"
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

void handle_get(int client_fd) {

  // if using GET to the root domain
  if (strcmp(request_s.path, "/") == 0) {

    // get file type
    const char *f_type = file_type(config.root_static);

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
               "Server: CornWeb/0.1\r\n"
               "Content-Type: %s\r\n"
               "Content-Length: %ld\r\n"
               "Connection: close\r\n"
               "\r\n",
               f_type, filesize);

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

  } else {
    // search for file
    for (int i = 0; i < endpoint_count; i++) {

      // parse the corresponding static file
      if (strcmp(request_s.path, endpoints[i].endpoint) == 0) {

        const char *f_type = file_type(endpoints[i].path);

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
                   "Server: CornWeb/0.1\r\n"
                   "Content-Type: %s\r\n"
                   "Content-Length: %ld\r\n"
                   "Connection: close\r\n"
                   "\r\n",
                   f_type, filesize);

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

        } else {
          // file not found
          const char *not_found = "HTTP/1.1 404 Not Found\r\nContent-Type: "
                                  "text/html\r\n\r\n<h1>404 Not Found</h1>";
          write(client_fd, not_found, strlen(not_found));
          printf("File not found\n");
        }
      } // parse the corresponding static file
    } // search for file
  }

  shutdown(client_fd, SHUT_WR);
  close(client_fd);
}

void handle_head(int client_fd) {

  if (strcmp(request_s.path, "/") == 0) {

    // get file type
    const char *f_type = file_type(config.root_static);

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
               "Server: CornWeb/0.1\r\n"
               "Content-Type: %s\r\n"
               "Content-Length: %ld\r\n"
               "Connection: close\r\n"
               "\r\n",
               f_type, filesize);

      write(client_fd, header, strlen(header));
      fclose(fptr);
    } else {

      // file not found
      const char *not_found = "HTTP/1.1 404 Not Found\r\nContent-Type: "
                              "text/html\r\n\r\n<h1>404 Not Found</h1>";
      write(client_fd, not_found, strlen(not_found));
      printf("File not found\n");
    }

  } else {

    // search for file
    for (int i = 0; i < endpoint_count; i++) {

      // parse the corresponding static file
      if (strcmp(request_s.path, endpoints[i].endpoint) == 0) {

        const char *f_type = file_type(endpoints[i].path);

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
                   "Server: CornWeb/0.1\r\n"
                   "Content-Type: %s\r\n"
                   "Content-Length: %ld\r\n"
                   "Connection: close\r\n"
                   "\r\n",
                   f_type, filesize);

          write(client_fd, header, strlen(header));
          fclose(fptr);

        } else {

          // file not found
          const char *not_found = "HTTP/1.1 404 Not Found\r\nContent-Type: "
                                  "text/html\r\n\r\n<h1>404 Not Found</h1>";
          write(client_fd, not_found, strlen(not_found));
          printf("File not found\n");
        }
      }
    }
  }
  shutdown(client_fd, SHUT_WR);
  close(client_fd);
}
