#include <stdio.h>
#include <stdlib.h>
#include <openssl/md5.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <byteswap.h>

#define M 6 // taille mot de passe
#define L 100 // longueur chaine
#define N 5000000 // nb chaines par table
#define R 15 // nb tables

#define HASHSIZE 8
#define SHIFT 6

typedef uint64_t pwhash;

pwhash target_hash_function(char* pwd) {
    unsigned char md5[MD5_DIGEST_LENGTH] = {0};
    MD5((const unsigned char*)pwd,  strlen(pwd), md5);

    unsigned long long myhash = 0;
    memcpy(&myhash, md5, sizeof(myhash));
    myhash = __bswap_64(myhash);

    return (myhash << SHIFT ) | ( myhash >> (64 - SHIFT));
}