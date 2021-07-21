#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[])
{
    // wgrep

    
    char currentLetter;
    char * searchTerm = argv[1];
    int searchTermLength = 0;
    int searchTermCounter = 0;

    int checkPoint = 0;
    int counter = 0;
    int newLine = 0;
    int isTermFound = 0;

	
    if(argc <= 1)
    {
	printf("wgrep: searchterm [file ...]\n");
	return 1;
    } 
    
    if (argc == 2)
    {
   	char * line = NULL;
   	size_t linecap = 0;
   	
   	while(getline(&line, &linecap, stdin) > 0)
   	{
   		if(strstr(line, argv[1]))
   		{
   			printf("%s", line);
   		}
   	}
   	
   		
    	return 0;
    }
    
    
    

    FILE * fr;
    
    for(int i = 2; i < argc; i++)
    {
   	searchTermLength = 0;
    	searchTermCounter = 0;

    	checkPoint = 0;
    	counter = 0;
    	newLine = 0;
    	isTermFound = 0;

	
	    
	    fr = fopen(argv[i], "r");
	    
	    
	    if(fr == NULL)
	    {
	    	printf("wgrep: cannot open file\n");
	    	return 1;
	    }

	    while(searchTerm[searchTermLength] != '\0')
	    {
		searchTermLength++;
	    }

	    while(fread(&currentLetter, 1, 1, fr))
	    {
		// If newLine has just been detected previously thus we are now on the next line
		if(newLine == 1)
		{
		    checkPoint = counter;
		    newLine = 0;
		}

		// term has not been found
		if(isTermFound == 0)
		{
		    // if letter matched our searched term
		    if(currentLetter == searchTerm[searchTermCounter])
		    {
		        searchTermCounter++;
		    }

		    // letter did not match so we reset the counter
		    else
		    {
		        searchTermCounter = 0;
		    }

		    // we have successfully found the sequence of letters that comprises the search term
		    if(searchTermCounter == searchTermLength)
		    {
		        // Reset back our fp to the beginning of our current line
		        fseek(fr, checkPoint, SEEK_SET);
		        counter = checkPoint;
		        // Iterate again but Print the Line (char per char) we will stop if we reach '\n'
		        while(currentLetter != '\n')
		        {
		            fread(&currentLetter, 1, 1, fr);
		            printf("%c", currentLetter);
		            counter++;
		        }

		        searchTermCounter = 0;
		        counter--;
		    }
		}

		if(currentLetter == '\n')
		{
		    newLine = 1;
		}

		// use ftell to update our counter
		counter++;
	    }

	    fclose(fr);
    }
    return 0;
}

