#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "hash.h"

int main(int argc, char const *argv[]) {
    char *test_pwd = "hello world";

    printf("Test hash: %" PRIu64 "\n", target_hash_function(test_pwd));
}