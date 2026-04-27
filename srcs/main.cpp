#include <iostream>
#include <string>
#include <limits>
#include <stdexcept>
#include <chrono>
#include <NTL/ZZ.h>
#include "headers/BigInt.h"

using namespace std;

// ============================================================
//  Exception dùng để hủy lựa chọn hiện tại
//  Người dùng gõ 'q' hoặc 'Q' tại bất kỳ prompt nào để kích hoạt
// ============================================================
struct CancelException : public exception {
    const char* what() const noexcept override {
        return "Nguoi dung huy thao tac.";
    }
};

// Ném khi người dùng gõ 'e'/'E' → thoát chương trình ngay lập tức
struct ExitException : public exception {
    const char* what() const noexcept override {
        return "Nguoi dung yeu cau thoat.";
    }
};

// Ném khi người dùng gõ ':clear' hoặc nhấn Ctrl+L → xóa màn hình, về menu chính
struct ClearException : public exception {
    const char* what() const noexcept override {
        return "Nguoi dung yeu cau xoa man hinh.";
    }
};

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
//  Tiện ích đo thời gian
// ============================================================
using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

TimePoint timeStart() { return Clock::now(); }

void timePrint(TimePoint start, const string& label = "") {
    auto end = Clock::now();
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    cout << "\n  [TIMER] ";
    if (!label.empty()) cout << label << ": ";
    if (us < 1000)
        cout << us << " us\n";
    else if (us < 1'000'000)
        cout << us / 1000.0 << " ms\n";
    else
        cout << us / 1'000'000.0 << " s\n";
}

// ============================================================
//  Tiện ích nhập liệu  –  gõ 'q' / 'Q' để hủy bất kỳ lúc nào
// ============================================================

// Kiểm tra chuỗi có phải lệnh hủy / thoát không (thêm dấu :)
inline bool isCancel(const string& s) { return s == ":q" || s == ":Q"; }
inline bool isExit(const string& s) { return s == ":e" || s == ":E"; }
inline bool isClear(const string& s) { return s == ":clear" || s == ":CLEAR"; }

// Xóa màn hình (Windows dùng "cls", Linux/Mac dùng "clear")
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Kiểm tra và ném exception tương ứng nếu cần
inline void checkSpecial(const string& s) {
    if (isExit(s))   throw ExitException{};
    if (isCancel(s)) throw CancelException{};
    if (isClear(s))  throw ClearException{};
    // Ctrl+L gửi ký tự \x0C (form feed) – xử lý nếu chuỗi chứa nó
    if (!s.empty() && s[0] == '\x0C') throw ClearException{};
}

// Đọc số nguyên lớn (ZZ). Gõ ':q' = hủy, ':e' = thoát, ':clear' = xóa màn hình
NTL::ZZ readZZ(const string& prompt) {
    while (true) {
        cout << prompt << " (:q=huy / :e=thoat / :clear=xoa man hinh): ";
        string s;
        cin >> s;
        checkSpecial(s);

        bool valid = !s.empty();
        size_t start = (s[0] == '-') ? 1 : 0;
        for (size_t i = start; i < s.size(); i++) {
            if (!isdigit((unsigned char)s[i])) { valid = false; break; }
        }
        if (valid) {
            NTL::ZZ val;
            NTL::conv(val, s.c_str());
            return val;
        }
        cout << "  [Loi] Chi nhap so nguyen, 'q' de huy, 'e' de thoat. Thu lai.\n";
    }
}

// Đọc số nguyên thông thường trong [lo, hi]. Gõ ':q' = hủy, ':e' = thoát, ':clear' = xóa màn hình
int readInt(const string& prompt, int lo, int hi) {
    while (true) {
        cout << prompt << " (:q=huy / :e=thoat / :clear=xoa man hinh): ";
        string s;
        cin >> s;
        checkSpecial(s);

        try {
            size_t pos;
            int v = stoi(s, &pos);
            if (pos == s.size() && v >= lo && v <= hi) return v;
        }
        catch (...) {}
        cout << "  [Loi] Nhap so trong [" << lo << ", " << hi << "], ':q' de huy. Thu lai.\n";
    }
}

