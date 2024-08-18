#include "debug_helper.h"

#include "../big_decimal/big_decimal.h"

#define P10_UINT64 10000000000000000000ULL /* 19 zeroes */
#define E10_UINT64 19

void Bdebug_info(big_decimal big_1, big_decimal *big_2, big_decimal *big_3) {
  fprintf(stderr, "\tbig1\n");
  print_Bdec_bin(big_1, 1, 1);
  fprintf(stderr, "\n");
  print_Bdec(big_1);
  fprintf(stderr, "\n");

  if (big_2 != NULL) {
    fprintf(stderr, "\tbig2\n");
    print_Bdec_bin(*big_2, 1, 1);
    fprintf(stderr, "\n");
    print_Bdec(*big_2);
    fprintf(stderr, "\n");
  }

  if (big_3 != NULL) {
    fprintf(stderr, "\nresult\n");
    print_Bdec_bin(*big_3, 1, 1);
    fprintf(stderr, "\n");
    print_Bdec(*big_3);
    fprintf(stderr, "\n");
  }
}

void debug_info(s21_decimal dec_1, s21_decimal *dec_2, s21_decimal *dec_3) {
  fprintf(stderr, "dec_1\n");
  print_dec_bin(dec_1, 1, 1);
  fprintf(stderr, "\n");
  print_dec(dec_1);
  fprintf(stderr, "\n");

  if (dec_2 != NULL) {
    fprintf(stderr, "dec_2\n");
    print_dec_bin(*dec_2, 1, 1);
    fprintf(stderr, "\n");
    print_dec(*dec_2);
    fprintf(stderr, "\n");
  }

  if (dec_3 != NULL) {
    fprintf(stderr, "\nresult\n");
    print_dec_bin(*dec_3, 1, 1);
    fprintf(stderr, "\n");
    print_dec(*dec_3);
    fprintf(stderr, "\n");
  }
  fprintf(stderr, "\n");
}

void alignment(s21_decimal *value_1, s21_decimal *value_2) {
  big_decimal big_1 = (big_decimal){{0}};
  big_decimal big_2 = (big_decimal){{0}};
  decimal_to_big(*value_1, &big_1);
  decimal_to_big(*value_2, &big_2);
  Balignment(&big_1, &big_2);
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

void print_dec(s21_decimal value) {
  char str[50] = {'\0'};
  s21_decimal dec_digit = (s21_decimal){{0}};
  scale_t scale = get_scale(value);
  const scale_t scale_buf = scale;
  if (get_sign(value) == plus) {
    fprintf(stderr, "+");
  } else {
    fprintf(stderr, "-");
  }

  set_sign(&value, plus);

  for (int i = 0; s21_is_not_equal(value, (s21_decimal){{0}}) == 1; ++i) {
    modulus10(value, &dec_digit);
    const uint8_t last_digit = dec_digit.bits[0] % 10;
    divide10(value, &value);
    str[i] = last_digit + '0';
    --scale;
  }

  s21_strrev(str);
  fprintf(stderr, "%s", str);
  fprintf(stderr, "(%d)", scale_buf);
}

void print_Bdec(big_decimal value) {
  char str[100] = {'\0'};
  big_decimal Bdigit = (big_decimal){{0}};
  scale_t scale = Bget_scale(value);
  const scale_t scale_buf = scale;
  if (Bget_sign(value) == plus) {
    fprintf(stderr, "+");
  } else {
    fprintf(stderr, "-");
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
  fprintf(stderr, "%s", str);
  fprintf(stderr, "(%d)", scale_buf);
}

// func for print_dec_bin()
static void uint_to_bin(uint32_t value) {
  int8_t bits = 31;

  while (value != 0) {
    int buf = value - (1U << bits);

    if (buf >= 0) {
      value = buf;
      fprintf(stderr, "1");
    } else {
      fprintf(stderr, "0");
    }
    --bits;
  }
  while (bits >= 0) {
    fprintf(stderr, "0");
    --bits;
  }
}

void print_Bdec_bin(big_decimal value, bool print_scale, bool print_separate) {
  if (Bget_sign(value))
    fprintf(stderr, "(-)");
  else
    fprintf(stderr, "(+)");

  if (print_scale) {
    uint_to_bin(value.bits[6]);
    if (print_separate) fprintf(stderr, "\'");
  }
  for (int8_t i = 5; i >= 0; --i) {
    uint_to_bin(value.bits[i]);
    if (print_separate) fprintf(stderr, "\'");
  }
  fprintf(stderr, "(scale=%d)", Bget_scale(value));
}

void print_dec_bin(s21_decimal value, bool print_scale, bool print_separate) {
  if (get_sign(value))
    fprintf(stderr, "(-)");
  else
    fprintf(stderr, "(+)");
  if (print_scale) {
    uint_to_bin(value.bits[3]);
  }
  for (int8_t i = 2; i >= 0; --i) {
    if (print_separate) fprintf(stderr, "\'");
    uint_to_bin(value.bits[i]);
  }
  fprintf(stderr, "(scale=%d)", get_scale(value));
}
