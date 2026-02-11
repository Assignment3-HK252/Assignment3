#pragma once
#include <NTL/ZZ.h>


class BigInt {
public:
	static NTL::ZZ GCD(NTL::ZZ a, NTL::ZZ b);
};


