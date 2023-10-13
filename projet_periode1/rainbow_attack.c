#include <stdlib.h>
#include <stdio.h>
#include "reduce.h"
#include <pthread.h>

#define NC M // number of characters to hash
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define NB_THREADS 7 // number of threads created by this program

#define BUF_SIZE 65536 // buffer size to read lines

const unsigned long long HT_SIZE = N*10*R; // the size of the hashtable that will contain all passxL values

/// @brief cell for linked lists, can be used in hashtables
typedef struct ht_cell_t
{
    char *pass0;
    struct ht_cell_t *next;
} ht_cell_t;

/// @brief free the provided cell and its next ones
/// @param cell the cell to free
void free_cell(ht_cell_t *cell)
{
    // if the current cell has a next cell, free the next cell first
    if (cell->next) free_cell(cell->next);

    // then free the current cell
    free(cell->pass0);
    free(cell);
}

/// @brief free all cells of the provided hashtable
/// @param ht hashtable
void free_hashtable(ht_cell_t **ht)
{
    for (int i = 0; i < HT_SIZE; i++)
    {
        // free each non null cell recursively
        if (ht[i]) free_cell(ht[i]);
    }
}

/// @brief hash function used to hash strings
/// @param word the string to hash
/// @return the address associated with the provided string, between 0 and HT_SIZE
unsigned long long hash_word(char *word)
{
    unsigned long long somme = 0;
    for (int i=0; i < MIN(strlen(word), NC) ; i ++) {
        somme += (word[i]-'a') * pow(26, i);
    }
    return somme%HT_SIZE;
}

/// @brief Add a passx0 value in the provided hashtable, at the passxL address 
/// @param ht the hashtable in which passx0 is inserted
/// @param passx0 a M-long string
/// @param passxL a M-long string
void add_passx0_to_hashtable_at_address_passxL(ht_cell_t **ht, char *passx0, char *passxL) {
    int hash_passL = hash_word(passxL);
    // if passxL's hash has already a key-value in the hashtable, check if passx0 is in the linked list
    if (ht[hash_passL]) {
        ht_cell_t *current_cell = ht[hash_passL];
        while (current_cell->next != NULL && strcmp(current_cell->pass0, passx0) != 0)
        {
            current_cell = current_cell->next;
        } // current_cell->next == NULL || strcmp(current_cell->pass0, passL) == 0

        // if pass0 is already in the hashtable, exit the function
        if (strcmp(current_cell->pass0, passx0) == 0) {
            return;
        }
        
        // else add pass0 to the hashtable in the linked list and then exit
        ht_cell_t *new_cell = (ht_cell_t *) malloc(sizeof(ht_cell_t));
        new_cell->pass0 = strdup(passx0);
        new_cell->next = NULL;
        current_cell->next = new_cell;

        return;
    } // ht[hash_passL] == NULL
    
    // if passx0 is not in the hashtable at the address passxL, add it
    ht_cell_t *new_cell = (ht_cell_t *) malloc(sizeof(ht_cell_t));
    new_cell->pass0 = strdup(passx0);
    new_cell->next = NULL;
    ht[hash_passL] = new_cell;
    return;
}

/// @brief load all (passxL, passx0) tuples in the provided hash table as (key, value)
/// @param input_file the file to extract the tuples from.
/// @param ht the hashtable in which the tuple is inserted
void load_rainbow_file_in_hashtable(FILE * input_file, ht_cell_t **ht)
{
    char * line = NULL;
    size_t len = 0;
    size_t read;

    // read all lines of the file
    while ((read = getline(&line, &len, input_file)) != -1) {

        // extract passx0 and passxL
        char * passx0 = strtok(line, " \n");
        char * passxL = strtok(NULL, " \n");

        // add the tuple to the hash table
        add_passx0_to_hashtable_at_address_passxL(ht, passx0, passxL);
    }

    if (line) free(line);
}

