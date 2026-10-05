#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/**
 * Executes the command "cat scores | grep Lakers".  In this quick-and-dirty
 * implementation the parent doesn't wait for the child to finish and
 * so the command prompt may reappear before the child terminates.
 *
 */

 static void close_pipe(int fd[2])
 {
  close(fd[0]);
  close(fd[1]);
 }

int main(int argc, char **argv)
{
  int pipe1[2];
  int pipe2[2];
  pid_t pid;

  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s <grep argument>\n", argv[0]);
    return 1;
  }

  char *cat_args[] = {"cat", "scores", NULL};
  char *grep_args[] = {"grep", argv[1], NULL};
  char *sort_args[] = {"sort", NULL};

  // make a pipe (fds go in pipefd[0] and pipefd[1])
  if (pipe(pipe1) == -1 || pipe(pipe2) == -1)
  {
    perror("pipe failed");
    return 1;
  }

  pid = fork();
  if (pid < 0)
  {
    perror("fork failed");
    return 1;
  }
  if (pid == 0)
    {


      pid_t pid2 = fork();
      if (pid2 < 0)
      {
        perror("fork failed");
        exit(1);
      }

      if (pid2 == 0)
      {
        dup2(pipe2[0], STDIN_FILENO);

        close_pipe(pipe1);
        close_pipe(pipe2);
        
        execvp("sort", sort_args);
        perror("execvp sort failed");
        exit(1);
      }

      else
      {
        dup2(pipe1[0], STDIN_FILENO);
        dup2(pipe2[1], STDOUT_FILENO);
        close_pipe(pipe1);
        close_pipe(pipe2);
        execvp("grep", grep_args);
        perror("execvp grep failed");
        exit(1);
      }
    }
  else
    {
      // parent gets here and handles "cat scores"


      dup2(pipe1[1], STDOUT_FILENO);



      close_pipe(pipe1);
      close_pipe(pipe2);



      execvp("cat", cat_args);
      perror("execvp cat failed");
      exit(1);
    }
    return 0;
}