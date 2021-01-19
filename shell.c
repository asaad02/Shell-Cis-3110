
/**
 * @file shell.c
 * @author Abdullah Saad
 * @date january 20 2021
 * @brief File containing the function operate for shell.
 **/

#include <stdio.h>

#define LINE 80
#define DELIMITERS  " \t\n\v\f\r"

taking_command(char *command){
    /* strcpy() function copies the string pointed by "" (including the null character) to the Command */
    strcpy(command,"");
}

char input( char *command){

    char buffer[LINE +1];

    if(fgets(buffer, LINE , stdin) == NULL){
        printf(stderr," Failed to read the command ! \n");
        return 0;
    }
    // The null pointer constant NULL's value equals 0
    if(strncmp(buffer,"!!",2)==0 ){
        if(strlen(command) == 0){
            printf(stderr," No history avaliable ! \n");
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


int main(void){

    char *arguments [LINE +1];
    char command[LINE +1];

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
        if ((strcmp(arguments[0], "exit") == 0) || (strcmp(arguments[0], "Exit") == 0)){
            //System calls: exit()
            exit(0);
        }

        run(arguments,arguments_number);



    }
    

}