
/**
 * @file myshell.c
 * @author Abdullah Saad
 * @date january 20 2021
 * @brief File containing the function operate for UNIX shell.
 **/


#include <stdio.h>      /* input/output*/
#include <stdlib.h>     /* General utilities */
#include <string.h>     /* string library */
#include <unistd.h>     /* Sympolic Constant */
#include <sys/stat.h>   /* data returned by the functions fstat(), lstat(), and stat(). */
#include <sys/types.h>  /* Primitive System Data Types */   
#include <sys/wait.h>   /* Wait fr Process Termination */
#include <errno.h>      /* Errors */
#include <fcntl.h>      /* functions fcntl() and open(). */
#include <signal.h>     /* Handle different signals reported during a program's execution. */
#include <stdbool.h>    /* bool as a Boolean data type */
#include <time.h>       /* for local time */
#include "myshell.h"

/* Implemneting a simple Unix shell program */


// Welcome message to print once on shell prompt 
void welcome_message(){
    fputs("**********************************  UNIX shell  **********************************\n", stdout);
    fputs("Please Enter The Command! \n", stdout);
    fputs("Type \"exit\" to exit \n", stdout);
    fputs("**********************************************************************************\n\n", stdout);
}

// predict the operating system id and user name and print the prompt sign 
void promot(){
    // user name using getenv() function
    char * username = getenv("LOGNAME");
    char host_name[1204] ;
    gethostname(host_name, sizeof(host_name));

    time_t time_now;
    time (&time_now);
    struct tm* current_time = localtime(&time_now);

    // promot statement
    // predit the operating system
    if( getuid() == 0){
        fputs("\n",stdout);
        printf("%s", asctime(current_time));
        printf("%s ", username);
        printf("@ %s ", host_name);
        fputs("> ",stdout);
    }else
    {
	    //printf("%s@%s %s > ", getenv("LOGNAME"), hostn, getcwd(currentDirectory, 1024));
        fputs("\n",stdout);
        printf("%s", asctime(current_time));
        printf("%s ", username);
        printf("@ %s ", host_name);
        fputs("$ ",stdout);
    }
}
// initital shell environment
void init_environment(char *arguments[] , char *command, char *history_FileName,char *log_FileName ,char *batch_FileName){
    //init_history(&history_FileName , &log_FileName ,&batch_FileName);
    // set all arguments to NUll 
    for (int i = 0 ; i != args_LINE ; i++ ){
        arguments[i] = NULL;
    }
    // Empty the command 
    /* strcpy() function copies the string pointed by "" (including the null character) to the Command */
    strcpy(command,"");
} 


// get input and stored in arguments
char command_input( char *command){

    // buffer hold the command 
    char buffer[Command_LINE +1];
    // get the command 
    if(fgets(buffer, Command_LINE + 1, stdin) == NULL){
        // if command is NULL print error mesage
        fprintf(stderr," Failed to read the command ! \n");
        return 0;
    }
    // copy the buffer to command 
    strcpy(command, buffer);
    return 1;
}


// Set function 1 (exit() system call) that terminate the shell
// exit() exiting the command  
void exit_function(char *argument[]){
    if ((strcmp(argument[0], "exit") == 0) || (strcmp(argument[0], "Exit") == 0)){
        free_arguments(argument);
        fputs("\n\n\n", stdout );
        fputs("myShell terminating.....\n\n", stdout );
        fputs("[Process completed]\n", stdout);
        //System calls: exit()
        exit(EXIT_SUCCESS);
    }

}

void free_arguments(char *arguments[]){
     while(*arguments) {
        free(*arguments);  // to avoid memory leaks
        *arguments++ = NULL;
    }
}



