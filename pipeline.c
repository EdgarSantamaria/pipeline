#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
  // file descriptors for pipe
  int pipefd[2];
  // child process id
  pid_t pid1, pid2;

  // Create the pipe
  if (pipe(pipefd) == -1) {
    perror("pipe failed");
    return 1;
  }

  /*Fork child process*/
  pid1 = fork();
  if (pid1 == -1) {  /*error occured*/
    fprintf(stderr, "fork failed"); 
    return 1;
  }

  if (pid1 == 0) {  // First child process
    // Redirect to the writing end of the pipe
    dup2(pipefd[1], STDOUT_FILENO);
    // Close the reading end of the pipe
    close(pipefd[0]);
    // Execute the ls -F command
    execlp("ls", "ls", "-F", NULL);
    // If execlp returns, it means there was an error
    fprintf(stderr, "execlp failed");
    return 1;
  }

  pid2 = fork();  // Second child process
  if (pid2 == -1) {
    fprintf(stderr, "fork failed"); 
    return 1;
  }

  if (pid2 == 0) {
    dup2(pipefd[0], STDIN_FILENO);
    close(pipefd[1]);
    execlp("nl", "nl", NULL);
    fprintf(stderr, "execlp failed");
    return 1;
  }

  // Close both ends of the pipe
  close(pipefd[0]);
  close(pipefd[1]);

  // Wait for both child processes to finish
  waitpid(pid1, NULL, 0);
  waitpid(pid2, NULL, 0);

  return 0;
}
