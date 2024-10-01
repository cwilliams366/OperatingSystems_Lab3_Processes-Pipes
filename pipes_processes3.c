#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h> 

int main(int argc, char **argv)
{
  //First pipeline Parent process for cat to the Child process for grep
  int pipefd[2];
  //Second pipeline Child process for grep to the Child process for sort
  int pipefd2[2];
  //Fork Parent-Child-Child processing 
  int pid, pid2;

  // Command arguments for each process
  char *cat_args[] = {"cat", "scores", NULL};
  char *grep_args[] = {"grep", argv[1], NULL};  
  char *sort_args[] = {"sort", NULL};

  // Create the first pipeline
  if(pipe(pipefd) == -1)
  {
    perror("Error creating the first pipeline");
    return 1;
  }

  // Create the second pipeline
  if(pipe(pipefd2) == -1)
  {
    perror("Error creating the second pipeline");
    return 1;
  }

  // Initiate the first Child prorcess
  pid = fork();
  
  if (pid == -1)
  {
    perror("Failed to initiate the first Child process for grep");
    return 1;
  }

  if (pid == 0)
  {
      // The first child process that handles the "grep" command

      // Initiate the second Child process
      pid2 = fork();
    
      if (pid2 == -1)
      {
          perror("Fork failed for the second Child processs for sort");
          return 1;
      }

    //The second child process
      if (pid2 == 0)
      {
          // The second child process that handles the "sort" command

          // Replace standard input with the read end of the second pipe
          if (dup2(pipefd2[0], 0) == -1)
          {
              perror("dup2 error within the second child process with the reading pipe#2");
              return 1;
          }

          // Close both reading and writing pipelines
          close(pipefd[0]);
          close(pipefd[1]);
          close(pipefd2[0]);
          close(pipefd2[1]);

          // Execute sort
          if (execvp("sort", sort_args) == -1)
          {
              perror("execvp error has occurred with the sort command");
              return 1;
          }
      }

      // Back to child process 1 (grep)

      // Replace standard input with the read end of the first pipe
      if (dup2(pipefd[0], 0) == -1)
      {
          perror("dup2 error within the first child process with the reading pipe#1");
          return 1;
      }

      // Replace standard output with the write end of the second pipe
      if (dup2(pipefd2[1], 1) == -1)
      {
          perror("dup2 error within the first child process with the writing pipe#2");
          return 1;
      }

    // Close both reading and writing pipelines
      close(pipefd[0]);
      close(pipefd[1]);
      close(pipefd2[0]);
      close(pipefd2[1]);

      // Execute grep
      if (execvp("grep", grep_args) == -1)
      {
          perror("execvp error has occurred with the grep command");
          return 1;
      }
  }
  else
  {
      // The parent process that handles  the "cat" command

      // Replace standard output with the write end of the first pipe
      if (dup2(pipefd[1], 1) == -1)
      {
          perror("dup2 error within the parent process with the writing pipe#1");
          return 1;
      }

    // Close both reading and writing pipelines just in case
      close(pipefd[0]);
      close(pipefd[1]);
      close(pipefd2[0]);
      close(pipefd2[1]);

      // Execute cat
      if (execvp("cat", cat_args) == -1)
      {
          perror("execvp error has occurred with the cat command");
          return 1;
      }
  }

  // Wait for first child process (grep)
  wait(NULL);
  // Wait for second child process (sort)
  wait(NULL);  
  return 0;
}
