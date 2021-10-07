/*

 Name: Prithvidhar Pudu
 ID: 1001570483

*/
//Code for parsing input and setting up shell provided by Trevor Bakker
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

#define MAX_NUM_ARGUMENTS 11     // Mav shell only supports five arguments
								// Changed for current assignment

int main()
{

  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
  //myvars------------------------------------------------
  // Initializing array to hold commands and process IDs so that i can
// iterate over them after the child process and display the results to the user
//The various counters are used to keep track of the commands position in their respective
//arrays.
// The child flag is used to identify whether a command created a child process. The cross_flags are 
//used to identify if the phistory and history tabs have crosses their 15 item limit.
// The head array is used to keep track of the values that the history command overwrite.
//This is to ensure the user can use the "!" command effectively.
//After a loop the head of command is replaced with "his" so that the user can call the "!" properly
//and get the correct result
   char history[15][MAX_COMMAND_SIZE];
   char head[15][MAX_COMMAND_SIZE];
    int status;
    pid_t phistory[15];
    int history_counter =-1;
    int p_counter = -1;
    int child_flag = 0;
    int cross_flag = 0;
    int cross_flag2 = 0;
	char* his = "history";
    pid_t child;
	char sub[MAX_COMMAND_SIZE];
	int c = 0;
	
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
   
    //Altering cmd_str when the !n command is used
    // this way i can use the same processes without
    // making a new if block
	//if the "!" command calls a history command the value of its head is used
	//which is located in the head array.
	//I use sub to store the command number after the "!" character. I use a while loop to create a
	//substring
	//The apprpriate boundary checks are also performed to prevent segmentation faults
	//The boundary checks are done checking the boundaries based on whther the 15 item limit is reached
	c = 0;
	sub[0] = '\0';
    if(cmd_str[0] == '!')
    {
		while(c<MAX_COMMAND_SIZE)
		{
			sub[c] = cmd_str[(2+c)-1];
			c++;
		}
		sub[c] = '\0';
		int num = atoi(sub);
		
        if(cross_flag2)
        {
            if( strcmp(history[num], "history") ==0 )
            {
                 
                strncpy(cmd_str, head[num], MAX_COMMAND_SIZE);
                
		
				cmd_str[strlen(cmd_str)] = '\n';
                
            }
            else
            {
                strncpy(cmd_str, history[num], MAX_COMMAND_SIZE);
                cmd_str[strlen(cmd_str)] = '\n';
            }
            
        }
        else if(num>=0 && num <=14)
        {
            if(!cross_flag2 && num<= history_counter)
            {
                strncpy(cmd_str, history[num], MAX_COMMAND_SIZE);
                cmd_str[strlen(cmd_str)] = '\n';
            }
			else if(cross_flag2)
			{
				strncpy(cmd_str, history[num], MAX_COMMAND_SIZE);
				cmd_str[strlen(cmd_str)] = '\n';
				printf("Debug: %s\n",history[num]);
			}
            else 
            {
                printf("Command not in history.\n");
                continue;
            } 
        }
        else
        {
            printf("Command not in history.\n");
            continue;
        }
        
    }
	//Here if the history array has crossed its 15 item limit for the first time
	//it will replace a command head with history to ensure that the "!" will call history
	//and not the previous value
	if(history_counter>=0)
	{
		 strncpy(head[history_counter],his,MAX_COMMAND_SIZE);
		
	}
     

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
     * new processes using fork()
     */
	//This if statement handles the cases where the does not enter anything
     if(token[0] == NULL)
     {
         free(working_root);
         continue;
         
     }
     //Resetting the child flag, in case no child is born.
     //This happens every loop in case the user uses the local
     //cd, history or showpids commands
     // I am also incrementing the command history counter here in order
     // to place it in the correct position.
	// Two for loops are used to display the history. One case when more than 15 commands have been entered
	// and another when less than 15 have been entered.
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
	//This else if statement handles the quit and exit statements which cause the program
	// to exit with a status of 0
    else if(strcmp(token[0],"exit")==0|| strcmp(token[0],"quit")==0)
    {
        exit(0);
    }
	//This else if statement executes the cd command using the chdir() method
    else if(strcmp(token[0],"cd")==0)
    {
       if(chdir(token[1])== -1)
		{
			printf("Error changing to directory. Please try again\n");
			
		}
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
    //Adding the recently typed in command to the history
    // array. I utilized the strncpy to copy the contents of 
    // working_root to the correct array position
	//I also added the overwritten command to the head array so that the user
	//can use the "!" command accurately
	if(strcmp(token[0],"history")==0)
	{
		strncpy(head[history_counter],history[history_counter],MAX_COMMAND_SIZE);
	}
    cmd_str[strlen(cmd_str)-1] = '\0';
    strncpy(history[history_counter], cmd_str,MAX_COMMAND_SIZE);
    
    
    

    free( working_root );
   

  }
  return 0;
}
