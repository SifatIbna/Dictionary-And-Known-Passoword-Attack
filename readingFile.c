// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// int main()
// {
//   char ch, file_name[25];
//   FILE *fp;
//   char *pass = "123456";
//   char *body;
//   // printf("Enter name of a file you wish to see\n");
//   // gets(file_name);

//   int bodyLength = 0;
//   int bodyLenghtbefore = 0;
//   char *openingBraces = "\{";
//   char *closingBraces = "\}";
//   char *email = "\"email\":\"admin@example.com\",";
//   char *password = "\"password\":";

//   bodyLength += strlen(openingBraces);
//   bodyLength += strlen(closingBraces);
//   bodyLength += strlen(email);
//   bodyLength += strlen(password);
//   bodyLength += strlen("\"");
//   bodyLength += strlen("\"");
//   bodyLenghtbefore = bodyLength;
//   fp = fopen("words.english", "r"); // read mode

//   if (fp == NULL)
//   {
//     perror("Error while opening the file.\n");
//     exit(EXIT_FAILURE);
//   }

//   printf("The contents of %s file are:\n", file_name);

//   while ((ch = fgetc(fp)) != EOF)
//   {

//     // bodyLength += strlen(&ch);
//     // body = malloc(bodyLength);
//     // strcat(body, openingBraces);
//     // strcat(body, email);
//     // strcat(body, password);
//     // strcat(body, "\"");
//     // strcat(body, &ch);
//     // strcat(body, "\"");
//     // strcat(body, closingBraces);
//     // printf("BODY: %s\n", body);
//     // free(body);
//     // bodyLength = bodyLenghtbefore;
//     printf("%c\n", ch);
//   }

//   fclose(fp);
//   return 0;
// }

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void)
{
  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

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

  while ((read = getline(&line, &len, fp)) != -1)
  {
    size_t ln = strlen(line) - 1;
    //* removing new line symbol

    if (*line && line[ln] == '\n')
    {

      line[ln] = '\0';
      line[ln - 1] = '\0';
    }

    bodyLength += strlen(line);
    char *body = malloc(bodyLength);
    strcat(body, openingBraces);
    strcat(body, email);
    strcat(body, password);
    strcat(body, "\"");
    strcat(body, line);
    strcat(body, "\"");
    strcat(body, closingBraces);
    printf("BODY:%s\n", body);

    printf("Retrieved line of length %zu:\n", read);
    printf("%s\n", line);
    printf("%ld\n", strlen(line));

    free(body);
    bodyLength = bodyLenghtbefore;
  }

  fclose(fp);
  if (line)
    free(line);
  exit(EXIT_SUCCESS);
}