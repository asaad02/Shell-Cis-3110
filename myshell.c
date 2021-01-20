
/**
 * @file shell.c
 * @author Abdullah Saad
 * @date january 20 2021
 * @brief File containing the function operate for shell.
 **/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>

find1_ampersand(arguments,*arguments_number);

#define LINE 80
#define DELIMITERS  " \t\n\v\f\r"

void taking_command(char *command){
    /* strcpy() function copies the string pointed by "" (including the null character) to the Command */
    strcpy(command,"");
}

char input( char *command){

    // buffer hold the command 
    char buffer[LINE +1];

    // get the command 
    if(fgets(buffer, LINE , stdin) == NULL){
        fprintf(stderr," Failed to read the command ! \n");
        return 0;
    }
    // The null pointer constant NULL's value equals 0
    if(strncmp(buffer,"!!",2)==0 ){
        if(strlen(command) == 0){
            fprintf(stderr," No history avaliable ! \n");
            return 0;
        }
        printf("%s",command);
        return 1;
    }

    strcpy(command, buffer);
    return 1;

}

void saving_arguemnts(char *arguments []){
    for(int i =0 ; i != LINE ; i++){
        arguments [i] = NULL ;
    }
}

int run(char **arguments, int arguments_number){

    // executed in the background using &
    int run_background = find1_ampersand(arguments,&arguments_number);
    //pid_t data type stands for process identification and it is used to represent process ids.
    //child process  
    pid_t child_id = fork();

    if(child_id < 0){
        //prints a descriptive error message to stderr
        fprintf(stderr, "error: fork error\n");
		return 0;
        

    }
}

int find1_ampersand( char **arguments, int*size){


return 0;

}


int main(void){

    char command[LINE +1];
    char *arguments [LINE +1];
    
    saving_arguemnts(arguments);
    taking_command(command);

    while (1)
    {
        fputs(">",stdout);

        if(!input(command)){
            continue;
        }

        int arguments_number = num_input(arguments,command);

        if (arguments_number == 0){
            fputs("UNIX shell progrom ", stdout);
            fputs("Please enter the Command!", stdout);
            fputs("Type \"exit\" to exit ", stdout);

        }
        // Set 1 FunctionsThe system calls that are listed are suggestions –you must decide what is appropriate.
        if ((strcmp(&command[0], "exit") == 0) || (strcmp(&command[0], "Exit") == 0)){
            //System calls: exit()
            fputs("myShell terminating..... ", stdout);
            fputs("[Process completed] ", stdout);
            exit(0);
        }

        run(arguments,arguments_number);



    }
    

}