// Đọc một dòng chuỗi. Gõ ':q' = hủy, ':e' = thoát, ':clear' = xóa màn hình
string readLine(const string& prompt) {
    cout << prompt << " (:q=huy / :e=thoat / :clear=xoa man hinh) ";
    if (cin.peek() == '\n') cin.ignore();
    string s;
    getline(cin, s);
    checkSpecial(s);
    return s;
}

// ============================================================
//  Demo 1: Kiểm tra các hàm toán học cơ bản
// ============================================================
void demo_math() {
    printSep("DEMO 1: Cac ham toan hoc co ban");

    NTL::ZZ a = NTL::ZZ(252), b = NTL::ZZ(105);

    auto t = timeStart();
    cout << "GCD(" << a << ", " << b << ") = " << BigInt::GCD(a, b) << "\n";
    timePrint(t, "GCD");

    NTL::ZZ x, y;
    t = timeStart();
    NTL::ZZ g = BigInt::ExtendedGCD(a, b, x, y);
    timePrint(t, "ExtendedGCD");
    cout << "ExtendedGCD: gcd=" << g
        << ",  x=" << x << ",  y=" << y << "\n";
    cout << "  Kiem tra: " << a << "*" << x
        << " + " << b << "*" << y << " = " << (a * x + b * y) << "\n";

    NTL::ZZ A = NTL::ZZ(3), M = NTL::ZZ(11);
    t = timeStart();
    NTL::ZZ inv = BigInt::ModularInverse(A, M);
    timePrint(t, "ModularInverse");
    cout << "ModularInverse(" << A << ", " << M << ") = " << inv << "\n";
    cout << "  Kiem tra: " << A << " * " << inv
        << " mod " << M << " = " << (A * inv) % M << "\n";

    NTL::ZZ base = NTL::ZZ(2), exp = NTL::ZZ(10), mod = NTL::ZZ(1000);
    t = timeStart();
    NTL::ZZ pm = BigInt::PowerMod(base, exp, mod);
    timePrint(t, "PowerMod");
    cout << "PowerMod(" << base << "^" << exp
        << " mod " << mod << ") = " << pm << "\n";
}

// ============================================================
//  Demo 2: Sinh số nguyên tố lớn
// ============================================================
void demo_prime() {
    printSep("DEMO 2: Sinh so nguyen to lon");

    cout << "Dang sinh so nguyen to 512-bit...\n";
    auto t = timeStart();
    NTL::ZZ p512 = BigInt::GenerateStrongLargePrime(512);
    timePrint(t, "GenerateStrongLargePrime 512-bit");
    cout << "p (512-bit) = " << p512 << "\n";
    cout << "So bit thuc te: " << NTL::NumBits(p512) << "\n";

    cout << "\nDang sinh so nguyen to 1024-bit...\n";
    t = timeStart();
    NTL::ZZ p1024 = BigInt::GenerateStrongLargePrime(1024);
    timePrint(t, "GenerateStrongLargePrime 1024-bit");
    cout << "p (1024-bit) = " << p1024 << "\n";
    cout << "So bit thuc te: " << NTL::NumBits(p1024) << "\n";
}

