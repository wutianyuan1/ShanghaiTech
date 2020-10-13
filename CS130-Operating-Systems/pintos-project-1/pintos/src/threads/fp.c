#include "threads/fp.h"
#include "lib/stdint.h"

// typedef unsigned long long int int64_t;
// #define F (1<<14)
// typedef int fp_t;

fp_t cvt_int_fp(int n)
{
  return (fp_t) n * F;
}

int  fp_round(fp_t a)
{
  return (int) a / F;
}

int  fp_to_nearest(fp_t a)
{
  return (a >= 0) ? ((a + F/2) / F) : ((a - F/2) / F);
}

fp_t fp_add(fp_t a, fp_t b)
{
  return a + b;
}

fp_t fp_sub(fp_t a, fp_t b)
{
  return a - b;
}

fp_t fp_mul(fp_t a, fp_t b)
{
  return ((int64_t) a) * b / F;
}

fp_t fp_div(fp_t a, fp_t b)
{
  return (((int64_t) a) * F / b);
}

fp_t fp_add_int (fp_t a, int n)
{
  return a + (n * F);
}

fp_t fp_sub_int (fp_t a, int n)
{
  return a - (n * F);
}

fp_t fp_mul_int (fp_t a, int n)
{
  return a * n;
}

fp_t fp_div_int (fp_t a, int n)
{
  return a / n;
}
