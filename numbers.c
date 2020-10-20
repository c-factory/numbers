/*
    Copyright (c) 2020 Ivan Kniazkov <ivan.kniazkov.com>

    The implementation of the numbers library
*/

#include "numbers.h"
#include <memory.h>
#include <assert.h>

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

    bool exp_neg = false;
    if (*p == 'e' || *p == 'E')
    {
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
    }

    while(*p == ' ')
        p++;
    if ((*p)) goto error;

    n->is_number = true;
    real_t value = (n->int_part + n->fract_part / pow_10(n->precision));
    if (n->is_negative) value = -value;
    if (exp_neg)
    {
        value /= pow_10(n->exponent);
        n->exponent = -n->exponent;
    }
    else
    {
        value *= pow_10(n->exponent);
    }
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
