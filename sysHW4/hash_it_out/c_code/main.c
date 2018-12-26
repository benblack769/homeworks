//#include <assert.h>
#include <stdio.h> 
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <time.h>

#include "dbLL_tests.h"
#include "cache_tests.h"
#include "evict_tests.h"

struct args {
    bool cache_tests;
    bool dbll_tests;
};

static struct option opts[] =
{
    {"cache-tests", no_argument, 0, 'c'},
    {"dbll-tests", no_argument, 0, 'd'},
};

static void args_init(struct args * args)
{
    args->cache_tests = false;
    args->dbll_tests = false;
}

static int go(struct args *args)
{
    if (args->cache_tests) {
        cache_tests();
        evict_tests();
    }

    if (args->dbll_tests) {
        printf("Running dbll tests\n");
        dbll_tests();
    }
    return 0;
}

int main(int argc, char *argv[]) 
{
    srand(time(NULL));
    struct args args;
    args_init(&args);
    int c, idx;
    while ((c = getopt_long(argc, argv, "", opts, &idx)) != -1) {
        switch(c) {
            case 'c':
                args.cache_tests = true;
                break;
            case 'd':
                args.dbll_tests = true;
                break;
            default:
                break;
        }
    }
    return go(&args);
}

