#include <stdbool.h>
#include <stdlib.h>

#include "zmath.h"

static inline bool
ispositive(ZNumber n) 
{ return n.sign == SIGN_POSITIVE; }

static inline void
Full_sep(Full f, Half *high, Half *low)
{
    *high = f >> 32;
    *low  = (Half)f;
}

Half *
znum_valloc(Len len)
{ return calloc(len, sizeof(Half)); }

Half *
znum_vrealloc(Half *v, Len len)
{ return realloc(v, len * sizeof(Half)); }

ZNumber 
znum_zero(void)
{
    ZNumber n;

    n.len = 1;
    n.sign = SIGN_POSITIVE;
    n.v = znum_valloc(n.len);

    return n;
}

void
znum_free(ZNumber *n)
{
    free(n->v);
    *n = znum_NaN;
}

void
znum_trim(ZNumber *n)
{
    if (n->len == 1) {
        return;
    }

    Len zeroc = 0;
    for (Len i = n->len-1; i >= 0; i--) {
        if (n->v[i] == 0) {
            zeroc++;
        } else {
            break;
        }
    }

    if (zeroc == 0) {
        return;
    }

    if (zeroc == n->len) {
        znum_free(n);
        *n = znum_zero();
        return;
    }

    n->len -= zeroc;
    n->v = znum_vrealloc(n->v, n->len);
}

znum_op_def(add)
{
    ZNumber res;
    Len len, len1, len2;
    Half *v1, *v2;

    if (ispositive(a) && !ispositive(b)) {
        b.sign = SIGN_POSITIVE;
        return znum_sub(a, b);
    } else if (!ispositive(a) && ispositive(b)) {
        a.sign = SIGN_POSITIVE;
        return znum_sub(b, a);
    } else {
        res.sign = a.sign;
    }

    if (a.len > b.len) {
        len = a.len;
        len1 = a.len;
        v1 = a.v;
        len2 = b.len;
        v2 = b.v;
    } else {
        len = b.len;
        len1 = b.len;
        v1 = b.v;
        len2 = a.len;
        v2 = a.v;
    }

    res.len = len + 1;
    res.v = znum_valloc(res.len);

    Half carry = 0;
    for (Len i = 0; i < len2; i++) {
        Half high, low;
        Full sum = (Full)v1[i] + (Full)v2[i] + (Full)carry;

        Full_sep(sum, &high, &low);
        res.v[i] = low;
        carry = high;
    }

    for (Len i = len2; i < len1; i++) {
        Half high, low;
        Full sum = (Full)v1[i] + (Full)carry;

        Full_sep(sum, &high, &low);
        res.v[i] = low;
        carry = high;
    }

    res.v[res.len-1] = carry;

    znum_trim(&res);
    return res;
}

znum_op_def(sub)
{ 
    ZNumber res;
    Half carry;

    if (a.sign != b.sign) {
        b.sign = a.sign;
        return znum_add(a, b);
    }

    Len len1 = a.len;
    Len len2 = b.len;

    if (a.len == b.len) {
        Half *p1 = a.v;
        Half *p2 = b.v;

        while ((len1 > 0) && (*p1 == *p2)) {
            p1++;
            p2++;
            len1--;
        }

        if (len1 == 0) {
            return znum_zero();
        }

        carry = (*p1 < *p2);
    } else {
        carry = (len1 < len2);
    }

    Half *v1 = a.v;
    Half *v2 = b.v;
    res.sign = a.sign;
    if (carry) {
        Half tmp = len1;
        len2 = len1;
        len1 = tmp;
        v1 = b.v;
        v2 = a.v;
        if (ispositive(res)) {
            res.sign = SIGN_NEGATIVE;
        } else {
            res.sign = SIGN_POSITIVE;
        }
    }

    res.v = znum_valloc(len1);
    res.len = len1;
    
    for (Len i = 0; i < len2; i++) {
        Full sub = ( Half_MAX - (Full)v1[i] ) + v2[i] + carry;
        Half high, low;
        Full_sep(sub, &high, &low);

        res.v[i] = (Half)( Half_MAX - low );
        carry = high;
    }

    for (Len i = len2; i < len1; i++) {
        Full sub = ( Half_MAX - (Full)v1[i] ) + carry;
        Half high, low;
        Full_sep(sub, &high, &low);

        res.v[i] = (Half)( Half_MAX - low );
        carry = high;
    }

    znum_trim(&res);
    return res;
}