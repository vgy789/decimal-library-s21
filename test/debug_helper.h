#ifndef DEBUG_HELPER_H
#define DEBUG_HELPER_H

#include <ctype.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "../big_decimal/big_decimal.h"
#include "../decimal/s21_decimal.h"

#define MAX_DECIMAL "79228162514264337593543950335"

bool set_scale(s21_decimal *value, uint8_t scale);

bool get_bit(s21_decimal value, uint8_t bit_pos);

/**
 * Устанавливает указанный бит числа s21_decimal. Отсчёт с нуля.
 */
void set_bit(s21_decimal *value, uint8_t bit_pos, bool state);

void str_2decimal(char *number, s21_decimal *result);

s21_decimal uint128_to_bin(__uint128_t n);

/**
 * Печатает big_decimal в двоичном предствалении.
 */
void Bdec_2bin(big_decimal value, bool print_scale, bool print_separate);

/**
 * Печатает s21_decimal в десятичном предствалении.
 */
void dec_2int(s21_decimal value);

/**
 * Печатает s21_decimal в двоичном предствалении.
 */
void dec_2bin(s21_decimal value, bool print_scale, bool print_separate);

/**
 * Конвертирует uint128 в decimal.
 */
s21_decimal uint128_2decimal(__uint128_t n);

#endif  // DEBUG_HELPER_H
