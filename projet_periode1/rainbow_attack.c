#include <stdlib.h>
#include <stdio.h>
#include "reduce.h"

int passL_in_file(char * pass, FILE * r_file) {
    int nc, read;
    char * line, * passL;
    // read lines in R file
    while (read = getline(&line, NULL, r_file)) {
        // Count number of chars until whitespace
        nc = 1;
        while(fgetc(r_file) != ' ') {
            nc += 1;
        }
        // Compare passL and pass
        passL = line + nc;
        if(strcmp(pass, passL) == 0) {
            printf("FOUND !\n");
            return 1;
        }
    }
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
    char pass[M];
    size_t len, read;
    pwhash hash;
    int call = 0, found = 0;

    input_file = fopen(argv[R+1], "r");
    output_file = fopen(argv[R+2], "r");

    while(read = getline(&line, &len, input_file)) {
        hash = atoi(line);
        reduce(hash, call, pass);
        // Do L times hash and reduction
        for(int l=0; l<L; l++) {
            // Get new word from hash
            // Test if word in files

            for(int r=0; r<R; r++) {
                r_file = fopen(argv[r], "w");
                found = passL_in_file(pass, r_file);
                if(found) {
                    break;
                }
                fclose(r_file);
            }
            if(found) {
                printf("%lu hash gives %s pass\n", hash, pass);
                fprintf(output_file, "%s\n", pass);
                break;
            }
            else {
                printf("%lu hash not in files, compute next pass\n", hash);
            }
            // Current pass not in R files, compute next pass
            call += 1;
            hash = target_hash_function(pass);
            reduce(hash, call, pass);
        }
        // Pass not found after L reductions
        if(!found) {
            printf("%s hash does not give any correct pass after L reductions\n", line);
            fprintf(output_file, "\n");
        }
    }
    fclose(input_file);
    fclose(output_file);
    return 0;
}
