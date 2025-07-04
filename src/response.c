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
               "Server: CornWeb/0.1\r\n"
               "Content-Type: %s\r\n"
               "Content-Length: %ld\r\n"
               "Connection: close\r\n"
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
  } else {

    // search for file
    for (int i = 0; i < endpoint_count; i++) {

      // parse the corresponding static file
      if (strcmp(request_s.path, endpoints[i].endpoint) == 0) {

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
                   "Server: CornWeb/0.1\r\n"
                   "Content-Type: %s\r\n"
                   "Content-Length: %ld\r\n"
                   "Connection: close\r\n"
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
}
