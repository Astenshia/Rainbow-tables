#include <stdlib.h>
#include <stdio.h>
#include "hash.h"

int main(int argc, char const *argv[])
{
    // checking the number of arguments
    if (argc < R + 3){
        printf("Usage : <file1>.....<file R><file R+1 = attacked><file R+2 = results>\n\n");
        exit(EXIT_FAILURE);
    }

    
    return 0;
}
