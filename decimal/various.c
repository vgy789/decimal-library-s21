#include <stddef.h>

#include "../s21_decimal.h"

err_t check_scale(s21_decimal value) {
  const scale_t scale = get_scale(value);
  if (scale > 28 || scale < 0) { /* max scale test */
    return get_sign(value) + 1;
  }
  return 0;  // correct scale
}

bool get_bit(s21_decimal value, uint8_t bit_pos) {
  uint8_t int_part = 0; /* индекс ячейки в массиве */

  while (bit_pos > 31) {
    bit_pos -= 32;
    int_part += 1;
  }
  return (value.bits[int_part] & (1U << bit_pos));
}

void set_bit(s21_decimal *value, uint8_t bit_pos, bool state) {
  big_decimal big = (big_decimal){{0}};
  decimal_to_big(*value, &big);
  Bset_bit(&big, bit_pos, state);
  big_to_decimal(big, value);
}

scale_t get_scale(s21_decimal value) {
  big_decimal big = (big_decimal){{0}};
  decimal_to_big(value, &big);
  return Bget_scale(big);
}

bool set_scale(s21_decimal *value, scale_t scale) {
  const bool is_err = check_scale(*value);
  big_decimal big = (big_decimal){{0}};
  decimal_to_big(*value, &big);
  Bset_scale(&big, scale);
  big_to_decimal(big, value);
  return is_err;
}

bool get_sign(s21_decimal value) { return get_bit(value, 127); }

void set_sign(s21_decimal *value, bool sign) {
  big_decimal big = (big_decimal){{0}};
  decimal_to_big(*value, &big);
  Bset_sign(&big, sign);
  big_to_decimal(big, value);
}

err_t s21_negate(s21_decimal value, s21_decimal *result) {
  err_t err_code = (result == NULL);
  if (err_code == 0) {
    err_code = check_scale(value);
  }
  if (err_code == 0) {
    *result = value;
    set_sign(result, !get_sign(*result));
  }
  return err_code;
}

static size_t s21_strlen(const char *str) {
  const char *end = str;
  while (*end != '\0') end += 1;
  return end - str;
}

void s21_strrev(char *str) {
  if (!str) {
    return;
  }
  int i = 0;
  int j = s21_strlen(str) - 1;

  while (i < j) {
    char c = str[i];
    str[i] = str[j];
    str[j] = c;
    i++;
    j--;
  }
}
