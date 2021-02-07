
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
#include <ctype.h>
#include "myshell.h"


/* Implemneting a simple Unix shell program */


// Welcome message to print once on shell prompt 
void welcome_message(){
    fputs("\n**********************************  UNIX shell  **********************************\n", stdout);
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
    char currentDirectory[100];
    // promot statement
    // predit the operating system
	//printf("%s@%s %s > ", getenv("LOGNAME"), hostn, getcwd(currentDirectory, 1024));
    fputs("\n",stdout);
    printf("%s", asctime(current_time));
    printf("%s", username);
    printf("@%s ", host_name);
    printf(":%s ", getcwd(currentDirectory, 1024));
    fputs("> ",stdout);
}

// initital shell environment
void init_environment(char *arguments[] , char *command , char **history_FileName){

    // set all arguments to NUll 
    for (int i = 0 ; i != args_LINE ; i++ ){
        arguments[i] = NULL;
    }
    
    // Empty the command 
    /* strcpy() function copies the string pointed by "" (including the null character) to the Command */
    strcpy(command,"");
    char history[] = "/CIS3110_history" ;
    char *home = getenv("HOME");
    *history_FileName = (char *) malloc(strlen(home) * sizeof(char) +strlen(history) * sizeof(char) + 1);
    strcpy(*history_FileName, home);
    strcat(*history_FileName, history);
    //free(*history_FileName);
    
} 

// check if the command is environment or not 
bool is_environment( char *command ){
    for ( int i = 0;  i < 3 ; i++ ){
        if (strcmp(environment[i],command) == 0){
            return true;
        } 
    }
    return false;
}

char *find_environment_variable(char *command_input) {
    for (int i = 0; i <= lastIndex ; ++i) {
        if (strcmp(variables[i].command, command_input) == 0) {
            
            return variables[i].value;
        }
    }
    return getenv("HOME");;
}


void change_directory(const char* path, int arguments_number) {
    // check how many arguments 
    // if more than two print { to many arguments}
    if (arguments_number > 2) {
        printf("ERROR: too many arguments for cd\n");
        return ;
    }else{
        // home directory id
        int directory_id;
        // get the value of the "HOME"
        char *home_temp = find_environment_variable("HOME");
        // get home path 
        char *home;
        // if not found the home 
        if (strcmp(home_temp, "NOT_FOUND") == 0) {
            // searched for environment name 
            home = getenv("HOME");
        } else {
            // find home and stored in home 
            home = (char*) malloc(strlen(home_temp) * sizeof(char));
            strcpy(home, home_temp);
        }
        // if (~) specifying your home directory.
        if (arguments_number == 1 || strcmp(path, "~") == 0) {
            // change the current working directory
            directory_id = chdir(home);
            // change the directory 
        } else if (path[0] == '~') {
            char *temp = (char *) malloc(strlen(path) + strlen(home));
            int j, k;
            for (j = 0; j < strlen(home); j++) {
                // append home to temp
                temp[j] = home[j];
                
            }
            for (k = 1; k < strlen(path); j++, k++) {
                // append path to home 
                temp[j] = path[k];
            }
            free(home);
            temp[j] = '\0';
            // change working directory 
            directory_id = chdir(temp);
            // if directory id returned an error 
            if (directory_id != 0) {
                free(temp);
                temp = (char *) malloc(strlen(path) + 6);
                int j = 6, k;
                // direct to home 
                strcpy(temp, "/home/");
                // append path 
                for (k = 1; k < strlen(path); j++, k++) {
                    temp[j] = path[k];
                }
                temp[j] = '\0';
                // change current working directory 
                directory_id = chdir(temp);
                free(temp);
            }
        } else {
            // change working directory to path 
            directory_id = chdir(path);
        }
        //  -1 is returned on an error and errno is set appropriately.
        if (directory_id != 0) {
            printf("ERROR: cannot change directory\n");
        }
        
    }
    
}

