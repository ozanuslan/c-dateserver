#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for strlen
#include <sys/socket.h>
#include <arpa/inet.h> // for inet_addr
#include <unistd.h>    // for write
#include <ctype.h>

size_t trimwhitespace(char *out, size_t len, const char *str);
int containsValidParams(char *str);
int containsValidParamChar(char *character);

char *GET_DATE = "GET_DATE";

char VALID_PARAMS[43][3] = {
    "\%%", // a literal %
    "\%a", // locale's abbreviated weekday name (e.g., Sun)
    "\%A", // locale's full weekday name (e.g., Sunday)
    "\%b", // locale's abbreviated month name (e.g., Jan)
    "\%B", // locale's full month name (e.g., January)
    "\%c", // locale's date and time (e.g., Thu Mar  3 23:05:25 2005)
    "\%C", // century; like %Y, except omit last two digits (e.g., 20)
    "\%d", // day of month (e.g., 01)
    "\%D", // date; same as %m/%d/%y
    "\%e", // day of month, space padded; same as %_d
    "\%F", // full date; like %+4Y-%m-%d
    "\%g", // last two digits of year of ISO week number (see %G)
    "\%G", // year of ISO week number (see %V); normally useful only
           // with %V
    "\%h", // same as %b
    "\%H", // hour (00..23)
    "\%I", // hour (01..12)
    "\%j", // day of year (001..366)
    "\%k", // hour, space padded ( 0..23); same as %_H
    "\%l", // hour, space padded ( 1..12); same as %_I
    "\%m", // month (01..12)
    "\%M", // minute (00..59)
    "\%n", // a newline
    "\%N", // nanoseconds (000000000..999999999)
    "\%p", // locale's equivalent of either AM or PM; blank if not known
    "\%P", // like %p, but lower case
    "\%q", // quarter of year (1..4)
    "\%r", // locale's 12-hour clock time (e.g., 11:11:04 PM)
    "\%R", // 24-hour hour and minute; same as %H:%M
    "\%s", // seconds since 1970-01-01 00:00:00 UTC
    "\%S", // second (00..60)
    "\%t", // a tab
    "\%T", // time; same as %H:%M:%S
    "\%u", // day of week (1..7); 1 is Monday
    "\%U", // week number of year, with Sunday as first day of week
           // (00..53)
    "\%V", // ISO week number, with Monday as first day of week (01..53)
    "\%w", // day of week (0..6); 0 is Sunday
    "\%W", // week number of year, with Monday as first day of week
           // (00..53)
    "\%x", // locale's date representation (e.g., 12/31/99)
    "\%X", // locale's time representation (e.g., 23:13:48)
    "\%y", // last two digits of year (00..99)
    "\%Y", // year
    "\%z", // +hhmm numeric time zone (e.g., -0400)
    "\%Z"  // alphabetic time zone abbreviation (e.g., EDT)
};

