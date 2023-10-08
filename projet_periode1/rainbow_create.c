#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash.h"

char * reduction(pwhash hash, int call) {
    char * word = malloc(sizeof(char) * M);
    for(int i=0; i<M; i++) {
        word[i] = (char) ((hash/(i+1) + call)%26) + 'a';
    }
    printf("%lu -> %s ->", hash, word);
    return word;
}

void create(FILE * in_file, FILE * out_file){
    char * pass0;
    char * passL;
    for(int i = 0; i < N; i++) {
        if(in_file==NULL) {
            pass0 = malloc(sizeof(char) * M);
            for(int j = 0; j < M; j++) {
                pass0[j] = (rand() % 26) + 'a';
            }
            printf("%s -> ",pass0);
            pwhash hashed = target_hash_function(pass0);
            char * word = reduction(hashed, 0); //jusque la ca marche
            for(int k=1; k<L; k++) {
                hashed = target_hash_function(word);
                word = reduction(hashed, k);
            }
            passL=word;
            printf("\n\n");
            fprintf(out_file, "%s %s\n", pass0, passL);
        }
        else {
            size_t len = 0;
            size_t read;
            read = getline(&pass0, &len, in_file);
            fread(pass0, sizeof(char), N, in_file);
            fprintf(out_file, pass0);
        }
    }
}

int main(int argc, char const *argv[])
{
    if (argc < R){
        printf("Usage : <file1>.....<file R><file R+1 = passx,0>\n\n");
        exit(EXIT_FAILURE);
    }

    FILE * input_file;
    if (argc == R+2) {
        input_file = fopen(argv[R+1], "r");
    }
    else {
        input_file = NULL;
    }
    for(int r = 0; r < R; r++) {
        FILE * rfile = fopen(argv[r+1], "w");
        if(rfile == NULL) {
            printf("Error opening file %s", argv[r-1]);
        }
        create(input_file, rfile);
        fclose(rfile);  
    }
    fclose(input_file);
    pwhash mot = target_hash_function("proute");
    return 0;
}