void append_HistoryFile(char *command ,char *history_FileName,int **history_id,char **history_array) {
    FILE* historyFile = fopen(history_FileName, "a");
    //static int i = 1 ;
    if (historyFile != NULL) {
        int  i = **history_id ;
        i ++;
        **history_id = i ;
        fprintf(historyFile, " %d  %s",i,command);
        fclose(historyFile);
        //*(history_id++);
        char * line_copy = (char *)malloc(Command_LINE * sizeof(char));
        strcpy(line_copy, command);
        //strcpy(history_array[i], line_copy);
        //strcpy(history_array[i], command);
        history_array[i] = line_copy; 
        //free(line_copy);
    } else {
        printf("ERROR: cannot open history file in append\n");
    }
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
    if(strcmp(buffer,"\n") ==0){
        return 0 ;
    }
    //char history_input[100];
    //strcpy(history_input[i++], buffer);
    // copy the buffer to command 
    strcpy(command, buffer);
    return 1;
}

// Set function 1 (exit() system call) that terminate the shell
// exit() exiting the command  
void exit_function(char *argument[],char *history_FileName,char *history_array[300]){
    if ((strcmp(argument[0], "exit") == 0) || (strcmp(argument[0], "Exit") == 0)){
        free_arguments(argument);
        fputs("\n\n\n", stdout );
        fputs("myShell terminating.....\n\n", stdout );
        fputs("[Process completed]\n", stdout);
        fputs("\n", stdout );
        clearHistory(history_FileName);
        free(history_FileName);
        free_history_arguments(history_array);
        //jobsList = NULL;
        //free(jobsList);
        //t_job* job = NULL;
        //System calls: exit()
        exit(EXIT_SUCCESS);
    }

}

void free_arguments(char *arguments[]){
    while(*arguments) {
        free(*arguments);  
        *arguments++ = NULL;
    }
}

void free_history_arguments(char *history_array[]){
    while(*history_array) {
        free(*history_array);  
        *history_array++ = NULL;
    }

}

// dectect ampersand for set function 1 (4) executed in background 
bool ampersand(char **argument , int *arguments_number){
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



void check_redirecting(char **arguments, char **input_File , char **output_File , int *input ,int *output, FILE ***fp,int arguments_numbe){
    int i = 0;
    *input = 0;
    *output = 0;
    
    while(arguments[i] != NULL){
        if (!strcmp(arguments[i], "<")){           
            *input_File = arguments[i+1];
            free(arguments[i]);
            *input = 1;
            for(int j = i; arguments[j-1] != NULL; j++) {
                arguments[j] = arguments[j+2];
            }

            break;
            
        } 
        
        i ++ ;
    }
    
    i =0 ;
    
    while(arguments[i] != NULL){
        
        if (!strcmp(arguments[i], ">")){      
                *output_File = arguments[i+1];
                free(arguments[i]);
                *output = 1;
                
                
                for(int j = i; arguments[j-1] != NULL; j++) {
                    arguments[j] = arguments[j+2];
                }
                
                        
                break;
        }
        i++;
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
            
            /*
            for(int j = i; arguments[j-1] != NULL; j++) {
                arguments[j] = arguments[j+2];
            }
            */
            break;
        }
        i ++ ;
    }
}

// parse argument into list of arguments
int parse(char *arguments[], char *command ,bool *execting_background, char *** arguments2 , int *arguments2_num ,char **input_File , char **output_File , int *input ,int *output, FILE **fp ,char *history_FileName ,int *history_id ,char **history_array){

    // variable holds the number of arguments
    int arguments_number = 0;
    // buffer for hold the command 
    char buffer[Command_LINE +1];
    char buffer_history[Command_LINE +1];
    // copy the command 
    strcpy(buffer,command);
    strcpy(buffer_history,command);
    // breaks the string of the Delimiters
    char *save_command = strtok(buffer,DELIMITERS);
    // breaks the string of the Delimiters

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
    //save command to history
    append_HistoryFile(buffer_history ,history_FileName ,&history_id ,history_array); 
    /* ------------------- set function 1 ---------------- */
    //The internal shell command "exit" which terminates the shell
    exit_function(arguments,history_FileName,history_array);
    // apersand function
    *execting_background = ampersand(arguments,&arguments_number) ;
    // pipe function
    pipe_function(arguments ,&arguments_number ,&arguments2 ,arguments2_num); 
    // directing function
    check_redirecting(arguments ,input_File ,output_File, input, output ,&fp ,arguments_number);

    //if(strcmp(arguments[arguments_number] , "export") == 0){
        // export function
    //}
    return arguments_number;
}

