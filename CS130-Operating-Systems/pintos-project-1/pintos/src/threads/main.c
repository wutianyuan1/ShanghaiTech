#include "fp.h"
#include <stdio.h>

int main()
{
    fp_t f_1 = cvt_int_fp(1);
    fp_t f_60 = cvt_int_fp(60);
    fp_t f = fp_mul(fp_div(f_1, f_60), 1);
    fp_t b = fp_mul_int (f, 10000);
    printf("%d %d %d %d\n", f, b, f_1, f_60);
    return 0;
}