// ============================================================
//  Demo 3: Tạo khóa RSA
// ============================================================
void demo_keygen(int bits = 512) {
    printSep("DEMO 3: Tao bo khoa RSA (" + to_string(bits * 2) + "-bit modulus)");

    cout << "Dang sinh p (" << bits << "-bit)...\n";
    auto t = timeStart();
    NTL::ZZ p = BigInt::GenerateStrongLargePrime(bits);
    timePrint(t, "Sinh p");

    cout << "Dang sinh q (" << bits << "-bit)...\n";
    t = timeStart();
    NTL::ZZ q = BigInt::GenerateStrongLargePrime(bits);
    timePrint(t, "Sinh q");

    while (p == q) {
        cout << "p == q, sinh lai q...\n";
        t = timeStart();
        q = BigInt::GenerateStrongLargePrime(bits);
        timePrint(t, "Sinh lai q");
    }

    cout << "\np = " << p << "\n";
    cout << "q = " << q << "\n";

    t = timeStart();
    BigInt::RSAKeyPair kp = BigInt::GenerateKeyPair(p, q);
    timePrint(t, "GenerateKeyPair");

    cout << "\n--- Bo khoa RSA ---\n";
    cout << "n   = " << kp.n << "\n";
    cout << "e   = " << kp.e << "\n";
    cout << "d   = " << kp.d << "\n";
    cout << "\nSo bit cua n: " << NTL::NumBits(kp.n) << "\n";

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

    NTL::ZZ p = BigInt::GenerateStrongLargePrime(512);
    NTL::ZZ q = BigInt::GenerateStrongLargePrime(512);
    while (p == q) q = BigInt::GenerateStrongLargePrime(512);

    BigInt::RSAKeyPair kp = BigInt::GenerateKeyPair(p, q);

    NTL::ZZ message = NTL::ZZ(123456789);
    cout << "Thong diep goc (M)       : " << message << "\n";

    auto t = timeStart();
    NTL::ZZ cipher = BigInt::Encrypt(message, kp.e, kp.n);
    timePrint(t, "Encrypt");
    cout << "Ban ma (C = M^e mod n)   : " << cipher << "\n";

    t = timeStart();
    NTL::ZZ decrypted = BigInt::Decrypt(cipher, kp.d, kp.n);
    timePrint(t, "Decrypt");
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

    NTL::ZZ p = BigInt::GenerateStrongLargePrime(512);
    NTL::ZZ q = BigInt::GenerateStrongLargePrime(512);
    while (p == q) q = BigInt::GenerateStrongLargePrime(512);

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
    auto t = timeStart();
    C = BigInt::Encrypt(M, kp.e, kp.n);
    timePrint(t, "Encrypt (string)");
    cout << "C (ban ma) : " << C << "\n";

    t = timeStart();
    NTL::ZZ M2 = BigInt::Decrypt(C, kp.d, kp.n);
    timePrint(t, "Decrypt (string)");
    string recovered = ZZToString(M2);
    cout << "Giai ma    : \"" << recovered << "\"\n";

    cout << "Ket qua    : "
        << (plaintext == recovered ? "THANH CONG [OK]" : "THAT BAI [FAILED]")
        << "\n";
}

