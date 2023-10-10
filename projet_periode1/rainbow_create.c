#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash.h"

/// @brief Reduction function used to create a rainbow table.
/// @param hash hash to be used to determine the next password
/// @param call reduction variation to prevent the same hash to get the same password
/// @param word string that will contain the next password.
void reduction(pwhash hash, int call, char *word) {
    unsigned long long modulo = (hash + call) % ((unsigned long long) pow(26, M));
    // if (call == L-1) printf("%ld\n", modulo);

    for (int i = M-1; i >= 0 ; i--)
    {
        word[i] = modulo / (unsigned long long) pow(26, i) + 'a';
        modulo = modulo % (unsigned long long) pow(26, i);
    }

    // for(int i=0; i<M; i++) {
    //     word[i] = (char) ((hash/(i+1) + call)%26) + 'a';
    // }
    // printf("%lu -> %s ->", hash, word);
}

/// @brief Writes N pass0 passL tuples in a file, which corresponds to a N size rainbow table.
/// @param in_file file containing oridinal input passwords. If NULL, randomly generates those passwords.
/// @param out_file file where to write the tuples.
void create(FILE * in_file, FILE * out_file) {
    char * pass0;
    char * passL;

    // adding the N tuples to the file
    for(int i = 0; i < N; i++) {
        // string allocation of the size of the password's length
        pass0 = (char*) malloc(sizeof(char) * M);
        passL = (char*) malloc(sizeof(char) * M);

        // if an input file is not provided, then randomly generates the input passwords "pass0"
        // else use the ones from the file
        if(in_file==NULL) {
            // random generation of M-long passowrd
            for(int j = 0; j < M; j++) {
                pass0[j] = (rand() % 26) + 'a';
            }
            // printf("%s -> ",pass0);
        }
        else {
            size_t len = 0;
            size_t read;
            char * line;
            read = getline(&line, &len, in_file);
            // removing newline character
            strtok(line, "\n");

            // pass0 cannot be used directly instead of line, as the file reading does not have the right size
            strcpy(pass0, line);

            // fread(pass0, sizeof(char), N, in_file);

            // exit if the size of the password read from the file is not the right one
            if (strlen(pass0) != M) {
               printf("Le mot de passe '%s' lu dans le fichier donné n'a pas la bonne longueur (longueur = %ld, attendue = %d)\n", pass0, strlen(pass0), M);
            }
            // printf("%s -> ",pass0);
        }
        
        // first hash outside the main hash-reduction loop, to keep pass0's value in memory
        pwhash hashed = target_hash_function(pass0);
        // first reduction to have the same number of hash and reduction operations in the loop
        reduction(hashed, 0, passL);

        // hash-reduction loop, generating an L-chain
        for(int k=1; k<L; k++) {
            hashed = target_hash_function(passL);
            reduction(hashed, k, passL);
        }
        // printf("\n\n");

        // writing pass0 passL tuple in the output file
        fprintf(out_file, "%s %s\n", pass0, passL);

        free(pass0);
        free(passL);
    }
}

int main(int argc, char const *argv[])
{
    if (argc < R + 1){
        printf("Usage : <file1>.....<file R><file R+1 = passx,0>\n\n");
        exit(EXIT_FAILURE);
    }

    // if an input file is provided, then reads it
    FILE *input_file = NULL;
    if (argc == R+2) {
        input_file = fopen(argv[R+1], "r");
    }

    // computing and writing N "pass0 passL" tuples into each file provided as parameter
    for(int r = 0; r < R; r++) {
        FILE * rfile = fopen(argv[r+1], "w");
        if(rfile == NULL) {
            printf("Error opening file %s", argv[r-1]);
        }
        // computing and writing the tuples
        create(input_file, rfile);
        fclose(rfile);  
    }
    
    // closing of the input file if provided
    if (input_file) fclose(input_file);
    
    // value testing
    pwhash mot = target_hash_function("testing");
    return 0;
}
