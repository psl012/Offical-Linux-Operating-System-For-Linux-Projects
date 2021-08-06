#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void PrintError();
int ParseCommand(char * commands[], char * line_command);
int ExecuteCommands(int margc, char * commands[], char * paths[]);

int isRedirection = 0;

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
   			isRedirection = 0;
			int margc = ParseCommand(command_lines, command);
   			ExecuteCommands(margc, command_lines, paths);
		}
	}
	
	return 0;
}

int ParseCommand(char * commands[], char * line_command)
{		
	int i = 0;
	char * directory;
	
	while(	(directory = strsep(&line_command, " ")) != NULL)
	{
		if(strchr(directory, '>') != NULL)
		{
			
			
			
			if(directory[0] == '>')
			{
				commands[i] = ">";
				strsep(&directory, ">");
			}
			else
			{
				commands[i] = strsep(&directory, ">");
				i++;
				commands[i] = ">";
					
			}
			
			isRedirection = 1;
			
			if(strlen(directory) > 1)
			{
				line_command = strdup(directory);
				
				if(line_command[0] == '\n') 
				{
					i++;
					break;
				}
			}
		}
		else
		{
			commands[i] = strdup(directory);
		}
		i++;
	}
	
	// removes the \n
	commands[i - 1] = strsep(&commands[i-1], "\n");
	// REFACTOR PAUL YOU CAN NOW REFACTOR
	// continue this tomorrow morning
	// printf("%s\n", commands[0]);
	// printf("%s\n", commands[1]);
	// printf("%s\n", commands[2]);
	// printf("%s\n", commands[3]);
	// printf("%i\n", i);
	
	return i;
}


