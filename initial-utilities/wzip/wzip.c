#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[])
{

    char * strData = NULL;
    char * strData2 = NULL;
    char * comStrData = NULL;
    int startOfFileIndex = 0;
    int endOfFileIndex = 0;
    size_t memo = 100000000;

    strData = malloc(memo);
    strData2 = malloc(memo);
    comStrData = malloc(memo*9);

    FILE * fr;

    if(argc <= 1)
    {
    	printf("wzip: file1 [file2 ...]\n");
    	return 1;
    }

    for(int i = 1; i < argc; i++)
    {	
	    fr = fopen(argv[i], "r");
	    while(1)
	    {
		    fgets(strData, 2, fr);
		    
		    if(feof(fr))
		    {
		    	break;
		    }
		    
		    endOfFileIndex = startOfFileIndex + strlen(strData);
		    for(int j = startOfFileIndex; j < endOfFileIndex; j++)
		    {
		    	comStrData[j] = strData[j - startOfFileIndex];  
		    }
		    startOfFileIndex = endOfFileIndex;
	    }
	    fclose(fr);
    }
    
    //printf("%s", comStrData);
    
    endOfFileIndex = strlen(comStrData);
    char currentLetter = comStrData[0];
    int currentLetterCount = 1;
    
    for(int i = 1; i < endOfFileIndex + 1; i++)
    {
    	if(comStrData[i] == comStrData[i-1])
    	{
    		currentLetterCount++;
    		
    	}
    	else
    	{	
    		//printf("%d", currentLetterCount);
    		fwrite(&currentLetterCount, 4, 1, stdout);
    		fwrite(&comStrData[i-1], 1, 1, stdout);
    		currentLetterCount = 1;
    	}
    	
    }
     
    return 0;
}

