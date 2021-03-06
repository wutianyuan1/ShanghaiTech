#include <stdio.h>
#include <math.h>
int main()
{
    int n;
    scanf("%x", &n);
    int sign = n >> 31;
    int exp = (n >> 24) & 0x7f;
    int frac = n & 0xffffff;
    float f = 0;
    for (double i = exp2(-24); frac != 0; i = i * 2)
    {
        f += i * (frac & 1);
        frac = (frac >> 1);
    }
    
    float v = pow(-1, sign) * (1 + f) * pow(2, exp - 63);
    printf("The number converts to %lf\n", v);
    return 0;
}