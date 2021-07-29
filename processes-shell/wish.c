#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	char const exit[6] = "exit\n";

    	char * buffer;
    	size_t buffsize = 32;
	buffer = (char *) malloc(buffsize * sizeof(char));

	while(1)
	{
		printf("wish> ");
	    	getline(&buffer, &buffsize, stdin);
	    	
	    	if((strcmp(buffer, exit)) == 0) break;
	    	else if((strcmp(buffer, "ls\n")) == 0) 
	    	{
	    		int rc = fork();
	    		if(rc == 0)
	    		{
		    		char * myargv[10];
		    		myargv[0] = strdup("/bin/ls");
		    		myargv[1] = NULL;
		    		printf("Hello");
		    		
		    		execv(myargv[0], myargv);
	    		}
	    	}	
    	}
	return 0;
}

