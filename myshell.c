
/**
 * @file myshell.c
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
#include <signal.h>
#include "myshell.h"


// Welcome message
void welcome_message(){
    fputs("**********************************  UNIX shell **********************************\n ", stdout);
    fputs("Please Enter The Command! \n", stdout);
    fputs("Type \"exit\" to exit \n ", stdout);
    fputs("*********************************************************************************\n\n", stdout);
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
    if(fgets(buffer, Command_LINE + 1, stdin) == NULL){
        fprintf(stderr," Failed to read the command ! \n");
        return 0;
    }
    strcpy(command, buffer);
    return 1;

}



// parse argument into list of arguments
int parse(char *arguments[], char *command){

    // variable holds the number of arguments
    int i = 0;
    // buffer for hold the command 
    char buffer[Command_LINE +1];
    // copy the command 
    strcpy(buffer,command);

   
    // breaks the string of the Delimiters
    char *save_command = strtok(buffer,DELIMITERS);
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
        save_command = strtok(NULL, DELIMITERS);
    }
    return i ;
}

void exit_function(char *argument[]){
    if ((strcmp(argument[0], "exit") == 0) || (strcmp(argument[0], "Exit") == 0)){
        fputs("myShell terminating.....\n", stdout );
        fputs("[Process completed]\n", stdout);
        //System calls: exit()
        exit(EXIT_SUCCESS);
    }

}

/*
 * Handle exit signals from child processes
 */
void sig_handler(int signal) {
  int status = wait(&status);
}


void free_arguments(char *arguments[]){
     while(*arguments) {
        free(*arguments);  // to avoid memory leaks
        *arguments++ = NULL;
    }
}

// predict the operating system and print the prompt sign 
void promot(){
     // promot statement
    if( getuid() == 0){
        printf("%s ",">"); 
    }else
    {
        printf("%s ","$");
    }
}


int ampersand(char **argument){
  int i =0;
  // background 
  int background=0;
  // check the arguments 
  while(argument[i] != NULL ){
    // if the end of the arguments equal to & 
    if (!strcmp(argument[i], "&")){
      // set the background to 1
      background = 1;
      // delete the & from the arguments 
      argument[i] = NULL;
    }
    i++;
  }
  return background;
}

void check_redirecting(char **arguments, char **input_File , char **output_File , int *input ,int *output, FILE *fp){

    
    int i = 0;
    *input = 0;
    *output = 0;


    while(arguments[i] != NULL){

        if (!strcmp(&arguments[i][0], "<")){           //check for input <
            input_File = &arguments[i+1];
            free(arguments[i]);
            *input = 1;

            for(int j = i; arguments[j-1] != NULL; j++) {
                arguments[j] = arguments[j+2];
            }

            //freopen(*input_File, "r", stdin);

            break;
            

        }else if (!strcmp(&arguments[i][0], ">")){      //check for output >
            output_File = &arguments[i+1];
            arguments[i] = NULL;
            *output = 1;
            
            
            for(int j = i; arguments[j-1] != NULL; j++) {
                arguments[j] = arguments[j+2];
            }
            freopen(*output_File, "w+", stdout);
            break;
        }
        
        i ++ ;
    }

}





int run(char **arguments, char **input_File , char **output_File , int *input ,int *output, FILE *fp){
    
    //Child's exit status
    int status;

    // execting in the background when (&)
    int exe_background = ampersand(arguments);
    // Forking a child process 
    pid_t childpid ; // child's process id
    childpid = fork();
    // if childpid bigger than 0 -> the for succeeded
    
    // if there was an input redirection (<) 
    
    

    /*
        --------------- set functions 1 -------------------
        A command with no arguments.
        • System calls: fork(), execvp(), exit(), wait(), waitpid()
    */
    if(childpid >= 0){

        if(childpid == 0){

            
            if(input){
                freopen(*input_File, "r", stdin);
            }
                      
            status = execvp(arguments[0],arguments);

            
            
            if( status < 0){

                perror("Error in Forking child process \n");

                exit(EXIT_FAILURE);
            }


            exit(status);
        }else{

            if(!exe_background){
                do{
                    waitpid(childpid, &status, WUNTRACED);
                } while (!WIFEXITED(status) && !WIFSIGNALED(status));
            }
            
        }
    }
    else
    {
        perror("fork");
        exit(-1);
    }
    
    
    
    
    return 1 ;
}

int main(void){

    // command array
    char command[Command_LINE +1];
    
    // arguments array
    char *arguments [args_LINE +1];

    
    // welcome message 
    welcome_message();
    // Empty the command 
    empty_command(command);

    // pointer to command
    char *ptr = command;

    //pointer to file for ouput file
    FILE *fp;

    int input_num;
    int output_num;
    char *input_File ;
    char *output_File;             

    // Set up the signal handler
    sigset(SIGCHLD, sig_handler);
    
    while (1)
    {
        // ignore empty commad 
        empty_command(command);

        initialize_arguments(arguments);

        // predict the operating system and print the prompt sign
        promot();

        
        if(!input(command)){
            continue;
        }

        if(*ptr == '\n'){
            continue;
        }

        // parse argument into list of arguments
        int arguments_number = parse(arguments,command);

        /* ------------------- set function 1 ---------------- */
        //The internal shell command "exit" which terminates the shell
        exit_function(arguments);

        ampersand(arguments);

        check_redirecting(arguments ,&input_File ,&output_File, &input_num, &output_num , fp);

        run(arguments,&input_File,&output_File,&input_num,&output_num,fp);
        

        free_arguments(arguments);

        fflush(stdout);
        fflush(stdin);
        

        

    }

    return 0 ;
    

}