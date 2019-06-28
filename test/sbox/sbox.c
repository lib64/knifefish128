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

  printf("[*] Testing the init_sbox function.\n");

  uint8_t sbox[SBOX_SIZE];

  kf_init_sbox(sbox, 0xDEADBEEF);

  int test[SBOX_SIZE];

  memset(test, 0, SBOX_SIZE);

  for (int i = 0; i < SBOX_SIZE; i++) {
    if (sbox[i] > SBOX_SIZE - 1 || sbox[i] < 0) {
      fail = 1;
      break;
    }
    test[sbox[i]] = 1;
  }

  for (int i = 0; i < SBOX_SIZE; i++) {
    if (test[i] != 1) {
      fail = 1;
      break;
    }
  }

  if (fail == 0) {
    printf("    [*] Test #1 Passed.\n");
  } else {
    printf("    [*] Test #1 Failed.\n");
  }

  if (fail == 0) printf("[*] All block tests passed.\n");

  return fail;
}
