#include "BigInt.h"

NTL::ZZ BigInt::GCD(NTL::ZZ a, NTL::ZZ b) {
	if (a < 0) a = -a;
	if (b < 0) b = -b;

	NTL::ZZ remainder;

	while (b != 0) {
		remainder = a % b;
		a = b;
		b = remainder;
	}

	return a;
}

NTL::ZZ BigInt::ExtendedGCD(NTL::ZZ a, NTL::ZZ b, NTL::ZZ& x, NTL::ZZ& y) {
	if (a < 0) a = -a;
	if (b < 0) b = -b;
	NTL::ZZ x1(1), y1(0);
	NTL::ZZ x2(0), y2(1);
	NTL::ZZ q, r, temp;
	while (b != 0) {
		q = a / b;
		r = a % b;
		temp = x2;
		x2 = x1 - q * x2;
		x1 = temp;
		temp = y2;
		y2 = y1 - q * y2;
		y1 = temp;
		a = b;
		b = r;
	}
	x = x1;
	y = y1;
	return a;
}

