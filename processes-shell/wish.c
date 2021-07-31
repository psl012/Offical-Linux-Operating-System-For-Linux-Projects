#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void PrintError();
int ParseCommand(char * commands[], char * line_command);
int ExecuteCommands(int margc, char * commands[], char * paths[]);

int main(int argc, char *argv[])
{
	const int BUFF_SIZE = 1024;
	
	char * paths[BUFF_SIZE];
	paths[0] = "/bin";
	paths[1] = NULL;
	
	char * command_lines[BUFF_SIZE];	
	
	
	if(argc == 2)
	{
		char * command = NULL;
		size_t linecap = 0;
		
		FILE * fr = fopen(argv[1], "r");
		
		while(getline(&command, &linecap, fr) > 0)
   		{
			int margc = ParseCommand(command_lines, command);
   			ExecuteCommands(margc, command_lines, paths);
		}
	}
	
	return 0;
}

int ParseCommand(char * commands[], char * line_command)
{		
	int i = 0;
	while(	(commands[i] = strsep(&line_command, " ")) != NULL)
	{
		i++;
	}
	commands[i - 1] = strsep(&commands[i-1], "\n");
	
	return i;
}


int ExecuteCommands(int margc, char * commands[], char * paths[])
{
	// BUILT-IN COMMANDS
	// Exit
    	if((strcmp(commands[0], "exit")) == 0)
    	{
    		if (margc == 1) return -1;
    		else PrintError();
    	}
    	// cd
    	else if((strcmp(commands[0], "cd")) == 0)
    	{
    		if(margc == 2) 
    		{	  
    			//printf("%c", commands[1][2]);
    			if(chdir(commands[1]) != 0) 
    			{
    				PrintError();
    				return -1;	
    			}
    		}
    		else
    		{
    			PrintError();
    			return -1;
    		}
    	}
    	else if((strcmp(commands[0], "path")) == 0)
    	{
    		int i = 0;
    		for(; i < margc; i++)
    		{
    			paths[i - 1] = commands[i];
    		}
    		paths[i] = NULL;
    	}
    	
    	// NOT BUILT IN
    	else if((strcmp(commands[0], "ls")) == 0) 
    	{
    		int i = 0;
    		char * my_bin = NULL;
    		while(paths[i] != NULL)
    		{
			if(strcmp(paths[i], "/bin") == 0) my_bin = strdup("/bin");
			i++;
    		}
    		
    		int rc = fork();
    		if(rc == 0)
    		{
    		
	    		char * myargv[10];
	    		strcat(my_bin, "/ls");
	    		myargv[0] = my_bin;
	    		if(margc == 1) 
	    		{
	    			myargv[1] = NULL;
	    			execv(myargv[0], myargv);
	    		}
	    		else
	    		{
	    			myargv[1] = commands[1];
	    			if(access(myargv[1], F_OK) == 0) execv(myargv[0], myargv);
    			}
	    		char error_message[100] = "ls: cannot access '/no/such/file': No such file or directory\n";
			write(STDERR_FILENO, error_message, strlen(error_message)); 
    		}
    		else
    		{
			wait(NULL);
    		}
    		
    		return 0;
    	}
    	else
    	{
    		PrintError();
    		//printf("GOTME");
    		return -1;
	}
	return 0;	    	
}

	
void PrintError()
{
	char error_message[30] = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message)); 
}

/**
	//char * paths[BUFF_SIZE];
	
  //  w	char * command_input;

 	
   // w	size_t buffsize = 32;
    	FILE * fr = fopen(argv[1], "r");
    	
//w	command_input = (char *) malloc(buffsize * sizeof(char));
	
	if(argc == 2)
	{
		char * command_lines[BUFF_SIZE];
		
		char * line = NULL;
   		size_t linecap = 0;
			
   		while(getline(&line, &linecap, fr) > 0)
   		{
   			int margc = ParseCommand(command_lines, line);
   			ExecuteCommands(margc, command_lines);
		}
	}
	
	while(1)
	{
		if(argc == 1) printf("wish> ");
		
	    	getline(&command_input, &buffsize, stdin);
	    	
	   
    		int margc = ParseCommand(commands, command_input);
    		ExecuteCommands(	
    	}
    
    	
    	// Internal Functions
    	int ExecuteCommands(int margc, char * commands[])
    	{
		// Exit
	    	if((strcmp(commands[0], "exit")) == 0) return -1;
	    
	    	// cd
	    	else if((strcmp(commands[0], "cd")) == 0)
	    	{
	    		if(margc == 2) 
	    		{	  
	    			printf("%c", commands[1][2]);
	    			if(chdir(commands[1]) != 0) 
	    			{
	    				PrintError();
	    				return -1;	
	    			}
	    		}
	    		else
	    		{
	    			PrintError();
	    			return -1;
	    		}
	    	}
	    	
	    	else if((strcmp(commands[0], "ls\n")) == 0) 
	    	{
	    		int rc = fork();
	    		if(rc == 0)
	    		{
		    		char * myargv[10];
		    		myargv[0] = strdup("/bin/ls");
		    		myargv[1] = NULL;
		    		execv(myargv[0], myargv);
	    		}
	    		return 0;
	    	}	    	
    	}
    	
	int ParseCommand(char * commands[], char * line_command)
	{		
		int i = 0;
		while(	(commands[i] = strsep(&line_command, " ")) != NULL)
		{
			i++;
		}
		commands[i - 1] = strsep(&commands[i-1], "\n");
		
		return i;
	}
	
	void PrintError()
	{
    		char error_message[30] = "An error has occurred\n";
    		write(STDERR_FILENO, error_message, strlen(error_message)); 
	}
	
	return 0;*/