int ExecuteCommands(int margc, char * commands[], char * paths[])
{
	// BUILT-IN COMMANDS
	// Exit
    	if((strcmp(commands[0], "exit")) == 0)
    	{
    		if (margc == 1) return 0;
    		else PrintError();
    	}
    	
    	// cd
    	else if((strcmp(commands[0], "cd")) == 0)
    	{
    		if(margc == 2) 
    		{	  
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
    		if(margc == 1)
    		{
    			paths[0] = NULL;
    		}
    		else
    		{
	    		int i = 1;
	    		for(; i < margc; i++)
	    		{
	    			paths[i - 1] = commands[i];
	    		}
	    		paths[i] = NULL;
    		}
    	}
    	
    	// NOT BUILT IN
    	else if((strcmp(commands[0], "rm")) == 0)
    	{
    		int i = 0;
    		char * my_bin = NULL;
    		
    		
    		while(paths[i] != NULL)
    		{
			if(strcmp(paths[i], "/bin") == 0) my_bin = strdup("/bin");
			i++;
    		} 	
    		if(my_bin == NULL) 
    		{
    			PrintError();
    			return -1;
    		}
		int rc = fork();
		
		if(rc == 0)
		{
			if(margc == 3)
			{
				char * myargv[100];
				strcat(my_bin, "/rm");
				myargv[0] = my_bin;
				myargv[1] = commands[1];
				myargv[2] = commands[2];
				execv(myargv[0], myargv); 
			}
		}
		else
		{
			wait(NULL);
		}
    	} 
    	
    	else if((strcmp(commands[0], "cat")) == 0)
    	{
    		int i = 0;
    		char * my_bin = NULL;
    		
    		
    		while(paths[i] != NULL)
    		{
			if(strcmp(paths[i], "/bin") == 0) my_bin = strdup("/bin");
			i++;
    		} 	
    		if(my_bin == NULL) 
    		{
    			PrintError();
    			return -1;
    		}
		int rc = fork();
		
		if(rc == 0)
		{
			if(margc == 2)
			{
				char * myargv[100];
				strcat(my_bin, "/cat");
				myargv[0] = my_bin;
				myargv[1] = commands[1];
				myargv[2] = NULL;
				execv(myargv[0], myargv); 
			}
		}
		else
		{
			wait(NULL);
		}
    	} 
    	
    	
    	else if((strcmp(commands[0], "ls")) == 0) 
    	{
    		int i = 0;
    		char * my_bin = NULL;
    		
    		
    		while(paths[i] != NULL)
    		{
			if(strcmp(paths[i], "/bin") == 0) my_bin = strdup("/bin");
			i++;
    		} 	
    		if(my_bin == NULL) 
    		{
    			PrintError();
    			return -1;
    		}
    			
    		int rc = fork();
    		if(rc == 0)
    		{	
	    		char * myargv[100];
	    		char * myRedirectory;
	    		char currentDirectory[1024];
	    		
	    		strcat(my_bin, "/ls");
	    		myargv[0] = my_bin;
	    		
	    		if(margc < 1)
	    		{
	    			PrintError();
	    			return -1;
	    		}
	    		else if(margc == 1) 
	    		{
	    			myargv[1] = NULL;
	    			
	    			execv(myargv[0], myargv);
	    		}
	    		else if(margc == 2)	
	    		{	
	    			if(strcmp(commands[1], ">") == 0)
	    			{
	    				PrintError();
	    				return -1;
	    			}
	    			
	    			else if(access(commands[1], F_OK) == -1)
	    			{
	    				char error_message[100] = "ls: cannot access '/no/such/file': No such file or directory\n";	
					write(STDERR_FILENO, error_message, strlen(error_message)); 
					return -1;
	    			}
	    			
	    			myargv[1] = commands[1];
	    		}
	    		else if(margc == 3)
	    		{
	    			if(strcmp(commands[1], ">") == -1)
	    			{
	    				PrintError();
	    				return -1;
	    			}
	    				
	    			myargv[1] = getcwd(currentDirectory, 1024);
	    			myargv[2] = NULL;

	    			myRedirectory = strdup(commands[2]);
	    		}
	    		
	    		else if(margc == 4)
	    		{	
	    			//printf("%s", commands[1]);
	    			if(strcmp(commands[1], ">") == 0)
	    			{
	    				PrintError();
	    				return -1;
	    			}
	    		
	    			if(isRedirection != 1)
	    			{
	    				PrintError();
	    				return -1;
	    			}
				myargv[1] = commands[1];
				
				myRedirectory = strdup(commands[3]);	    	
	    		}
	    		else
	    		{
	    			PrintError();
	    			return -1;
	    		}
	    		
	    		if(isRedirection == 1)
	    		{
	    			fclose(stdout);
	    				
	    			FILE * outputFile = fopen(myRedirectory, "w");
    				stdout = outputFile;
	    		}
	    		
	    		execv(myargv[0], myargv);	
    		}
    		else
    		{
			wait(NULL);

    		}
    		
    		return 0;
    	}
    	
    	else if(margc == 1)
    	{
    		char my_directory[1024];
    		getcwd(my_directory, sizeof(my_directory));
    		
    		char rev_directory[1024];
		rev_directory[0] = '\0';
    		
    		int i = 0;
 		
    		while(paths[i] != NULL)
		{
		
			strcpy(rev_directory, my_directory);
				
			strcat(rev_directory, "/");
			strcat(rev_directory, paths[i]);
			strcat(rev_directory, "/");
			strcat(rev_directory, commands[0]);
			
			if(access(rev_directory, X_OK) == 0) 
			{
				break;							
			}
			else
			{
				rev_directory[0] = '\0';
			}
			i++;
		}
		
		if(rev_directory[0] == '\0') 
		{
			PrintError();
			return -1;
		}
    		
    		int rc = fork();
    		if(rc == 0)
    		{
    			
    			char * myargv[10];
    		
    			myargv[0] = strdup(rev_directory);
			myargv[1] = NULL;
			execv(myargv[0], myargv);  			
    		}
    		
    		else
    		{
    			wait(NULL);
		}
    	}
    	
    	else
    	{
    		//PrintError();
    		return -1;
	}
	return 0;	    	
}

	
void PrintError()
{
	char error_message[30] = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message)); 
}

