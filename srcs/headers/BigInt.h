#pragma once
#include <NTL/ZZ.h>
#include <stdexcept>

class BigInt {
public:
    // === Số học cơ bản ===
    static NTL::ZZ GCD(NTL::ZZ a, NTL::ZZ b);
    static NTL::ZZ ExtendedGCD(NTL::ZZ a, NTL::ZZ b, NTL::ZZ& x, NTL::ZZ& y);
    static NTL::ZZ ModularInverse(NTL::ZZ a, NTL::ZZ m);
    static NTL::ZZ PowerMod(NTL::ZZ base, NTL::ZZ exp, NTL::ZZ mod);

    // === Kiểm tra nguyên tố ===
    static void    decompose(NTL::ZZ n_minus_1, NTL::ZZ& d, long& r);
    static bool    PassSmallPrimeTests(NTL::ZZ n);
    static bool    MillerRabinTest(NTL::ZZ n, int k = 40);
    static NTL::ZZ GenerateLargePrime(int bits = 512);
	static NTL::ZZ GenerateStrongLargePrime(int bits = 512);   // p, q sao cho p-1 và q-1 đều có ít nhất một thừa số nguyên tố lớn

    // === RSA ===
    struct RSAKeyPair {
        NTL::ZZ n;   // modulus
        NTL::ZZ e;   // public exponent
        NTL::ZZ d;   // private exponent
    };

    static RSAKeyPair GenerateKeyPair(NTL::ZZ p, NTL::ZZ q);
    static NTL::ZZ    Encrypt(NTL::ZZ message, NTL::ZZ e, NTL::ZZ n);
    static NTL::ZZ    Decrypt(NTL::ZZ ciphertext, NTL::ZZ d, NTL::ZZ n);
};