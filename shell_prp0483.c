// Code for parsing input and setting up shell provided by Trevor Bakker
// for CSE 3320
#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports five arguments

int main()
{

  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;                                         
                                                           
    char *working_str  = strdup( cmd_str );                

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input strings with whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    //My code starts here-------------------------------------------------------------------------------------------------------
    
    // Initializing array to hold commnad and process IDs so that i can
    // iterate over them after the child process and display the results to the user
    char history[15][MAX_COMMAND_SIZE];
    int status;
    pid_t phistory[15];
    pid_t child;
    /*
     * Creating a series of if else loops to check the user command and perform
     * the approriate linux response. I will start by checking for 
     * built in commands such as history and showpids before creating 
     * new processing using fork()
     */
    if(strcmp(token[0],"history")==0)
    {
    }
    else if(strcmp(token[0],"showpids")==0)
    {
    }
    else if(strcmp(token[0],"exit")==0|| strcmp(token[0],"quit")==0)
    {
        exit(0);
    }
    //Starting with the child processes who will
    //Take advantage of the fork() function
    else
    {   //This section of code will now be utilized by
        //the child processes and will use execvp to perform
        //its tasks
        child = fork();
        if(child == 0)
        {
            
            int birth = execvp(token[0], &token[0]);
            if(birth == -1)
            {
                printf("%s: Command not found\n",token[0]);
                
            }
            
        }
    }
    wait(&status);
       

    free( working_root );

  }
  return 0;
}
