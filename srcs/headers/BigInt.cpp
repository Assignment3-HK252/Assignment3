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

NTL::ZZ BigInt::ModularInverse(NTL::ZZ a, NTL::ZZ m)
{
	NTL::ZZ x, y;
	NTL::ZZ gcd = BigInt::ExtendedGCD(a, m, x, y);
	if (gcd != 1) {
		throw std::invalid_argument("Inverse doesn't exist");
	}
	return (x % m + m) % m;
}

NTL::ZZ BigInt::PowerMod(NTL::ZZ base, NTL::ZZ exp, NTL::ZZ mod)
{
	NTL::ZZ result(1);
	base = base % mod;
	while (exp > 0) {
		if (exp % 2 == 1) {
			result = (result * base) % mod;
		}
		exp = exp >> 1; // equivalent to exp /= 2
		base = (base * base) % mod;
	}
	return result;
}

void BigInt::decompose(NTL::ZZ n_minus_1, NTL::ZZ& d, long& r) {
	d = n_minus_1;
	r = 0;
	while (NTL::IsOdd(d) == 0) {
		d >>= 1;
		r++;
	}
}

static const long small_primes[] = {
	3,5,7,11,13,17,19,23,29,31,37,41,43,47,
	53,59,61,67,71,73,79,83,89,97,
	101,103,107,109,113,127,131,137,139,149,
	// ... thêm nữa tùy muốn
};

bool BigInt::PassSmallPrimeTests(NTL::ZZ n)
{
	for (long p : small_primes) {
		if (NTL::rem(n, p) == 0) return false;
	}
	return true;
}

bool BigInt::MillerRabinTest(NTL::ZZ n, int k){
	if (n <= 1) return false;
	if (n <= 3) return true;
	if (NTL::IsOdd(n) == 0) return false;

	// --- Bước 1: Tách n-1 = 2^r * d ---
	NTL::ZZ d;
	long r;
	decompose(n - 1, d, r);

	// --- Bước 2: Lặp k lần, mỗi lần chọn một nhân chứng a ---
	for (int i = 0; i < k; i++) {

		// Chọn a ngẫu nhiên trong khoảng [2, n-2]
		NTL::ZZ a;
		// RandomBnd(n-3) cho ra số trong [0, n-4], cộng 2 → [2, n-2]
		a = NTL::RandomBnd(n - 3) + 2;

		// --- Bước 3: Tính x = a^d mod n ---
		NTL::ZZ x = BigInt::PowerMod(a, d, n); // dùng hàm đã có!

		// Nếu x == 1 hoặc x == n-1: nhân chứng này không bác bỏ được
		// → sang nhân chứng tiếp theo
		if (x == 1 || x == n - 1) continue;

		// --- Bước 4: Bình phương tối đa r-1 lần ---
		bool composite = true; // assume composite until proven otherwise

		for (long j = 0; j < r - 1; j++) {
			// x = BigInt::PowerMod(x, NTL::ZZ(2), n); // x = x^2 mod n

			NTL::MulMod(x, x, NTL::ZZ(1), n); // x = x^2 mod n

			if (x == n - 1) {
				composite = false; // nhân chứng này không bác bỏ được
				break;
			}
		}

		// Nếu qua r-1 lần bình phương mà vẫn không gặp n-1
		// → n chắc chắn là hợp số (nhân chứng a đã "tố cáo" n)
		if (composite) return false;
	}

	// Qua hết k vòng mà không bị bác bỏ → probably prime
	return true;
	
}


NTL::ZZ BigInt::GenerateLargePrime(int bits) {
	NTL::ZZ candidate;

	while (true) {
		// 1. Sinh số ngẫu nhiên có độ dài bit chính xác
		candidate = NTL::RandomLen_ZZ(bits);

		// 2. Ép bit thấp nhất là 1 (số lẻ) và bit cao nhất là 1 (đúng độ dài bits)
		NTL::SetBit(candidate, 0);
		NTL::SetBit(candidate, bits - 1);

		// 3. Sàng sơ bộ với các số nguyên tố nhỏ (tối ưu hiệu suất)
		// Loại bỏ nhanh các số chia hết cho 3, 5, 7, 11...
		//if (NTL::GCD(candidate, NTL::ZZ(30030)) != 1) continue;

		// 4. Kiểm tra Miller-Rabin với k = 40
		if (MillerRabinTest(candidate, 40)) {
			return candidate;
		}
	}
}
