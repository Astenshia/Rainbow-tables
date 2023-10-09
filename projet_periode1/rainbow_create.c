#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash.h"

void reduction(pwhash hash, int call, char *word) {
    for(int i=0; i<M; i++) {
        word[i] = (char) ((hash/(i+1) + call)%26) + 'a';
    }
    printf("%lu -> %s ->", hash, word);
}

void create(FILE * in_file, FILE * out_file){
    char * pass0;
    char * passL;
    for(int i = 0; i < N; i++) {
        // si un fichier où lire les password pass0 n'est pas donné, alors générer ces nombres aléatoirements
        // sinon les tirer du fichier
        if(in_file==NULL) {
            // allocation de deux chaine de même longueur que le mot de passe
            pass0 = (char*) malloc(sizeof(char) * M);
            passL = (char*) malloc(sizeof(char) * M);

            // génération aléatoire du mot de passe à M caractères
            for(int j = 0; j < M; j++) {
                pass0[j] = (rand() % 26) + 'a';
            }
            printf("%s -> ",pass0);
            
            // premier hash en dehors de la boucle car on souhaite conserver la valeur de pass0
            pwhash hashed = target_hash_function(pass0);
            // premiere réduction en dehors de la boucle
            reduction(hashed, 0, passL);

            // boucle de hash et de réduciton
            for(int k=1; k<L; k++) {
                hashed = target_hash_function(passL);
                reduction(hashed, k, passL);
            }
            printf("\n\n");

            // écriture du couple pass0 passL dans le fichier de sortie
            fprintf(out_file, "%s %s\n", pass0, passL);

            free(pass0);
            free(passL);
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

    // si un fichier d'entrée des pass0 est donné, alors lecture de ce fichier
    FILE *input_file = NULL;
    if (argc == R+2) {
        input_file = fopen(argv[R+1], "r");
    }

    // écriture pour chaque fichier passé en paramètre
    for(int r = 0; r < R; r++) {
        FILE * rfile = fopen(argv[r+1], "w");
        if(rfile == NULL) {
            printf("Error opening file %s", argv[r-1]);
        }
        // écriture dans le fichier des couples pass0, passL
        create(input_file, rfile);
        fclose(rfile);  
    }
    
    // fermeture du fichier d'entrée s'il est donné
    if (input_file) fclose(input_file);
    
    // test de valeur
    pwhash mot = target_hash_function("testing");
    return 0;
}