/// @brief Attack a specific hash using a rainbow table
/// @param hash the hash to attack
/// @param ht the hashtable used as rainbow table
/// @param res_file the file into which the cracked password is printed
/// @return 1 if the hash was found in the hashtable, else O
int attack_hash_with_rainbow_table(pwhash hash, ht_cell_t **ht, FILE *res_file)
{
    // copy the hash so the original hash can be used later to compare with the hash of the found password
    pwhash current_hash = hash;
    char * pwd = (char *) malloc(sizeof(char) * M);

    // double loop to check the hash at every position in the line
    // as the reduction function changes from one hash to another, we need to test every positions where the hash could have been
    for (int i = 0; i < L; i++)
    {
        pwhash current_hash = hash;
        reduce(current_hash, L-i-1, pwd);

        // re create the rainbow chain, starting from the hash's position, chain of length i-1
        for (int j = 1; j <= i; j++)
        {
            current_hash = target_hash_function(pwd);
            reduce(current_hash, L-i-1+j, pwd);
        }

        // for every chain created, if passxL is in the hashtable, find the associated passx0 value
        if (ht[hash_word(pwd)]) {

            // for every passx0, find the password candidate
            ht_cell_t *current_cell = ht[hash_word(pwd)];
            while (current_cell != NULL)
            {
                pwd = strdup(current_cell->pass0);

                // find the candidate associated with the current passx0
                for (int k = 0; k < L-i-1; k++)
                {
                    current_hash = target_hash_function(pwd);
                    reduce(current_hash, k, pwd);
                }

                // if the candidate's hash if the same as the attacked hash, write it in the return file
                if (target_hash_function(pwd) == hash) {
                    fprintf(res_file, "%s\n", pwd);

                    // return 1 as the password is found
                    return 1;
                }

                current_cell = current_cell->next;
            } // current_cell == NULL
        }
    }
    return 0;
}

/// @brief Attack all hashes contained in a file using a hashtable
/// @param input_file file from which to read the hashes
/// @param ht the hashtable used as rainbow table 
/// @param path_to_res_file path ot the output file, where to write the found password
void attack_file_with_rainbow_table(FILE *input_file, ht_cell_t **ht, const char *path_to_res_file)
{
    char * hash = NULL;
    size_t len = 0;
    size_t read;
    
    FILE * res_file = fopen(path_to_res_file, "w");

    // read all lines of the file to attack
    int res = -1;
    while ((read = getline(&hash, &len, input_file)) != -1) {
        // read the hash
        hash = strtok(hash, " \n");

        // attack the hash, and if the hash was not cracked, prints an empty line in the output file
        res = attack_hash_with_rainbow_table(strtoull(hash, NULL, 10), ht, res_file);
        if (res == 0) {
            fprintf(res_file, "\n");
        }
    }
    fclose(res_file);

    if (hash) free(hash);
}

/// @brief Utils function used to print the provided cell.
/// @param cell the cell to print
void print_cell(ht_cell_t *cell)
{
    printf(" -> %s\n", cell->pass0);

    // if there is a next cell then print it
    if (cell->next) {
        print_cell(cell->next);
    }
}

/// @brief Utils function used to print all cells of a hashtable
/// @param ht the hashtable to print
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

/// @brief Function used to quickly count the number of lines in a file. Found here: https://stackoverflow.com/questions/12733105/c-function-that-counts-lines-in-file, by https://stackoverflow.com/users/3722288/mike-siomkin.
/// @param file 
/// @return the number of lines
int count_lines(FILE* file)
{
    char buf[BUF_SIZE];
    int counter = 0;
    for(;;)
    {
        size_t res = fread(buf, 1, BUF_SIZE, file);
        if (ferror(file))
            return -1;

        int i;
        for(i = 0; i < res; i++)
            if (buf[i] == '\n')
                counter++;

        if (feof(file))
            break;
    }

    return counter;
}

