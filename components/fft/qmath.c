/* **********************************************************************
 *
 * Fixed Point Math Library
 *
 * **********************************************************************
 *
 * qmath.c
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

#include "qmath.h"

/**
 * square root
 */
fixedp qsqrt(fixedp p_Square)
{	int x=0;
	fixedp   res;
	fixedp   delta;
	fixedp   maxError;

	if ( p_Square <= 0 )
		return 0;

	res = (p_Square >> 1);

	maxError =  qmul(p_Square, FIXED_SQRT_ERR);

	do
	{
		delta =  (qmul( res, res ) - p_Square);
		res -=  qdiv(delta, ( res << 1 ));
		x++;
	}
	while ( delta > maxError || delta < -maxError );

	return res;
}

fixedp raiz(fixedp x){
    fixedp r = x, t = 0, i=0;
	//printf("%d ",x);
    while (i<20){
	//printf("%d\n", r);        
	t = r;
        r=(t==0)?0:((qdiv(x,r) + r)>>1);
	i++;
//	printf("%d\n", r);
    }
	//printf("%d\n ",r);
    return r;
}

/**
 * log (base e)
 */
fixedp qlog( fixedp p_Base )
{
    fixedp w = 0;
	fixedp y = 0;
	fixedp z = 0;
	fixedp num = int2q(1);
	fixedp dec = 0;

	if ( p_Base == int2q(1) )
		return 0;

	if ( p_Base == 0 )
		return 0xffffffff;

	for ( dec=0 ; qabs( p_Base ) >= int2q(2) ; dec += int2q(1) )
		p_Base = qdiv(p_Base, QLN_E);

	p_Base -= int2q(1);
	z = p_Base;
	y = p_Base;
	w = int2q(1);

	while ( y != y + w )
	{
		z = 0 - qmul( z , p_Base );
		num += int2q(1);
		w = qdiv( z , num );
		y += w;
	}

	return y + dec;
}

/**
 * log base 10
 */
fixedp qlog10( fixedp p_Base )
{
    // ln(p_Base)/ln(10)
    // more accurately, ln(p_Base) * (1/ln(10))
    return qmul(qlog(p_Base), Q1OLN_10);
}

/**
 * exp (e to the x)
 */
fixedp qexp(fixedp p_Base)
{
	fixedp w;
	fixedp y;
	fixedp num;

	for ( w=int2q(1), y=int2q(1), num=int2q(1) ; y != y+w ; num += int2q(1) )
	{
		w = qmul(w, qdiv(p_Base, num));
		y += w;
	}

	return y;
}

/**
 * pow
 */
fixedp qpow( fixedp p_Base, fixedp p_Power )
{
	if ( p_Base < 0 && qmod(p_Power, int2q(2)) != 0 )
		return - qexp( qmul(p_Power, qlog( -p_Base )) );
	else
		return qexp( qmul(p_Power, qlog(qabs( p_Base ))) );
}

/**
 * sinx, internal sine function
 */
static fixedp sinx(fixedp x)
{
	fixedp xpwr;
	long xftl;
	fixedp xresult;
	short positive;
	int i;

	xresult = 0;
	xpwr = x;
	xftl = 1;
	positive = -1;

	// Note: 12! largest for long
	for (i = 1; i < 7; i+=2)
	{
		if ( positive )
			xresult += qdiv(xpwr, long2q(xftl));
		else
			xresult -= qdiv(xpwr, long2q(xftl));

		xpwr = qmul(x, xpwr);
		xpwr = qmul(x, xpwr);
		xftl *= i+1;
		xftl *= i+2;
		positive = !positive;
	}

	return xresult;
}

/**
 * sine
 */
fixedp qsin(fixedp theta)
{
	fixedp xresult;
	short bBottom = 0;
	//static fixed xPI = XPI;
	//static fixed x2PI = X2PI;
	//static fixed xPIO2 = XPIO2;

	fixedp x = qmod(theta, Q2PI);
	if ( x < 0 )
		x += Q2PI;

	if ( x > QPI )
	{
		bBottom = -1;
		x -= QPI;
	}

	if ( x <= QPIO2 )
		xresult = sinx(x);
	else
		xresult = sinx(QPIO2-(x-QPIO2));

	if ( bBottom )
		return -xresult;

	return xresult;
}

/**
 * cosine
 */
fixedp qcos(fixedp theta)
{
	return qsin(theta + QPIO2);
}

/**
 * tangent
 */
fixedp qtan(fixedp theta)
{
	return qdiv(qsin(theta), qcos(theta));
}

/**
 * q2a - converts a fixed point number to a string
 */
char *q2a(char *buf, fixedp n)
{
	long ipart = qipart(n);
	long fpart = qfpart(n);
	long intdigits = 0;

	int i = 0;
	int j = 0;
	int d = 0;

	int offset = 0;

	long v;
	long t;
	long st = 0;

	if (n != 0)
	{
		intdigits = qipart(qceil(qlog10(qabs(n))));
	}

	if (intdigits < 1) intdigits = 1;

	offset = intdigits - 1;

	if (n < 0)
	{
		buf[0] = '-';
		offset++;
		n = -n;
		ipart = -ipart;
		fpart = -fpart;
	}

	// integer portion

	for (i = 0; i < intdigits; i++)
	{
		j = offset - i;
		d = ipart % 10;
		ipart = ipart / 10;
		buf[j] = '0' + d;
	}

	// decimal point

	buf[offset + 1] = '.';

	// fractional portion

	v = 5;
	t = FIXED_RESOLUTION >> 1;

	for (i = 0; i < FIXED_DECIMALDIGITS - 1; i++)
	{
		v = (v << 1) + (v << 3);
	}

	for (i = 0; i < FIXED_FRACBITS; i++)
	{
		if (fpart & t)
		{
			st += v;
		}
		v = v >> 1;
		t = t >> 1;
	}

	offset += FIXED_DECIMALDIGITS + 1;

	for (i = 0; i < FIXED_DECIMALDIGITS; i++)
	{
		j = offset - i;
		d = st % 10;
		st = st / 10;
		buf[j] = '0' + d;
	}

	// ending zero
	buf[offset + 1] = '\0';

	return buf;
}



