#include "debug_helper.h"

#include "../big_decimal/big_decimal.h"

#define P10_UINT64 10000000000000000000ULL /* 19 zeroes */
#define E10_UINT64 19

#define STRINGIZER(x) #x
#define TO_STRING(x) STRINGIZER(x)

bool get_bit(s21_decimal value, uint8_t bit_pos) {
  uint8_t int_part = 0; /* индекс ячейки в массиве */

  while (bit_pos > 31) {
    bit_pos -= 32;
    int_part += 1;
  }
  return (value.bits[int_part] & (1 << bit_pos));
}

void set_bit(s21_decimal *value, uint8_t bit_pos, bool state) {
  big_decimal big = (big_decimal){{0}};
  decimal_to_big(*value, &big);
  Bset_bit(&big, bit_pos, state);
  big_to_decimal(big, value);
}

bool set_scale(s21_decimal *value, uint8_t scale) {
  big_decimal big = (big_decimal){{0}};
  decimal_to_big(*value, &big);
  Bset_scale(&big, scale);
  big_to_decimal(big, value);
}

void str_2decimal(char *number, s21_decimal *result) {
  *result = (s21_decimal){{0}};
  bool sign = plus;
  if (*number == '-') {
    sign = minus;
    ++number;
  }

  bool radix_point = false;
  uint8_t scale = 0;
  for (int i = 0; number[i] != '\0'; ++i) {
    if (number[i] == '.' || number[i] == ',') {
      radix_point = true;
      continue;
    }
    if (radix_point) {
      scale += 1;
    }

    uint8_t digit = digittoint(number[i]);
    s21_decimal dec_digit = (s21_decimal){{digit}};
    s21_mul(*result, (s21_decimal){{10}}, result);
    s21_add(*result, dec_digit, result);
  }
  set_scale(result, scale);
  set_sign(result, sign);
}

void str_2bigdecimal(char *number, big_decimal *result) {
  *result = (big_decimal){{0}};
  bool sign = plus;
  if (*number == '-') {
    sign = minus;
    ++number;
  }

  bool radix_point = false;
  uint8_t scale = 0;
  for (int i = 0; number[i] != '\0'; ++i) {
    if (number[i] == '.' || number[i] == ',') {
      radix_point = true;
      continue;
    }
    if (radix_point) {
      scale += 1;
    }

    uint8_t digit = digittoint(number[i]);
    big_decimal dec_digit = (big_decimal){{digit}};
    Bdigits_mul(*result, (big_decimal){{10}}, result);
    Bdigits_add(*result, dec_digit, result);
  }
  Bset_scale(result, scale);
  Bset_sign(result, sign);
}

s21_decimal uint128_to_bin(__uint128_t n) {
  // use it → s21_decimal result = uint128_to_bin(1234123125678965432);
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

// https://stackoverflow.com/questions/11656241/how-to-print-uint128-t-number-using-gcc
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

// func for dec_2int()
static __uint128_t pow_uint128(__uint128_t x, __uint128_t y) {
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

// func for dec_2bin()
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

void Bdec_2bin(big_decimal value, bool print_scale, bool print_separate) {
  if (Bget_sign(value))
    printf("(-)");
  else
    printf("(+)");

  if (print_scale) {
    uint_2bin(value.bits[6]);
    if (print_separate) printf("\'");
  }
  for (int8_t i = 5; i >= 0; --i) {
    uint_2bin(value.bits[i]);
    if (print_separate) printf("\'");
  }
  printf("(scale=%d)", Bget_scale(value));
}

void dec_2bin(s21_decimal value, bool print_scale, bool print_separate) {
  if (get_sign(value))
    printf("(-)");
  else
    printf("(+)");

  if (print_scale) {
    uint_2bin(value.bits[3]);
  }
  for (int8_t i = 2; i >= 0; --i) {
    if (print_separate) printf("\'");
    uint_2bin(value.bits[i]);
  }
  printf("(scale=%d)", get_scale(value));
}

#undef P10_UINT64
#undef E10_UINT64
#undef STRINGIZER
#undef TO_STRING