// dectect ampersand for set function 1 (4) executed in background 
bool ampersand(char **argument , int *arguments_number){
    /* The ps &  does not exit when you press enter - you just saw that it had exited when you pressed enter 
    (it actually exited way before your enter).  As I have suggested, 
    write a program that just sleeps for 10 seconds and then run it in the background.  
    You will see a different behaviour.*/
    int i =0;
    // length of the arguments 
    int length = strlen(argument[*arguments_number-1]);   
    // background 
    bool background ;
    if(strcmp(&argument[*arguments_number - 1][length - 1], "&") != 0) {
        return false;
    }
    // check the arguments 
    while(argument[i] != NULL ){

        int length = strlen(argument[i]);   
        // if the end of the arguments equal to & 
        if (strcmp(&argument[i][0], "&") == 0){
            // set the background to 1
            background = true;
            // delete the & from the arguments
            free(argument[i]);
            argument[i] = NULL;
            (*arguments_number) --;

        }else if(strcmp(&argument[i][length - 1], "&") == 0){

            argument[i][length - 1] = '\0';
            background = true;

        }
        i++;
    }
    return background;
}




// parse argument into list of arguments
int parse(char *arguments[], char *command ,bool *execting_background, char *** arguments2 , int *arguments2_num ,char **input_File , char **output_File , int *input ,int *output, FILE **fp  ){

    // variable holds the number of arguments
    int arguments_number = 0;
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
        arguments[arguments_number]=malloc(strlen(save_command) + 1);
        // copy the command without Delimiters into the arguments
        strcpy(arguments[arguments_number],save_command);
        // increase the number of command (i)
        arguments_number ++;
        save_command = strtok(NULL, DELIMITERS);
    }

    /* ------------------- set function 1 ---------------- */
    //The internal shell command "exit" which terminates the shell
    exit_function(arguments);

    *execting_background = ampersand(arguments,&arguments_number) ;

    pipe_function(arguments ,&arguments_number ,&arguments2 ,arguments2_num); 


    check_redirecting(arguments ,input_File ,output_File, input, output ,&fp);
    
    //if(strcmp(arguments[arguments_number] , "export") == 0){
        // export function
    //}


    return arguments_number;
}

void check_redirecting(char **arguments, char **input_File , char **output_File , int *input ,int *output, FILE ***fp ){

    
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

            if(input){
                **fp =freopen(*input_File, "r", stdin);
                *input = 0;

            }
            if(! *input_File){
                // if file is NULL print error mesage
                fprintf(stderr," Failed to find the file ! \n");
            }

            break;
            

        }else if (!strcmp(&arguments[i][0], ">")){      //check for output >
            output_File = &arguments[i+1];
            free(arguments[i]);
            *output = 1;
            
            
            for(int j = i; arguments[j-1] != NULL; j++) {
                arguments[j] = arguments[j+2];
            }
            
            

            
            
            break;
        }
        
        i ++ ;
    }

}


//A command, with or without arguments, whose output is piped to the input of another command.

void pipe_function(char ** arguments, int *arguments_num , char **** argument2 , int *argument_num2){

    int i = 0 ;
    while(i != *arguments_num )
    {
        
        if(strcmp(arguments[i], "|") == 0 ){
            free(arguments[i]);
            arguments[i] = NULL;

            *argument_num2 = *arguments_num - i - 1 ;
            *arguments_num = i ;
            **argument2 = arguments + i + 1;
            break;

        }
        i ++ ;
    }
}



//sigquit :  terminate the child shell by sending it a SIGQUIT signal.
void sigquit(int signo) {
    // printf("Terminating after receipt of SIGQUIT signal\n");
    fflush(stdout);
    exit(0);
}
void sigint(int signo) {
    // printf("Terminating after receipt of SIGQUIT signal\n");
    exit(0);
}