//sigquit :  terminate the child shell by sending it a SIGQUIT signal.
void sigquit(int signo) {
    printf("Terminating after receipt of SIGQUIT signal\n");
    fflush(stdout);
    exit(0);
}

void sigint(int signo) {
    printf("Terminating after receipt of SIGQUIT signal\n");
    exit(0);
}


void cis3110_profile_input(char *arguments[] , char *command,bool execting_background,char **arguments2,int arguments2_num ,int input_desc, int output_desc,FILE *fp ,int input_num,int output_num ,char *input_File , char *output_File, char *history_FileName,int *history_id , char **history_array ){

    FILE * bash_profile = fopen("cis3110_profile", "r");
    if (bash_profile == NULL) {
        printf("%s\n", "ERROR : batch file does not exist or cannot be opened");
        return;
    }
    char buffer1[300];
    // buffer hold the command 
    //char buffer[Command_LINE +1][args_LINE+1];
    // get the command
    char *command_file[50] ;
    
    printf("******************************* \n");
    printf("Shell environment variables \n");
    printf("*******************************\n");
    
    while(fgets(buffer1, sizeof(buffer1)+1, bash_profile) != NULL){
        
        // if command is NULL print error mesage
        //printf("%s\n",buffer[i++]);

        int i =0 ;
        


        //char delim[3] = {" ","\n","="};
        #define delim  "=" " "
        
        char *save_line = strtok(buffer1, delim);

        while(save_line !=NULL ){
            // test if has new line 
            if (*save_line == '\n'){
                //break;
            }
            // malloc the arguments
            command_file[i]=malloc(strlen(save_line) + 1);

            strcpy(command_file[i],save_line);
            // increase the number of command (i)
            i ++;

            save_line = strtok(NULL, delim);
        }
  
        // copy the buffer to command 
        // parse argument into list of arguments      
        if(strcmp(command_file[0],"export") == 0){
            
            if(strcmp(command_file[1],"PATH") ==0){
                //printf(" \n im path \n");                    
                Variable variable;
                variable.command = command_file[1];
                variable.value = command_file[2];
                lastIndex ++ ;
                variables[lastIndex] = variable;


            }else {
                //printf(" \n im HOME \n"); 
                Variable variable;
                variable.command = command_file[1];
                variable.value = command_file[2];
                lastIndex ++ ;
                variables[lastIndex] = variable;

            }
            
            
        } 
              
        free(command_file[0]);
        free(command_file[1]);
        free(command_file[2]);

        //strcpy(command,"");
        //free_arguments(arguments);
        fflush(stdout);
        fflush(stdin);
    }

    

            

    
    
    fclose(bash_profile);
    
}

void prints_specific_history(char **arguments, char **history_array ,int *history_id ){

    int stop_loop = *history_id ;
    int begin_loop = atoi(arguments[1]);
    int j ;
    if( stop_loop  < begin_loop){
        j = 1 ;
    }else {
        j = stop_loop -begin_loop ;
    }
    for(int i = j; i < stop_loop ; i ++){
        printf(" %d  %s",i,history_array[i]);
    }
}

void showHistory(char *history_FileName) {
    char command[514];
    FILE* historyFile = fopen(history_FileName, "r");
    if (historyFile == NULL) {
        printf("ERROR: cannot open history file in show \n");
    }else{
        while (fgets(command, 514, historyFile)) {
            printf("%s", command);
        }
        fclose(historyFile);
    }
}

void clearHistory(char *history_FileName){

    fclose(fopen(history_FileName, "w"));
    //free(history_FileName);
}

