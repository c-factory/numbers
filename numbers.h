/*
    Copyright (c) 2020 Ivan Kniazkov <ivan.kniazkov.com>

    The declaration of the 'numbers' library:
    structs representing numeric values
*/

#pragma once

#include <stdbool.h>

typedef long long int integer_t;
typedef long double real_t;

typedef struct
{
    bool is_number;
    integer_t int_value;
    real_t real_value;

    bool is_negative;
    integer_t int_part;
    integer_t fract_part;
    int precision;
    int exponent;
} number_t;

void init_number_by_integer(number_t *num, integer_t value);
void parse_number(number_t *num, const char *str);
real_t pow_10(int value);