// ============================================================
//  INTERACTIVE: Toán học cơ bản
// ============================================================
void interactive_math() {
    printSep("INTERACTIVE: Cac ham toan hoc co ban");

    cout << "[1] GCD\n"
        << "[2] Extended GCD\n"
        << "[3] Modular Inverse\n"
        << "[4] Power Mod\n"
        << "[0] Huy / Quay lai menu\n";
    int choice = readInt("Chon chuc nang (0-4):", 0, 4);
    if (choice == 0) return;

    if (choice == 1) {
        NTL::ZZ a = readZZ("  Nhap a: ");
        NTL::ZZ b = readZZ("  Nhap b: ");
        auto t = timeStart();
        NTL::ZZ result = BigInt::GCD(a, b);
        timePrint(t, "GCD");
        cout << "  GCD(" << a << ", " << b << ") = " << result << "\n";
    }
    else if (choice == 2) {
        NTL::ZZ a = readZZ("  Nhap a: ");
        NTL::ZZ b = readZZ("  Nhap b: ");
        NTL::ZZ x, y;
        auto t = timeStart();
        NTL::ZZ g = BigInt::ExtendedGCD(a, b, x, y);
        timePrint(t, "ExtendedGCD");
        cout << "  gcd = " << g << ",  x = " << x << ",  y = " << y << "\n";
        cout << "  Kiem tra: " << a << "*" << x << " + " << b << "*" << y
            << " = " << (a * x + b * y) << "\n";
    }
    else if (choice == 3) {
        NTL::ZZ a = readZZ("  Nhap a: ");
        NTL::ZZ m = readZZ("  Nhap m: ");
        try {
            auto t = timeStart();
            NTL::ZZ inv = BigInt::ModularInverse(a, m);
            timePrint(t, "ModularInverse");
            cout << "  " << a << "^(-1) mod " << m << " = " << inv << "\n";
            cout << "  Kiem tra: " << a << " * " << inv << " mod " << m
                << " = " << (a * inv) % m << "\n";
        }
        catch (const exception& e) {
            cout << "  [Loi] " << e.what() << "\n";
        }
    }
    else {
        NTL::ZZ base = readZZ("  Nhap base: ");
        NTL::ZZ exp = readZZ("  Nhap exp (>= 0): ");
        NTL::ZZ mod = readZZ("  Nhap mod (> 0): ");
        if (mod <= 0) { cout << "  [Loi] mod phai > 0.\n"; return; }
        auto t = timeStart();
        NTL::ZZ result = BigInt::PowerMod(base, exp, mod);
        timePrint(t, "PowerMod");
        cout << "  " << base << "^" << exp << " mod " << mod
            << " = " << result << "\n";
    }
}

// ============================================================
//  INTERACTIVE: Sinh số nguyên tố
// ============================================================
void interactive_prime() {
    printSep("INTERACTIVE: Sinh so nguyen to lon");

    int bits = readInt("Nhap so bit (32 - 2048): ", 32, 2048);
    cout << "Dang sinh so nguyen to " << bits << "-bit...\n";
    auto t = timeStart();
    NTL::ZZ p = BigInt::GenerateStrongLargePrime(bits);
    timePrint(t, "GenerateStrongLargePrime");
    cout << "p = " << p << "\n";
    cout << "So bit thuc te: " << NTL::NumBits(p) << "\n";
}

// ============================================================
//  INTERACTIVE: Tạo & kiểm tra cặp khóa RSA
// ============================================================
void interactive_keygen() {
    printSep("INTERACTIVE: Tao bo khoa RSA");

    cout << "[1] Tu dong sinh p, q\n"
        << "[2] Tu nhap p, q\n"
        << "[0] Huy / Quay lai menu\n";
    int choice = readInt("Chon (0-2):", 0, 2);
    if (choice == 0) return;

    NTL::ZZ p, q;
    if (choice == 1) {
        int bits = readInt("Nhap so bit cho moi so nguyen to (16 - 1024): ", 16, 1024);
        cout << "Dang sinh p...\n";
        auto t = timeStart();
        p = BigInt::GenerateStrongLargePrime(bits);
        timePrint(t, "Sinh p");
        cout << "Dang sinh q...\n";
        t = timeStart();
        q = BigInt::GenerateStrongLargePrime(bits);
        timePrint(t, "Sinh q");
        while (p == q) {
            cout << "p == q, sinh lai q...\n";
            t = timeStart();
            q = BigInt::GenerateStrongLargePrime(bits);
            timePrint(t, "Sinh lai q");
        }
    }
    else {
        p = readZZ("Nhap p (so nguyen to): ");
        q = readZZ("Nhap q (so nguyen to, p != q): ");
        if (p == q) { cout << "[Loi] p va q phai khac nhau.\n"; return; }
    }

    cout << "\np = " << p << "\n";
    cout << "q = " << q << "\n";

    try {
        auto t = timeStart();
        BigInt::RSAKeyPair kp = BigInt::GenerateKeyPair(p, q);
        timePrint(t, "GenerateKeyPair");
        cout << "\n--- Bo khoa RSA ---\n";
        cout << "n = " << kp.n << "\n";
        cout << "e = " << kp.e << "\n";
        cout << "d = " << kp.d << "\n";
        cout << "So bit cua n: " << NTL::NumBits(kp.n) << "\n";

        NTL::ZZ phi = (p - 1) * (q - 1);
        NTL::ZZ check = (kp.e * kp.d) % phi;
        cout << "Kiem tra: e*d mod phi(n) = " << check
            << (check == 1 ? "  [OK]" : "  [FAILED]") << "\n";
    }
    catch (const exception& e) {
        cout << "[Loi] " << e.what() << "\n";
    }
}