int test_history_input(char ** arguments , char *history_FileName , char **history_array ,int *history_id ){
    if ((strcmp(arguments[0], "history") == 0  || strcmp(arguments[0], "History") == 0) && arguments[1] == NULL  ){
        showHistory(history_FileName);
        printf("\n");
        free_arguments(arguments);
        return 1;
    }
    else if ((strcmp(arguments[0], "history") == 0  || strcmp(arguments[0], "History") == 0) && (strcmp(arguments[1], "-c") == 0)){
        clearHistory(history_FileName);
        /*
        while(*history_array) {
            free(*history_array);  
            *history_array++ = NULL;
        }
        */
        *history_id = 0 ;
        printf("\n");
        free_arguments(arguments);
        return 1;
    }
    else if ((strcmp(arguments[0], "history") == 0  || strcmp(arguments[0], "History") == 0) && isdigit(atoi(arguments[1])) ==0){
        printf("\n");
        prints_specific_history(arguments, history_array ,history_id );
        //clearHistory(history_FileName);
        //history_id = 0 ;
        free_arguments(arguments);
        return 1;
    }else{
        return 0;
    }
}

typedef struct job { 									
        int id;
        char *name;
        pid_t pid;
        int status;
        struct job *next;
} t_job;


static int background_jobs = 0; 	

static t_job* jobsList = NULL; 

t_job * getJob(int searchValue){
    usleep(10000);
    t_job* job = jobsList;
    while (job != NULL) {
        if (job->pid == searchValue)
            return job;
        else
            job = job->next;
            
    }
    return job ;
}

t_job* insertJob(pid_t pid, char* name,int status){
        usleep(10000);
        t_job *newJob = malloc(sizeof(t_job));
        newJob->name = (char*) malloc(sizeof(name));
        newJob->name = strcpy(newJob->name, name);
        newJob->pid = pid;
        newJob->status = status;
        newJob->next = NULL;
        if (jobsList == NULL) {
            background_jobs++;
            newJob->id = background_jobs;
            return newJob;
        } else {
            t_job *auxNode = jobsList;
            while (auxNode->next != NULL) {
                    auxNode = auxNode->next;
            }
            newJob->id = auxNode->id + 1;
            auxNode->next = newJob;
            background_jobs++;
            return jobsList;
        }
}




void signalHandler_child(int p)
{
    pid_t pid;
    int terminationStatus;
    pid = waitpid(WAIT_ANY, &terminationStatus, WUNTRACED | WNOHANG);                                                                     
    t_job* job = getJob(pid);                
    if (job == NULL)
        return;
    if (WIFEXITED(terminationStatus)) {                                                  
        printf("\n[%d]+  Done\t   %s\n", job->id, job->name);
        //job->id -- ;
    return;       
    }                                                              
}





