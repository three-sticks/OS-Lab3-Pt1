// C program to demonstrate use of fork() and pipe() 
#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 
#define BUF_SIZE 200
int main() 
{ 
    // We use two pipes 
    // First pipe to send input string from parent 
    // Second pipe to send concatenated string from child 
  
    int fd1[2];  // Used to store two ends of first pipe 
    int fd2[2];  // Used to store two ends of second pipe 
  
    char fixed_str[] = "howard.edu";
    char fixed_str2[] = "gobison.org";
    char input_str[BUF_SIZE]; 
    pid_t p; 
  
    if (pipe(fd1)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
    if (pipe(fd2)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
  
    printf("Enter a string to concatenate:");
    fflush(stdout);
    scanf("%s", input_str);
    fflush(stdout); 
    p = fork(); 
  
    if (p < 0) 
    { 
        fprintf(stderr, "fork Failed" ); 
        return 1; 
    } 
  
    // Parent process 
    else if (p > 0) 
    { 
  
        close(fd1[0]);  // Close reading end of pipes 
        close(fd2[0]);
  
        // Write input string and close writing end of first 
        // pipe.
        if (write(fd1[1], input_str, strlen(input_str) + 1) == -1)
        {
            fprintf(stderr, "write Failed");
            return 1;
        }   
  
        // Wait for child to print the concatenated string 
        wait(NULL); 
  
        char final_str[BUF_SIZE];
        ssize_t n = read(fd2[0], final_str, sizeof(final_str) - 1); // Close writing end of pipes 
        if (n == -1)
        {
          fprintf(stderr, "read Failed");
        }
        final_str[n] = '\0';
        close(fd2[0]);
        
        strcat(final_str, fixed_str2);
        printf("Final concatenated string %s\n", final_str);
    } 
  
    // child process 
    else
    { 
        close(fd1[1]);  // Close writing end of first pipes 
        close(fd2[0]); 
      
        // Read a string using first pipe 
        char concat_str[BUF_SIZE]; 
        ssize_t n = read(fd1[0], concat_str, sizeof(concat_str) - 1);
        if (n == -1)
        {
          fprintf(stderr, "read Failed");
          exit(1);
        } 
        concat_str[n] = '\0';
        close(fd1[0]);
  
        // Concatenate a fixed string with it 
        strcat(concat_str, fixed_str);

        printf("Concatenated string %s\n", concat_str);
        // Close both reading ends
        char second_str[100];
        printf("Enter a second string");
        fflush(stdout); 
        scanf("%s", second_str); 
        strcat(concat_str, second_str);
        
        if (write(fd2[1], concat_str, strlen(concat_str) + 1) == -1)
        {
            fprintf(stderr, "write Failed");
            exit(1);
        }
        close(fd2[1]);
  
        exit(0); 
    } 
    return 0;
} 