#include "numbers.h"
#include <math.h>

int main(void)
{
    number_t n;
    init_number_by_real(&n, 13.001);

    char buff[64];
    char *s = buff;
    print_number(s, &n);

    return 0;
}
