#ifndef BITWISE_HELPER_H
#define BITWISE_HELPER_H

#include "./decimal/s21_decimal.h"

enum sign { plus, minus };

/**
 * Возварщает знак s21_decimal.
 *
 * @param dec Проверяемое число.
 * @return 0 — положительный, 1 — отрицательный.
 */
bool get_sign(s21_decimal dec);

/**
 * Устанавливает знак числа decimal. (работает)
 *
 * @param *dec Указатель на изменяемое число.
 * @param sign Знак числа (0 — положительный, 1 — отрицательный).
 */
void set_sign(s21_decimal *dec, bool sign);

/**
 * Печатает s21_decimal в двоичном предствалении. (работает)
 */
void print_0b_decimal(s21_decimal value);

/**
 * Печатает unsigned long в двоичном предствалении. (работает)
 */
void ulong_binary(unsigned long value);

/**
 * Печатает unsigned int в двоичном предствалении. (работает)
 */
void uint_binary(unsigned int value);

// Возвращает указанный бит числа s21_decimal. Отсчёт с нуля.
bool get_bit(s21_decimal dec, size_t bit);

// Устанавливает указанный бит числа s21_decimal. Отсчёт с нуля.
void set_bit(s21_decimal *dec, size_t bit_pos, bool status);

#endif  // BITWISE_HELPER_H
