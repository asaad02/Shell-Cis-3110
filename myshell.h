/**
 * @file meshell.h
 * @author Abdullah Saad
 * @date january 20 2021
 * @brief File containing the function prototypes operate for shell.
 **/

#define Command_LINE 100              /* 50 chars per command  */
#define args_LINE    50             /* 50 Arguments */
#define DELIMITERS  " \t\n\v\f\r"   /* characters that separate text strings */


// Welcome message
void welcome_message();
// initital shell environment
void init_environment(char *arguments[] , char *command, char *history_FileName,char *log_FileName ,char *batch_FileName);
// get input 
char command_input( char *command);
// parse argument into list of arguments
int parse(char *arguments[], char *command ,bool *execting_background, char *** arguments2 , int *arguments2_num ,char **input_File , char **output_File , int *input ,int *output, FILE *fp);
// run command  
int run_command(char **arguments, char **input_File , char **output_File , int *input ,int *output, FILE *fp , int arguments_number , bool *execting_background);
//
bool ampersand(char **argument,int *arguments_number);

void check_redirecting(char **arguments, char **input_File , char **output_File , int *input ,int *output, FILE *fp);


void free_arguments(char *arguments[]);

void pipe_function(char ** arguments, int *arguments_num , char *** argument2 , int *argument_num2);