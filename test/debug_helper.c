#include "debug_helper.h"

#include "../big_decimal/big_decimal.h"

#define P10_UINT64 10000000000000000000ULL /* 19 zeroes */
#define E10_UINT64 19

#define STRINGIZER(x) #x
#define TO_STRING(x) STRINGIZER(x)

void alignment(s21_decimal *value_1, s21_decimal *value_2, bool for_add) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(*value_1, &big_1);
  decimal_to_big(*value_2, &big_2);
  Balignment(&big_1, &big_2, for_add);
  big_to_decimal(big_1, value_1);
  big_to_decimal(big_2, value_2);
}

void str_to_decimal(const char *number, s21_decimal *result) {
  *result = (s21_decimal){{0}};
  bool sign = plus;
  if (*number == '-') {
    sign = minus;
    ++number;
  }

  bool radix_point = false;
  scale_t scale = 0;
  for (int i = 0; number[i] != '\0'; ++i) {
    if (number[i] == '.' || number[i] == ',') {
      radix_point = true;
      continue;
    }
    if (radix_point) {
      scale += 1;
    }

    uint8_t digit = number[i] - '0';
    s21_decimal dec_digit = (s21_decimal){{digit}};
    s21_mul(*result, (s21_decimal){{10}}, result);
    s21_add(*result, dec_digit, result);
  }
  set_scale(result, scale);
  set_sign(result, sign);
}

void str_to_bigdecimal(const char *number, big_decimal *result) {
  *result = (big_decimal){{0}};
  bool sign = plus;
  if (*number == '-') {
    sign = minus;
    ++number;
  }

  bool radix_point = false;
  scale_t scale = 0;
  for (int i = 0; number[i] != '\0'; ++i) {
    if (number[i] == '.' || number[i] == ',') {
      radix_point = true;
      continue;
    }
    if (radix_point) {
      scale += 1;
    }

    uint8_t digit = number[i] - '0';
    big_decimal dec_digit = (big_decimal){{digit}};
    Bdigits_mul(*result, (big_decimal){{10}}, result);
    Bdigits_add(*result, dec_digit, result);
  }
  Bset_scale(result, scale);
  Bset_sign(result, sign);
}

void print_Bdec(big_decimal value) {
  char str[50] = {'\0'};
  big_decimal Bdigit = (big_decimal){{0}};
  scale_t scale = Bget_scale(value);
  const scale_t scale_buf = scale;
  if (Bget_sign(value) == plus) {
    printf("+");
  } else {
    printf("-");
  }

  Bset_sign(&value, plus);

  for (int i = 0; Bdigits_ne(value, (big_decimal){{0}}) == 1; ++i) {
    Bmodulus10(value, &Bdigit);
    const uint8_t last_digit = Bdigit.bits[0] % 10;
    Bdivide10(value, &value);
    str[i] = last_digit + '0';
    --scale;
  }

  s21_strrev(str);
  printf("%s", str);
  printf("(%d)", scale_buf);
}

// func for print_dec_bin()
static void uint_to_bin(uint32_t value) {
  int8_t bits = 31;

  while (value != 0) {
    int buf = value - (1U << bits);

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

void print_dec(s21_decimal value) {
  __uint128_t number = 0u;

  for (int bit_i = 0; bit_i < 96; ++bit_i) {
    const bool bit = get_bit(value, bit_i);
    if (bit) number += pow_uint128(2, bit_i);
  }

  print_u128_u(number);
}

void print_Bdec_bin(big_decimal value, bool print_scale, bool print_separate) {
  if (Bget_sign(value))
    printf("(-)");
  else
    printf("(+)");

  if (print_scale) {
    uint_to_bin(value.bits[6]);
    if (print_separate) printf("\'");
  }
  for (int8_t i = 5; i >= 0; --i) {
    uint_to_bin(value.bits[i]);
    if (print_separate) printf("\'");
  }
  printf("(scale=%d)", Bget_scale(value));
}

void print_dec_bin(s21_decimal value, bool print_scale, bool print_separate) {
  if (get_sign(value))
    printf("(-)");
  else
    printf("(+)");
  if (print_scale) {
    uint_to_bin(value.bits[3]);
  }
  for (int8_t i = 2; i >= 0; --i) {
    if (print_separate) printf("\'");
    uint_to_bin(value.bits[i]);
  }
  printf("(scale=%d)", get_scale(value));
}
