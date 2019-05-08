/* **********************************************************************
 *
 * Fixed Point Math Library
 *
 * **********************************************************************
 *
 * qmath.h
 *
 * Alex Forencich
 * alex@alexelectronics.com
 *
 * Jordan Rhee
 * rhee.jordan@gmail.com
 *
 * IEEE UCSD
 * http://ieee.ucsd.edu
 *
 * **********************************************************************/

#ifndef _QMATH_H_
#define _QMATH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifndef INLINE
#ifdef _MSC_VER
#define INLINE __inline
#else
#define INLINE inline
#endif /* _MSC_VER */
#endif /* INLINE */

/*
 * Default fractional bits. This precision is used in the routines
 * and macros without a leading underscore.
 * For example, if you are mostly working with values that come from
 * a 10-bit A/D converter, you may want to choose 21. This leaves 11
 * bits for the whole part, which will help avoid overflow in addition.
 * On ARM, bit shifts require a single cycle, so all fracbits
 * require the same amount of time to compute and there is no advantage
 * to selecting fracbits that are a multiple of 8.
 */
#define FIXED_FRACBITS 30

#define FIXED_RESOLUTION (1 << FIXED_FRACBITS)
#define FIXED_INT_MASK (0xffffffffL << FIXED_FRACBITS)
#define FIXED_FRAC_MASK (~FIXED_INT_MASK)

// square roots
#define FIXED_SQRT_ERR 1 << (FIXED_FRACBITS - 10)

// fixedp2a
#define FIXED_DECIMALDIGITS 6

typedef int32_t fixedp;

// conversions for arbitrary fracbits
#define _short2q(x, fb) ((fixedp)((x) << (fb)))
#define _int2q(x, fb) ((fixedp)((x) << (fb)))
#define _long2q(x, fb) ((fixedp)((x) << (fb)))
#define _float2q(x, fb) ((fixedp)((x) * (1 << (fb))))
#define _double2q(x, fb) ((fixedp)((x) * (1 << (fb))))

// conversions for default fracbits
#define short2q(x) _short2q(x, FIXED_FRACBITS)
#define int2q(x) _int2q(x, FIXED_FRACBITS)
#define long2q(x) _long2q(x, FIXED_FRACBITS)
#define float2q(x) _float2q(x, FIXED_FRACBITS)
#define double2q(x) _double2q(x, FIXED_FRACBITS)

// conversions for arbitrary fracbits
#define _q2short(x, fb) ((short)((x) >> (fb)))
#define _q2int(x, fb) ((int)((x) >> (fb)))
#define _q2long(x, fb) ((long)((x) >> (fb)))
#define _q2float(x, fb) ((float)(x) / (1 << (fb)))
#define _q2double(x, fb) ((double)(x) / (1 << (fb)))

// conversions for default fracbits
#define q2short(x) _q2short(x, FIXED_FRACBITS)
#define q2int(x) _q2int(x, FIXED_FRACBITS)
#define q2long(x) _q2long(x, FIXED_FRACBITS)
#define q2float(x) _q2float(x, FIXED_FRACBITS)
#define q2double(x) _q2double(x, FIXED_FRACBITS)

// evaluates to the whole (integer) part of x
#define qipart(x) q2long(x)

// evaluates to the fractional part of x
#define qfpart(x) ((x)&FIXED_FRAC_MASK)

/*
 * Constants
 */
#define _QPI 3.1415926535897932384626433832795
#define QPI double2q(_QPI)
#define _Q2PI 6.283185307179586476925286766559
#define Q2PI double2q(_Q2PI)
#define _QPIO2 1.5707963267948966192313216916398
#define QPIO2 double2q(_QPIO2)
#define _QPIO4 0.78539816339744830961566084581988
#define QPIO4 double2q(_QPIO4)
#define _QLN_E 2.71828182845904523536
#define QLN_E double2q(_QLN_E)
#define _QLN_10 2.30258509299404568402
#define QLN_10 double2q(_QLN_10)
#define _Q1OLN_10 0.43429448190325182765
#define Q1OLN_10 double2q(_Q1OLN_10)

