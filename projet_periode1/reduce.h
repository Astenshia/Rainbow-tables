#include <stdio.h>
#include <stdlib.h>
#include <openssl/md5.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <byteswap.h>
#include "hash.h"

/// @brief Reduction function used to create a rainbow table.
/// @param hash hash to be used to determine the next password
/// @param call reduction variation to prevent the same hash to get the same password
/// @param word string that will contain the next password.
void reduce(pwhash hash, int call, char *word) {
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