#ifndef DEBUG_HELPER_H
#define DEBUG_HELPER_H

#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "../decimal/s21_decimal.h"

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
