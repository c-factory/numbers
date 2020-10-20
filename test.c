#include "numbers.h"

int main(void)
{
    number_t n;
    real_t *rv = (real_t*)&n;
    parse_number(&n, "1024e-3");
    return 0;
}