// C program to demonstrate use of fork() and pipe() 
#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

int main() 
{ 
    // We use two pipes 
    // First pipe to send input string from parent 
    // Second pipe to send concatenated string from child 

    int fd1[2];  // Used to store two ends of first pipe 
    int fd2[2];  // Used to store two ends of second pipe 


    char fixed_str[] = "howard.edu"; //Default concatenated string
    char input_str[100];             //User input
    char another_usr_str[100];      //2nd user input            
    pid_t p; 

    if (pipe(fd1)==-1) 
    { 
        fprintf(stderr, "Pipe #1 Failed" ); 
        return 1; 
    } 
    if (pipe(fd2)==-1) 
    { 
        fprintf(stderr, "Pipe #2 Failed" ); 
        return 1; 
    } 
    printf("Enter the first string to concatenate: ");
    scanf("%s", input_str); 
    p = fork(); 
    //Child Process failed
    if (p < 0) 
    { 
        fprintf(stderr, "fork Failed" ); 
        return 1; 
    } 

    // Parent process 
    else if (p > 0) 
    { 
        // Close reading end of pipe#1 
       // close(fd1[0]); 
        // Close writing end of pipe#2 
        close(fd2[1]); 

        // Write input string and close writing end of first 
        // pipe. 
        write(fd1[1], input_str, strlen(input_str)+1); 

        // Close writing end of pipe#1
        close(fd1[1]); 

        // Wait for child to send a string 
        wait(NULL); 

        // Read string from child to retrieve the newly concatenated string from pipe#1
        char new_concat_str[100];
        read(fd1[0], new_concat_str, sizeof(new_concat_str));
        
        //Close reading end of pipe#1
        close(fd1[0]);
        

        //Read/Retrieve the user's newly entered string to put concatenated with the first concatenated
        //string from pipe#2
        char recent_input_str[100];
        read(fd2[0], recent_input_str, sizeof(recent_input_str));
        
        // Close reading end of pipe#2
        close(fd2[0]); 

        // Concatenate the second user input string with it 
        int k = strlen(new_concat_str); 
        int i; 
        for (i=0; i<strlen(recent_input_str); i++) 
            new_concat_str[k++] = recent_input_str[i];

        //Add Null value at the end of the string
        new_concat_str[k] = '\0';


        //Print first concatenated string
        printf("The final concatenated string is %s\n", new_concat_str);


    } 

    // child process 
    else
    { 
        // Close reading end of pipe#2
        close(fd2[0]); 

        // Read a string using first pipe 
        char concat_str[100]; 
        read(fd1[0], concat_str, sizeof(concat_str)); 

        // Concatenate a fixed string with it 
        int k = strlen(concat_str); 
        int i; 
        for (i=0; i<strlen(fixed_str); i++) 
            concat_str[k++] = fixed_str[i]; 

        //Add null terminator value
        concat_str[k] = '\0';

        //Print first concatenated string
        printf("The first concatenated string is %s\n", concat_str);

        //Write concatenated to second pipe
        write(fd1[1], concat_str, strlen(concat_str)+1);
        
        // Close writing end of pipe#1
        close(fd1[1]); 

        //Prompt user to input another string to concatenate 
        printf("Enter another string to concatenate: ");
        scanf("%s", another_usr_str);  

        //Write concatenated to second pipe
        write(fd2[1], another_usr_str, strlen(another_usr_str)+1);

        // Close the reading end of pipe#1
        close(fd1[0]); 
        
        // Close the writing end of pipe#2
        close(fd2[1]); 


        exit(0); 
    } 
} 