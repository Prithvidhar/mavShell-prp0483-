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
  //myvars------------------------------------------------
  // Initializing array to hold commandsd and process IDs so that i can
// iterate over them after the child process and display the results to the user
   char history[15][MAX_COMMAND_SIZE];
    int status;
    pid_t phistory[15];
    int history_counter =-1;
    int p_counter = -1;
    int child_flag = 0;
    int cross_flag = 0;
    int cross_flag2 = 0;
    pid_t child;
    //my vars-----------------------------------------------

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
    
    
   
    /*
     * Creating a series of if else loops to check the user command and perform
     * the approriate linux response. I will start by checking for 
     * built in commands such as history and showpids before creating 
     * new processing using fork()
     */
     if(token[0] == NULL)
     {
         free(working_root);
         continue;
         
     }
     //Resetting the child flag, in case no child is born.
     //This happens every loop in case the user using the local
     //cd, history or showpids commands
     // I am also incrementing the command history counter here in order
     // keep place it in the correct position.
     child_flag = 0;
     history_counter++;
    if(strcmp(token[0],"history")==0)
    {
        
        if(cross_flag2)
        {
           for(int i = 0; i <=14; i++)
           {
               printf("%d: %s\n",i,history[i]);
           } 
        }
        else
        {
            for(int i = 0; i <history_counter; i++)
            {
               printf("%d: %s\n",i,history[i]);
            } 
        }
    }
    //This section of code is responsible for printing the 
    // the pids of children born from this shell
    // a flag is used to identify when 15 children have made
    // when flag is raised all past 15 children's process IDs
    // will be displayes else fewer ids will be displayed
    else if(strcmp(token[0],"showpids")==0)
    {
        if(cross_flag)
        {
            for(int i =0;i <= 14;i++)
            {
                printf("%d: %ld\n",i,phistory[i]);
            }
        }
        else
        {
            for(int i =0;i <= p_counter;i++)
            {
                printf("%d: %ld\n",i,phistory[i]);
            }
        }
        
    }
    else if(strcmp(token[0],"exit")==0|| strcmp(token[0],"quit")==0)
    {
        exit(0);
    }
    else if(strcmp(token[0],"cd")==0)
    {
        chdir(token[1]);
    }
    
    //Starting with the child processes who will
    //Take advantage of the fork() function
    else
    {   //This section of code will now be utilized by
        //the child processes and will use execvp to perform
        //its tasks
        child_flag = 1;
        p_counter++;
        child = fork();
        if(child == 0)
        {
            
            int birth = execvp(token[0], &token[0]);
            if(birth == -1)
            {
                printf("%s: Command not found\n",token[0]);
                exit(1);
                
            }
            
        }
    }
    wait(&status);
    //After the child is finished, if executed its pid is stored
    // in the array. It's position is based on the counter. If
    // The counter goes over 15 positions, the pid will be placed
    // from the top again using the if statement
    if(child_flag)
    {
        if(p_counter  == 15)
        {
            p_counter = 0;
            cross_flag = 1;
        }
        phistory[p_counter] = child;
    }
    //Doing the same thing as the showpids command for the history
    // command. If the counter is 15, the position will loop back to
    // the beginning of the history array
    if(history_counter == 15)
    {
        history_counter = 0;
        cross_flag2 = 1;
    }
    //Adding the recently typing in command to the history
    // array. I utilized the strncpy to copy the contents of 
    // working_root to the correct array position
    strncpy(history[history_counter], working_root,strlen(working_root));
    
    
    

    free( working_root );

  }
  return 0;
}
