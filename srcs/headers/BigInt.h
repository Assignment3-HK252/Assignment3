#pragma once
#include <NTL/ZZ.h>
#include <random>


class BigInt {
public:
	static NTL::ZZ GCD(NTL::ZZ a, NTL::ZZ b);
	static NTL::ZZ ExtendedGCD(NTL::ZZ a, NTL::ZZ b, NTL::ZZ& x, NTL::ZZ& y);
	static NTL::ZZ ModularInverse(NTL::ZZ a, NTL::ZZ m);
	static NTL::ZZ PowerMod(NTL::ZZ base, NTL::ZZ exp, NTL::ZZ mod);
	static void decompose(NTL::ZZ n_minus_1, NTL::ZZ& d, long& r);
	static bool PassSmallPrimeTests(NTL::ZZ n);
	static bool MillerRabinTest(NTL::ZZ n, int k = 40);
	static NTL::ZZ GenerateLargePrime(int bits = 512);
};


