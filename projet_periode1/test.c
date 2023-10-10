#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include "hash.h"

void reduction(pwhash hash, int call, char *word) {
    unsigned long long modulo = (hash + call) % ((unsigned long long) pow(26, M));
    // if (call == L-1) printf("%lld\n", modulo);

    modulo = 273271647;

    printf("\n");
    for (int i = M-1; i >= 0 ; i--)
    {
        char tmp = modulo / ((unsigned long long) pow(26, i)) + 'a';
        printf("%c", tmp);
        modulo = modulo % ((unsigned long long) pow(26, i));
    }
    printf("\n");
}    


int main(int argc, char const *argv[]) {
    char *test_pwd = "abcdef";
    pwhash test_hash = target_hash_function(test_pwd);
    printf("Test hash: %ld\n", test_hash);

    reduction(test_hash, 0, test_pwd);
    printf("Test reduction: %s\n", test_pwd);
}