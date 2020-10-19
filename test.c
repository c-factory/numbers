#include "numbers.h"

int main(void)
{
    real_t p = pow_10(25);
    number_t n;
    parse_number(&n, " 10.0024");
    return 0;
}