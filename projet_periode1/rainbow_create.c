#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "reduce.h"

#define NC 4 // number of characters to hash
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

const unsigned long long HT_SIZE = N*10; // the size of the hashtable that will contain all passxL values

// global variable used to insert only unique passx0 values between the different files
unsigned long long passx0_counter = 0;

typedef struct ht_cell_t
{
    char *passL;
    struct ht_cell_t *next;
} ht_cell_t;

void free_cell(ht_cell_t *cell)
{
    // if the current cell has a next cell, free the next cell first
    if (cell->next) free_cell(cell->next);

    // then free the current cell
    free(cell->passL);
    free(cell);
}

void free_hashtable(ht_cell_t **ht)
{
    for (int i = 0; i < HT_SIZE; i++)
    {
        // free each non null cell recursively
        if (ht[i]) free_cell(ht[i]);
    }
}

unsigned long long hash_word(char *word)
{
    unsigned long long somme = 0;
    for (int i=0; i < MIN(strlen(word), NC) ; i ++) {
        somme += (word[i]-'a') * pow(26, i);
    }
    return somme%HT_SIZE;
}

/// @brief Checks if passL is already in the hashtable ht, adds passL to the hashtable if not already there
/// @param ht the hashtable
/// @param passL the password string
/// @return 1 if passL is already in the hahstable, else 0
int check_passL_in_hashtable(ht_cell_t **ht, char *passL) {
    int hash_passL = hash_word(passL);
    // if passL's hash has already a key-value in the hashtable, check if passL is in the linked list
    if (ht[hash_passL]) {
        ht_cell_t *current_cell = ht[hash_passL];
        while (current_cell->next != NULL && strcmp(current_cell->passL, passL) != 0)
        {
            current_cell = current_cell->next;
        } // current_cell->next == NULL || strcmp(current_cell->passL, passL) == 0

        // if passL is found in the hashtable, return 1
        if (strcmp(current_cell->passL, passL) == 0) {
            return 1;
        }
        
        ht_cell_t *new_cell = (ht_cell_t *) malloc(sizeof(ht_cell_t));
        new_cell->passL = strdup(passL);
        new_cell->next = NULL;
        current_cell->next = new_cell;

        return 0;
    } // ht[hash_passL] == NULL
    
    // if passL is not in the hashtable, add it
    ht_cell_t *new_cell = (ht_cell_t *) malloc(sizeof(ht_cell_t));
    new_cell->passL = strdup(passL);
    new_cell->next = NULL;
    ht[hash_passL] = new_cell;

    // return 0 as passL was not already in the hashtable
    return 0;
}

/// @brief Writes N pass0 passL tuples in a file, which corresponds to a N size rainbow table.
/// @param in_file file containing input passwords. If NULL, randomly generates those passwords.
/// @param out_file file where to write the tuples.
void create(FILE * in_file, FILE * out_file) {
    char * pass0;
    char * passL;

    ht_cell_t **hash_table = (ht_cell_t **) malloc(sizeof(ht_cell_t *) * HT_SIZE);

    // adding the N tuples to the file
    int i = 0;
    while (i < N) {
        // string allocation of the size of the password's length
        pass0 = (char*) malloc(sizeof(char) * M);
        passL = (char*) malloc(sizeof(char) * M);

        // if an input file is not provided, then randomly generates the input passwords "pass0"
        // else use the ones from the file
        if (in_file == NULL) {
            // use the reduction function to provide a unique string of M-characters for every integer
            reduce(passx0_counter, 0, pass0);
            passx0_counter++;
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

            // exit if the size of the password read from the file is not the right one
            if (strlen(pass0) != M) {
               printf("Le mot de passe '%s' lu dans le fichier donné n'a pas la bonne longueur (longueur = %ld, attendue = %d)\n", pass0, strlen(pass0), M);
            }
            if (line) free(line);
        }
        // first hash outside the main hash-reduction loop, to keep pass0's value in memory
        pwhash hashed = target_hash_function(pass0);
        // first reduction to have the same number of hash and reduction operations in the loop
        reduce(hashed, 0, passL);
        // hash-reduction loop, generating an L-chain
        for(int k=1; k<L; k++) {
            hashed = target_hash_function(passL);
            reduce(hashed, k, passL);
        }
        
        if (check_passL_in_hashtable(hash_table, passL) == 1) {
            free(pass0);
            free(passL);
            continue;
        }
        
        // writing pass0 passL tuple in the output file
        fprintf(out_file, "%s %s\n", pass0, passL);

        free(pass0);
        free(passL);
        i++;
    }

    free_hashtable(hash_table);
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
    
    // close the input file if provided
    if (input_file) fclose(input_file);
    return 0;
}
