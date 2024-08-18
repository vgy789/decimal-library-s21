#ifndef DEBUG_HELPER_H
#define DEBUG_HELPER_H

#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "../big_decimal/big_decimal.h"
#include "../s21_decimal.h"

#define MAX_DECIMAL "79228162514264337593543950335"

void Bdebug_info(big_decimal big_1, big_decimal *big_2, big_decimal *big_3);

void debug_info(s21_decimal dec_1, s21_decimal *dec_2, s21_decimal *dec_3);

void alignment(s21_decimal *value1, s21_decimal *value2);

/**
 * Конвертирует число из строкового литерала в s21_decimal.
 */
void str_to_decimal(const char *number, s21_decimal *result);

/**
 * Конвертирует число из строкового литерала в big_decimal.
 */
void str_to_bigdecimal(const char *number, big_decimal *result);

/**
 * Печатает big_decimal в двоичном предствалении.
 */
void print_Bdec_bin(big_decimal value, bool print_scale, bool print_separate);

/**
 * Печатает s21_decimal в десятичном предствалении.
 */
void print_dec(s21_decimal value);

/**
 * Печатает s21_decimal в двоичном предствалении.
 */
void print_dec_bin(s21_decimal value, bool print_scale, bool print_separate);

void print_Bdec(big_decimal value);

#endif  // DEBUG_HELPER_H
