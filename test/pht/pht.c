/* 
 * Copyright (c) 2018-2019, Michael Harper
 * 
/* See LICENSE for licensing information */

#include "../../src/kf128.h"

#include <stdio.h>
#include <inttypes.h>

int main(void)
{
    int fail = 0;

    printf("[*] Testing the PHT function.\n\n");

    uint32_t a = 0xDEADBEEF;
    uint32_t b = 0xBAADF00D;
    uint32_t ares = 0x995BAEFC;
    uint32_t bres = 0x54099F09;
    uint32_t a_prime,b_prime;

    kf_pht(&a,&b, &a_prime, &b_prime);

    printf("    [*] Test #1\n");
    printf("        Input    -> a:  0x%08"PRIx32" b:  0x%08"PRIx32"\n",a,b);
    printf("        Expected -> a': 0x%08"PRIx32" b': 0x%08"PRIx32"\n",ares,bres);
    printf("        Got      -> a': 0x%08"PRIx32" b': 0x%08"PRIx32"\n",a_prime,b_prime);

    if(a_prime == ares && b_prime == bres)
    {
        printf("    [*] Test #1 Passed.\n\n");
    }
    else
    {
        printf("    [*] Test #1 Failed.\n\n");
        fail = 1;
    }

    if(fail == 0)
        printf("[*] All PHT tests passed.\n\n");

    return fail;
}
