
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

    *history_FileName = (char *) malloc(strlen(home) * sizeof(char) +strlen(history) * sizeof(char)  );
    strcpy(*history_FileName, home);
    strcat(*history_FileName, history);
 
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
        char *line_copy = (char *)malloc(Command_LINE * sizeof(char));
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
    // copy the buffer to command 
    strcpy(command, buffer);
    return 1;
}


// Set function 1 (exit() system call) that terminate the shell
// exit() exiting the command  
void exit_function(char *argument[],char *history_FileName){
    if ((strcmp(argument[0], "exit") == 0) || (strcmp(argument[0], "Exit") == 0)){
        free_arguments(argument);
        fputs("\n\n\n", stdout );
        fputs("myShell terminating.....\n\n", stdout );
        fputs("[Process completed]\n", stdout);
        clearHistory(history_FileName);
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
void free_history_arguments(char *history_array[]){
        while(*history_array) {
            free(*history_array);  // to avoid memory leaks
            *history_array++ = NULL;
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
    exit_function(arguments,history_FileName);

    

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
            *input_File = arguments[i+1];
            free(arguments[i]);
            *input = 1;

            for(int j = i; arguments[j-1] != NULL; j++) {
                arguments[j] = arguments[j+2];
            }


            break;
            

        }else if (!strcmp(&arguments[i][0], ">")){      //check for output >
            *output_File = arguments[i+1];
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



int run_command(char **arguments, char *input_File , char *output_File , int *input ,int *output, FILE *fp , char **argument2 ,int arguments_number , int arguments2_num , bool *execting_background){
    
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
                        int output_desc,input_desc;
                        if(*output){
                            
                            output_desc = open(output_File, O_WRONLY | O_CREAT | O_TRUNC, 644);
                            if(output_desc < 0) {
                                fprintf(stderr, "Failed to open the output file: %s\n",output_File);
                                return 0;
                            }
                            dup2(output_desc, STDOUT_FILENO);
                        }
                        if(*input){
                            input_desc = open(input_File, O_RDONLY, 0644);
                            if(input_desc < 0) {
                                fprintf(stderr, "Failed to open the input file: %s\n", input_File);
                                return 0;
                            }
                            dup2(input_desc, STDIN_FILENO);
                        }

                        
                        printf("pipe found \n");
                        //not ignore SIGINT
                        close(init_pipe[1]);
                        dup2(init_pipe[0], STDIN_FILENO);
                        wait(NULL);

                        status = execvp(argument2[0],argument2);
                        if(*output){
                            close(output_desc);
                        }
                        if(*input){
                            close(input_desc);
                        }
                        close(init_pipe[0]);
                        fflush(stdin);


                        if( status < 0){

                            perror("Error in Forking child process in pip function \n");

                            exit(EXIT_FAILURE);
                        }
                    }else if (pip_id2 == 0 ) {
                        int output_desc ,input_desc;
                        if(*output){
                            output_desc = open(output_File, O_WRONLY | O_CREAT | O_TRUNC, 644);
                            if(output_desc < 0) {
                                fprintf(stderr, "Failed to open the output file: %s\n",output_File);
                                return 0;
                            }
                            dup2(output_desc, STDOUT_FILENO);
                        }
                        if(*input){
                            input_desc = open(input_File, O_RDONLY, 0644);
                            if(input_desc < 0) {
                                fprintf(stderr, "Failed to open the input file: %s\n", input_File);
                                return 0;
                            }
                            dup2(input_desc, STDIN_FILENO);
                        }

                        close(init_pipe[0]);
                        dup2(init_pipe[1], STDOUT_FILENO);
                        status = execvp(arguments[0],arguments);
                        if(*output){
                            close(output_desc);
                        }
                        if(*input){
                            close(input_desc);
                        }
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
                    int output_desc , input_desc;

                    if(*output){
                        output_desc = open(output_File, O_WRONLY | O_CREAT | O_TRUNC, 644);
                        if(output_desc < 0) {
                            fprintf(stderr, "Failed to open the output file: %s\n",output_File);
                            return 0;
                        }
                        dup2(output_desc, STDOUT_FILENO);
                    }

                    if(*input){
                        input_desc = open(input_File, O_RDONLY, 0644);
                        if(input_desc < 0) {
                            fprintf(stderr, "Failed to open the input file: %s\n", input_File);
                            return 0;
                        }
                        dup2(input_desc, STDIN_FILENO);
                    }
                    // execute the command 
                
                    status = execvp(arguments[0],arguments);
                    if(*output){
                        close(output_desc);
                    }
                    if(*input){
                        close(input_desc);
                    }

                
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

    // command array
    char command[Command_LINE +1];
    
    // arguments array
    char *arguments [args_LINE +1];


    bool execting_background ;

    char **arguments2 ;
    int arguments2_num = 0 ;

    /* --------------------- set function 3 ----------------------- */

    /* variables for the shell */
    char *history_FileName;
    //char *cis3110_profile;
    char **source;

    int history_id = 0;
    char *history_array[300];



    // initital shell environment
    init_environment(arguments,command,&history_FileName);

    cis3110_profile_input(arguments,command,execting_background,arguments2,arguments2_num,input_desc,output_desc,fp,input_num,output_num,input_File,output_File,history_FileName ,&history_id,history_array);
    
    // welcome message 
    welcome_message();
    
    while (1)
    {
        fflush(stdout);
        fflush(stdin);
        //predict the operating system and print the prompt sign
        promot();
        // test if empty command 
        if(!command_input(command)){
            continue;
        }



        // parse argument into list of arguments
        int arguments_number = parse(arguments,command,&execting_background,&arguments2,&arguments2_num, &input_File ,&output_File, &input_num, &output_num ,&fp,history_FileName,&history_id,history_array);


        if(test_history_input(arguments , history_FileName , history_array , &history_id)){
            continue;
        }


        // start from home 
        //Change_directory("",1);

        run_command(arguments,input_File,output_File,&input_num,&output_num,fp ,arguments2 ,arguments_number,arguments2_num,&execting_background);
        

        
        free_arguments(arguments);
        free_history_arguments(history_array);


        fflush(stdout);
        fflush(stdin);
        

    }

    return 0 ;
    

}


void cis3110_profile_input(char *arguments[] , char *command,bool execting_background,char **arguments2,int arguments2_num ,int input_desc, int output_desc,FILE *fp ,int input_num,int output_num ,char *input_File , char *output_File, char *history_FileName,int *history_id , char **history_array ){

    FILE * bash_profile = fopen("cis3110_profile", "r");
    if (bash_profile == NULL) {
        printf("%s\n", "ERROR : batch file does not exist or cannot be opened");
        return;
    }
    int i = 0;
    // buffer hold the command 
    char buffer[Command_LINE +1][args_LINE+1];
    // get the command 
    while(fgets(buffer[i], sizeof(buffer), bash_profile) != NULL){
        // if command is NULL print error mesage
        //printf("%s\n",buffer[i++]);
        strcpy(command,buffer[i++]);
        // copy the buffer to command 
        printf("%s", command);
        // parse argument into list of arguments
        int arguments_number = parse(arguments,command,&execting_background,&arguments2,&arguments2_num, &input_File ,&output_File, &input_num, &output_num ,&fp,history_FileName,history_id,history_array);
        run_command(arguments,input_File,output_File,&input_num,&output_num,fp ,arguments2 ,arguments_number,arguments2_num,&execting_background);
        // strcpy() function copies the string pointed by "" (including the null character) to the Command 
        strcpy(command,"");
        free_arguments(arguments);
        fflush(stdout);
        fflush(stdin);
        printf("\n \n ");
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
}

int test_history_input(char ** arguments , char *history_FileName , char **history_array ,int *history_id ){
    if ((strcmp(arguments[0], "history") == 0  || strcmp(arguments[0], "History") == 0) && arguments[1] == NULL  ){
        showHistory(history_FileName);
        printf("\n");
        free_arguments(arguments);
        return 1;
    }
    if ((strcmp(arguments[0], "history") == 0  || strcmp(arguments[0], "History") == 0) && (strcmp(arguments[1], "-c") == 0)){
        clearHistory(history_FileName);
        while(*history_array) {
            free(*history_array);  // to avoid memory leaks
            *history_array++ = NULL;
        }
        *history_id = 0 ;
        printf("\n");
        free_arguments(arguments);
        return 1;
    }


    if ((strcmp(arguments[0], "history") == 0  || strcmp(arguments[0], "History") == 0) && isdigit(atoi(arguments[1])) ==0){
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

/*
/Limited shell environment variables: PATH, HISTFILE, HOME.
$PATH: contains the list of directories to be searched when commands are
executed. For this exercise the default will be /bin to facilitate testing. This
is not the normal default. To find the default on a UNIX bash shell, execute
the following command: echo $PATH. Hint: execvP().
$HISTFILE: contains the name of the file that contains a list of all inputs to
the shell. The default name of this file is ~/.bash_history. For the purposes
of this assignment, the default will be the file .CIS3110_history in the
directory in which the shell is initialized, i.e. the current working directory
when the shell program is executed.
$HOME: contains the home directory for the user. For the purposes of this
assignment, the default home directory will be the directory in which the
shell is initialized, i.e. the current working directory when the shell program
is executed.


// 3 environment variables {"PATH", "HOME","HISTFILE"}
const char *environment[] = {"PATH", "HOME","HISTFILE"};


    char *history_FileName;
    char *log_FileName;
    char *cis3110_profile;
    char **source;
    typedef struct var {
        char *command ;
        char *value; 
    } var ;

    var variables[100];
    int last_index = -1;

char **source;


// check if the command is environment or not 
bool is_environment( char *command){

    for ( int i = 0;  i < 3 ; i++ ){
        if (strcmp(environment[i],command) == 0){
            return true;
        } 
    }
    return false;
}



char* find_variable(char *command_input) {
    for (int i = 0; i <= -1; ++i) {
        if (strcmp(variables[i].command, command_input) == 0) {
            return variables[i].value;
        }
    }
    return "NOT_FOUND";
}


//• $PATH: contains the list of directories to be searched when commands are

void setsource(){
    char **source;

    free(source);

    source = (char **) malloc(30 * sizeof(char *));

    char * temp = find_variable("PATH");

    if (strcmp(temp, "NOT_FOUND") == 0) {
        temp = (char *) malloc(514 * sizeof(char));
        strcpy(temp, getenv("PATH"));
    }

    char *path =(char *) malloc(strlen(temp) * sizeof(char)); 
    strcpy(path, temp);

    const char DELIMITER[0] = ":";

    char *dir = strtok(path, DELIMITER);

    int i =0;

    while (dir != NULL) {
        source[i] = (char *) malloc(strlen(dir) + 1);
        strcpy(source[i], dir);
        i++;
        dir = strtok(NULL, ":");
    }
    source[i] = NULL;

     
}





//• $HISTFILE: contains the name of the file that contains a list of all inputs to


//$HOME: contains the home directory for the user. For the purposes of this


//By default in the bash shell, the profile file is call .bash_profile and is in the user’s











By default in the bash shell, the profile file is call .bash_profile and is in the user’s
home directory. Please see the notes on $HOME about the directory that will be
considered HOME for this assignment. Also the profile file will be named
.CIS3110_profile for the purposes of this assignment


The cd builtin command
• The cd or “change directory” changes the notion of which directory the
command is being issued from.
• Hint: chdir()
• If you implement the cd command (and thus your shell is aware of the
current working directory’s full path name) then replace the “> “ prompt in
your shell with “cwd > “ where cwd is the full path name of the current
working directory. For example:
$ ./myShell
/Users/dastacey/Teaching/CIS3110> cd ..
/Users/dastacey/Teaching> cd ..
/Users/dastacey> exit
$


void Change_directory(const char* path, int arguments_number) {
    // check how many arguments 
    // if more than two print { to many arguments}
    if (arguments_number > 2) {
        printf("ERROR: too many arguments for cd\n");
        return 0;
    }else{
        // home directory id
        int directory_id;
        // get the value of the "HOME"
        char *home_temp = find_variable("HOME");
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
*/