void split_attacked_file(const char * attacked_file_path)
{
    FILE * attacked_file = fopen(attacked_file_path, "r");
    if (attacked_file == NULL) {
        printf("Attacked file could not be open.\n");
        exit(EXIT_FAILURE);
    }
    int nb_lines = count_lines(attacked_file);
    fclose(attacked_file);

    char * line;
    size_t read;
    size_t len = 0;

    // re open the file to get a pointer at the beginning again
    attacked_file = fopen(attacked_file_path, "r");
    FILE *tmp_file;
    for (int i = 0; i < NB_THREADS; i++)
    {
        // get the number as a string
        int length = snprintf(NULL, 0, "%d", i);
        char *number = malloc(sizeof(char) * (length + 1));
        snprintf(number, length + 1, "%d", i);
        
        // create a file path of form "tmp/plit<i>.txt" with <i> being the value of variable i
        char file_path[14] = "tmp/split";
        strcat(file_path, number);
        strcat(file_path, ".txt");  

        // open the corresponding file in write mode
        tmp_file = fopen(file_path, "w");

        // copy lines from the original attacked file
        int line_counter = 0;
        while ((read = getline(&line, &len, attacked_file)) != -1)
        {
            // remove the line return character
            line = strtok(line, "\n");

            // print in file
            fprintf(tmp_file, "%s\n", line);

            // line counter increment
            line_counter++;
            if (line_counter >= nb_lines/NB_THREADS + 1) break;
        }
        if (line) free(line);
        
        fclose(tmp_file);
        free(number);
    }
}

typedef struct attacked_split_file {
    const char *input_file_path;
    const char *output_file_path;
    ht_cell_t **hash_table;
} cpargs;

void * thread_attack_file_with_rainbow_table(void * args) {
    cpargs * arg = (cpargs *) args;
    const char *input_file_path = strdup(arg->input_file_path);
    const char *output_file_path = strdup(arg->output_file_path);
    ht_cell_t **ht = arg->hash_table;

    char * hash = NULL;
    size_t len = 0;
    size_t read;
    
    FILE * input_file = fopen(input_file_path, "r");
    FILE * res_file = fopen(output_file_path, "w");

    // read all lines of the file to attack
    int res = -1;
    while ((read = getline(&hash, &len, input_file)) != -1) {
        // read the hash
        hash = strtok(hash, " \n");

        // attack the hash, and if the hash was not cracked, prints an empty line in the output file
        res = attack_hash_with_rainbow_table(strtoull(hash, NULL, 10), ht, res_file);
        if (res == 0) {
            fprintf(res_file, "\n");
        }
    }
    fclose(res_file);
    fclose(input_file);

    if (hash) free(hash);
}

void create_threads(ht_cell_t **hash_table)
{
    // create a thread on each file
    pthread_t my_threads[NB_THREADS];
    cpargs my_args[NB_THREADS];
    for (int i = 0; i < NB_THREADS; i++)
    {
        my_args[i].hash_table = hash_table;

        int length = snprintf(NULL, 0, "%d", i);
        char* number = malloc(sizeof(char) * (length + 1));
        snprintf(number, length + 1, "%d", i);

        // create a file path of form "tmp/plit<i>.txt" with <i> being the value of variable i
        char input_file_path[14] = "tmp/split";
        strcat(input_file_path, number);
        strcat(input_file_path, ".txt");
        my_args[i].input_file_path = strdup(input_file_path);

        // create a file path of form "tmp/res<i>.txt" with <i> being the value of variable i
        char output_file_path[12] = "tmp/res";
        strcat(output_file_path, number);
        strcat(output_file_path, ".txt");
        my_args[i].output_file_path = strdup(output_file_path);

        pthread_create(&my_threads[i], NULL, thread_attack_file_with_rainbow_table, (void *) &(my_args[i]));
        
        free(number);
    }

    for (int i = 0; i < NB_THREADS; i++)
    {
        pthread_join(my_threads[i], NULL);
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
    
    if (NB_THREADS > 1) {
        // split the attacked file into NB_THREADS files
        split_attacked_file(argv[R + 1]);

        // start the threads, which will access the same hash_table
        create_threads(hash_table);

    } else {
        // attack the file provided as R+1 argument
        FILE * attacked_file = fopen(argv[R + 1], "r");
        attack_file_with_rainbow_table(attacked_file, hash_table, argv[R + 2]);
        fclose(attacked_file);
    } 

    free_hashtable(hash_table);
    exit(EXIT_SUCCESS);
}