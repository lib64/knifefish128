/*
 * Copyright (c) 2018-2019, Michael Harper
 *
 * See LICENSE for licensing information */

#include "../../src/kf128.h"

#include <stdio.h>
#include <string.h>

int main(void) {
  int fail = 0;

  printf("[*] Testing the expand_passphrase function.\n");

  char passphrase[] = "AbCdEfGhIj";

  kf_ctx ctx, ctx2;

  kf_expand_passphrase(passphrase, &ctx);
  kf_expand_passphrase(passphrase, &ctx2);

  if (memcmp(&ctx, &ctx2, sizeof(kf_ctx)) == 0) {
    printf("    [*] Test #1 Passed.\n");
  } else {
    printf("    [*] Test #1 Failed.\n");
    fail = 1;
  }

  if (fail == 0)
    printf("[*] All expand_passphrase tests passed.\n");

  return fail;
}