// ============================================================
//  INTERACTIVE: Mã hóa 
// ============================================================

void interactive_encrypt_only() {
    printSep("MA HOA RIENG BIET (Public Key Only)");

    cout << "--- Nhap Public Key ---\n";
    NTL::ZZ n = readZZ("Nhap n (modulus): ");
    NTL::ZZ e = readZZ("Nhap e (public exponent): ");

    cout << "\n[1] Ma hoa so nguyen\n"
        << "[2] Ma hoa chuoi van ban\n"
        << "[0] Quay lai\n";
    int sub = readInt("Chon loai du lieu:", 0, 2);

    if (sub == 1) {
        NTL::ZZ M = readZZ("Nhap M (0 <= M < n): ");
        if (M < 0 || M >= n) { cout << "[Loi] M sai khoang cho phep.\n"; return; }
        auto t = timeStart();
        NTL::ZZ C = BigInt::Encrypt(M, e, n);
        timePrint(t, "Encrypt (so nguyen)");
        cout << "Ban ma C = " << C << "\n";
    }
    else if (sub == 2) {
        string pt = readLine("Nhap plaintext:");
        NTL::ZZ M = StringToZZ(pt);
        if (M >= n) { cout << "[Loi] Chuoi qua dai.\n"; return; }
        auto t = timeStart();
        NTL::ZZ C = BigInt::Encrypt(M, e, n);
        timePrint(t, "Encrypt (chuoi)");
        cout << "Ban ma C = " << C << "\n";
    }
}

// ============================================================
//  INTERACTIVE: Giải mã 
// ============================================================

void interactive_decrypt_only() {
    printSep("GIAI MA RIENG BIET (Private Key Only)");

    cout << "--- Nhap Private Key ---\n";
    NTL::ZZ n = readZZ("Nhap n (modulus): ");
    NTL::ZZ d = readZZ("Nhap d (private exponent): ");

    NTL::ZZ C = readZZ("Nhap ban ma C can giai ma: ");

    cout << "\n[1] Giai ma ra so nguyen\n"
        << "[2] Giai ma ra chuoi van ban\n"
        << "[0] Quay lai\n";
    int sub = readInt("Chon loai ket qua mong muon:", 0, 2);

    auto t = timeStart();
    NTL::ZZ M = BigInt::Decrypt(C, d, n);
    timePrint(t, "Decrypt");

    if (sub == 1) {
        cout << "Ket qua M = " << M << "\n";
    }
    else if (sub == 2) {
        try {
            string res = ZZToString(M);
            cout << "Ket qua plaintext: \"" << res << "\"\n";
        }
        catch (...) {
            cout << "[Loi] Khong the chuyen so nay thanh chuoi van ban.\n";
        }
    }
}

