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

/**
 * Конвертирует число из строкового литерала в s21_decimal.
 */
void str_to_decimal(char *number, s21_decimal *result);

/**
 * Печатает big_decimal в двоичном предствалении.
 */
void Bdec_to_bin(big_decimal value, bool print_scale, bool print_separate);

/**
 * Печатает s21_decimal в десятичном предствалении.
 */
void dec_2int(s21_decimal value);

/**
 * Печатает s21_decimal в двоичном предствалении.
 */
void dec_to_bin(s21_decimal value, bool print_scale, bool print_separate);

#endif  // DEBUG_HELPER_H
