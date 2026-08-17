#ifndef ZMATH_H
#define ZMATH_H

#include <inttypes.h>

#define znum_NaN (ZNumber){ 0 }

typedef uint32_t Half;

#define Half_MAX 0xFFFFFFFF

typedef uint64_t Full;

typedef long long unsigned Len;

typedef enum {
    SIGN_POSITIVE,
    SIGN_NEGATIVE,
} Sign;

typedef struct {
    Half *v;
    Len  len;
    Sign sign;
} ZNumber;

Half *znum_valloc(Len len);

ZNumber znum_zero();
void znum_free(ZNumber *n);

// Operations between two numbers
#define znum_op_def(name) ZNumber znum_##name(ZNumber a, ZNumber b)

znum_op_def(add);
znum_op_def(sub);

// Operations between an immediate and a number
#define znum_iop_def(name) ZNumber znum_##name##i(ZNumber a, const int imm)

znum_iop_def(add);

// One argument functions
#define znum_fn1_def(name) ZNumber znum_##name(ZNumber n)

znum_fn1_def(neg);

#endif