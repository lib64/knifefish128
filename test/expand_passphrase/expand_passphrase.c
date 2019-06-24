/* 
 * Copyright (c) 2018-2019, Michael Harper
 * 
 * See LICENSE for licensing information */

#include "../../src/kf128.h"

#include <stdio.h>
#include <string.h>


int main(void)
{
    int fail = 0;

    printf("[*] Testing the expand_passphrase function.\n\n");

    char passphrase[] = "AbCdEfGhIj";

    kf_ctx ctx,ctx2;

    kf_expand_passphrase(passphrase, &ctx);
    kf_expand_passphrase(passphrase, &ctx2);

    printf("    [*] Test #1\n");
    printf("        Expanding ctx1.\n");
    printf("        Expanding ctx2.\n");
    printf("        Checking for equality.\n");

    if(memcmp(&ctx,&ctx2,sizeof(kf_ctx)) == 0)
    {
        printf("    [*] Test #1 Passed.\n\n");
    }
    else
    {
        printf("    [*] Test #1 Failed.\n\n");
        fail = 1;
    }

    if(fail == 0)
        printf("[*] All expand_passphrase tests passed.\n\n");

    return fail;
}
