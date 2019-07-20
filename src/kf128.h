/*
 * Copyright (c) 2018-2019, Michael Harper
 *
 * See LICENSE for licensing information */

#ifndef KF128_H
#define KF128_H

#include <inttypes.h>
#include <stdlib.h>

#define ROUNDS 16
#define SBOX_SIZE 256
#define SBOX_COUNT 8
#define PBOX_SIZE 8
#define WKEY_COUNT 2
#define WKEY_SIZE 4
#define SKEY_SIZE 2
#define KEY_SIZE 49
#define BLOCK_SIZE 16
#define PHT_MAX 4294967296

/**
 * @brief the ctx object holds sboxes, pboxes, and key material.
 *
 */
typedef struct {
  uint8_t sbox[SBOX_COUNT][SBOX_SIZE];
  uint8_t pbox[PBOX_SIZE];
  uint32_t skey[ROUNDS][SKEY_SIZE];
  uint32_t wkey[WKEY_COUNT][WKEY_SIZE];

} kf_ctx;

void kf_lfsr(uint32_t *shift_register);

uint8_t kf_lfsr_byte(uint32_t *shift_register);

void kf_pht(const uint32_t *a, const uint32_t *b, uint32_t *a_prime,
            uint32_t *b_prime);

void kf_invert_ctx(const kf_ctx *key, kf_ctx *inv);

void kf_init_sbox(uint8_t s[SBOX_SIZE], const uint32_t seed);

void kf_init_pbox(uint8_t p[PBOX_SIZE], const uint32_t seed);

void kf_expand_passphrase(const char *passphrase, kf_ctx *ctx);

void kf_f(const uint32_t *in, uint32_t *out, const size_t round, kf_ctx *ctx);

void kf_round(const uint32_t *in, uint32_t *out, const size_t round,
              kf_ctx *ctx);

void kf_block(const uint32_t *in, uint32_t *out, kf_ctx *ctx);

void kf_encrypt_file_cbc(const char *infile, const char *outfile,
                         const char *passphrase, const char *iv,
                         const char *padding);

void kf_decrypt_file_cbc(const char *infile, const char *outfile,
                         const char *passphrase);

#endif // KF128_H
