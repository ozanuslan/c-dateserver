#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for strlen
#include <sys/socket.h>
#include <arpa/inet.h> // for inet_addr
#include <unistd.h>    // for write

char *trimwhitespace(char *str);

char *GET_DATE = "GET_DATE";

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
  setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int));

  uint16_t port = 5555;

  // Prepare the sockaddr_in structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);

  // Bind
  if (bind(socket_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
  {
    printf("Bind failed on port: %d\n", port);
    return 1;
  }
  printf("Socket binded on: %d\n", port);

  // Listen
  listen(socket_fd, 3);
  puts("Waiting for incoming connections...");

  char *welcome_message = "\nWelcome to the date server!!\n";
  char *usage_message = "Usage:\n\tGET_DATE - invoke 'date' command on the server\n\thelp - show usage message\n\tclose - shut down the server\n\texit - disconnect from the server\n";
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
      char *message = client_message;
      message[strcspn(message, "\r\n")] = 0; // Remove newline character

      char command[strlen(GET_DATE) + 1];              // +1 for null terminator
      strncpy(command, &message[0], strlen(GET_DATE)); // Copy command to buffer
      command[strlen(GET_DATE)] = '\0';                // Null terminate string

      if (strcmp(command, GET_DATE) == 0) // If the command is GET_DATE
      {
        char date_params[200];                  // Buffer for date command parameters
        if (strlen(message) > strlen(GET_DATE)) // If there are parameters
        {
          strncpy(date_params, &message[strlen(GET_DATE) + 1], strlen(message) - strlen(GET_DATE));
          date_params[strlen(message) - strlen(GET_DATE)] = '\0';
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
        char *help_message = "Incorrect input. Type 'help' to see usage.\n";
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