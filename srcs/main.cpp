#include <iostream>
#include <string>
#include <NTL/ZZ.h>
#include "headers/BigInt.h"

using namespace std;

// ============================================================
//  Tiện ích: chuyển string <-> ZZ (encode từng byte)
//  Lưu ý: chỉ dùng cho demo với chuỗi ngắn (message < n)
// ============================================================
NTL::ZZ StringToZZ(const string& s) {
    NTL::ZZ result(0);
    for (unsigned char c : s) {
        result = result * 256 + NTL::ZZ(c);
    }
    return result;
}

string ZZToString(NTL::ZZ n) {
    if (n == 0) return string(1, '\0');
    string result;
    while (n > 0) {
        long byte_val = NTL::to_long(n % 256);
        result = char(byte_val) + result;
        n /= 256;
    }
    return result;
}

// ============================================================
//  In đường phân cách
// ============================================================
void printSep(const string& title = "") {
    cout << "\n" << string(60, '=');
    if (!title.empty()) cout << "\n  " << title;
    cout << "\n" << string(60, '=') << "\n";
}

// ============================================================
//  Demo 1: Kiểm tra các hàm toán học cơ bản
// ============================================================
void demo_math() {
    printSep("DEMO 1: Các hàm toán học cơ bản");

    NTL::ZZ a = NTL::ZZ(252), b = NTL::ZZ(105);

    // GCD
    cout << "GCD(" << a << ", " << b << ") = "
        << BigInt::GCD(a, b) << "\n";

    // Extended GCD
    NTL::ZZ x, y;
    NTL::ZZ g = BigInt::ExtendedGCD(a, b, x, y);
    cout << "ExtendedGCD: gcd=" << g
        << ",  x=" << x << ",  y=" << y << "\n";
    cout << "  Kiem tra: " << a << "*" << x
        << " + " << b << "*" << y << " = " << (a * x + b * y) << "\n";

    // Modular Inverse
    NTL::ZZ A = NTL::ZZ(3), M = NTL::ZZ(11);
    NTL::ZZ inv = BigInt::ModularInverse(A, M);
    cout << "ModularInverse(" << A << ", " << M << ") = " << inv << "\n";
    cout << "  Kiem tra: " << A << " * " << inv
        << " mod " << M << " = " << (A * inv) % M << "\n";

    // PowerMod
    NTL::ZZ base = NTL::ZZ(2), exp = NTL::ZZ(10), mod = NTL::ZZ(1000);
    cout << "PowerMod(" << base << "^" << exp
        << " mod " << mod << ") = "
        << BigInt::PowerMod(base, exp, mod) << "\n";
}

// ============================================================
//  Demo 2: Sinh số nguyên tố lớn
// ============================================================
void demo_prime() {
    printSep("DEMO 2: Sinh so nguyen to lon");

    cout << "Dang sinh so nguyen to 512-bit...\n";
    NTL::ZZ p512 = BigInt::GenerateLargePrime(512);
    cout << "p (512-bit) = " << p512 << "\n";
    cout << "So bit thuc te: " << NTL::NumBits(p512) << "\n";

    cout << "\nDang sinh so nguyen to 1024-bit...\n";
    NTL::ZZ p1024 = BigInt::GenerateLargePrime(1024);
    cout << "p (1024-bit) = " << p1024 << "\n";
    cout << "So bit thuc te: " << NTL::NumBits(p1024) << "\n";
}

