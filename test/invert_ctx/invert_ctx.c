/* 
 * Copyright (c) 2018-2019, Michael Harper
 * 
 * See LICENSE for licensing information */

#include "../../src/kf128.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

int main(void)
{
    int fail = 0;

    printf("[*] Testing the intvert_ctx function.\n\n");

    char passphrase[] = "AbCdEfGhIj";

    kf_ctx ctx,inv,ctx2;

    kf_expand_passphrase(passphrase, &ctx);

    kf_invert_ctx(&ctx,&inv);

    kf_invert_ctx(&inv,&ctx2);

    printf("    [*] Test #1\n");
    printf("        Expanding Passphrase.\n");
    printf("        Inverting the ctx twice.\n");
    printf("        Checking for equality.\n");

    if(memcmp(&ctx,&ctx2, sizeof(uint32_t) * 4) == 0)
    {
        printf("    [*] Test #1 Passed.\n\n");
    }
    else
    {
        printf("    [*] Test #1 Failed.\n\n");
        fail = 1;
    }

    if(fail == 0)
        printf("[*] All invert_ctx tests passed.\n\n");

    return fail;
}