int run_command(char **arguments, char *input_File , char *output_File , int *input ,int *output, FILE *fp , char **argument2 ,int arguments_number , int arguments2_num , bool *execting_background){
    
    //Child's exit status
    int status;
    // Forking a child process 
    pid_t pid ; // child's process id
    pid = fork();
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
    signal(SIGCHLD, SIG_IGN);
    
    /* set function 1 Command with arguments */ 
    if(pid >= 0){
        
        /* set Function 1 */
        /* command with Arguments Fork() , waitpid(), Exit() */
            if(pid ==0 ){
                if(arguments2_num != 0 ){
                    
                    // pipe let the shell use more command one output of one command saves as input for next
                    // create pipe 
                    int init_pipe[2];
                    pipe(init_pipe);
                    // fork the two processor 
                    pid_t pip_id2 = fork();
                    // child process for second command 
                    if(pip_id2 > 0) {
                        int output_desc,input_desc;
                        
                        if(*input == 1 && *output == 0 ){
                            input_desc = open(input_File, O_RDONLY, 0666);
                            if(input_desc < 0) {
                                fprintf(stderr, "Failed to open the input file: %s\n", input_File);
                                return 0;
                            }
                            dup2(input_desc, STDIN_FILENO);
                        }
                        if(*output == 1 &&  *input == 0){
                            
                            output_desc = open(output_File, O_CREAT|O_RDWR|O_TRUNC, 0666);
                            if(output_desc < 0) {
                                fprintf(stderr, "Failed to open the output file: %s\n",output_File);
                                return 0;
                            }
                            dup2(output_desc, STDOUT_FILENO);
                        
                        }
                        if(*input == 1 && *output == 1){
                            input_desc = open(input_File, O_RDONLY, 0666);
                            if(input_desc < 0) {
                                fprintf(stderr, "Failed to open the input file: %s\n", input_File);
                                return 0;
                            }
                            output_desc = open(output_File, O_CREAT|O_RDWR|O_TRUNC, 0666);
                        
                            if(output_desc < 0) {
                                fprintf(stderr, "Failed to open the output file: %s\n",output_File);
                                return 0;
                            }
                            dup2(input_desc, 0);
                            dup2(output_desc, 1);
                        }
                        
                        close(init_pipe[1]);
                        dup2(init_pipe[0], STDIN_FILENO);
                        wait(NULL);
                        
                        
                        status = execvp(argument2[0],argument2);
                        close(output_desc);
                        close(input_desc);
                        close(init_pipe[0]);
                        fflush(stdin);
                        if( status < 0){
                            perror("command not found");
                            exit(EXIT_FAILURE);
                        }
                    }else if (pip_id2 == 0 ) {
                        int output_desc ,input_desc;
                        if(*input == 1 && *output == 0 ){
                            input_desc = open(input_File, O_RDONLY, 0666);
                            if(input_desc < 0) {
                                fprintf(stderr, "Failed to open the input file:5 %s\n", input_File);
                                return 0;
                            }
                            dup2(input_desc, STDIN_FILENO);
                        }
                    
                    
                        if(*output == 1 &&  *input == 0){
                            output_desc = open(output_File, O_CREAT|O_RDWR|O_TRUNC, 0666);
                            if(output_desc < 0) {
                                fprintf(stderr, "Failed to open the output file: %s\n",output_File);
                                return 0;
                            }
                            dup2(output_desc, STDOUT_FILENO);
                        
                        }
                
                        if(*input == 1 && *output == 1){
                            input_desc = open(input_File, O_RDONLY, 0666);
                            if(input_desc < 0) {
                                fprintf(stderr, "Failed to open the input file: %s\n", input_File);
                                return 0;
                            }
                            output_desc = open(output_File, O_CREAT|O_RDWR|O_TRUNC, 0666);                      
                            if(output_desc < 0) {
                                fprintf(stderr, "Failed to open the output file: %s\n",output_File);
                                return 0;
                            }
                            dup2(input_desc, 0);
                            dup2(output_desc, 1);
                            close(output_desc);
                            close(input_desc);
                        
                        //dup2(STDOUT_FILENO,2);
                        
                        }
                        
                        close(init_pipe[0]);
                        
                        dup2(init_pipe[1], STDOUT_FILENO);
                        status = execvp(arguments[0],arguments);
                        
                        if(*output){
                            close(output_desc);
                        }
                        else if(*input){
                            close(input_desc);
                        }
                        close(init_pipe[1]);
                        fflush(stdin);

                        if( status < 0){
                            perror("command not found:\n");
                            exit(EXIT_FAILURE);
                        }
                        //
                        
                    }
                    //free_arguments(argument2);
                
                    
                }else{
                    //Zero: Returned to the newly created child process.
                    // child processer
                    int output_desc , input_desc;
                    
                    
                    if(*input == 1 && *output == 0 ){
                        input_desc = open(input_File, O_RDONLY, 0666);
                        if(input_desc < 0) {
                            fprintf(stderr, "Failed to open the input file: %s\n", input_File);
                            return 0;
                        }
                        dup2(input_desc, STDIN_FILENO);
                    }
                    
                    
                    if(*output == 1 &&  *input == 0){
                        output_desc = open(output_File, O_CREAT|O_RDWR|O_TRUNC, 0666);
                        if(output_desc < 0) {
                            fprintf(stderr, "Failed to open the output file: %s\n",output_File);
                            return 0;
                        }
                        dup2(output_desc, STDOUT_FILENO);
                        
                    }
                    
                    
                    if(*input == 1 && *output == 1){
                        input_desc = open(input_File, O_RDONLY, 0666);
                        if(input_desc < 0) {
                            fprintf(stderr, "Failed to open the input file: %s\n", input_File);
                            return 0;
                        }
                        output_desc = open(output_File, O_CREAT|O_RDWR|O_TRUNC, 0666);
                        
                        if(output_desc < 0) {
                            fprintf(stderr, "Failed to open the output file: %s\n",output_File);
                            return 0;
                        }
                        dup2(input_desc, 0);
                        dup2(output_desc, 1);
                        close(output_desc);
                        close(input_desc);
                        
                        //dup2(STDOUT_FILENO,2);
                        
                    }
                    char *testing = strchr(arguments[0], '=');
                    if(testing != NULL && strcmp(arguments[0],"cd") ==0){
                        // execute the command 
                        if (access(arguments[0], F_OK) != -1) { // check current directory to run command
                            if (execv(arguments[0], arguments) == -1) {
                                perror("ERROR ");
                                exit(0);
                            }
                            
                        } else {
                            char * temp = find_environment_variable("PATH");
                            // test if not found 
                            if (strcmp(temp, "NOT_FOUND") == 0) {
                                temp = (char *) malloc(514 * sizeof(char));
                                    // get environment by system call 
                                strcpy(temp, getenv("PATH"));
                            }
                            char *path =(char *) malloc(strlen(temp) * sizeof(char) + 2);
                            // copy the path  
                            strcpy(path, temp);
                            free(temp);
                            // remover dotes
                            char *dir = strtok(path, ":");
                            free(path);
                        
                            free(source);
                            source = (char **) malloc( 20* sizeof(char *));
                        
                            
                            int i =0;
                        
                            // store direction in source 
                            while (dir != NULL) {
                                source[i] = (char *) malloc(strlen(dir) + 1);
                                strcpy(source[i], dir);
                                //source[i] = NULL;
                            
                                //printf(" \n %s",source[i]);
                                i++;
                                dir = strtok(NULL, ":");
                            }
                            source[i] = NULL;
                        
                            
                            
                                    
                            // check other directories from PATH
                            for (int i = 0; source[i] != NULL; i++) { 
                                char *executablePath = (char *) malloc(strlen(source[i]) + 2);
                                strcpy(executablePath, source[i]);
                                strcat(executablePath, "/");
                                strcpy(executablePath, arguments[0]);
                                int i =0;
                                while (source[i] != NULL) {
                                    //source[i] = (char *) malloc(strlen(dir) + 1);
                                    //strcpy(source[i], dir);
                                    source[i] = NULL;
                                    free(source[i]);
                                    //printf(" \n %s",source[i]);
                                    i++;
                                    //dir = strtok(NULL, ":");
                                }
                                free(source);               
                                char **parsed;
                                parsed[0]= (char *) malloc(strlen(executablePath) + 1);
                                strcpy(parsed[0], executablePath);
                                free(executablePath);
                                
                                status = execvp(parsed[0],parsed);
                                free(parsed);
                                        
                                if( status < 0){
                                    if(arguments[0][0] == '.' && arguments[0][1] == '/' ){
                                        printf("-myShell: %s: No such file or directory \n" ,arguments[0]);
                                    }else{
                                        printf("-bash: %s :command not found \n" ,arguments[0]);
                                    }
                                    exit(EXIT_FAILURE);
                                    
                                }
                            }
                            
                            
                            
                        }
                    }else{
                        
                        // execute the command 
                        status = execvp(arguments[0],arguments);
                        //status = execvp(arguments[2],arguments);
                        if( status < 0){
                            if(arguments[0][0] == '.' && arguments[0][1] == '/' ){
                                printf("-myShell: %s: No such file or directory \n" ,arguments[0]);
                            }else{
                                printf("-bash: %s :command not found \n" ,arguments[0]);
                            }
                            exit(EXIT_FAILURE);
                        }
                        exit(status);
                        
                    }
                    if(*output){
                        close(output_desc);
                    }
                    else if(*input){
                        close(input_desc);
                    }
                    
                }
                
            }else if( pid > 0){
                // parent processor
                //Positive value: Returned to parent or caller
                if(!*execting_background){
                    sigaction(SIGQUIT, &sigact, NULL);
                    /* pid holds the id of child */
                    do{
                        waitpid(pid, &status, WUNTRACED);
                    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
                    //printf("PARENT (%d): sending SIGQUIT/kill to %d\n", getpid(), pid);
                    //sleep(1); /* pause for 1 secs */
                    
                    kill(pid,SIGQUIT);
                        
                }else{
                    
                    
                    static int i = 1 ;
                    
                jobsList = insertJob(pid,*(arguments),(int) status);
                t_job* job = getJob(pid);
                printf("[%d] %d \n", i ,(int) pid);
                i ++ ;
                    
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
    
    fflush(stdout);
    fflush(stdin);
    //free_arguments(arguments);
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
    // command array
    char command[Command_LINE +1];
    
    // arguments array
    char *arguments [args_LINE +1];
    bool execting_background ;
    char **arguments2 ;
    int arguments2_num = 0 ;

    /* variables for the shell */
    char *history_FileName;
    //char *cis3110_profile;
    char **source;

    int history_id = 0;
    char *history_array[300];





    /* --------------------- set function 3 ----------------------- */
    // initital shell environment
    init_environment(arguments,command,&history_FileName);
    cis3110_profile_input(arguments,command,execting_background,arguments2,arguments2_num,input_desc,output_desc,fp,input_num,output_num,input_File,output_File,history_FileName ,&history_id,history_array);
    
    // welcome message 
    welcome_message();
    free_arguments(arguments);
    //change_directory("HOME",1);
    while (1)
    {
        //setsource(true);
        // start from home 
        free_arguments(arguments);
        fflush(stdout);
        fflush(stdin);
        //predict the operating system and print the prompt sign
        promot();
        
        // test if empty command 
        if(!command_input(command)){
            free_arguments(arguments);
            continue;
        }
        
        
        // parse argument into list of arguments
        int arguments_number = parse(arguments,command,&execting_background,&arguments2,&arguments2_num, &input_File ,&output_File, &input_num, &output_num ,&fp,history_FileName,&history_id,history_array);
        
        
        if(test_history_input(arguments , history_FileName , history_array , &history_id)){
            free_arguments(arguments);
            continue;
        }

        if (strcmp(arguments[0], "cd") == 0) {
            change_directory(arguments[1], arguments_number);
            free_arguments(arguments);
            continue;
        } 
        
        
            
        run_command(arguments,input_File,output_File,&input_num,&output_num,fp ,arguments2 ,arguments_number,arguments2_num,&execting_background);
        
        if (strcmp(arguments[0], "ps") == 0) {
            signal(SIGCHLD, &signalHandler_child);
            sleep(1);
            free_arguments(arguments);
            continue;
        }
        if (strcmp(arguments[0], "echo") == 0) {
            if (arguments[0] == NULL){
                continue;
            }
            if (arguments[1] == NULL){
                continue;
            }
            if (strcmp(arguments[1],"$PATH")==0){
                char * home =find_environment_variable("PATH");
                printf("%s \n", home);
                //printf("%s",variables[1].value);
                free_arguments(arguments);
                continue;
            }
            else if (strcmp(arguments[1],"$HOME")==0){
                char * home =find_environment_variable("HOME");
                printf("%s \n", home);
                free_arguments(arguments);
                continue;
            }
        }

        
        
        free_arguments(arguments);
        arguments2_num = 0;
        
        
        fflush(stdout);
        fflush(stdin);
        
    }

    return 0 ;
    
}




