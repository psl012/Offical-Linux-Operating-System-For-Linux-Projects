#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <sys/stat.h>

void PrintError();
int ParseCommand(char * commands[], char * line_command);
int ExecuteCommands(int margc, char * commands[], char * paths[]);
char * CheckCommandExists(char * paths[]);
void StandardArgvExec(char * my_bin, char * operation, char * commands[], int margc, int needDirectory);
void LSError();
int isRedirection = 0;
int IsBadDirectory(char * my_directory, char * my_operation);

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
		
		// Check if File is accessible
		if(access(argv[1], F_OK) == -1)
		{
			PrintError();
			return 1;
		}
		
		FILE * fr = fopen(argv[1], "r");
		
		while(getline(&command, &linecap, fr) > 0)
   		{ 
   			isRedirection = 0;
			int margc = ParseCommand(command_lines, command);
			if(margc >= 0) ExecuteCommands(margc, command_lines, paths);
		}
	}
	else if(argc > 2)
	{
		PrintError();
		return 1;
	}
	
	return 0;
}

int ParseCommand(char * commands[], char * line_command)
{		
	int i = 0;
	char * directory;
	int whiteStart = 0;
	

	// for whitespace on start
	for(; whiteStart < strlen(line_command); whiteStart++)
	{
		if(isspace(line_command[whiteStart]) == 0)
		{
			break;
		}
	}
	
	if(whiteStart == strlen(line_command))
	{
		return -1;
	}


	int j = 0;
	for(int i = 0; i < strlen(line_command); i++)
	{
		if(line_command[i] != ' ' && line_command[i] != '\t') 
		{
			line_command = &line_command[i];
			break;
		}
	}
	
	for(int i = strlen(line_command) - 2; i >= 0; i--)
	{
		
		if(line_command[i] != ' '  && line_command[i] != '\t') 
		{
			line_command[i+1] = '\0' ;
			break;
		}
	}
	
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

	commands[i - 1] = strsep(&commands[i-1], "\n");

	// printf("%s\n", commands[0]);
	// printf("%s\n", commands[1]);
	// printf("%s\n", commands[2]);
	// printf("%s\n", commands[3]);
	// printf("%i\n", i);
	
	return i;
}