int run_command(char **arguments, char **input_File , char **output_File , int *input ,int *output, FILE *fp , char **argument2 ,int arguments_number , int arguments2_num , bool *execting_background){
    
    //Child's exit status
    int status;

    //char **arguments2 ;
    // consists of two types of signal, signal default and signal ignore.
    
    struct sigaction sigact;
    memset(&sigact, 0, sizeof(sigact));

    sigact.sa_handler = sigint;
    sigact.sa_flags = 0;
    // block signal of handled 
    sigemptyset(&sigact.sa_mask);
    sigaddset(&sigact.sa_mask, SIGINT);
    sigaddset(&sigact.sa_mask, SIGQUIT);



    if (sigaction(SIGINT, &sigact, NULL) < 0){
        perror("sigaction()");
        exit(1);
    }

    sigact.sa_handler = sigquit;
    sigemptyset(&sigact.sa_mask);
    sigaddset(&sigact.sa_mask, SIGQUIT);
    if (sigaction(SIGQUIT, &sigact, NULL) < 0){
        perror("sigaction()");
        exit(1);
    }



    // Forking a child process 
    pid_t pid ; // child's process id
    pid = fork();


    /* set function 1 Command with arguments */ 
    if(pid >= 0){
        
        /* set Function 1 */
        /* command with Arguments Fork() , waitpid(), Exit() */
        

        switch(pid)
        {
            case 0 :
                if(arguments2_num != 0 ){
                    // pipe let the shell use more command one output of one command saves as input for next
                    // create pipe 
                    int init_pipe[2];
                    pipe(init_pipe);

                    // fork the two processor 
                    pid_t pip_id2 = fork();
                
            
                
                    // child process for second command 
                    if(pip_id2 > 0) {

                        printf("pipe found \n");
                        //not ignore SIGINT
                        close(init_pipe[1]);
                        dup2(init_pipe[0], 0);
                        wait(NULL);

                        status = execvp(argument2[0],argument2);
                        close(init_pipe[0]);
                        fflush(stdin);


                        if( status < 0){

                            perror("Error in Forking child process in pip function \n");

                            exit(EXIT_FAILURE);
                        }
                    }else if (pip_id2 == 0 ) {

                        close(init_pipe[0]);
                        dup2(init_pipe[1], 0);
                        status = execvp(arguments[0],arguments);
                        close(init_pipe[1]);
                        fflush(stdin);

                        if( status < 0){

                            perror("Error in Forking grandchild process in pip function \n");

                            exit(EXIT_FAILURE);
                        }
                    }
                
                
                
                    
                }else{

                    //Zero: Returned to the newly created child process.

                    // child processer

                    // execute the command 
                
                    status = execvp(arguments[0],arguments);

                
                    if( status < 0){

                        perror("Error in Forking child process \n");

                        exit(EXIT_FAILURE);
                    }
                    
                    exit(status);
                    fflush(stdin);
                }

            default:
                // parent processor
                //Positive value: Returned to parent or caller

                if(!*execting_background ){
                
                    sigaction(SIGQUIT, &sigact, NULL);
                    /* pid holds the id of child */
                    do{
                        waitpid(pid, &status, WUNTRACED);
                    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
                    //sleep(1); /* pause for 1 secs */
                    //printf("PARENT (%d): sending SIGQUIT/kill to %d\n", getpid(), pid);
                    kill(pid,SIGQUIT);
                        
                }
        }

    }
    else
    {
        //Negative Value: creation of a child process was unsuccessful.
        perror("Failed to fork \n");
        exit(0);
        wait(NULL);
        return 0 ;
    }

    return 1 ;
}

int main(void){

    //pointer to file for ouput file
    FILE *fp;

    int input_num = 0;
    int output_num = 0;
    char *input_File ;
    char *output_File;

    int input_desc, output_desc;

    /* variables for the shell */
    char *history_FileName;
    char *log_FileName;
    char *batch_FileName;
    const char *environment[] = {"PATH", "HOME","HISTFILE"};
    char **source;
    typedef struct var {
        char *command ;
        char *value; 
    } var ;

    var variables[100];
    int last_index = -1;

    // command array
    char command[Command_LINE +1];
    
    // arguments array
    char *arguments [args_LINE +1];


    // pointer to command
    char *ptr = command;

    bool execting_background ;

    char **arguments2 ;
    int arguments2_num = 0 ;

    

    // welcome message 
    welcome_message();
    // initital shell environment
    init_environment(arguments,command, history_FileName,log_FileName ,batch_FileName);

    
    while (1)
    {
        

        //predict the operating system and print the prompt sign
        promot();

        //fflush(stdout);
        //fflush(stdin);

        // test if empty command 
        if(!command_input(command)){
            continue;
        }
        if(*ptr == '\n'){
            continue;
        }


        // parse argument into list of arguments
        int arguments_number = parse(arguments,command,&execting_background,&arguments2,&arguments2_num, &input_File ,&output_File, &input_num, &output_num ,&fp);

        if(output_num){
            printf(" hello ");

            int    fd;
            fpos_t pos;

            printf("stdout, ");
            
            //fflush(stdout);
            fgetpos(stdout, &pos);
            fd = dup(fileno(stdout));
            freopen("sad123.txt", "w", stdout);
            
    

            fflush(stdout);
            dup2(fd, fileno(stdout));
            close(fd);
            clearerr(stdout);
            fsetpos(stdout, &pos);

            /*   
            //freopen(*output_File, "w+", stdout);
            //input = 0;
            int i = open("text.txt", O_CREAT | O_TRUNC | O_WRONLY, 0600); 
			dup2(i, STDOUT_FILENO); 
			close(i);
            output_num = 0 ;
            */
        }

        run_command(arguments,&input_File,&output_File,&input_num,&output_num,fp ,arguments2 ,arguments_number,arguments2_num,&execting_background);
        

        
        free_arguments(arguments);


        //fflush(stdout);
        //fflush(stdin);
        

    }

    return 0 ;
    

}
/*

// 3 environment variables {"PATH", "HOME","HISTFILE"}
const char *environment[] = {"PATH", "HOME","HISTFILE"};



char **source;


// check if the command is environment or not 
bool is_environment( char *command){

    for ( int i = 0;  i < sizeof(environment) ; i++ ){
        if (strcmp(environment[i],command) == 0){
            return true;
        } 
    }
    return false;


}



char* find_variable(char *command_input) {
    for (int i = 0; i <= last_index; ++i) {
        if (strcmp(variables[i].command, command_input) == 0) {
            return variables[i].value;
        }
    }
    return "NOT_FOUND";
}


//• $PATH: contains the list of directories to be searched when commands are

void setsource(){
    free(source);
    source = (char **) malloc(30 * sizeof(char *));

    char * temp = find_variable("PATH");

    if (strcmp(temp, "NOT_FOUND") == 0) {
        temp = (char *) malloc(514 * sizeof(char));
        strcpy(temp, getenv("PATH"));
    }

    char *path =(char *) malloc(strlen(temp) * sizeof(char)); 
    strcpy(path, temp);

    const char DELIMITER[2] = ":";

    char *dir = strtok(path, DELIMITER);
    int ind =0;
    while (dir != NULL) {
        source[ind] = (char *) malloc(strlen(dir) + 1);
        strcpy(source[ind], dir);
        ind++;
        dir = strtok(NULL, ":");
    }
    source[ind] = NULL;

     
}





//• $HISTFILE: contains the name of the file that contains a list of all inputs to


//$HOME: contains the home directory for the user. For the purposes of this


//By default in the bash shell, the profile file is call .bash_profile and is in the user’s



void init_history(&history_FileName , &log_FileName ,&batch_FileName){
    char history[] = "/CIS3110_history" ;
    char log[] = "/CIS3110_Log";
    char *home = getenv("HOME");

    char *history_temp = malloc(strlen(home) + 15);
    strcpy(history_temp, home);
    strcat(history_temp, history);

    history_FileName = (char *) malloc(strlen(history_temp) * sizeof(char));
    strcpy(history_FileName, history_temp);

    char *log_temp = malloc(strlen(home) + 11);
    strcpy(log_temp, home);
    strcat(log_temp, log);

    log_FileName = (char *) malloc(strlen(log_temp) * sizeof(char));
    strcpy(log_FileName, log_temp);


}

void append_LogFile() {
    FILE* logFile = fopen(log_FileName, "a");
    if (logFile != NULL) {
        fprintf(logFile, "%s\n", "child process was terminated");
        closeFile(logFile);
    } else {
        printf("ERROR: cannot open log file'n");
    }
}

void append_HistoryFile(char *command) {
    FILE* historyFile = fopen(history_FileName, "a");
    if (historyFile != NULL) {
        fprintf(historyFile, "%s\n", command);
        closeFile(historyFile);
    } else {
        printf("ERROR: cannot open history file\n");
    }
}

void showHistory() {
    char command[514];
    FILE* historyFile = fopen(history_FileName, "r");
    if (historyFile == NULL) {
        printf("ERROR: cannot open history file\n");
    } else {
        while (fgets(command, 514, historyFile)) {
            printf("%s", command);
        }
        closeFile(historyFile);
    }
}

*/

