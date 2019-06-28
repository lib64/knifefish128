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

    printf("[*] Testing the init_pbox function.\n");

    uint8_t pbox[PBOX_SIZE];

    kf_init_pbox(pbox, 0xDEADBEEF);

    int test[PBOX_SIZE];

    memset(test, 0, PBOX_SIZE);

    for(int i = 0; i < PBOX_SIZE; i++)
    {
        if(pbox[i] > PBOX_SIZE - 1 || pbox[i] < 0)
        {
            fail = 1;
            break;
        }
        test[pbox[i]] = 1;
    }

    for(int i = 0; i < PBOX_SIZE; i++)
    {
        if(test[i] != 1)
        {
            fail = 1;
            break;
        }
    }

    if(fail == 0)
    {
        printf("    [*] Test #1 Passed.\n");
    }
    else
    {
        printf("    [*] Test #1 Failed.\n");
    }

    if(fail == 0)
        printf("[*] All block tests passed.\n");

    return fail;
}