// ============================================================
//  Demo 3: Tạo khóa RSA
// ============================================================
void demo_keygen(int bits = 512) {
    printSep("DEMO 3: Tao bo khoa RSA (" + to_string(bits * 2) + "-bit modulus)");

    cout << "Dang sinh p (" << bits << "-bit)...\n";
    NTL::ZZ p = BigInt::GenerateLargePrime(bits);

    cout << "Dang sinh q (" << bits << "-bit)...\n";
    NTL::ZZ q = BigInt::GenerateLargePrime(bits);

    // Đảm bảo p != q
    while (p == q) {
        cout << "p == q, sinh lai q...\n";
        q = BigInt::GenerateLargePrime(bits);
    }

    cout << "\np = " << p << "\n";
    cout << "q = " << q << "\n";

    BigInt::RSAKeyPair kp = BigInt::GenerateKeyPair(p, q);

    cout << "\n--- Bo khoa RSA ---\n";
    cout << "n   = " << kp.n << "\n";
    cout << "e   = " << kp.e << "\n";
    cout << "d   = " << kp.d << "\n";
    cout << "\nSo bit cua n: " << NTL::NumBits(kp.n) << "\n";

    // Kiểm tra: e * d mod phi == 1
    NTL::ZZ phi = (p - 1) * (q - 1);
    NTL::ZZ check = (kp.e * kp.d) % phi;
    cout << "Kiem tra: e*d mod phi(n) = " << check
        << (check == 1 ? "  [OK]" : "  [FAILED]") << "\n";
}

// ============================================================
//  Demo 4: Mã hóa / Giải mã số nguyên
// ============================================================
void demo_encrypt_number() {
    printSep("DEMO 4: Ma hoa / Giai ma so nguyen (512-bit keys)");

    // Dùng khóa nhỏ hơn để demo nhanh
    NTL::ZZ p = BigInt::GenerateLargePrime(512);
    NTL::ZZ q = BigInt::GenerateLargePrime(512);
    while (p == q) q = BigInt::GenerateLargePrime(512);

    BigInt::RSAKeyPair kp = BigInt::GenerateKeyPair(p, q);

    NTL::ZZ message = NTL::ZZ(123456789);
    cout << "Thong diep goc (M)       : " << message << "\n";

    NTL::ZZ cipher = BigInt::Encrypt(message, kp.e, kp.n);
    cout << "Ban ma (C = M^e mod n)   : " << cipher << "\n";

    NTL::ZZ decrypted = BigInt::Decrypt(cipher, kp.d, kp.n);
    cout << "Giai ma (M = C^d mod n)  : " << decrypted << "\n";

    cout << "Ket qua: "
        << (message == decrypted ? "THANH CONG [OK]" : "THAT BAI [FAILED]")
        << "\n";
}

// ============================================================
//  Demo 5: Mã hóa / Giải mã chuỗi văn bản
// ============================================================
void demo_encrypt_string() {
    printSep("DEMO 5: Ma hoa / Giai ma chuoi van ban");

    // Dùng key 512-bit để message ngắn vẫn nhỏ hơn n
    NTL::ZZ p = BigInt::GenerateLargePrime(512);
    NTL::ZZ q = BigInt::GenerateLargePrime(512);
    while (p == q) q = BigInt::GenerateLargePrime(512);

    BigInt::RSAKeyPair kp = BigInt::GenerateKeyPair(p, q);

    string plaintext = "Hello RSA!";
    cout << "Plaintext  : \"" << plaintext << "\"\n";

    NTL::ZZ M = StringToZZ(plaintext);
    cout << "M (so)     : " << M << "\n";

    if (M >= kp.n) {
        cout << "[LOI] Message qua lon so voi n. Chon chuoi ngan hon hoac tang bit.\n";
        return;
    }

    NTL::ZZ C = BigInt::Encrypt(M, kp.e, kp.n);
    cout << "C (ban ma) : " << C << "\n";

    NTL::ZZ M2 = BigInt::Decrypt(C, kp.d, kp.n);
    string recovered = ZZToString(M2);
    cout << "Giai ma    : \"" << recovered << "\"\n";

    cout << "Ket qua    : "
        << (plaintext == recovered ? "THANH CONG [OK]" : "THAT BAI [FAILED]")
        << "\n";
}

// ============================================================
//  main
// ============================================================
int main() {
    cout << "===========================================\n";
    cout << "  BTL3 - He ma RSA  |  NTL + C++\n";
    cout << "===========================================\n";

    demo_math();
    demo_prime();
    demo_keygen(512);
    demo_encrypt_number();
    demo_encrypt_string();

    printSep("HOAN THANH");
    return 0;
}