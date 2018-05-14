/*
 * Brandon Gotay - br109653 - 3858803
 * COP 3402-1 Fall 2017
 * 11/18/2017
 */

#include <string.h>
#include "core.h"

short l = 0, a = 0, v = 0;
FILE *fin, *fout;

int main(int argc, char **argv)
{
    int i = 2;
    if(argc > 6)
    {
        printf("Too many arguments!\n");
        return 1;
    }
    if(argc < 2)
    {
        printf("No arguments specified!");
        return 1;
    }

    fin = fopen(argv[1], "r");
    if(fin == NULL)
    {
        printf("%s\n", argv[1]);
        printf("The file %s was not found!\n", argv[1]);
        return 1;
    }

    if(argc > i && argv[i][0] != '-')
        fout = fopen(argv[i++], "w");
    else
        fout = fopen("log.txt", "w");


    if(fout == NULL)
    {
        printf("%s could not be created in this directory!\n", argv[2]);
        return 1;
    }

    for(; i < argc; i++)
    {
        if(directives(argv[i]))
        {
            fclose(fin);
            fclose(fout);
            return 1;
        }
    }

    analyzer();
    fclose(fin);
    if(HALT)
    {
        quit();
        return HALT;
    }

    parser();
    if(HALT)
    {
        quit();
        return HALT;
    }

    vm();

    quit();
    return 0;
}

// Activates compiler directives
int directives(char *arg)
{
    if(arg[0] != '-' || strlen(arg) > 2)
    {
        printf("%s is not a valid compiler directive!\n", arg);
        return 1;
    }

    switch(arg[1])
    {
    case 'l':
        l = 1;
        return 0;
    case 'a':
        a = 1;
        return 0;
    case 'v':
        v = 1;
        return 0;
    default:
        printf("%s is not a valid compiler directive!\n", arg);
        return 1;
    }
}

void quit()
{
    int i;
    fclose(fout);

    for(i = 0; i < lexeme_count; i++)
        free(lexeme_list[i].lexeme);
    free(lexeme_list);


}
