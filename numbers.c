/*
    Copyright (c) 2020 Ivan Kniazkov <ivan.kniazkov.com>

    The implementation of the numbers library
*/

#include "numbers.h"
#include <memory.h>
#include <math.h>

void init_number_by_integer(number_t *n, integer_t value)
{
    n->is_number = true;
    n->int_value = value;
    n->real_value = value;
    if (value < 0)
    {
        n->is_negative = true;
        n->int_part = -value;    
    }
    else
    {
        n->is_negative = false;
        n->int_part = value;
    }
    n->fract_part = 0;
    n->precision = 0;
    n->exponent = 0;
}

void parse_number(number_t *n, const char *str)
{
    memset(n, 0, sizeof(number_t));

    const char *p = str;
    if (!p)
        return;

    while(*p == ' ')
        p++;
    if (!(*p)) goto error;

    if (*p == '-')
    {
        n->is_negative = true;
        p++;
        if (!(*p)) goto error;
    }

    while(*p >= '0' && *p < '9')
    {
        n->int_part = n->int_part * 10 + *p - '0';
        p++;
    }

    if (*p != '.' && *p != 'e' && *p != 'E')
    {
        while(*p == ' ')
            p++;
        if ((*p)) goto error;

        n->is_number = true;
        n->int_value = n->is_negative ? -n->int_part : n->int_part;
        n->real_value = n->int_value;
        return;
    }

    if (*p == '.')
    {
        *p++;
        if (!(*p)) goto error;
        while(*p >= '0' && *p < '9')
        {
            n->fract_part = n->fract_part * 10 + *p - '0';
            n->precision++;
            p++;
        }
        n->is_number = true;
        n->int_value = n->is_negative ? -n->int_part : n->int_part;
        n->real_value = n->int_value + n->fract_part / pow_10(n->precision);
        return;
   }

error:
    memset(n, 0, sizeof(number_t));
}

static const integer_t pow10_table[] =
{
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
    1000000000,
    10000000000,
    100000000000,
    1000000000000,
    10000000000000,
    100000000000000,
    1000000000000000,
    10000000000000000,
    100000000000000000,
    1000000000000000000
};

real_t pow_10(int value)
{
    if (value < 0)
        return 0;
    if (value < sizeof(pow10_table) / sizeof(integer_t))
        return pow10_table[value];
    int k = sizeof(pow10_table) / sizeof(integer_t) - 1;
    real_t result = pow10_table[k];
    while(k < value)
    {
        k++;
        result *= 10;
    }
    return result;
}
