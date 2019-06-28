/*
 * Copyright (c) 2018-2019, Michael Harper
 *
 * See LICENSE for licensing information */

#include "../../src/kf128.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  int fail = 0;

  printf("[*] Testing the block function.\n");

  kf_ctx ctx, inv;
  char pass[] = "This is a password";
  uint32_t in[4] = {0xDEADBEEF, 0xBAADF00D, 0xBEADDEEF, 0xFAADB00D};
  uint32_t out[4] = {0};
  uint32_t dec[4] = {0};

  kf_expand_passphrase(pass, &ctx);

  kf_block(in, out, &ctx);

  kf_expand_passphrase(pass, &ctx);

  kf_invert_ctx(&ctx, &inv);

  kf_block(out, dec, &inv);

  if (memcmp(in, dec, sizeof(uint32_t) * 4) == 0) {
    printf("    [*] Test #1 Passed.\n");
  } else {
    printf("    [*] Test #1 Failed.\n");
    fail = 1;
  }

  if (fail == 0)
    printf("[*] All block tests passed.\n");

  return fail;
}
