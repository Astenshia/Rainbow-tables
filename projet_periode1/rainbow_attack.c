#include <stdlib.h>
#include <stdio.h>
#include "reduce.h"

int passL_in_file(char * pass, FILE * r_file) {
    printf("pif.1\n");

    int nc;
    char * line, * passL;
    size_t len = 0;
    size_t read;
    printf("pif.2\n");
    if (r_file == NULL) printf("NULL\n");
    // read lines in R file
    while (read = getline(&line, &len, r_file)) {
        // Count number of chars until whitespace
        printf("pif.3\n");
        nc = 1;
        while(fgetc(r_file) != ' ') {
            nc += 1;
        }
        printf("pif.4\n");
        // Compare passL and pass
        passL = line + nc;
        if(strcmp(pass, passL) == 0) {
            printf("FOUND !\n");
            return 1;
        }
        printf("pif.5\n");
    }

    printf("pif - fin\n");

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
    char pass[M];
    size_t len, read;
    pwhash hash;
    int call = 0, found = 0;

    input_file = fopen(argv[R+1], "r");
    output_file = fopen(argv[R+2], "r");
    printf("a\n");
    while(read = getline(&line, &len, input_file)) {
        printf("b\n");
        hash = atoi(line);
        reduce(hash, call, pass);
        printf("c\n");
        // Do L times hash and reduction
        for(int l=0; l<L; l++) {
            // Get new word from hash
            // Test if word in files

            printf("d\n");
            for(int r=1; r<R; r++) {
                printf("d.1\n");
                r_file = fopen(argv[r], "w");
                printf("d.2\n");
                found = passL_in_file(pass, r_file);
                printf("d.3\n");
                if(found) {
                    break;
                }
                printf("d.4\n");
                fclose(r_file);
            }
            printf("e\n");
            if(found) {
                printf("%lu hash gives %s pass\n", hash, pass);
                fprintf(output_file, "%s\n", pass);
                break;
            }
            else {
                printf("%lu hash not in files, compute next pass\n", hash);
            }
            printf("f\n");
            // Current pass not in R files, compute next pass
            call += 1;
            hash = target_hash_function(pass);
            reduce(hash, call, pass);
            printf("g\n");
        }
        printf("h\n");
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