// ============================================================
//  INTERACTIVE: Mã hóa / Giải mã số nguyên
// ============================================================
void interactive_encrypt_number() {
    printSep("INTERACTIVE: Ma hoa / Giai ma so nguyen");

    cout << "--- Nhap khoa RSA ---\n";
    NTL::ZZ n = readZZ("Nhap n (modulus): ");
    NTL::ZZ e = readZZ("Nhap e (public exponent): ");
    NTL::ZZ d = readZZ("Nhap d (private exponent, 0 = bo qua giai ma): ");

    NTL::ZZ M = readZZ("Nhap message M (0 <= M < n): ");
    if (M < 0 || M >= n) {
        cout << "[Loi] M phai thoa 0 <= M < n.\n";
        return;
    }

    auto t = timeStart();
    NTL::ZZ C = BigInt::Encrypt(M, e, n);
    timePrint(t, "Encrypt");
    cout << "Ban ma C = M^e mod n = " << C << "\n";

    if (d > 0) {
        t = timeStart();
        NTL::ZZ M2 = BigInt::Decrypt(C, d, n);
        timePrint(t, "Decrypt");
        cout << "Giai ma M' = C^d mod n = " << M2 << "\n";
        cout << "Ket qua: " << (M == M2 ? "THANH CONG [OK]" : "THAT BAI [FAILED]") << "\n";
    }
}

// ============================================================
//  INTERACTIVE: Mã hóa / Giải mã chuỗi văn bản
// ============================================================
void interactive_encrypt_string() {
    printSep("INTERACTIVE: Ma hoa / Giai ma chuoi van ban");

    cout << "--- Nhap khoa RSA ---\n";
    NTL::ZZ n = readZZ("Nhap n (modulus): ");
    NTL::ZZ e = readZZ("Nhap e (public exponent): ");
    NTL::ZZ d = readZZ("Nhap d (private exponent, 0 = bo qua giai ma): ");

    string plaintext = readLine("Nhap chuoi plaintext:");

    NTL::ZZ M = StringToZZ(plaintext);
    cout << "M (so) = " << M << "\n";

    if (M >= n) {
        cout << "[Loi] Message qua lon so voi n. Dung chuoi ngan hon hoac n lon hon.\n";
        return;
    }

    auto t = timeStart();
    NTL::ZZ C = BigInt::Encrypt(M, e, n);
    timePrint(t, "Encrypt");
    cout << "Ban ma C = " << C << "\n";

    if (d > 0) {
        t = timeStart();
        NTL::ZZ M2 = BigInt::Decrypt(C, d, n);
        timePrint(t, "Decrypt");
        string recovered = ZZToString(M2);
        cout << "Giai ma    : \"" << recovered << "\"\n";
        cout << "Ket qua    : "
            << (plaintext == recovered ? "THANH CONG [OK]" : "THAT BAI [FAILED]")
            << "\n";
    }
}

// ============================================================
//  INTERACTIVE: Luồng end-to-end (sinh key + mã hóa chuỗi)
// ============================================================
void interactive_full_flow() {
    printSep("INTERACTIVE: Luong RSA day du (sinh khoa + ma hoa chuoi)");

    int bits = readInt("Nhap so bit cho moi so nguyen to (16 - 1024): ", 16, 1024);

    cout << "Dang sinh p...\n";
    auto t = timeStart();
    NTL::ZZ p = BigInt::GenerateStrongLargePrime(bits);
    timePrint(t, "Sinh p");
    cout << "Dang sinh q...\n";
    t = timeStart();
    NTL::ZZ q = BigInt::GenerateStrongLargePrime(bits);
    timePrint(t, "Sinh q");
    while (p == q) {
        cout << "p == q, sinh lai...\n";
        t = timeStart();
        q = BigInt::GenerateStrongLargePrime(bits);
        timePrint(t, "Sinh lai q");
    }

    t = timeStart();
    BigInt::RSAKeyPair kp = BigInt::GenerateKeyPair(p, q);
    timePrint(t, "GenerateKeyPair");
    cout << "\n[Khoa da tao]\n";
    cout << "n = " << kp.n << "\n";
    cout << "e = " << kp.e << "\n";
    cout << "d = " << kp.d << "\n";

    string plaintext = readLine("Nhap chuoi can ma hoa:");

    NTL::ZZ M = StringToZZ(plaintext);
    if (M >= kp.n) {
        cout << "[Loi] Chuoi qua dai voi kich thuoc khoa hien tai. Tang so bit hoac rut ngan chuoi.\n";
        return;
    }

    t = timeStart();
    NTL::ZZ C = BigInt::Encrypt(M, kp.e, kp.n);
    timePrint(t, "Encrypt");
    t = timeStart();
    NTL::ZZ M2 = BigInt::Decrypt(C, kp.d, kp.n);
    timePrint(t, "Decrypt");
    string recovered = ZZToString(M2);

    cout << "\nPlaintext  : \"" << plaintext << "\"\n";
    cout << "Ban ma C   : " << C << "\n";
    cout << "Giai ma    : \"" << recovered << "\"\n";
    cout << "Ket qua    : "
        << (plaintext == recovered ? "THANH CONG [OK]" : "THAT BAI [FAILED]")
        << "\n";
}

