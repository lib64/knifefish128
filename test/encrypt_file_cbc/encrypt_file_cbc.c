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

  printf("[*] Testing the encrypt_file_cbc function.\n");

  char iv[] = "ABCDabcd1234EFGH";
  char padding[] = "vdslsilvfdkvlfdn";
  char passphrase[] = "this is my password";

  char plaintext[64] = {0};
  for (int i = 1; i < 64 + 1; i++) {
    for (int j = 0; j < i; j++) {
      plaintext[j] = rand() % 128;
    }

    FILE *test = fopen("kf_test_plain.txt", "w");

    fwrite(plaintext, sizeof(char), i - 1, test);

    fclose(test);

    kf_encrypt_file_cbc("kf_test_plain.txt", "kf_test_enc.txt", passphrase, iv,
                        padding);

    kf_decrypt_file_cbc("kf_test_enc.txt", "kf_test_dec.txt", passphrase);

    FILE *in, *in2;

    in = fopen("kf_test_plain.txt", "r");
    in2 = fopen("kf_test_dec.txt", "r");

    int ch1 = getc(in);
    int ch2 = getc(in2);

    while ((ch1 != EOF) && (ch2 != EOF) && (ch1 == ch2)) {
      ch1 = getc(in);
      ch2 = getc(in2);
    }

    fclose(in);
    fclose(in2);

    if (ch1 == ch2) {
      printf("    [*] Test #%d Passed.\n", i);
    } else {
      printf("    [*] Test #%d Failed.\n", i);
      fail++;
    }

    remove("kf_test_plain.txt");
    remove("kf_test_enc.txt");
    remove("kf_test_dec.txt");
  }

  if (fail == 0)
    printf("[*] All encrypt_file_cbc tests passed.\n");

  return fail;
}
