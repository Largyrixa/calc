#include <stdio.h>

#include "zmath.h"

int main (int argc, char **argv)
{
    ZNumber a = znum_zero();
    ZNumber b = znum_zero();

    *a.v = 0xFFFFFFFF;
    *b.v = 0x0000FFFF;

    ZNumber c = znum_add(a, b);
    ZNumber d = znum_sub(c, b);
    znum_free(&a);
    znum_free(&b);
    znum_free(&c);
    znum_free(&d);
    return 0;
}