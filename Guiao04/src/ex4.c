#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
  int command_index = 1;
  char command[1024];
  bzero(command, 1024);
  setbuf(stdout, NULL);

  // redirect stdin
  if (!strcmp(argv[command_index], "-i")) {
    int input_fd = open(argv[command_index + 1], O_RDONLY);
    if (input_fd < 0)
      return -1;

    int res = dup2(input_fd, STDIN_FILENO);
    if (res < 0)
      return -1;
    close(input_fd);

    command_index += 2;
  }

  // redirect stdout
  if (!strcmp(argv[command_index], "-o")) {
    int output_fd =
        open(argv[command_index + 1], O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if (output_fd < 0)
      return -1;

    int res = dup2(output_fd, STDOUT_FILENO);
    if (res < 0)
      return -1;
    close(output_fd);

    command_index += 2;
  }

  // parse remainder commands
  for (; command_index < argc; command_index++) {
    strcat(command, argv[command_index]);
    strcat(command, " ");
  }

  system(command);

  return 0;
}
