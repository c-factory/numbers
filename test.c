#include "numbers.h"

int main(void)
{
    number_t n;
    real_t *rv = (real_t*)&n;
    parse_number(&n, "-10.024e+3");
    char buff[64];
    char *s = buff;
    print_number(s, &n);
    return 0;
}