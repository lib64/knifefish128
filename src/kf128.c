/*
 * Copyright (c) 2018-2019, Michael Harper
 *
 * See LICENSE for licensing information */

#include "kf128.h"
#include <stdio.h>
#include <string.h>

void kf_lfsr(uint32_t *shift_register) {
  *shift_register = (((((*shift_register >> 31) ^ (*shift_register >> 6) ^
                        (*shift_register >> 4) ^ (*shift_register >> 2) ^
                        (*shift_register >> 1) ^ *shift_register)) &
                      0x00000001)
                     << 31) |
                    (*shift_register >> 1);
}

uint8_t kf_lfsr_byte(uint32_t *shift_register) {
  for (int i = 0; i < 8; i++)
    kf_lfsr(shift_register);
  return *shift_register & 0x000000FF;
}

void kf_pht(const uint32_t *a, const uint32_t *b, uint32_t *a_prime,
            uint32_t *b_prime) {
  uint32_t ap = (*a) + (*b) % PHT_MAX;
  uint32_t bp = *b_prime = (*a) + 2 * (*b) % PHT_MAX;
  *a_prime = ap;
  *b_prime = bp;
}

void kf_invert_ctx(const kf_ctx *key, kf_ctx *inv) {
  memcpy(inv, key, sizeof(kf_ctx));
  for (int i = 0; i < ROUNDS; i++) {
    inv->skey[i][0] = key->skey[ROUNDS - i - 1][0];
    inv->skey[i][1] = key->skey[ROUNDS - i - 1][1];
  }
  for (int i = 0; i < 4; i++) {
    inv->wkey[0][i] = key->wkey[1][i];
    inv->wkey[1][i] = key->wkey[0][i];
  }
}

void kf_init_sbox(uint8_t s[SBOX_SIZE], const uint32_t seed) {
  uint8_t indexes[SBOX_SIZE];

  uint32_t seed_copy = seed;

  for (size_t i = 0; i < SBOX_SIZE; i++) {
    indexes[i] = (uint8_t)i;
  }

  for (int i = 0; i < SBOX_SIZE; i++) {
    size_t index = kf_lfsr_byte(&seed_copy) % (SBOX_SIZE - i);
    s[i] = indexes[index];
    memmove(indexes + index, indexes + index + 1, SBOX_SIZE - index - 1);
  }
}

void kf_init_pbox(uint8_t p[PBOX_SIZE], const uint32_t seed) {
  uint8_t indexes[PBOX_SIZE];

  uint32_t seed_copy = seed;

  for (size_t i = 0; i < PBOX_SIZE; i++) {
    indexes[i] = (uint8_t)i;
  }

  for (int i = 0; i < PBOX_SIZE; i++) {
    size_t index = kf_lfsr_byte(&seed_copy) % (PBOX_SIZE - i);
    p[i] = indexes[index];
    memmove(indexes + index, indexes + index + 1, PBOX_SIZE - index - 1);
  }
}

void kf_expand_passphrase(const char *passphrase, kf_ctx *ctx) {
  uint32_t key[KEY_SIZE] = {0};

  uint32_t shift_register = 0;

  size_t passlen = strlen(passphrase);

  if (passlen > 256)
    passlen = 256;

  for (size_t i = 0; i < passlen - 4; i += 4) {
    shift_register =
        (uint8_t)passphrase[i] | (uint8_t)(passphrase[i + 1] << 8) |
        (uint8_t)(passphrase[i + 2] << 16) | (uint8_t)(passphrase[i + 3] << 24);

    for (int j = 0; j < KEY_SIZE; j++) {
      key[j] ^= shift_register;

      for (int k = 0; k < 32; k++)
        kf_lfsr(&shift_register);
    }
  }

  char last[4] = {0};

  for (size_t i = 0; i < passlen % 4; i++) {
    last[i] = passphrase[passlen - 4 + i];
  }

  shift_register = (uint8_t)last[0] | (uint8_t)(last[1] << 8) |
                   (uint8_t)(last[2] << 16) | (uint8_t)(last[3] << 24);

  for (int i = 0; i < KEY_SIZE; i++) {
    key[i] ^= shift_register;

    for (int j = 0; j < 32; j++)
      kf_lfsr(&shift_register);
  }

  int count = 0;
  for (int i = 0; i < SBOX_COUNT; i++) {
    kf_init_sbox(ctx->sbox[i], key[count++]);
  }

  kf_init_pbox(ctx->pbox, key[count++]);

  for (int i = 0; i < ROUNDS; i++) {
    ctx->skey[i][0] = key[count++];
    ctx->skey[i][1] = key[count++];
  }

  for (int i = 0; i < WKEY_COUNT; i++) {
    ctx->wkey[i][0] = key[count++];
    ctx->wkey[i][1] = key[count++];
    ctx->wkey[i][2] = key[count++];
    ctx->wkey[i][3] = key[count++];
  }
}

