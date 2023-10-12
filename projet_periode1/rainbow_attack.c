#include <stdlib.h>
#include <stdio.h>
#include "reduce.h"

#define NC M // number of characters to hash
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

const unsigned long long HT_SIZE = N*10*R; // the size of the hashtable that will contain all passxL values

typedef struct ht_cell_t
{
    char *pass0;
    struct ht_cell_t *next;
} ht_cell_t;

void free_cell(ht_cell_t *cell)
{
    // if the current cell has a next cell, free the next cell first
    if (cell->next) free_cell(cell->next);

    // then free the current cell
    free(cell->pass0);
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

void add_passx0_to_hashtable_at_address_passxL(ht_cell_t **ht, char *passx0, char *passxL) {
    int hash_passL = hash_word(passxL);
    // if passL's hash has already a key-value in the hashtable, check if passL is in the linked list
    if (ht[hash_passL]) {
        ht_cell_t *current_cell = ht[hash_passL];
        while (current_cell->next != NULL && strcmp(current_cell->pass0, passx0) != 0)
        {
            current_cell = current_cell->next;
        } // current_cell->next == NULL || strcmp(current_cell->pass0, passL) == 0

        // if passL is found in the hashtable, return 1
        if (strcmp(current_cell->pass0, passx0) == 0) {
            return;
        }
        
        ht_cell_t *new_cell = (ht_cell_t *) malloc(sizeof(ht_cell_t));
        new_cell->pass0 = strdup(passx0);
        new_cell->next = NULL;
        current_cell->next = new_cell;

        return;
    } // ht[hash_passL] == NULL
    
    // if passL is not in the hashtable, add it
    ht_cell_t *new_cell = (ht_cell_t *) malloc(sizeof(ht_cell_t));
    new_cell->pass0 = strdup(passx0);
    new_cell->next = NULL;
    ht[hash_passL] = new_cell;

    // return 0 as passL was not already in the hashtable
    return;
}

void load_rainbow_file_in_hashtable(FILE * input_file, ht_cell_t **ht)
{
    char * line = NULL;
    size_t len = 0;
    size_t read;

    while ((read = getline(&line, &len, input_file)) != -1) {
        char * passx0 = strtok(line, " \n");
        char * passxL = strtok(NULL, " \n");

        add_passx0_to_hashtable_at_address_passxL(ht, passx0, passxL);
    }

    if (line) free(line);
}

int attack_hash_with_rainbow_table(pwhash hash, ht_cell_t **ht, FILE *res_file)
{
    // printf("\n\n###################################################\n");
    // printf("Hash Base = %llu\n", hash);
    pwhash current_hash = hash;
    char * pwd = (char *) malloc(sizeof(char) * M);
    for (int i = 0; i < L; i++)
    {
        pwhash current_hash = hash;
        reduce(current_hash, L-i-1, pwd);
        //printf("%d | %llu -> %s", L-i-1, hash, pwd);
        for (int j = 1; j <= i; j++)
        {
            current_hash = target_hash_function(pwd);
            reduce(current_hash, L-i-1+j, pwd);
            //printf(" -> %llu -> %s", current_hash, pwd);
        }

        if (ht[hash_word(pwd)]) {
            // printf("\n=======%llu\n", hash_word(pwd));
            // printf(" PASSXL FOUND!!! passx0 = %s", pwd);
            ht_cell_t *current_cell = ht[hash_word(pwd)];
            while (current_cell != NULL)
            {
                pwd = strdup(current_cell->pass0);

                for (int k = 0; k < L-i-1; k++)
                {
                    current_hash = target_hash_function(pwd);
                    reduce(current_hash, k, pwd);
                }

                // printf("\nRecomputed base hash = %llu (with pwd = %s)\n", target_hash_function(pwd), pwd);
                if (target_hash_function(pwd) == hash) {
                    // printf("Found: %s (hash = %llu)\n", pwd, target_hash_function(pwd));
                    
                    // write in file
                    fprintf(res_file, "%s\n", pwd);

                    return 1;
                }

                current_cell = current_cell->next;
            } // current_cell == NULL
        }
    }
    return 0;
}

void attack_file_with_rainbow_table(FILE *input_file, ht_cell_t **ht, const char *path_to_res_file)
{
    char * hash = NULL;
    size_t len = 0;
    size_t read;
    
    FILE * res_file = fopen(path_to_res_file, "w");

    int res = -1;
    while ((read = getline(&hash, &len, input_file)) != -1) {
        hash = strtok(hash, " \n");

        res = attack_hash_with_rainbow_table(strtoull(hash, NULL, 10), ht, res_file);
        if (res == 0) {
            fprintf(res_file, "\n");
        }
    }
    fclose(res_file);

    if (hash) free(hash);
}

void print_cell(ht_cell_t *cell)
{
    printf(" -> %s\n", cell->pass0);

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
    if (argc < R + 2){
        printf("Usage : <file1>.....<file R><file R+1 = attacked><file R+2 = results>\n\n");
        exit(EXIT_FAILURE);
    }
    // initialize a hashtable that can contain all passxL values
    ht_cell_t **hash_table = (ht_cell_t **) malloc(sizeof(ht_cell_t *) * HT_SIZE);

    // load all passxL values contained in multiple files into the hashtable
    for (int i = 0; i < R; i++)
    {
        FILE * rainbow_file = fopen(argv[i+1], "r");
        load_rainbow_file_in_hashtable(rainbow_file, hash_table);
        fclose(rainbow_file);
    }

    FILE * attacked_file = fopen(argv[R + 1], "r");
    attack_file_with_rainbow_table(attacked_file, hash_table, argv[R + 2]);
    fclose(attacked_file);

    free_hashtable(hash_table);
    exit(EXIT_SUCCESS);
}