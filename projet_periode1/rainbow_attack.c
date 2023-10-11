#include <stdlib.h>
#include <stdio.h>
#include "reduce.h"

#define HT_SIZE N*10
#define NC 4 // number of characters to hash
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

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

void load_rb_file_in_hashtable(FILE * input_file, ht_cell_t **ht)
{
    char * line = NULL;
    size_t len = 0;
    size_t read;

    while ((read = getline(&line, &len, input_file)) != -1) {
        char * passx0 = strtok(line, " \n");
        char * passxL = strtok(NULL, " \n");

        check_passL_in_hashtable(ht, passxL);
    }

    if (line) free(line);
}

void print_cell(ht_cell_t *cell)
{
    printf(" -> %s\n", cell->passL);

    // if there is a next cell then print it
    if (cell->next) {
        print_cell(cell->next);
    }
}

void print_ht(ht_cell_t **ht)
{
    for (int i = 0; i < HT_SIZE; i++)
    {
        // if pointer at the current index in the hashtable is not NULL then print it
        if (ht[i]) {
            printf("[%d]\n", i);
            print_cell(ht[i]);
        }
    }
}

int main(int argc, char const *argv[])
{
    // checking the number of arguments
    if (argc < R + 3){
        printf("Usage : <file1>.....<file R><file R+1 = attacked><file R+2 = results>\n\n");
        exit(EXIT_FAILURE);
    }
    FILE * rainbow_file = fopen(argv[1], "r");
    ht_cell_t *ht[HT_SIZE] = {NULL};
    load_rb_file_in_hashtable(rainbow_file, ht);
    fclose(rainbow_file);

    print_ht(ht);

    free_hashtable(ht);

    return 0;
}
