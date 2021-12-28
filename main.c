#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for strlen
#include <sys/socket.h>
#include <arpa/inet.h> // for inet_addr
#include <unistd.h>    // for write

int main(int argc, char *argv[])
{
  int socket_fd, new_socket, c, read_size;
  int MSG_LEN = 2000;
  char client_message[MSG_LEN];
  struct sockaddr_in server, client;

  // Create socket
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  int true = 1;
  setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int));
  if (socket_fd == -1)
  {
    puts("Could not create socket");
    return 1;
  }

  uint16_t port = 8888;

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

  c = sizeof(struct sockaddr_in);
  int closeServer = 0;

  char *welcome_message = "\nWelcome to the date server!!\n";
  char *usage_message = "Usage:\n\tGET_DATE - invoke 'date' command on the server\n\thelp - show usage message\n\tclose - shut down the server\n\texit - disconnect from the server\n";
  char *prompt_message = "▶ ";
  // Start waiting for connections
  while (new_socket = accept(socket_fd, (struct sockaddr *)&client, (socklen_t *)&c))
  {
    printf("Connection accepted from client: %d\n", client.sin_addr.s_addr);
    write(new_socket, welcome_message, strlen(welcome_message));
    write(new_socket, usage_message, strlen(usage_message));
    write(new_socket, prompt_message, strlen(prompt_message));
    //Receive a message from client
    while ((read_size = recv(new_socket, client_message, 2000, 0)) > 0)
    {
      char *message = client_message;
      message[strcspn(message, "\r\n")] = 0;

      if (strcmp(message, "GET_DATE") == 0)
      {
      }
      else if (strcmp(message, "close") == 0)
      {
        printf("Client: %d shut down the server.\n", client.sin_addr.s_addr);
        char *response = "Server shutting down.\n";
        write(new_socket, response, strlen(response));
        closeServer = 1;
        break;
      }
      else if (strcmp(message, "exit") == 0)
      {
        printf("Client: %d disconnected from the server.\n", client.sin_addr.s_addr);
        char *response = "Disconnected from server.\n";
        write(new_socket, response, strlen(response));
        break;
      }
      else if (strcmp(message, "help") == 0)
      {
        write(new_socket, usage_message, strlen(usage_message));
      }
      else if (strcmp(message, "") == 0)
      {
        // Do nothing
      }
      else
      {
        char *help_message = "Incorrect input. Type 'help' to see usage.\n";
        write(new_socket, help_message, strlen(help_message));
      }

      memset(message, 0, MSG_LEN);
      write(new_socket, prompt_message, strlen(prompt_message));
    }
    if (read_size == 0)
    {
      printf("Client: %d disconnected\n", client.sin_addr.s_addr);
      fflush(stdout);
    }
    else if (read_size == -1)
    {
      perror("recv failed");
    }

    if (closeServer)
    {
      break;
    }
  }

  close(socket_fd);

  return 0;
}
