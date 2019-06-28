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

  printf("[*] Testing the block function.\n\n");

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

  printf("    [*] Test #1\n");
  printf("        Input    -> 0x%08" PRIx32 " 0x%08" PRIx32 " 0x%08" PRIx32
         " 0x%08" PRIx32 "\n",
         in[0], in[1], in[2], in[3]);
  printf("        Expected -> 0x%08" PRIx32 " 0x%08" PRIx32 " 0x%08" PRIx32
         " 0x%08" PRIx32 "\n",
         in[0], in[1], in[2], in[3]);
  printf("        Got      -> 0x%08" PRIx32 " 0x%08" PRIx32 " 0x%08" PRIx32
         " 0x%08" PRIx32 "\n",
         dec[0], dec[1], dec[2], dec[3]);

  if (memcmp(in, dec, sizeof(uint32_t) * 4) == 0) {
    printf("    [*] Test #1 Passed.\n\n");
  } else {
    printf("    [*] Test #1 Failed.\n\n");
    fail = 1;
  }

  if (fail == 0) printf("[*] All block tests passed.\n\n");

  return fail;
}