void kf_f(const uint32_t *in, uint32_t *out, const size_t round, kf_ctx *ctx) {
  const uint8_t *in8 = (const uint8_t *)in;
  uint8_t *out8 = (uint8_t *)out;

  for (int i = 0; i < SBOX_COUNT; i++) {
    out8[ctx->pbox[i]] = ctx->sbox[i][in8[i]];
  }

  kf_pht(out, out + 1, out, out + 1);

  *out ^= ctx->skey[round][0];
  *(out + 1) ^= ctx->skey[round][1];
}

void kf_round(const uint32_t *in, uint32_t *out, const size_t round,
              kf_ctx *ctx) {
  uint32_t tmp[2];

  kf_f(in + 2, tmp, round, ctx);

  tmp[0] ^= *in;
  tmp[1] ^= *(in + 1);

  if (round != ROUNDS - 1) {
    memcpy(out, in + 2, 8);
    memcpy(out + 2, tmp, 8);
  } else {
    memcpy(out, tmp, 8);
    memcpy(out + 2, in + 2, 8);
  }
}

void kf_block(const uint32_t *in, uint32_t *out, kf_ctx *ctx) {
  memcpy(out, in, sizeof(uint32_t) * 4);

  out[0] ^= ctx->wkey[0][0];
  out[1] ^= ctx->wkey[0][1];
  out[2] ^= ctx->wkey[0][2];
  out[3] ^= ctx->wkey[0][3];

  for (size_t i = 0; i < ROUNDS; i++) {
    kf_round(out, out, i, ctx);
  }

  out[0] ^= ctx->wkey[1][0];
  out[1] ^= ctx->wkey[1][1];
  out[2] ^= ctx->wkey[1][2];
  out[3] ^= ctx->wkey[1][3];
}

void kf_encrypt_file_cbc(const char *infile, const char *outfile,
                         const char *passphrase, const char *iv,
                         const char *padding) {
  kf_ctx ctx;
  kf_expand_passphrase(passphrase, &ctx);

  FILE *in = fopen(infile, "r");
  FILE *out = fopen(outfile, "w");

  fwrite(iv, sizeof(char), BLOCK_SIZE, out);

  fseek(in, 0L, SEEK_END);
  long input_size = ftell(in);
  rewind(in);

  long remaining = input_size % BLOCK_SIZE;
  long block_count = input_size / BLOCK_SIZE + (remaining > 0);

  uint32_t block[4], key[4], tmp[4];

  memcpy(key, iv, sizeof(uint32_t) * 4);

  for (int i = 0; i < block_count; i++) {
    if (i == block_count - 1 && remaining != 0) {
      memcpy(block, padding, sizeof(uint32_t) * 4);
      fread(block, sizeof(uint8_t), remaining, in);
      ((uint8_t *)block)[BLOCK_SIZE - 1] = (uint8_t)remaining;
    } else {
      fread(block, sizeof(uint32_t), 4, in);
    }

    block[0] ^= key[0];
    block[1] ^= key[1];
    block[2] ^= key[2];
    block[3] ^= key[3];

    kf_block(block, tmp, &ctx);

    memcpy(key, tmp, sizeof(uint32_t) * 4);

    fwrite(tmp, sizeof(uint32_t), 4, out);
  }

  if (remaining == 0) {
    memset(block, 0, sizeof(uint32_t) * 4);

    block[0] ^= key[0];
    block[1] ^= key[1];
    block[2] ^= key[2];
    block[3] ^= key[3];

    kf_block(block, tmp, &ctx);

    fwrite(tmp, sizeof(uint32_t), 4, out);
  }

  fclose(in);
  fclose(out);
}

void kf_decrypt_file_cbc(const char *infile, const char *outfile,
                         const char *passphrase) {
  kf_ctx ctx, inv;
  kf_expand_passphrase(passphrase, &ctx);
  kf_invert_ctx(&ctx, &inv);

  FILE *in = fopen(infile, "r");
  FILE *out = fopen(outfile, "w");

  fseek(in, 0L, SEEK_END);
  long input_size = ftell(in);
  rewind(in);

  uint32_t block[4], key[4], tmp[4];

  fread(key, sizeof(uint32_t), 4, in);

  long block_count = input_size / BLOCK_SIZE - 1;

  for (int i = 0; i < block_count; i++) {
    fread(block, sizeof(uint32_t), 4, in);

    kf_block(block, tmp, &inv);

    tmp[0] ^= key[0];
    tmp[1] ^= key[1];
    tmp[2] ^= key[2];
    tmp[3] ^= key[3];

    if (i != block_count - 1) {
      fwrite(tmp, sizeof(uint32_t), 4, out);
    } else {
      uint8_t remaining = ((uint8_t *)tmp)[BLOCK_SIZE - 1];
      fwrite(tmp, sizeof(uint8_t), remaining, out);
    }
    memcpy(key, block, sizeof(uint32_t) * 4);
  }

  fclose(in);
  fclose(out);
}