int main(int argc, char *argv[])
{
  int socket_fd, new_socket, c, read_size;
  int MSG_LEN = 2000;
  char client_message[MSG_LEN];
  struct sockaddr_in server, client;

  // Start server setup //
  // Create socket
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1)
  {
    puts("Could not create socket");
    return 1;
  }

  int true = 1;
  setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int)); // Necessary for server to shutdown properly

  uint16_t PORT = 5555;

  // Prepare the sockaddr_in structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(PORT);

  // Bind
  if (bind(socket_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
  {
    printf("Bind failed on port: %d\n", PORT);
    return 1;
  }
  printf("Socket binded on: %d\n", PORT);

  // Listen
  listen(socket_fd, 3);
  puts("Waiting for incoming connections...");

  char *welcome_message = "\nWelcome to the date server!!\n";
  char *usage_message = "Usage:\n"
                        "\tGET_DATE - invoke 'date' command on the server\n"
                        "\t           only format arguments are supported\n"
                        "\thelp     - show usage message\n"
                        "\tclose    - shut down the server\n"
                        "\texit     - disconnect from the server\n";
  char *prompt_message = "▶ ";

  c = sizeof(struct sockaddr_in);
  int closeServer = 0;
  // End server setup //

  // Start waiting for connections
  while (new_socket = accept(socket_fd, (struct sockaddr *)&client, (socklen_t *)&c))
  {
    printf("Connection accepted from client: %d\n", client.sin_addr.s_addr); // Log client address
    write(new_socket, welcome_message, strlen(welcome_message));             // Send welcome message
    write(new_socket, usage_message, strlen(usage_message));                 // Send usage message
    write(new_socket, prompt_message, strlen(prompt_message));               // Send prompt character to client

    //Receive a message from client
    while ((read_size = recv(new_socket, client_message, MSG_LEN, 0)) > 0)
    {
      char message[read_size];
      strcpy(message, client_message);
      message[strcspn(message, "\r\n")] = 0; // Remove newline character

      char command[strlen(GET_DATE) + 1];              // +1 for null terminator
      strncpy(command, &message[0], strlen(GET_DATE)); // Copy command to buffer
      command[strlen(GET_DATE)] = '\0';                // Null terminate string

      if (strcmp(command, GET_DATE) == 0 && containsValidParams(message)) // If the command is GET_DATE
      {
        char date_params[200];                  // Buffer for date command parameters
        if (strlen(message) > strlen(GET_DATE)) // If there are parameters
        {
          char non_trimmed_params[strlen(message) - strlen(GET_DATE)]; // Buffer for non-trimmed parameters
          strncpy(non_trimmed_params, &message[strlen(GET_DATE) + 1], strlen(message) - strlen(GET_DATE));
          non_trimmed_params[strlen(message) - strlen(GET_DATE)] = '\0'; // Null terminate string
          trimwhitespace(date_params, 200, non_trimmed_params);
        }
        else // If there are no parameters
        {
          strcpy(date_params, "");
        }

        char date_command[strlen(date_params) + strlen(GET_DATE) + 3]; // +3 for null terminator and space
        strcpy(date_command, "date ");                                 // Copy command to buffer

        if (strlen(date_params) > 0) // If therea are parameters
        {
          strcat(date_command, "+\"");
          strcat(date_command, date_params);
          strcat(date_command, "\"");
        }

        FILE *fp;          // File pointer for the command
        char output[2500]; // Buffer for the command output

        fp = popen(date_command, "r"); // Open the command for reading purposes
        if (fp == NULL)                // If the command has failed to open
        {
          printf("Failed to run command.\n");
          write(new_socket, "Failed to run command on the server (Something went wrong).\n", strlen("Failed to run command on the server (Something went wrong).\n"));
        }
        else
        {
          if (fgets(output, sizeof(output), fp) != NULL) // Read the output.
          {
            write(new_socket, output, strlen(output));
          }
          else // Command has failed
          {
            printf("Incorrect request from client: %d\n", client.sin_addr.s_addr);
            write(new_socket, "INCORRECT REQUEST\n", strlen("INCORRECT REQUEST\n"));
          }
        }

        pclose(fp);                                    // Close the file pointer
        memset(date_params, 0, sizeof(date_params));   // Clear the buffer for the parameters
        memset(date_command, 0, sizeof(date_command)); // Clear the buffer for the command
      }
      else if (strcmp(message, "close") == 0) // If the command is close
      {
        printf("Client: %d shut down the server.\n", client.sin_addr.s_addr);
        char *response = "Server shutting down.\n";
        write(new_socket, response, strlen(response));
        closeServer = 1;
        break;
      }
      else if (strcmp(message, "exit") == 0) // If the command is exit
      {
        printf("Client: %d disconnected from the server.\n", client.sin_addr.s_addr);
        char *response = "Disconnected from server.\n";
        write(new_socket, response, strlen(response));
        break;
      }
      else if (strcmp(message, "help") == 0) // If the command is help
      {
        write(new_socket, usage_message, strlen(usage_message));
      }
      else if (strcmp(message, "") == 0) // If the message is empty
      {
        // Do nothing
      }
      else // Other commands are considered incorrect
      {
        char *help_message = "INCORRECT REQUEST. Type 'help' to see usage.\n";
        write(new_socket, help_message, strlen(help_message));
      }

      memset(message, 0, MSG_LEN);                               // Clean up the buffer
      write(new_socket, prompt_message, strlen(prompt_message)); // Send prompt character to client
    }

    if (closeServer) // If the client requested to close the server
    {
      break;
    }
  }

  close(socket_fd); // Close the socket file descriptor, if not closed the socket is left hanging until OS cleans up

  return 0;
}

// Return a pointer to the (shifted) trimmed string
// Taken from: https://stackoverflow.com/a/122721
size_t trimwhitespace(char *out, size_t len, const char *str)
{
  if (len == 0)
    return 0;

  const char *end;
  size_t out_size;

  // Trim leading space
  while (isspace((unsigned char)*str))
    str++;

  if (*str == 0) // All spaces?
  {
    *out = 0;
    return 1;
  }

  // Trim trailing space
  end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char)*end))
    end--;
  end++;

  // Set output size to minimum of trimmed string length and buffer size minus 1
  out_size = (end - str) < len - 1 ? (end - str) : len - 1;

  // Copy trimmed string and add null terminator
  memcpy(out, str, out_size);
  out[out_size] = 0;

  return out_size;
}

// Check if the character equals any of the valid params character part excluding the '%' part of the valid parameter
int containsValidParamChar(char *character)
{
  for (int i = 0; i < sizeof(VALID_PARAMS) / sizeof(VALID_PARAMS[0]); i++)
  {
    if (character[0] == VALID_PARAMS[i][1])
    {
      return 1;
    }
  }

  return 0;
}

/*
Write a function which takes a string as parameter and
checks if all % signs are followed by a valid parameter character.
If not all % signs are followed by a valid parameter character,
the function returns 0.
If all % signs are followed by a valid parameter character,
the function returns 1.
*/
int containsValidParams(char *str)
{
  int containsPercentSign = 0;
  // check if the given string contains a valid parameter character
  for (int i = 0; i < strlen(str); i++)
  {
    if (str[i] == '%')
    {
      containsPercentSign = 1;
      if (i + 1 < strlen(str))
      {
        if (containsValidParamChar(&str[i + 1]) == 0)
        {
          return 0;
        }
        if (str[i + 1] == '%')
        {
          i++;
        }
      }
      else
      {
        return 0;
      }
    }
  }

  return containsPercentSign;
}