int ExecuteCommands(int margc, char * commands[], char * paths[])
{
	// errors
	if(strcmp(commands[0], ">") == 0)
	{
		PrintError();
		return -1;
	}

	// BUILT-IN COMMANDS
	// Exit
    	else if((strcmp(commands[0], "exit")) == 0)
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
    	
    	// hard-coded error for singeline only containing &
    	else if(commands[0][0] == '&')
    	{
    		return 0;
    	}
    	
    	// NOT BUILT IN
    	else if((strcmp(commands[0], "rm")) == 0)
    	{
    		char * my_bin;	
 	 		
    		if((my_bin = CheckCommandExists(paths)) == NULL) 
    		{
    			PrintError();
    			return 1;	
    		}
    		
		int rc = fork();
		if(rc == 0)
		{
			if(margc == 3)
			{			
				StandardArgvExec(my_bin, "/rm", commands, margc, 1);	
			}
		}
		else
		{
			wait(NULL);
		}
    	} 
    	
    	else if((strcmp(commands[0], "echo")) == 0) 
    	{
    		char * my_bin;	
 	 		
    		if((my_bin = CheckCommandExists(paths)) == NULL) 
    		{
    			PrintError();
    			return 1;	
    		}
  	 			
    		int rc;
    		
    		char * processed_commands[margc];
    	  		
    		int pc = 0;
    		int p_index = 0;
    		int start_index = 0;
    		 		
    		for(; pc < margc; pc++)
    		{
    			if(commands[pc][0] != '&')
    			{
    				processed_commands[p_index] = strdup(commands[pc]);
    				p_index++;
			}
    		
			if(commands[pc][0] == '&' || pc == margc - 1)
    			{
    				p_index = 0;
				
				
				rc = fork();
		    		if(rc == 0)
		    		{		
    					if(commands[pc][0] == '&') StandardArgvExec(my_bin, "/echo", processed_commands, pc - start_index, 0);			    
		    			else StandardArgvExec(my_bin, "/echo", processed_commands, pc - start_index + 1, 0);			    
		    		}
		    		else
		    		{
					start_index =  pc;
		    		}
    			}    			
    		}
    		
    		if(rc != 0)
    		{
    			wait(NULL);
    		}
    		/**
    		rc = fork();
    		if(rc == 0)
    		{	
    			StandardArgvExec(my_bin, "/echo", commands, margc, 0);		    		
    		}
    		else
    		{
			wait(NULL);
    		}
    			*/
    		return 0;
    	}
    	
    	else if((strcmp(commands[0], "cat")) == 0)
    	{
		char * my_bin;	
 	 	
    		if((my_bin = CheckCommandExists(paths)) == NULL) 
    		{
    			PrintError();
    			return 1;	
    		}
    		
		int rc = fork();
		if(rc == 0)
		{
			if(margc == 2)
			{
				StandardArgvExec(my_bin, "/cat", commands, margc, 1);
			}
		}
		else
		{
			wait(NULL);
		}
    	} 
    	
    	else if((strcmp(commands[0], "ls")) == 0) 
    	{
    		char * my_bin;	
 	 		
    		if((my_bin = CheckCommandExists(paths)) == NULL) 
    		{
    			PrintError();
    			return 1;	
    		}
  	 			
    		int rc = fork();
    		if(rc == 0)
    		{	
    			StandardArgvExec(my_bin, "/ls", commands, margc, 1);		    		
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

void LSError()
{
	char error_message[100] = "ls: cannot access '/no/such/file': No such file or directory\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
}

char * CheckCommandExists(char * paths[])
{
	char * my_bin = NULL;    		
	int i = 0;
	
	while(paths[i] != NULL)
	{
		if(strcmp(paths[i], "/bin") == 0) my_bin = strdup("/bin");
		i++;
	} 	
	if(my_bin == NULL) 
	{
		return NULL;
	}
	
	return my_bin;
}

int IsBadDirectory(char * my_directory, char * my_operation)
{
	char * trunc_operation = strdup(my_operation);
	trunc_operation = &trunc_operation[1];
	
	if(access(my_directory, F_OK) != 0 && strcmp(my_directory, trunc_operation) != 0)
	{
		return 1;
	}
	
	return 0;
}

int IsBadOutputDirectory(char * my_directory)
{
	char * processedDir;
	processedDir = strdup(my_directory);
	
	int endLim = strlen(my_directory) - 1;
	

	
	for(int i = endLim; endLim >= 0; i--)
	{
		if(my_directory[i] == '/')
		{
			processedDir[i] = '\0';
			if(access(processedDir, F_OK) != 0)
			{
				return 1;
			}
			break;
		}
	}

	return 0;
}

void StandardArgvExec(char * my_bin, char * operation, char * commands[], int margc, int needDirectory)
{
	char * myargv[1024];
	strcat(my_bin, operation);
	
	struct stat sb;
	int isHaveInputDirectory = 0;
	int isErrorDirectory = 0;
	
	myargv[0] = my_bin;

	if(isRedirection == 0)
	{
		
		// bad input directory
		if((isErrorDirectory = IsBadDirectory(commands[margc - 1], operation)) == 1 && needDirectory == 1) 
		{
			if(strcmp(operation, "/ls") != 0)
			{  
				PrintError();
				return;
			}
		}		

		int i = 1;
		for(; i < margc; i++)
		{
			myargv[i] = commands[i];
			if(stat(commands[i], &sb) == 0 && S_ISDIR(sb.st_mode))
			{
				isHaveInputDirectory = 1;
			}
		}
		
		if(isHaveInputDirectory == 0 && isErrorDirectory == 0)
		{
			i++;
			char *buff;
			myargv[i] = getcwd(buff, 1024);	
		}

		if(strcmp(operation, "/ls") == 0 && access(myargv[i], F_OK) != 0)
		{
			LSError();

			return;
		}
		
		myargv[i + 1] = NULL;	
		
		
	}
	else
	{
		// redirection with no output directory
		if(strcmp(commands[margc - 1], ">") == 0)
		{
			PrintError();
			return;
		}
		
		int j = 0;
		int i = 1;
		for(; i < margc; i++)
		{
			if(strcmp(commands[i], ">") == 0) 
			{
				j = i + 1;
				break;
			}
			myargv[i] = commands[i];
			
			if(stat(commands[i], &sb) == 0 && S_ISDIR(sb.st_mode))
			{
				isHaveInputDirectory = 1;
			}
			
		}
							
		// more than one output directory
		if(margc - j > 1)
		{
			PrintError();
			return;
		}
		
		// bad output Directory
		if((isErrorDirectory = IsBadOutputDirectory(commands[margc - 1])) == 1) 
		{
			PrintError();
			return;	
		}
		
		
		// bad input directory
		if((isErrorDirectory = IsBadDirectory(commands[i - 1], operation)) == 1 && needDirectory == 1) 
		{
			if(strcmp(operation, "/ls") != 0)
			{  
				PrintError();
				return;
			}
		}
		
			
		if(isHaveInputDirectory == 0 && isErrorDirectory == 0)
		{
			char *buff;
			myargv[i] = getcwd(buff, 1024);
		}
		

		if(strcmp(operation, "/ls") == 0 && access(myargv[i - 1], F_OK) != 0)
		{
			LSError();
			return;
		}
		
		myargv[i+1] = NULL;
		
		fclose(stdout);
		FILE * outputFile = fopen(commands[j], "w");
		stdout = outputFile;	
	}
	
	
	execv(myargv[0], myargv);
	
	
}

