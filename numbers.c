/*
    Copyright (c) 2020 Ivan Kniazkov <ivan.kniazkov.com>

    The implementation of the numbers library
*/

#include "numbers.h"
#include <memory.h>
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

void init_number_by_integer(number_t *n, integer_t value)
{
    n->real_value = value;
    n->is_number = true;
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

static int max_precision = 0;

int get_max_integer_precision()
{
    if (!max_precision)
        max_precision = (int)log10f(LLONG_MAX);
    return max_precision;
}

void init_number_by_real(number_t *n, real_t value)
{
    integer_t int_value = (integer_t)value;
    if (int_value == value)
    {
        init_number_by_integer(n, int_value);
        return;
    }

    n->real_value = value;
    n->is_number = true;
    if (value < 0)
    {
        n->is_negative = true;
        value = -value;
    }
    else
    {
        n->is_negative = false;
    }

    int exp2;
    frexpl(value, &exp2);
    n->exponent = (int)log10l(pow_2(exp2));
    if (n->exponent)
    {
        real_t new_value = value / pow_10(n->exponent);
        if (new_value < 1)
        {
            n->exponent--;
            value /= pow_10(n->exponent);
        }
        else
        {
            value = new_value;
            while (value > LLONG_MAX)
            {
                n->exponent++;
                value /= 10;
            }
        }
    }

    n->int_part = (integer_t)value;
    real_t fract_part_real = (value - (integer_t)value);
    if (fract_part_real)
    {
        int precision = get_max_integer_precision() - 3;
        integer_t fract_part_int = (integer_t)(fract_part_real * pow_10(precision));
        if (fract_part_int)
        {
            if (fract_part_int % 10 > 4)
                fract_part_int++;
            while(precision && fract_part_int % 10 == 0)
            {
                fract_part_int /= 10;
                precision--;
            }
            n->precision = precision;
            n->fract_part = fract_part_int;
            return;
        }
    }
    n->precision = 0;
    n->fract_part = 0;
}

static __inline bool is_number(char c)
{
    return c >= '0' && c <= '9';
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
    }

    if (!is_number(*p))
        goto error;

    do
    {
        n->int_part = n->int_part * 10 + *p - '0';
        p++;
    } while(is_number(*p));

    if (*p == '.')
    {
        *p++;
        if (!is_number(*p))
            goto error;
        do
        {
            n->fract_part = n->fract_part * 10 + *p - '0';
            n->precision++;
            p++;
        } while (is_number(*p));        
    }

    if (*p == 'e' || *p == 'E')
    {
        bool exp_neg = false;
        *p++;
        if (*p == '+')
        {
            *p++;
        }
        else if (*p == '-')
        {
            exp_neg = true;
            *p++;
        }
        if (!is_number(*p))
            goto error;
        do
        {
            n->exponent = n->exponent * 10 + *p - '0';
            p++;
        } while(is_number(*p));
        if (exp_neg) n->exponent = -n->exponent;
    }

    while(*p == ' ')
        p++;
    if ((*p)) goto error;

    n->is_number = true;
    real_t value = (n->int_part + n->fract_part / pow_10(n->precision));
    if (n->is_negative) value = -value;
    if (n->exponent) value *= pow_10(n->exponent);
    n->real_value = value;
    return;

error:
    memset(n, 0, sizeof(number_t));
}

static char * print_integer(integer_t i, char *s)
{
    assert(i >= 0);
    do
    {
        *s++ = (char)(i % 10 + '0');
        i /= 10;
    } while (i > 0);
    return s;   
}

void print_number(char *s, number_t *n)
{
    char *p = s;
    if (n->exponent)
    {
        if (n->exponent < 0)
        {
            p = print_integer(-n->exponent, p);
            *p++ = '-';
        }
        else
        {
            p = print_integer(n->exponent, p);
        }
        *p++ = 'e';        
    }
    if (n->fract_part)
    {
        char *p1 = print_integer(n->fract_part, p);
        int k = p1 - p;
        p = p1;
        while(k < n->precision)
        {
            *p++ = '0';
            k++;            
        }
        *p++ = '.';
    }
    p = print_integer(n->int_part, p);
    if (n->is_negative)
    {
        *p++ = '-';
    }
    
    *p = '\0';
    p--;
    char *mid = (p - s) / 2 + s;
    while (s <= mid)
    {
        char c = *p;
        *p = *s;
        *s = c;
        s++;
        p--;
    }
}

static const integer_t pow_2_table[] =
{
    1,
    2,
    4,
    8,
    16,
    32,
    64,
    128,
    256,
    512,
    1024,
    2048,
    4096,
    8192,
    16384,
    32768,
    65536,
    131072,
    262144,
    524288,
    1048576,
    2097152,
    4194304,
    8388608,
    16777216,
    33554432,
    67108864,
    134217728,
    268435456,
    536870912,
    1073741824,
    2147483648,
    4294967296,
    8589934592,
    17179869184,
    34359738368,
    68719476736,
    137438953472,
    274877906944,
    549755813888,
    1099511627776,
    2199023255552,
    4398046511104,
    8796093022208,
    17592186044416,
    35184372088832,
    70368744177664,
    140737488355328,
    281474976710656,
    562949953421312,
    1125899906842624,
    2251799813685248,
    4503599627370496,
    9007199254740992,
    18014398509481984,
    36028797018963968,
    72057594037927936,
    144115188075855872,
    288230376151711744,
    576460752303423488,
    1152921504606846976,
    2305843009213693952,
    4611686018427387904
};

real_t pow_2(int value)
{
    if (value < 0)
        return (real_t)1 / pow_2(-value);
    if (value < sizeof(pow_2_table) / sizeof(integer_t))
        return pow_2_table[value];
    int k = sizeof(pow_2_table) / sizeof(integer_t) - 1;
    real_t result = pow_2_table[k];
    while(k < value)
    {
        k++;
        result *= 2;
    }
    return result;
}

static const integer_t pow_10_table[] =
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
        return (real_t)1 / pow_10(-value);
    if (value < sizeof(pow_10_table) / sizeof(integer_t))
        return pow_10_table[value];
    int k = sizeof(pow_10_table) / sizeof(integer_t) - 1;
    real_t result = pow_10_table[k];
    while(k < value)
    {
        k++;
        result *= 10;
    }
    return result;
}
