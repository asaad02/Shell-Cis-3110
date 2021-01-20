
/**
 * @file shell.c
 * @author Abdullah Saad
 * @date january 20 2021
 * @brief File containing the function operate for shell.
 **/


#include <stdio.h>      /* input/output*/
#include <stdlib.h>     /* General utilities */
#include <string.h>     /* string library */
#include <unistd.h>     /* Sympolic Constant */
#include <sys/stat.h>     
#include <sys/types.h>  /* Primitive System Data Types */   
#include <sys/wait.h>   /* Wait fr Process Termination */
#include <errno.h>      /* Errors */
#include <fcntl.h>      /* ?? */
#include "myshell.h"

// Welcome message
void welcome_message(){
    fputs("-----------------------UNIX shell progrom---------------------------\n ", stdout);
    fputs("Please enter the Command! \n", stdout);
    fputs("Type \"exit\" to exit \n ", stdout);
    fputs("-------------------------------------------------------------------- \n", stdout);
}
// initialize arguments to NULL 
void initialize_arguments(char *arguments[]){
    for (int i = 0 ; i != args_LINE ; i++ ){
        arguments[i] = NULL;
    }
}

// Empty the command 
void empty_command(char *command){
    /* strcpy() function copies the string pointed by "" (including the null character) to the Command */
    strcpy(command,"");
}

// get input 
char input( char *command){

    // buffer hold the command 
    char buffer[Command_LINE +1];

    // get the command 
    if(fgets(buffer, Command_LINE , stdin) == NULL){
        fprintf(stderr," Failed to read the command ! \n");
        return 0;
    }

    /*
    // The null pointer constant NULL's value equals 0
    if(strncmp(buffer,"!!",2)==0 ){
        if(strlen(command) == 0){
            fprintf(stderr," No history avaliable ! \n");
            return 0;
        }
        printf("%s",command);
        return 1;
    }
    */

    strcpy(command, buffer);
    return 1;

}



// parse argument into list of arguments
int parse(char *arguments[], char *command){

    // variable holds the number of arguments
    int i ;
    char temp_command[Command_LINE +1];
    // copy the command 
    strcpy(temp_command,command);
    // breaks the string of the Delimiters
    char *save_command = strtok(temp_command,DELIMITERS);
    // saving the command in arguments 

    while(save_command !=NULL ){
        // test if has new line 
        if (*save_command == '\n'){
            break;
        }
        // malloc the arguments
        arguments[i]=malloc(strlen(save_command) + 1);
        // copy the command without Delimiters into the arguments
        strcpy(arguments[i],save_command);
        // increase the number of command (i)
        i ++;
    }
    return i ;
}

void exit_function(char *command){
    if ((strcmp(&command[0], "exit") == 0) || (strcmp(&command[0], "Exit") == 0)){
        //System calls: exit()
        fputs("myShell terminating.....", stdout );
        fputs("[Process completed] ", stdout);
        exit(0);
    }

}

    /*
        A command with no arguments.
        • Example: ls
        • Details: Your shell must block until the command completes and, if the return code is
        abnormal, print out a message to that effect.
        • Concepts: Forking a child process, waiting for it to complete, synchronous execution
        • System calls: fork(), execvp(), exit(), wait(), waitpid()
    */
int run(char **arguments){
    
    // Forking a child process 
    pid_t childpid ; // child's process id
    childpid = fork();
    // if childpid bigger than 0 -> the for succeeded
    if(childpid >= 0){
        // Child process 
        if (childpid == 0)
        {
            execvp(arguments[0], arguments);
            signal(SIGINT, SIG_IGN);
        }
        
        


    }
    else
    {
        perror("fork");
        exit(-1);
    }
    
    return 1 ;
}
/*
 * Function: main function   
 * 
 * returns: success or not
 */
int main(void){

    // command array
    char command[Command_LINE +1];
    // pointer to command
    //char *ptr = command;
    // arguments array
    char *arguments [args_LINE +1];
    
    // welcome message 
    welcome_message();
    // initialize arguments to NULL 
    initialize_arguments(arguments);
    // Empty the command 
    empty_command(command);
    
    while (1)
    {
        // promot statement
        fputs(">",stdout);

        // ignore empty commad 
        //if(*ptr == '\n'){
            //continue;
        //}

        if(!input(command)){
            continue;
        }

        // parse argument into list of arguments
        int arguments_number = parse(arguments,command);


        // Set 1 Functions
        //The internal shell command "exit" which terminates the shell
        exit_function(command);

        /*
        A command with no arguments.
        • Example: ls
        • Details: Your shell must block until the command completes and, if the return code is
        abnormal, print out a message to that effect.
        • Concepts: Forking a child process, waiting for it to complete, synchronous execution
        • System calls: fork(), execvp(), exit(), wait(), waitpid()
        */


        run(arguments);



    }
    

}