// ============================================================
//  Menu chính
// ============================================================
void showMainMenu() {
    cout << "\n" << string(60, '-') << "\n";
    cout << "  MENU CHINH\n";
    cout << string(60, '-') << "\n";
    cout << "  [1]  Chay tat ca demo mau     (auto)\n";
    cout << "  [2]  Demo toan hoc co ban     (interactive)\n";
    cout << "  [3]  Sinh so nguyen to lon    (interactive)\n";
    cout << "  [4]  Tao bo khoa RSA          (interactive)\n";
    cout << "  [5]  Ma hoa                   (interactive)\n";
    cout << "  [6]  Giai ma                  (interactive)\n";
    cout << "  [7]  Ma hoa va giai ma chuoi  (interactive)\n";
    cout << "  [8]  Luong RSA day du         (interactive)\n";
    cout << "  [0]  Thoat\n";
    cout << string(60, '-') << "\n";
    cout << "  Tip: :q=huy thao tac  |  :e=thoat  |  :clear / Ctrl+L=xoa man hinh\n";
    cout << string(60, '-') << "\n";
}

// ============================================================
//  main
// ============================================================
int main() {
    cout << "===========================================\n";
    cout << "  BTL3 - He ma RSA  |  NTL + C++\n";
    cout << "===========================================\n";

    try {
        while (true) {
            showMainMenu();
            int choice;

            try {
                // Nếu gõ :q ở đây, readInt sẽ ném CancelException
                choice = readInt("Chon chuc nang", 0, 8);
            }
            catch (const CancelException&) {
                // Bắt CancelException tại menu chính -> Chuyển thành thoát
                throw ExitException{};
            }
            catch (const ClearException&) {
                clearScreen();
                continue;  // vẽ lại menu ngay lập tức
            }

            try {
                switch (choice) {
                case 0:
                    printSep("HOAN THANH");
                    return 0;

                case 1:
                    demo_math();
                    demo_prime();
                    demo_keygen(512);
                    demo_encrypt_number();
                    demo_encrypt_string();
                    printSep("HOAN THANH DEMO MAU");
                    break;

                case 2: interactive_math();             break;
                case 3: interactive_prime();            break;
                case 4: interactive_keygen();           break;
                case 5: interactive_encrypt_only();     break;
                case 6: interactive_decrypt_only();     break;
                case 7: interactive_encrypt_string();   break;
                case 8: interactive_full_flow();        break;
                }
            }
            catch (const CancelException&) {
                cout << "\n  [Huy] Da quay ve menu chinh.\n";
            }
            catch (const ClearException&) {
                clearScreen();
                continue;  // vẽ lại menu ngay lập tức, bỏ qua "press enter"
            }
            // ExitException KHÔNG bị bắt ở đây → nổi lên catch ngoài
        }
    }
    catch (const ExitException&) {
        printSep("THOAT CHUONG TRINH");
    }
    return 0;
}