#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[])
{
    int letterCount;
    char letter;

    FILE * fr;

    if(argc <= 1)
    {
    	printf("wunzip: file1 [file2 ...]\n");
    	return 1;
    }

    for(int i = 1; i < argc; i++)
    {
    fr = fopen(argv[i], "r");

    while(1)
    {
        fread(&letterCount, 1, 4, fr);
        fread(&letter, 1, 1, fr);

        if(feof(fr))
        {
            break;
        }

        for(int i = 0; i < letterCount; i++)
        {
            printf("%c", letter);
        }
    }
    fclose(fr);
	}
    return 0;
}
