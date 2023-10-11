#include <stdlib.h>
#include <stdio.h>
#include "reduce.h"

int passL_in_file(char * pass, FILE * r_file) {

    int nc;
    char * line;
    char * passL = (char *) malloc(sizeof(char) * M);
    size_t len = 0;
    size_t read;
    // if (r_file == NULL) printf("NULL\n");
    // read lines in R file
    while ((read = getline(&line, &len, r_file)) != -1) {
        nc = 0;
        while(nc < M+1) {
            fgetc(r_file);
            nc += 1;
        }
        passL = strtok(line, "\n");
        //printf("passL [%s]\n", passL);
        if(strcmp(pass, passL) == 0) {
            printf("FOUND : %s\n", passL);
            return 1;
        }
    }
    if (line) free(line);
    return 0;
}

int main(int argc, char const *argv[])
{
    // checking the number of arguments
    if (argc < R + 3){
        printf("Usage : <file1>.....<file R><file R+1 = attacked><file R+2 = results>\n\n");
        exit(EXIT_FAILURE);
    }

    FILE * input_file, * output_file, * r_file;
    char * line;
    char * pass = (char *) malloc(sizeof(char) * M);
    size_t len, read;
    pwhash hash;
    int call = 0, found = 0;

    input_file = fopen(argv[R+1], "r");
    if(input_file == NULL) {
        printf("error opening file %s\n\n",argv[R+1]);
        exit(EXIT_FAILURE);
    }
    output_file = fopen(argv[R+2], "w");
    if(output_file == NULL) {
        printf("error opening file %s\n\n",argv[R+2]);
        exit(EXIT_FAILURE);
    }

    while(read = getline(&line, &len, input_file)) {
        hash = atoi(line);
        reduce(hash, call, pass);
        printf("HASH : %lu\n", hash);
        // Do L times hash and reduction
        for(int l=0; l<L; l++) {
            // Get new word from hash
            // Test if word in files
            for(int r=1; r<R; r++) {
                r_file = fopen(argv[r], "r");
                found = passL_in_file(pass, r_file);
                if(found) {
                    break;
                }
                fclose(r_file);
            }
            if(found) {
                fprintf(output_file, "%s\n", pass);
                break;
            }
            // Current pass not in R files, compute next pass
            call += 1;
            hash = target_hash_function(pass);
            reduce(hash, call, pass);
        }
        // Pass not found after L reductions
        if(!found) {
            fprintf(output_file, "\n");
        }
    }
    fclose(input_file);
    fclose(output_file);
    return 0;
}
