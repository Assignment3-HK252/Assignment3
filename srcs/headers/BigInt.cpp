#include "BigInt.h"

// ============================================================
//  GCD  –  Euclidean algorithm
// ============================================================
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

// ============================================================
//  Extended GCD  –  trả về gcd và hệ số Bézout x, y
//  sao cho: a*x + b*y = gcd(a, b)
// ============================================================
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

// ============================================================
//  Modular Inverse  –  trả về a^(-1) mod m
// ============================================================
NTL::ZZ BigInt::ModularInverse(NTL::ZZ a, NTL::ZZ m) {
    NTL::ZZ x, y;
    NTL::ZZ gcd = BigInt::ExtendedGCD(a, m, x, y);
    if (gcd != 1) {
        throw std::invalid_argument("Modular inverse does not exist (gcd != 1)");
    }
    return (x % m + m) % m;
}

// ============================================================
//  PowerMod  –  trả về base^exp mod mod  (square-and-multiply)
// ============================================================
NTL::ZZ BigInt::PowerMod(NTL::ZZ base, NTL::ZZ exp, NTL::ZZ mod) {
    NTL::ZZ result(1);
    base = base % mod;

    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % mod;
        }
        exp = exp >> 1;
        base = (base * base) % mod;
    }
    return result;
}

// ============================================================
//  decompose  –  tách n-1 = 2^r * d  (d lẻ)
// ============================================================
void BigInt::decompose(NTL::ZZ n_minus_1, NTL::ZZ& d, long& r) {
    d = n_minus_1;
    r = 0;
    while (NTL::IsOdd(d) == 0) {   // d chẵn
        d >>= 1;
        r++;
    }
}

// ============================================================
//  PassSmallPrimeTests  –  loại nhanh bằng các số nguyên tố nhỏ
// ============================================================
static const long SMALL_PRIMES[] = {
    2, 3, 5, 7, 11, 13, 17, 19, 23, 29,
    31, 37, 41, 43, 47, 53, 59, 61, 67, 71,
    73, 79, 83, 89, 97, 101, 103, 107, 109, 113,
    127, 131, 137, 139, 149, 151, 157, 163, 167, 173,
    179, 181, 191, 193, 197, 199, 211, 223, 227, 229
};

bool BigInt::PassSmallPrimeTests(NTL::ZZ n) {
    for (long p : SMALL_PRIMES) {
        NTL::ZZ pz(p);
        if (n == pz) return true;           // chính là số nguyên tố nhỏ này
        if (NTL::rem(n, p) == 0) return false;
    }
    return true;
}

// ============================================================
//  MillerRabinTest  –  kiểm tra nguyên tố xác suất (k lần)
// ============================================================
bool BigInt::MillerRabinTest(NTL::ZZ n, int k) {
    if (n <= 1)  return false;
    if (n == 2 || n == 3) return true;
    if (NTL::IsOdd(n) == 0) return false;

    // Bước 1: tách n-1 = 2^r * d
    NTL::ZZ d;
    long    r;
    decompose(n - 1, d, r);

    // Bước 2: lặp k lần
    for (int i = 0; i < k; i++) {

        // Chọn nhân chứng a ngẫu nhiên trong [2, n-2]
        NTL::ZZ a = NTL::RandomBnd(n - 3) + 2;

        // Bước 3: x = a^d mod n
        NTL::ZZ x = BigInt::PowerMod(a, d, n);

        if (x == 1 || x == n - 1) continue;   // nhân chứng không bác bỏ

        // Bước 4: bình phương tối đa r-1 lần
        bool probably_prime = false;
        for (long j = 0; j < r - 1; j++) {
            x = (x * x) % n;           // FIX: dùng phép toán thông thường, KHÔNG dùng MulMod sai

            if (x == n - 1) {
                probably_prime = true;
                break;
            }
        }

        if (!probably_prime) return false;     // nhân chứng a "tố cáo" n là hợp số
    }

    return true;   // probably prime
}

