#include "debug_helper.h"

#include "../decimal/s21_decimal.h"

#define P10_UINT64 10000000000000000000ULL /* 19 zeroes */
#define E10_UINT64 19

#define STRINGIZER(x) #x
#define TO_STRING(x) STRINGIZER(x)

static int print_u128_u(__uint128_t u128) {
  int rc;
  if (u128 > UINT64_MAX) {
    __uint128_t leading = u128 / P10_UINT64;
    uint64_t trailing = u128 % P10_UINT64;
    rc = print_u128_u(leading);
    rc += printf("%." TO_STRING(E10_UINT64) PRIu64, trailing);
  } else {
    uint64_t u64 = u128;
    rc = printf("%" PRIu64, u64);
  }
  return rc;
}

__uint128_t pow_uint128(__uint128_t x, __uint128_t y) {
  __uint128_t result = 1;
  while (y) {
    if (y & 1) {
      result *= x;
    }
    y >>= 1;
    x *= x;
  }
  return result;
}

void dec_2int(s21_decimal value) {
  __uint128_t number = 0u;

  for (int bit_i = 0; bit_i < 96; ++bit_i) {
    const bool bit = get_bit(value, bit_i);
    if (bit) number += pow_uint128(2, bit_i);
  }

  print_u128_u(number);
}

s21_decimal uint128_2decimal(__uint128_t n) {
  // use s21_decimal result = uint128_2decimal(1234123125678965432);
  s21_decimal result = {{0, 0, 0, 0}};
  int binary_num[128];
  int i = 0;

  while (n > 0) {
    binary_num[i++] = n % 2;
    n /= 2;
  }

  for (int j = i - 1; j >= 0; j--) {
    set_bit(&result, j, binary_num[j]);
  }

  return result;
}

static void uint_2bin(uint32_t value) {
  int8_t bits = 31;

  while (value != 0) {
    int buf = value - (1 << bits);

    if (buf >= 0) {
      value = buf;
      printf("1");
    } else {
      printf("0");
    }
    --bits;
  }
  while (bits >= 0) {
    printf("0");
    --bits;
  }
}

void dec_2bin(s21_decimal value, bool print_scale, bool print_separate) {
  if (get_sign(value))
    printf("(-)");
  else
    printf("(+)");

  if (print_scale) {
    uint_2bin(value.bits[3]);
    if (print_separate) printf("\'");
  }
  uint_2bin(value.bits[2]);
  if (print_separate) printf("\'");
  uint_2bin(value.bits[1]);
  if (print_separate) printf("\'");
  uint_2bin(value.bits[0]);
  printf("(scale=%d)", get_scale(value));
}

#undef P10_UINT64
#undef E10_UINT64

#undef STRINGIZER
#undef TO_STRING