// Both operands in addition and subtraction must have the same fracbits.
// If you need to add or subtract fixed point numbers with different
// fracbits, then use q2q to convert each operand beforehand.
#define qadd(a, b) ((a) + (b))
#define qsub(a, b) ((a) - (b))

/**
 * q2q - convert one fixed point type to another
 * x - the fixed point number to convert
 * xFb - source fracbits
 * yFb - destination fracbits
 */
static INLINE fixedp q2q(fixedp x, int xFb, int yFb) {
  if (xFb == yFb) {
    return x;
  } else if (xFb < yFb) {
    return x << (yFb - xFb);
  } else {
    return x >> (xFb - yFb);
  }
}

/**
 * Multiply two fixed point numbers with arbitrary fracbits
 * x - left operand
 * y - right operand
 * xFb - number of fracbits for X
 * yFb - number of fracbits for Y
 * resFb - number of fracbits for the result
 *
 */
#define _qmul(x, y, xFb, yFb, resFb) \
  ((fixedp)(((long long)(x) * (long long)(y)) >> ((xFb) + (yFb) - (resFb))))

/**
 * Fixed point multiply for default fracbits.
 */
#define qmul(x, y) _qmul(x, y, FIXED_FRACBITS, FIXED_FRACBITS, FIXED_FRACBITS)

/**
 * divide
 * shift into 64 bits and divide, then truncate
 */
#define _qdiv(x, y, xFb, yFb, resFb) \
  ((fixedp)((((long long)x) << ((xFb) + (yFb) - (resFb))) / y))

/**
 * Fixed point divide for default fracbbits.
 */
#define qdiv(x, y) _qdiv(x, y, FIXED_FRACBITS, FIXED_FRACBITS, FIXED_FRACBITS)

/**
 * Invert a number (x^-1) for arbitrary fracbits
 */
#define _qinv(x, xFb, resFb) ((fixedp)((((long long)1) << (xFb + resFb)) / x))

/**
 * Invert a number with default fracbits.
 */
#define qinv(x) _qinv(x, FIXED_FRACBITS, FIXED_FRACBITS);

/**
 * Modulus. Modulus is only defined for two numbers of the same fracbits
 */
#define qmod(x, y) ((x) % (y))

/**
 * Absolute value. Works for any fracbits.
 */
#define qabs(x) (((x) < 0) ? (-x) : (x))

/**
 * Floor for arbitrary fracbits
 */
#define _qfloor(x, fb) ((x) & (0xffffffff << (fb)))

/**
 * Floor for default fracbits
 */
#define qfloor(x) _qfloor(x, FIXED_FRACBITS)

/**
 * ceil for arbitrary fracbits.
 */
static INLINE fixedp _qceil(fixedp x, int fb) {
  // masks off fraction bits and adds one if there were some fractional bits
  fixedp f = _qfloor(x, fb);
  if (f != x) return qadd(f, _int2q(1, fb));
  return x;
}

/**
 * ceil for default fracbits
 */
#define qceil(x) _qceil(x, FIXED_FRACBITS)

/**
 * square root for default fracbits
 */
fixedp qsqrt(fixedp p_Square);
fixedp raiz(fixedp x);

/**
 * log (base e) for default fracbits
 */
fixedp qlog(fixedp p_Base);

/**
 * log base 10 for default fracbits
 */
fixedp qlog10(fixedp p_Base);

/**
 * exp (e to the x) for default fracbits
 */
fixedp qexp(fixedp p_Base);

/**
 * pow for default fracbits
 */
fixedp qpow(fixedp p_Base, fixedp p_Power);

/**
 * sine for default fracbits
 */
fixedp qsin(fixedp theta);

/**
 * cosine for default fracbits
 */
fixedp qcos(fixedp theta);

/**
 * tangent for default fracbits
 */
fixedp qtan(fixedp theta);

/**
 * fixedp2a - converts a fixed point number with default fracbits to a string
 */
char *q2a(char *buf, fixedp n);

#ifdef __cplusplus
}  // extern C
#endif

#endif /* _QMATH_H_ */
