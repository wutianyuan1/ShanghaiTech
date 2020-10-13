#ifndef THREADS_FP_H
#define THREADS_FP_H

#define F (1<<14)
typedef int fp_t;

fp_t cvt_int_fp(int n);

int  fp_round(fp_t a);

int  fp_to_nearest(fp_t a);

fp_t fp_add(fp_t a, fp_t b);

fp_t fp_sub(fp_t a, fp_t b);

fp_t fp_mul(fp_t a, fp_t b);

fp_t fp_div(fp_t a, fp_t b);

fp_t fp_add_int (fp_t a, int n);

fp_t fp_sub_int (fp_t a, int n);

fp_t fp_mul_int (fp_t a, int n);

fp_t fp_div_int (fp_t a, int n);

#endif


