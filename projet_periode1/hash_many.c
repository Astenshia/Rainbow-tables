#include <stdlib.h>
#include <stdio.h>
#include "hash.h"

int main(int argc, char const *argv[])
{
    if (argc < 2){
        printf("Usage : <input_file> <output_file>\n\n");
        exit(EXIT_FAILURE);
    }

    FILE *input_file = fopen(argv[1], "r");
    if (input_file == NULL)
    {
        printf("Input file could not be open.\n");
        exit(EXIT_FAILURE);
    }

    FILE *output_file = fopen(argv[2], "w");
    if (output_file == NULL)
    {
        printf("Output file could not be open.\n");
        exit(EXIT_FAILURE);
    }
    
    pwhash hash;

    char * line;
    size_t read;
    size_t len = 0;
    while ((read = getline(&line, &len, input_file)) != -1)
    {
        char * pwd = strtok(line, " \n");
        hash = target_hash_function(pwd);

        fprintf(output_file, "%llu\n", hash);
    }
    
    if (line) free(line);

    fclose(input_file);
    return 0;
}
