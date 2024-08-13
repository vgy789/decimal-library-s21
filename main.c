#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../big_decimal/big_decimal.h"
#include "../s21_decimal.h"
#include "../test/debug_helper.h"

// При получении чисел, не вмещающихся в мантиссу при арифметических операциях,
// использовать банковское округление (например,
// 79,228,162,514,264,337,593,543,950,335 — 0.6 = 79,228,162,514,264,337,593,543

int main(void) {
  // s21_decimal value = (s21_decimal){{1, 0, 0, 0x80000000}};
  // set_scale(&value, 1);
  // dec_to_bin(value, 1, 1);
  // dec_2int(value);
  // printf("\n");

  // printf("%d<<\n", get_sign(value));
  // s21_truncate(value, &value);

  // printf("\n%d<<\n", get_sign(value));
  // dec_to_bin(value, 1, 1);
  // dec_2int(value);

  // printf("\t%d", s21_is_equal((s21_decimal){{0, 0, 0, 0}},
  //                             (s21_decimal){{0, 0, 0, 0x80000000}}));

  // s21_decimal value = (s21_decimal){{1, 0, 0, 0x80000000}};
  s21_decimal value_1 = {{0}};
  // s21_decimal value_2 = {{0}};
  // s21_decimal result = {{0}};
  // str_to_decimal(MAX_DECIMAL, &value_1);

  // str_to_decimal("113", &value_2);
  big_decimal big_1 = (big_decimal){{0}};

  str_to_bigdecimal("7922816251426433759354.39503328", &big_1);
  // printf("int: \n");
  // dec_to_bin(value_1, 1, 1);
  // dec_2int(value_1);

  // decimal_to_big(value_1, &big_1);
  big_to_decimal(big_1, &value_1);

  // s21_div(value_1, value_2, &result);
  // bank_round(value_1, &value_1);
  printf("\nout: \n");
  dec_to_bin(value_1, 1, 1);
  dec_2int(value_1);
  // printf("\n");
  // dec_to_bin(value_2, 1, 1);
  // dec_2int(value_2);
  // printf("\n");
  // dec_to_bin(result, 0, 0);
  // dec_2int(result);

  // int result = 0;
  // s21_from_decimal_to_int(value_1, &result);
}
