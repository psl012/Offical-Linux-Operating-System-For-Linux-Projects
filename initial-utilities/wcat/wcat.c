#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    FILE * fp;
    char str[100];

    if(argc <= 1)
    {
        return 0;
    }

    else
    {
        for(int i = 1; i < argc; i++)
        {
            fp = fopen(argv[i], "r");

            if(fp == NULL)
            {
                printf("wcat: cannot open file\n");
                return 1;
            }

            while(1)
            {
                if(fgets(str, 100, fp) != NULL)
                {
                    printf("%s", str);
                }
                else
                {
                    break;
                }
            }

            fclose(fp);
        }
    }
    return 0;
}

