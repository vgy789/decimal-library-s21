#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "../decimal/bitwise_helper.h"
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

static void dec_int(s21_decimal value) {
  __uint128_t number = 0u;

  for (int bit_i = 0; bit_i < 96; ++bit_i) {
    const bool bit = get_bit(value, bit_i);
    if (bit) number += pow_uint128(2, bit_i);
  }

  print_u128_u(number);
}

static s21_decimal uint128_to_bin(__uint128_t n) {
  // use s21_decimal result = uint128_to_bin(1234123125678965432);
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

// int main(void) {
//   s21_decimal num1 = {{0, 0, 0, 0}};
//   s21_decimal num2 = {{0, 0, 0, 0}};
//   s21_decimal result = (s21_decimal){{0, 0, 0, 0}};

//   // set_sign(&num1, minus);
//   // set_sign(&num2, minus);
//   // set_scale(&num1, 10);
//   // set_scale(&num2, 5);

//   {
//     decimal_binary(num1, 1);
//     dec_int(num1);
//     printf("\n");
//     decimal_binary(num2, 1);
//     dec_int(num2);
//     printf("\n");
//   }

//   s21_add(num1, num2, &result);
//   decimal_binary(result, 1);
//   dec_int(result);
// }
