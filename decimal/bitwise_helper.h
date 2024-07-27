#ifndef BITWISE_HELPER_H
#define BITWISE_HELPER_H

#include "s21_decimal.h"

#define SCALE_MAX (28)

enum sign { plus, minus };
enum { scale_start = 112, scale_end = 119 };

bool mul_by_ten_bin(s21_decimal *value);
void mul_by_ten(s21_decimal *value);
void div_by_ten(s21_decimal *value);
int get_first_num(int value, int *size);
void alignment(s21_decimal *value1, s21_decimal *value2);

/**
 * Возварщает знак s21_decimal.
 *
 * @param value Проверяемое число.
 * @return 0 — положительный, 1 — отрицательный.
 */
bool get_sign(s21_decimal value);

/**
 * Устанавливает знак числа decimal.
 *
 * @param *value Указатель на изменяемое число.
 * @param sign Знак числа (0 — положительный, 1 — отрицательный).
 */
void set_sign(s21_decimal *value, bool sign);

/**
 * Печатает s21_decimal в двоичном предствалении.
 */
void decimal_binary(s21_decimal value, bool print_scale);

/**
 * Печатает unsigned int в двоичном предствалении.
 */
void uint_binary(uint32_t value);

// Возвращает указанный бит числа s21_decimal. Отсчёт с нуля.
bool get_bit(s21_decimal value, uint8_t bit_pos);

// Устанавливает указанный бит числа s21_decimal. Отсчёт с нуля.
void set_bit(s21_decimal *value, uint8_t bit_pos, bool status);

// Возвращает десятичную точку числа s21_decimal.
uint8_t get_scale(s21_decimal value);

// Устанавливает десятичную точку в числе s21_decimal.
bool set_scale(s21_decimal *value, uint8_t scale);

// Функция инкремента.
int s21_inc(s21_decimal value, s21_decimal *result);

// Функция декремента.
int s21_dec(s21_decimal value, s21_decimal *result);

#endif  // BITWISE_HELPER_H