// ============================================================
//  GenerateLargePrime  –  sinh số nguyên tố ngẫu nhiên 'bits' bit
// ============================================================
NTL::ZZ BigInt::GenerateLargePrime(int bits) {
    NTL::ZZ candidate;

    while (true) {
        // 1. Sinh số ngẫu nhiên đúng độ dài bits
        candidate = NTL::RandomLen_ZZ(bits);

        // 2. Ép bit 0 = 1 (số lẻ) và bit (bits-1) = 1 (đúng độ dài)
        NTL::SetBit(candidate, 0);
        NTL::SetBit(candidate, bits - 1);

        // 3. Sàng sơ bộ với các số nguyên tố nhỏ (loại ~80% ứng viên nhanh)
        if (!PassSmallPrimeTests(candidate)) continue;

        // 4. Miller-Rabin với k = 40 vòng lặp
        if (MillerRabinTest(candidate, 40)) {
            return candidate;
        }
    }
}

NTL::ZZ BigInt::GenerateStrongLargePrime(int bits)
{
    // 1. Sinh hai số nguyên tố lớn ngẫu nhiên s và t (khoảng bits/2)
    int sub_bits = bits / 2 - 8;
    if (sub_bits < 2) sub_bits = 2; // Đảm bảo tối thiểu 2 bit

    NTL::ZZ s = GenerateLargePrime(sub_bits);
    NTL::ZZ t = GenerateLargePrime(sub_bits);

    // 2. Tìm r = 2 * i * t + 1 sao cho r là số nguyên tố
    NTL::ZZ r;
    NTL::ZZ i(1);
    while (true) {
        r = 2 * i * t + 1;
        if (MillerRabinTest(r, 40)) break;
        i++;
    }

    // 3. Tính p_zero = 2 * (s^(r-2) mod r) * s - 1
    NTL::ZZ s_inv_r = PowerMod(s, r - 2, r); // Theo định lý Fermat nhỏ
    NTL::ZZ p_zero = 2 * s_inv_r * s - 1;

    // 4. Tìm p = p_zero + 2 * j * r * s sao cho p là số nguyên tố
    NTL::ZZ p;
    NTL::ZZ rs2 = 2 * r * s;
    NTL::ZZ j(1);
    while (true) {
        p = p_zero + j * rs2;
        // Đảm bảo p có đủ số bit yêu cầu
        if (NTL::NumBits(p) >= bits && MillerRabinTest(p, 40)) {
            return p;
        }
        j++;
    }
}

// ============================================================
//  GenerateKeyPair  –  tạo bộ khóa RSA từ 2 số nguyên tố p, q
// ============================================================
BigInt::RSAKeyPair BigInt::GenerateKeyPair(NTL::ZZ p, NTL::ZZ q) {
    RSAKeyPair kp;

    // n = p * q
    kp.n = p * q;

    // phi(n) = (p-1)(q-1)
    NTL::ZZ phi = (p - 1) * (q - 1);

    // Chọn e = 65537 (chuẩn công nghiệp); nếu gcd(e, phi) != 1 thì báo lỗi
    kp.e = NTL::ZZ(65537);
    if (GCD(kp.e, phi) != 1) {
        throw std::invalid_argument("e = 65537 is not coprime with phi(n). Choose different p, q.");
    }

    // d = e^(-1) mod phi
    kp.d = ModularInverse(kp.e, phi);

    return kp;
}

// ============================================================
//  Encrypt  –  C = M^e mod n
// ============================================================
NTL::ZZ BigInt::Encrypt(NTL::ZZ message, NTL::ZZ e, NTL::ZZ n) {
    if (message >= n) {
        throw std::invalid_argument("Message must be less than n");
    }
    return PowerMod(message, e, n);
}

// ============================================================
//  Decrypt  –  M = C^d mod n
// ============================================================
NTL::ZZ BigInt::Decrypt(NTL::ZZ ciphertext, NTL::ZZ d, NTL::ZZ n) {
    return PowerMod(ciphertext, d, n);
}